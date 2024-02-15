/*
 * bg95_usart.h
 *
 *  Created on: 25-Jan-2021
 *      Author: Rohan Dey
 */

#ifndef COMPONENT_BG95_BG95_USART_H_
#define COMPONENT_BG95_BG95_USART_H_

#ifdef __cplusplus
extern "C" {
#endif

#define BG95_CMD_RESP_LEN	1024

int bg95_usart_transmit(const char *data, const int len);
void bg95_usart_init(void);

#ifdef __cplusplus
}
#endif

#endif /* COMPONENT_BG95_BG95_USART_H_ */
