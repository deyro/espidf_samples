/*
 * modem_cmds.c
 *
 *  Created on: 25-Jan-2021
 *      Author: Rohan Dey
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "esp_log.h"
#include "esp_console.h"
//#include "driver/uart.h"
#include "argtable3/argtable3.h"
//#include "freertos/FreeRTOS.h"
//#include "freertos/task.h"
//#include "sdkconfig.h"

#include "modem_cmds.h"
#include "bg95_at.h"
#include "bg95_usart.h"

static const char *TAG = "modem_cmd";

static bool modeminit_stat = false;

/* send command */
static struct {
    struct arg_str *at_cmd;
    struct arg_end *end;
} send_args;

static int send_cmd(int argc, char **argv)
{
	if (!modeminit_stat) {
		ESP_LOGE(TAG, "Initialize the modem communication port first");
		return -1;
	}

    int nerrors = arg_parse(argc, argv, (void **) &send_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, send_args.end, argv[0]);
        return 1;
    }

    const char *at_cmd = send_args.at_cmd->sval[0];
    ESP_LOGI(TAG, "Sending AT CMD: <%s>", at_cmd);

    int len = strlen(at_cmd);
    int txbytes = bg95_usart_transmit(at_cmd, len);
//    if (txbytes != len) {
//    	ESP_LOGE(TAG, "Error while sending command!");
//    	return -1;
//    }

    return 0;
}

static void register_send(void)
{
	send_args.at_cmd = arg_str1(NULL, NULL, "<at_cmd>", "at command to send");
	send_args.end = arg_end(1);

    const esp_console_cmd_t cmd = {
        .command = "send",
        .help = "Send AT command and receive response",
        .hint = NULL,
        .func = &send_cmd,
		.argtable = &send_args
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

/* modem init command */
static int modeminit(int argc, char **argv)
{
	bg95_usart_init();
    ESP_LOGI(TAG, "modem init done");
    modeminit_stat = true;

    return 0;
}

static void register_modeminit(void)
{
    const esp_console_cmd_t cmd = {
        .command = "modeminit",
        .help = "Initialize the modem communication port",
        .hint = NULL,
        .func = &modeminit,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

/* register all commands */
void register_modem_cmds() {
	register_send();
	register_modeminit();
}
