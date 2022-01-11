#include "nvs_flash.h"
#include "esp_log.h"

void nvs_get(char *key, char* value, size_t len){
    // size_t len = sizeof(&value); 通过这个方式不能获取数组真实长度
    nvs_handle nvs_handler; 
    ESP_LOGI("nvs", "get nvs key = %s, buflength = %d ", key, len);
    ESP_ERROR_CHECK( nvs_open("WiFi_cfg", NVS_READWRITE, &nvs_handler) );
    // 可能不存在 key
    // ESP_ERROR_CHECK( nvs_get_str(nvs_handler,key,value,&len) );
    nvs_get_str(nvs_handler,key,value,&len);
    ESP_ERROR_CHECK( nvs_commit(nvs_handler) ); /* 提交 */
    nvs_close(nvs_handler);                     /* 关闭 */
}

void nvs_set(char *key, char* value){
    nvs_handle nvs_handler; 
    ESP_LOGI("nvs", "set nvs key = %s, value = %s", key, value);
    ESP_ERROR_CHECK( nvs_open("WiFi_cfg", NVS_READWRITE, &nvs_handler) );
    ESP_ERROR_CHECK( nvs_set_str(nvs_handler,key,value) );
    ESP_ERROR_CHECK( nvs_commit(nvs_handler) ); /* 提交 */
    nvs_close(nvs_handler);                     /* 关闭 */
}