/*
 * main.c
 *
 *  Created on: Dec 11, 2023
 *      Author: M
 */

#include "stm32f4xx_it.h"
#include "GPIO.h"
#include "irq.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#include "SysTick.h"
#include "usart.h"




int main (void)
{


	 SysTick_Init() ;
	 IRQ_Init () ;

	 GPIO_Init_Led(Green_led)  ;
	 GPIO_Init_Led(Red_led)  ;

	 USART1_GPIO_Config ();
	 USART_Init ();

	 __enable_irq() ;

	 USART1_Enable ();
	 while(1)
	  {
		 USART1_Process();
	  }

}


