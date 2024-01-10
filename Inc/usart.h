/*
 * usart.h
 *
 *  Created on: Jan 4, 2024
 *      Author: M
 */

#ifndef INC_USART_H_
#define INC_USART_H_


void USART1_GPIO_Config (void);
void USART_Init (void);
void USART1_Enable (void);
void USART_IRQ_CallBack (void);
void USART1_Process(void);

#endif /* INC_USART_H_ */
