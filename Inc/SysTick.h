/*
 * SysTick.h
 *
 *  Created on: Dec 21, 2023
 *      Author: M
 */
#include "stm32f4xx_it.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"

#ifndef INC_SYSTICK_H_
#define INC_SYSTICK_H_

void SysTick_Init(void) ;

void Delay_ms(uint32_t time) ;

void inc_SysTickCounter (void) ;

uint32_t Get_SystickCounter (void) ;


#endif /* INC_SYSTICK_H_ */
