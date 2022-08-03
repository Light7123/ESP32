#include "main.h"

xSemaphoreHandle PushDatatoClient; //отвечает за начало передачи данных клиенту 

void app_main(void)
{
  ESP_ERROR_CHECK(nvs_flash_init());
  //printf("Create Semaphore");
  PushDatatoClient = xSemaphoreCreateBinary();
  //check_efuse();
  //adcInit();

 /*
  if(!MPU6050_init()) 
  {
    ESP_LOGE("mpu6050", "init failed!");
      vTaskDelete(0);
  }
	ESP_LOGI("mpu6050", "init success!");
*/
  //ADC_init();


  // ESP_ERROR_CHECK(esp_netif_init());
  // ESP_ERROR_CHECK(esp_event_loop_create_default());

  /* Register event handlers to stop the server when Wi-Fi or Ethernet is disconnected,
     * and re-start it upon connection.
     */
  // ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, &server));
  // ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server));

  /* Start the server for the first time */
  
  
  ////wifi_init_sta();
  wifi_init_softap();
  ws_server_start();
  //printf("DATA=%s\n",cert_sfh_start);
  
  printf("[DEBUG] Starting web server...\n");
  xTaskCreate(&server_task, "server_task", 4096, NULL, 9, NULL);
  xTaskCreate(&server_handle_task, "server_handle_task", 4096, NULL, 6, NULL);
  //xTaskCreate(&count_task, "count_task", 12000, NULL, 2, NULL);
  xTaskCreate(&transmitUART_task, "transmitUART_task", 4096, NULL, 2, NULL);
  (void)start_webserver();
  

}