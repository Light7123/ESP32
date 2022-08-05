#ifndef MAIN_H_
#define MAIN_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sys/param.h"
#include "esp_wifi.h"
#include "esp_event.h"
//#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_netif.h"
#include "esp_eth.h"
#include "esp_http_server.h"
#include "nvs_flash.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/api.h"

#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

/* App includes */
#include "websocket_server.h"
#include "app_config.h"
#include "app_websocket.h"
#include "app_wifi.h"
#include "app_http.h"
#include "UART.h"
#include "ota_update.h"



#endif /* MAIN_H_ */
