/*
 * GPIO.h
 *
 *  Created on: Dec 11, 2023
 *      Author: M
 */

#ifndef INC_GPIO_H_
#define INC_GPIO_H_

typedef enum
{
	Green_led ,
	Red_led
}Led;


void GPIO_Init_Led(Led led)  ;
void GPIO_TurnON_Led (Led led) ;
void GPIO_TurnOFF_Led (Led led) ;
void GPIO_Init_Button(void) ;

#endif /* INC_GPIO_H_ */
