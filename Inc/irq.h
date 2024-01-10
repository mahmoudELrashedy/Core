/*
 * irq.h
 *
 *  Created on: Dec 20, 2023
 *      Author: M
 */

#ifndef INC_IRQ_H_
#define INC_IRQ_H_
#include "stm32f429xx.h"
typedef enum My_IRQ_e {
	green_ledON = EXTI0_IRQn ,
	green_ledOFF = EXTI1_IRQn,
	red_ledON = EXTI2_IRQn ,
	red_ledOFF = EXTI3_IRQn
} My_IRQ;

void IRQ_Init (void) ;

#endif /* INC_IRQ_H_ */
