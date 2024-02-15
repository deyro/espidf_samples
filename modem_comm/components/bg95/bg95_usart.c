/*
 * bg95_usart.c
 *
 *  Created on: 25-Jan-2021
 *      Author: Rohan Dey
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"

static const char *TAG = "BG95_USART";

static const int RX_BUF_SIZE = 1024;

#define TXD_PIN (GPIO_NUM_27)
#define RXD_PIN (GPIO_NUM_32)

static void bg95_usart_driver_init(void) {
	const uart_config_t uart_config = {
			.baud_rate = 115200,
			.data_bits = UART_DATA_8_BITS,
			.parity = UART_PARITY_DISABLE,
			.stop_bits = UART_STOP_BITS_1,
			.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
			.source_clk = UART_SCLK_APB,
	};

	// We won't use a buffer for sending data.
	uart_driver_install(UART_NUM_1, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
	uart_param_config(UART_NUM_1, &uart_config);
	uart_set_pin(UART_NUM_1, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

int bg95_usart_transmit(const char *data, const int len) {
//	const int len = strlen(data);
	ESP_LOG_BUFFER_HEXDUMP(TAG, data, len, ESP_LOG_INFO);
	int txBytes = uart_write_bytes(UART_NUM_1, data, len);
	ESP_LOGI(TAG, "Wrote %d bytes", txBytes);

	char end[2] = "\r\n";
	txBytes = uart_write_bytes(UART_NUM_1, end, sizeof(end));
	ESP_LOG_BUFFER_HEXDUMP(TAG, end, sizeof(end), ESP_LOG_INFO);
	ESP_LOGI(TAG, "Wrote %d bytes", txBytes);
	return txBytes;
}

#if 0
static void tx_task(void *arg)
{
    static const char *TX_TASK_TAG = "TX_TASK";
    esp_log_level_set(TX_TASK_TAG, ESP_LOG_INFO);
    while (1) {
//        bg95_usart_transmit(TX_TASK_TAG, "Hello world");
        bg95_usart_transmit(TX_TASK_TAG, "version\n");
//        bg95_usart_transmit(TX_TASK_TAG, "demo hexdump\n");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}
#endif

static void rx_task(void *arg) {
	uint8_t *data = (uint8_t*) malloc(RX_BUF_SIZE + 1);
	ESP_LOGW(TAG, "rx_task started");
	while (1) {
		const int rxBytes = uart_read_bytes(UART_NUM_1, data, RX_BUF_SIZE, 1000 / portTICK_RATE_MS);
		if (rxBytes > 0) {
			data[rxBytes] = 0;
			ESP_LOGI(TAG, "Read %d bytes: '%s'", rxBytes, data);
			ESP_LOG_BUFFER_HEXDUMP(TAG, data, rxBytes, ESP_LOG_INFO);
		}
	}
	free(data);
}

int bg95_usart_init(void) {
	esp_log_level_set(TAG, ESP_LOG_INFO);
	bg95_usart_driver_init();

	xTaskCreate(rx_task, "uart_rx_task", 1024 * 4, NULL, configMAX_PRIORITIES, NULL);
#if 0
    xTaskCreate(tx_task, "uart_tx_task", 1024*2, NULL, configMAX_PRIORITIES-1, NULL);
#endif

    return 0;
}
