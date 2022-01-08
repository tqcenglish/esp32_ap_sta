#include "nvs_flash.h"

static char* nvs_get(char *key){
	/* 定义一个NVS操作句柄 */
    nvs_handle wificfg_nvs_handler; 
	/* 定义一个数组用来存储*/
    char value[32] = { 0 };     
    size_t len;
    /* 打开一个NVS命名空间 */
    ESP_ERROR_CHECK( nvs_open("WiFi_cfg", NVS_READWRITE, &wificfg_nvs_handler) );
    len = sizeof(value);    /* 从NVS中获取ssid */
    ESP_ERROR_CHECK( nvs_get_str(wificfg_nvs_handler,key,value,&len) );
    ESP_ERROR_CHECK( nvs_commit(wificfg_nvs_handler) ); /* 提交 */
    nvs_close(wificfg_nvs_handler);                     /* 关闭 */
	return &value
}