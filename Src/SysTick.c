/*
 * SysTick.c
 *
 *  Created on: Dec 21, 2023
 *      Author: M
 */


#include "SysTick.h"

volatile static uint32_t tickCounter ;
void SysTick_Init(void)
{
	SysTick_Config(SystemCoreClock/1000) ;

}

void Delay_ms(uint32_t time)
{
	uint32_t StartTick = Get_SystickCounter () ;
	while ((Get_SystickCounter () - StartTick) < time ) ;

}

uint32_t Get_SystickCounter (void)
{
	return tickCounter ;
}

void inc_SysTickCounter (void)
{
	++ tickCounter ;
}


