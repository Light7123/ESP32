//Чтобы АЦП заработало нужно понизить частоту, выставить mode=1, и подать 3.3 в на ножку ресет
#include "UART.h"
#include "main.h"
#include "string.h"
#include "app_wifi.h"



#define  time_graph  6000 // количество данных в массиве ( с учетом того, что опрос каждые 10 ms, массив даст на выходе 60 с)
extern xSemaphoreHandle PushDatatoClient; //отвечает за начало передачи данных клиенту 




float acc_reading;
float acc_array[time_graph]; //массив данных акселерометра
uint32_t adc_array[time_graph]; // массив данных ацп
uint16_t adc_count=0;
int adc_reading;
static uint8_t readFlag = 1;
int wifi_state = 0;



typedef struct Node
{
	char value[12];
	struct Node *next;
} Node;

Node *data_from_stm32 = NULL;

struct Node* newNode(char data[12])
{
    struct Node* node = (struct Node*)malloc(sizeof(struct Node));
    memcpy(node->value, data, 12);
    node->next = NULL;
    return node;
}

// Function to add a new node at the tail end of the list instead of its head
void appendNode(struct Node** head, char data[12])
{
    struct Node* current = *head;
    struct Node* node = newNode(data);
 
    // special case for length 0
    if (current == NULL) {
        *head = node;
    }
    else {
        // locate the last node
        while (current->next != NULL) {
            current = current->next;
        }
 
        current->next = node;
    }
}

void printList(struct Node* head)
{
    struct Node* ptr = head;
	uart_write_bytes(UART_NUM_2, (const char *)"DATA_RECEIVED_BY_UART_START\r\n", 29);
    while (ptr)
    {
		uart_write_bytes(UART_NUM_2, (const char *)ptr->value, 12);
		uart_write_bytes(UART_NUM_2, (const char *)"\r\n", 2);
        ptr = ptr->next;
    }
	uart_write_bytes(UART_NUM_2, (const char *)"DATA_RECEIVED_BY_UART_END!!\r\n", 29);
}


void USART1_init(void)
{
	uart_config_t uart_config = {
		.baud_rate = 115200,
		.data_bits = UART_DATA_8_BITS,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
		.source_clk = UART_SCLK_APB,
	};

	uart_driver_install(UART_NUM_1, BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, UART1_TXD, UART1_RXD, UART1_RTS, UART1_CTS);
}

void USART2_init(void)
{
        uart_config_t uart_config2 = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    uart_driver_install(UART_NUM_2, BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_2, &uart_config2);
    uart_set_pin(UART_NUM_2, 17, 16, UART2_RTS, UART2_CTS);

}

void transmitUART_task(void *pvParameters)
{	
	USART1_init();
	USART2_init();
	// Configure a temporary buffer for the incoming data
	char data[12];

	while (1)
	{
		// Read data from the UART
		int len = uart_read_bytes(UART_NUM_1, data, 12, 200);
		if(data[1]=='*')
		{
			if(!wifi_state)
			{
				ESP_ERROR_CHECK(esp_wifi_start());
				wifi_state = 1;
				uart_write_bytes(UART_NUM_2, (const char *)"WiFi-ON\r\n", 9);
			}
			else		
			{
				ESP_ERROR_CHECK(esp_wifi_stop());
				wifi_state = 0;
				uart_write_bytes(UART_NUM_2, (const char *)"WiFi-OFF\r\n", 10);
			}
			data[1] = '0';
		}

		if (readFlag && len == 12 && data[0]!='#' && data[0]!='*')
		{
			appendNode(&data_from_stm32, data);
			//uart_write_bytes(UART_NUM_2, (const char *)data_from_stm32->value, 12);
			//uart_write_bytes(UART_NUM_2, (const char *)"\r\n", 2);
			//data[0]='#';
		}

		if(data[0]=='#')
		{
			
			//Вызываем функцию для просмотра всего массива данных принятого по UART
			printList(data_from_stm32);

			uart_write_bytes(UART_NUM_2, (const char *)"count_task1\r\n", 13);
			//printf("count_task1\r\n");
			xTaskCreate(&count_task, "count_task", 4096, NULL, 2, NULL); //создаем задачу для отправки данных
			xSemaphoreGive(PushDatatoClient);
			readFlag = 0;
			data[0]='0';
			//printf("count_task2\r\n");
			uart_write_bytes(UART_NUM_2, (const char *)"count_task2\r\n", 13);

		}

		if(data[0]=='%')
		{	
			uart_write_bytes(UART_NUM_2, (const char *)"@RESTART\r\n", 10);
			esp_restart();

		}
		if(data[0]=='&')
		{
			vTaskDelete(server_task);
			vTaskDelete(server_handle_task);
			vTaskDelete(transmitUART_task);
			ESP_ERROR_CHECK(esp_wifi_stop());
				wifi_state = 0;
				uart_write_bytes(UART_NUM_2, (const char *)"WiFi-OFF\r\n", 10);
			xTaskCreate(&app_ota,"app_ota" ,4096, NULL, 2, NULL);
		}
		vTaskDelay(pdMS_TO_TICKS(10));
	}
}

/* FreeRTOS user task */
void count_task(void *pvParameters)
{
	uart_write_bytes(UART_NUM_2, (const char *)"count_task3\r\n", 13);
	//printf("count_task3\r\n");
	const static char *TAG = "count_task";
	const int DELAY = 500 / portTICK_PERIOD_MS; // 500 ms
	ESP_LOGI(TAG, "starting task");
	Node *webGraphData = data_from_stm32;

	while(1)
	{
		xSemaphoreTake( PushDatatoClient, portMAX_DELAY); // ожидаем пока задача не создастся или пока кто-то не подключится
		uart_write_bytes(UART_NUM_2, (const char *)"PUSHING_DATA_TO_CLIENT\r\n", 24);
		while (webGraphData != NULL)
		{
			// Вывод значения узла
			ws_server_send_text_all(webGraphData->value, 12);
			// Сдвиг указателя к следующему узлу
			uart_write_bytes(UART_NUM_2, (const char *)webGraphData->value, 12);
			uart_write_bytes(UART_NUM_2, (const char *)"\r\n", 2);
			webGraphData = webGraphData->next;
		} 
	
		if(webGraphData == NULL)
		{
			webGraphData=data_from_stm32;
			ws_server_send_text_all("\n",1);
			uart_write_bytes(UART_NUM_2, (const char *)"DATA_TO_CLIENT_SENDED!\r\n", 24);
		}
		vTaskDelay(DELAY);
	}

//   const static char *TAG = "count_task";
//   char out[50];
//   int len;
//   const static char *word = "%i %f %i";
//   const int DELAY = 500 / portTICK_PERIOD_MS; // 500 ms
//   ESP_LOGI(TAG, "starting task");
//   for (;;)
//   {
// 	xSemaphoreTake( PushDatatoClient, portMAX_DELAY); // ожидаем пока задача не создастся или пока кто-то не подключится
// 	for(uint16_t i=0;i<time_graph;i++)
// 	{
// 		len = sprintf(out, word, adc_array[i], acc_array[i],i );
// 		ws_server_send_text_all(out, len);
// 	}
//     vTaskDelay(DELAY);
//   }
}

