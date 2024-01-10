/*
 * GPIO.c
 *
 *  Created on: Dec 11, 2023
 *      Author: M
 */

#include "stm32f429xx.h"
#include "GPIO.h"


void GPIO_Init_Led(Led led)
{

	switch(led)
	{
	case Green_led:
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN  ;
		GPIOG->MODER |= GPIO_MODER_MODER13_0 ;
		GPIOG->OSPEEDR |= (GPIO_OSPEEDR_OSPEED13_0 | GPIO_OSPEEDR_OSPEED13_1);
		RCC->APB2ENR |=  RCC_APB2ENR_SYSCFGEN ;
		SYSCFG->CMPCR |= SYSCFG_CMPCR_CMP_PD  ;

		while ((SYSCFG->CMPCR & SYSCFG_CMPCR_READY ) !=SYSCFG_CMPCR_READY ) ;
		GPIOG->PUPDR |= GPIO_PUPDR_PUPD13  ;
		break ;

	case Red_led:
			RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN  ;
			GPIOG->MODER |= GPIO_MODER_MODER14_0 ;
			GPIOG->OSPEEDR |= (GPIO_OSPEEDR_OSPEED14_0 | GPIO_OSPEEDR_OSPEED14_1);
			RCC->APB2ENR |=  RCC_APB2ENR_SYSCFGEN ;
			SYSCFG->CMPCR |= SYSCFG_CMPCR_CMP_PD  ;

			while ((SYSCFG->CMPCR & SYSCFG_CMPCR_READY ) !=SYSCFG_CMPCR_READY ) ;
			GPIOG->PUPDR |= GPIO_PUPDR_PUPD14  ;
			break ;



	default :
		break ;

	}

}


void GPIO_TurnON_Led (Led led)
{

	switch(led)
		{
		case Green_led:
			GPIOG->BSRR |= GPIO_BSRR_BS13;
			break ;

		case Red_led:
			GPIOG->BSRR |= GPIO_BSRR_BS14;
			break ;


		default :
			break ;

		}



}

void GPIO_TurnOFF_Led (Led led)
{

	switch(led)
		{
		case Green_led:
			GPIOG->BSRR |= GPIO_BSRR_BR13;
			break ;

		case Red_led:
			GPIOG->BSRR |= GPIO_BSRR_BR14;
			break ;


		default :
			break ;

		}



}




