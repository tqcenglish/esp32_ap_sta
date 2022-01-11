#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"


#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "sdkconfig.h"
#include "driver/gpio.h"

#include "esp_vfs_semihost.h"
#include "esp_vfs_fat.h"
#include "esp_spiffs.h"
#include "sdmmc_cmd.h"
#include "esp_netif.h"
#include "lwip/apps/netbiosns.h"

#include "esp_tls.h"
#include "esp_http_client.h"
#include "driver/touch_pad.h"

#define MAX_HTTP_RECV_BUFFER 512
#define MAX_HTTP_OUTPUT_BUFFER 2048
#define G_LED    12     //G_LED
#define R_LED    13     //R_LED
#define ZY_LED   14     //ZY_LED
#define B_LED    15     //B_LED
#define Motor    5      //MOTER
#define SPK_SW   19     //SPK_SW
#define Teleing  23     //Teleing
#define Oxgen_chk  21//输氧检测
#define Call_reqs  22//对讲请求


bool s_pad_activated[2];
static int show_message;
static const char *TAG = "task";
extern xQueueHandle gpio_evt_queue;

esp_err_t _http_event_handler(esp_http_client_event_t *evt);
void nvs_set(char *key, char* value);


static void http_rest_with_url(void)
{
    char local_response_buffer[MAX_HTTP_OUTPUT_BUFFER] = {0};

    esp_http_client_config_t config = {
        .host = "httpbin.org",
        .path = "/get",
        .query = "esp",
        .event_handler = _http_event_handler,
        .user_data = local_response_buffer,        // Pass address of local buffer to get response
        .disable_auto_redirect = true,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    // POST body = '{"mac": "14","key1": "off","key2": "on", "key3": "off","key4": "off" }'
    //const char *post_data = "{\"field1\":\"value1\"}";
    const char *post_data = "{\"mac\":\"14\",\"key1\": \"off\",\"key2\": \"on\", \"key3\": \"off\",\"key4\": \"off\" }";
    
    esp_http_client_set_url(client, "http://121.4.116.243/api/open/callbox");
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, post_data, strlen(post_data));
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        // ESP_LOGI(TAG, "HTTP POST Status = %I64d, content_length = %I64d",
        //         esp_http_client_get_status_code(client),
        //         esp_http_client_get_content_length(client));
    } else {
        ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
    }
    esp_http_client_cleanup(client);
}


esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    static char *output_buffer;  // Buffer to store response of http request from event handler
    static int output_len;       // Stores number of bytes read
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            /*
             *  Check for chunked encoding is added as the URL for chunked encoding used in this example returns binary data.
             *  However, event handler can also be used in case chunked encoding is used.
             */
            if (!esp_http_client_is_chunked_response(evt->client)) {
                // If user_data buffer is configured, copy the response into the buffer
                if (evt->user_data) {
                    memcpy(evt->user_data + output_len, evt->data, evt->data_len);
                } else {
                    if (output_buffer == NULL) {
                        output_buffer = (char *) malloc(esp_http_client_get_content_length(evt->client));
                        output_len = 0;
                        if (output_buffer == NULL) {
                            ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
                            return ESP_FAIL;
                        }
                    }
                    memcpy(output_buffer + output_len, evt->data, evt->data_len);
                }
                output_len += evt->data_len;
            }

            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
            if (output_buffer != NULL) {
                // Response is accumulated in output_buffer. Uncomment the below line to print the accumulated response
                // ESP_LOG_BUFFER_HEX(TAG, output_buffer, output_len);
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            int mbedtls_err = 0;
            esp_err_t err = esp_tls_get_and_clear_last_error(evt->data, &mbedtls_err, NULL);
            if (err != 0) {
                if (output_buffer != NULL) {
                    free(output_buffer);
                    output_buffer = NULL;
                }
                output_len = 0;
                ESP_LOGI(TAG, "Last esp error code: 0x%x", err);
                ESP_LOGI(TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
            }
            break;
    }
    return ESP_OK;
}
static void gpio_rw_task(void * parm)//指示灯，震动马达，输氧检测，对讲检测
{
    uint32_t io_num;
    uint16_t chang_flag;
    for(;;) {
        if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            if(io_num==21)
            {
                if(gpio_get_level(21))
                {
                    gpio_set_level(B_LED, 0);//B_LED
                }
                else
                {
                    gpio_set_level(B_LED, 1);//输氧检测
                }
            }
            if(io_num==22)
            {
                if(!gpio_get_level(22))
                {
                    gpio_set_level(B_LED, 1);//对讲请求
                    //gpio_set_level(GPIO_OUTPUT_IO_4, 0);//切换音频功放---->有线通话
                    //gpio_set_level(GPIO_OUTPUT_IO_5, 0);//接通电话线路
                }    
                else
                {
                    gpio_set_level(B_LED, 0);//G_LED
                    //gpio_set_level(GPIO_OUTPUT_IO_4, 1);//切换音频功放---->ESP32
                    //gpio_set_level(GPIO_OUTPUT_IO_5, 1);//断开电话线路
                }
            }
        }
        //vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}


void touchpad_read_task(void * parm)//增援、呼叫触摸检测
{
    int change_mode = 0;
    int filter_mode = 0;
    int cfg_count=0;
    touch_pad_intr_enable();
    while (1) {
        
        if (s_pad_activated[0] == true) {
            gpio_set_level(G_LED, 1);//G_LED呼叫请求
            gpio_set_level(Motor, 1);//Motor
            vTaskDelay(200 / portTICK_PERIOD_MS);
            s_pad_activated[0] = false;
            show_message = 1;
            cfg_count++;
        }
        else
        {
            cfg_count=0;
        }
        ESP_LOGI(TAG, "cfg_count=%d",cfg_count);

        if(cfg_count>=1000)//WiFi连接失败后，长按呼叫按钮10秒后进入配置模式
        {
            // 清空 nvs
            nvs_set("wifi_sta_ip", "" );
            nvs_set("wifi_sta_gw", "");
            nvs_set("wifi_sta_mask", "");
        }

        if (s_pad_activated[2] == true) {
            gpio_set_level(ZY_LED, 1);//增援请求
            gpio_set_level(Motor, 1);
            vTaskDelay(200 / portTICK_PERIOD_MS);
            s_pad_activated[2] = false;
            show_message = 1;
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
        if(show_message!=0)
        {
            if (show_message++ % 2 == 0) {
            gpio_set_level(G_LED, 0);
            gpio_set_level(Motor, 0);
            gpio_set_level(ZY_LED, 0);
            }
        }
        
    }
}

static void http_help_task(void * parm)//增援请求
{
    while(1)
    {
        vTaskDelay(60000 / portTICK_PERIOD_MS);
        //http_rest_with_url();
    }
}
static void http_inter_task(void * parm)//对讲请求
{
    while(1)
    {
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
static void http_call_task(void * parm)//呼叫请求
{
    while(1)
    {
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
static void http_heartbeat_task(void * parm)//心跳包
{
    while(1)
    {
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

