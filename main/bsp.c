

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

static const char *TAG = "bsp";
static uint32_t s_pad_init_val[2];
extern bool s_pad_activated[2];

#define G_LED    12     //G_LED
#define R_LED    13     //R_LED
#define ZY_LED   14     //ZY_LED
#define B_LED    15     //B_LED
#define Motor    5      //MOTER
#define SPK_SW   19     //SPK_SW
#define Teleing  23     //Teleing
#define Oxgen_chk  21//输氧检测
#define Call_reqs  22//对讲请求


#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<G_LED) | (1ULL<<R_LED)| (1ULL<<ZY_LED)| (1ULL<<B_LED)| (1ULL<<Motor)| (1ULL<<SPK_SW)| (1ULL<<Teleing))
#define GPIO_INPUT_PIN_SEL  ((1ULL<<Oxgen_chk) | (1ULL<<Call_reqs))
#define ESP_INTR_FLAG_DEFAULT 0
#define TOUCH_THRESH_NO_USE   (0)
#define TOUCH_THRESH_PERCENT  (80)
#define TOUCHPAD_FILTER_TOUCH_PERIOD (10)



// static void smartconfig_init_start(void);//智能配网--系统启动后，载入上一次的WIFI信息，不能联网后再启动智能配网
// static void gpio_rw_task(void * parm);//指示灯，震动马达，输氧检测，对讲检测
// static void IRAM_ATTR gpio_isr_handler(void* arg);
void touchpad_read_task(void * parm);//增援、呼叫触摸检测
// static void http_help_task(void * parm);//增援请求
// static void http_inter_task(void * parm);//对讲请求
// static void http_call_task(void * parm);//呼叫请求
// static void http_heartbeat_task(void * parm);//心跳包

void touchpad_rtc_intr(void *arg);//触摸中断状态
xQueueHandle gpio_evt_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

void touchpad_rtc_intr(void *arg)//触摸中断状态
{
    uint32_t pad_intr = touch_pad_get_status();
    //clear interrupt
    touch_pad_clear_status();
    if (pad_intr & 0x01) 
    {
        s_pad_activated[0] = true;
    }
    if (pad_intr & 0x04) 
    {
        s_pad_activated[2] = true;
    }
}

void BSP_init(void){
    uint16_t touch_value;
    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 1;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    //interrupt of rising edge
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    //change gpio intrrupt type for one pin
    gpio_set_intr_type(Oxgen_chk, GPIO_INTR_ANYEDGE);

    //create a queue to handle gpio event from isr
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    
    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(Oxgen_chk, gpio_isr_handler, (void*) Oxgen_chk);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(Call_reqs, gpio_isr_handler, (void*) Call_reqs);

    ///////////////////////////
    ESP_LOGI(TAG, "Initializing touch pad");
    ESP_ERROR_CHECK(touch_pad_init());
    touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);
    // Set reference voltage for charging/discharging
    // For most usage scenarios, we recommend using the following combination:
    // the high reference valtage will be 2.7V - 1V = 1.7V, The low reference voltage will be 0.5V.
    touch_pad_set_voltage(TOUCH_HVOLT_MAX, TOUCH_LVOLT_MAX, TOUCH_HVOLT_ATTEN_0V );
    
    touch_pad_config(0, TOUCH_THRESH_NO_USE);
    touch_pad_config(2, TOUCH_THRESH_NO_USE);

    touch_pad_filter_start(TOUCHPAD_FILTER_TOUCH_PERIOD);
    while(1)
    {
        touch_pad_read_filtered(0, &touch_value);
        s_pad_init_val[0] = touch_value;
        ESP_LOGI(TAG, "touch init: touch pad [%d] val is %d", 0, touch_value);
        //set interrupt threshold.
        ESP_ERROR_CHECK(touch_pad_set_thresh(0, touch_value-8));
    
        touch_pad_read_filtered(2, &touch_value);
        s_pad_init_val[1] = touch_value;
        ESP_LOGI(TAG, "touch init: touch pad [%d] val is %d", 2, touch_value);
    }
    
    //set interrupt threshold.
    ESP_ERROR_CHECK(touch_pad_set_thresh(2, touch_value-8));

    touch_pad_isr_register(touchpad_rtc_intr, NULL);

}
