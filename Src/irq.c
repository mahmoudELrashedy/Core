/*
 * irq.c
 *
 *  Created on: Dec 20, 2023
 *      Author: M
 */

#include "irq.h"

void IRQ_Init (void)
{
	/* Set priority group to 3
	   * bits[3:0] are the sub-priority,
	   * bits[7:4] are the pre-empt priority (0-15) */
	  NVIC_SetPriorityGrouping(3);

	  /* Set priority levels */
	  NVIC_SetPriority(SysTick_IRQn, 0);
	  NVIC_SetPriority(EXTI0_IRQn, 1);
	  NVIC_SetPriority(USART1_IRQn, 1);

	  /* Enable interrupts at NVIC */
	  NVIC_EnableIRQ(EXTI0_IRQn);
	  NVIC_EnableIRQ(DMA2_Stream0_IRQn);
	  NVIC_EnableIRQ(USART1_IRQn);




}
