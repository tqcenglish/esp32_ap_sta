/* HTTP Restful API Server

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include <fcntl.h>
#include "nvs_flash.h"
#include "esp_http_server.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_vfs.h"
#include "cJSON.h"

static const char *REST_TAG = "esp-rest";
#define REST_CHECK(a, str, goto_tag, ...)                                              \
    do                                                                                 \
    {                                                                                  \
        if (!(a))                                                                      \
        {                                                                              \
            ESP_LOGE(REST_TAG, "%s(%d): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
            goto goto_tag;                                                             \
        }                                                                              \
    } while (0)

#define FILE_PATH_MAX (ESP_VFS_PATH_MAX + 128)
#define SCRATCH_BUFSIZE (10240)

typedef struct rest_server_context {
    char base_path[ESP_VFS_PATH_MAX + 1];
    char scratch[SCRATCH_BUFSIZE];
} rest_server_context_t;

#define CHECK_FILE_EXTENSION(filename, ext) (strcasecmp(&filename[strlen(filename) - strlen(ext)], ext) == 0)

void nvs_get(char *key, char* value, size_t len);
void nvs_set(char *key, char* value);
void wifi_init_sta(void);
void restart_wifi(void);

/* 定义一个NVS操作句柄 */
nvs_handle wificfg_nvs_handler;

/* 通过文件扩展设置反应类型 */
static esp_err_t set_content_type_from_file(httpd_req_t *req, const char *filepath)
{
    const char *type = "text/plain";
    if (CHECK_FILE_EXTENSION(filepath, ".html")) {
        type = "text/html";
    } else if (CHECK_FILE_EXTENSION(filepath, ".js")) {
        type = "application/javascript";
    } else if (CHECK_FILE_EXTENSION(filepath, ".css")) {
        type = "text/css";
    } else if (CHECK_FILE_EXTENSION(filepath, ".png")) {
        type = "image/png";
    } else if (CHECK_FILE_EXTENSION(filepath, ".ico")) {
        type = "image/x-icon";
    } else if (CHECK_FILE_EXTENSION(filepath, ".svg")) {
        type = "text/xml";
    }
    return httpd_resp_set_type(req, type);
}

/* 静态文件处理 */
static esp_err_t rest_common_get_handler(httpd_req_t *req)
{
    char filepath[FILE_PATH_MAX];
    rest_server_context_t *rest_context = (rest_server_context_t *)req->user_ctx;
    strlcpy(filepath, rest_context->base_path, sizeof(filepath));
    if (req->uri[strlen(req->uri) - 1] == '/') {
        strlcat(filepath, "/index.html", sizeof(filepath));
    } else {
        strlcat(filepath, req->uri, sizeof(filepath));
    }
    int fd = open(filepath, O_RDONLY, 0);
    if (fd == -1) {
        ESP_LOGE(REST_TAG, "Failed to open file : %s", filepath);
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to read existing file");
        return ESP_FAIL;
    }

    set_content_type_from_file(req, filepath);

    char *chunk = rest_context->scratch;
    ssize_t read_bytes;
    do {
        /* Read file in chunks into the scratch buffer */
        read_bytes = read(fd, chunk, SCRATCH_BUFSIZE);
        if (read_bytes == -1) {
            ESP_LOGE(REST_TAG, "Failed to read file : %s", filepath);
        } else if (read_bytes > 0) {
            /* Send the buffer contents as HTTP response chunk */
            if (httpd_resp_send_chunk(req, chunk, read_bytes) != ESP_OK) {
                close(fd);
                ESP_LOGE(REST_TAG, "File sending failed!");
                /* Abort sending file */
                httpd_resp_sendstr_chunk(req, NULL);
                /* Respond with 500 Internal Server Error */
                httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to send file");
                return ESP_FAIL;
            }
        }
    } while (read_bytes > 0);
    /* Close file after sending complete */
    close(fd);
    ESP_LOGI(REST_TAG, "File sending complete");
    /* Respond with an empty chunk to signal HTTP response completion */
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

/* 配置网络 */
static esp_err_t wifi_config_post_handler(httpd_req_t *req)
{
    ESP_LOGI(REST_TAG, "call wifi_config_post_handler");
    int total_len = req->content_len;
    int cur_len = 0;
    char *buf = ((rest_server_context_t *)(req->user_ctx))->scratch;
    int received = 0;
    if (total_len >= SCRATCH_BUFSIZE) {
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "content too long");
        return ESP_FAIL;
    }
    while (cur_len < total_len) {
        received = httpd_req_recv(req, buf + cur_len, total_len);
        if (received <= 0) {
            /* Respond with 500 Internal Server Error */
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to post control value");
            return ESP_FAIL;
        }
        cur_len += received;
    }
    buf[total_len] = '\0';

    cJSON *root = cJSON_Parse(buf);
    cJSON  *ssid = cJSON_GetObjectItem(root, "ssid");
    cJSON  *passwd = cJSON_GetObjectItem(root, "passwd");
    cJSON  *serial = cJSON_GetObjectItem(root, "serial");
    ESP_LOGI(REST_TAG, "wifi config:  ssid = %s, passwd = %s", ssid->valuestring, passwd->valuestring);

    nvs_set("wifi_ssid", ssid->valuestring);
    nvs_set("wifi_passwd", passwd->valuestring);
    nvs_set("serial", serial->valuestring);
    
    // 重新连接 wifi
    restart_wifi();

    cJSON_Delete(root);
    httpd_resp_sendstr(req, "Post successfully");
    return ESP_OK;
}

/* 获取网络配置 */
static esp_err_t wifi_info_get_handler(httpd_req_t *req)
{
    char wifi_ssid[32] = { 0 };     
    char wifi_passwd[64] = { 0 }; 
    char serial[64] = { 0 }; 

    char wifi_ip[32] = { 0 };     
    char wifi_gw[64] = { 0 }; 
    char wifi_netmask[64] = {0};
    char wifi_sta_mac[32] = {0};

    nvs_get("wifi_ssid", wifi_ssid, sizeof(wifi_ssid));
    nvs_get("wifi_passwd", wifi_passwd, sizeof(wifi_passwd));
    nvs_get("serial", serial, sizeof(serial));

    nvs_get("wifi_sta_ip", wifi_ip, sizeof(wifi_passwd));
    nvs_get("wifi_sta_gw", wifi_gw, sizeof(wifi_passwd));
    nvs_get("wifi_sta_mask", wifi_netmask, sizeof(wifi_passwd));
    nvs_get("wifi_sta_mac", wifi_sta_mac, sizeof(wifi_sta_mac));

    httpd_resp_set_type(req, "application/json");
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "ssid", wifi_ssid);
    cJSON_AddStringToObject(root, "passwd", wifi_passwd);
    cJSON_AddStringToObject(root, "serial", serial);
    cJSON_AddStringToObject(root, "ip", wifi_ip);
    cJSON_AddStringToObject(root, "gw", wifi_gw);
    cJSON_AddStringToObject(root, "netmask", wifi_netmask);
    cJSON_AddStringToObject(root, "mac", wifi_sta_mac);

    
    const char *sys_info = cJSON_Print(root);
    httpd_resp_sendstr(req, sys_info);
    free((void *)sys_info);
    cJSON_Delete(root);
    return ESP_OK;
}


/*开启 http server*/
esp_err_t start_rest_server(const char *base_path)
{
    REST_CHECK(base_path, "wrong base path", err);
    rest_server_context_t *rest_context = calloc(1, sizeof(rest_server_context_t));
    REST_CHECK(rest_context, "No memory for rest context", err);
    strlcpy(rest_context->base_path, base_path, sizeof(rest_context->base_path));

    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;

    ESP_LOGI(REST_TAG, "Starting HTTP Server");
    REST_CHECK(httpd_start(&server, &config) == ESP_OK, "Start server failed", err_start);

    /* 获取网络信息 */
    httpd_uri_t wifi_info_get_uri = {
        .uri = "/api/wifi/info",
        .method = HTTP_GET,
        .handler = wifi_info_get_handler,
        .user_ctx = rest_context
    };
    httpd_register_uri_handler(server, &wifi_info_get_uri);

    /* 配置Wi-Fi连接 */
    httpd_uri_t wifi_config_post_uri = {
        .uri = "/api/wifi/config",
        .method = HTTP_POST,
        .handler = wifi_config_post_handler,
        .user_ctx = rest_context
    };
    httpd_register_uri_handler(server, &wifi_config_post_uri);


    /* URI handler for getting web server files */
    httpd_uri_t common_get_uri = {
        .uri = "/*",
        .method = HTTP_GET,
        .handler = rest_common_get_handler,
        .user_ctx = rest_context
    };
    httpd_register_uri_handler(server, &common_get_uri);

    return ESP_OK;
err_start:
    free(rest_context);
err:
    return ESP_FAIL;
}
