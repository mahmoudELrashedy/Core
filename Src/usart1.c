/*
 * usart1.c
 *
 *  Created on: Jan 3, 2024
 *      Author: M
 */

#include "stm32f4xx.h"
#include <stddef.h>
#include "GPIO.h"
#include "usart.h"

/**
 * @brief   Maximum USART reception buffer length
 */
#define MAX_BUFFER_LENGTH                     ((uint32_t) 200u)

/**
 * @brief   USART1 states definition
 */
typedef enum
{
  USART1_IDLE,
  USART1_WAIT_FOR_RESPONCE,
  USART1_ASK_FOR_NAME,
  USART1_WAIT_FOR_NAME,
  USART1_WAIT_FOR_COMMAND,
} USART1_StateType;

/**
 * @brief   USART1 IRQ status definition
 */

typedef enum
{
  USART1_NO_IRQ,
  USART1_CHAR_RECEIVED,
  USART1_PARITY_ERROR,
} USART1_IRQStatusType;

/**
 * @brief   Return type
 */
typedef enum
{
  STR_NOT_EQUAL,
  STR_EQUAL
} strCmpReturnType;


/**
 * @brief   USART1 messages to be transmitted
 */
static const char hello_world[]        = "Hello World!";
static const char ask_for_name[]       = "What is your name?";
static const char hi[]                 = "Hi,";
static const char ask_for_command[]    = "Please, send command";
static const char ask_for_command_ex[] = "Action[turn_on / turn_off] Led[green_led / red_led]";
static const char turn_on_green_led[]  = "turn_on green_led";
static const char turn_on_red_led[]    = "turn_on red_led";
static const char turn_off_green_led[] = "turn_off green_led";
static const char turn_off_red_led[]   = "turn_off red_led";
static const char done[]               = "Done";
static const char wrong_command[]      = "Wrong Command";
static const char parity_error[]       = "Parity Error";


/**
 * @defgroup usart1_private_variables
 * @{
 */

/**
 * @brief   USART1 current state
 */
static USART1_StateType currentState = USART1_IDLE;

/**
 * @brief   USART1 current IRQ status
 */
static USART1_IRQStatusType currentIRQStatus = USART1_NO_IRQ;

/**
 * @brief   USART1 last char recieved
 */
static char RxChar = 0;

/**
 * @brief   USART1 message buffer
 */
static char RxBuffer[MAX_BUFFER_LENGTH + 1];

/**
 * @brief   USART1 message length
 */
static uint8_t RxMessageLength = 0;



static strCmpReturnType StrCmp (const char* str1 , const char* str2 , uint8_t size)
{
	strCmpReturnType cmpStatus = STR_EQUAL ;

	if (str1 != NULL && str2!=NULL)
	{
		for (int idx = 0 ; idx<size ; idx++ )
		{
			if (str1[idx] != str2[idx])
				cmpStatus = STR_NOT_EQUAL ;
		}
	}

	return cmpStatus ;
}



static void strTransmit (const char* str , uint8_t size)
{
	if (str != NULL)
	{

		for (uint8_t idx = 0 ; idx<size ; idx++)
		{
			//wait for the transmission buffer to be empty
			while ((USART1->SR & USART_SR_TXE) != USART_SR_TXE) ;

			//send single char to the transmission buffer
			USART1->DR = str[idx] ;
		}


	}
}


static void strReceive (void)
{
	static char localBuffer [MAX_BUFFER_LENGTH] ;
	static uint32_t RxIndex = 0 ;

	//check for the last char
	if (RxChar == '\0')
	{
		if (RxIndex != 0)
		{
			//send the local buffer to the receiver buffer
			for (int idx = 0 ; idx <RxIndex ; idx++)
			{
				RxBuffer[idx] = localBuffer[idx] ;
			}

			//terminate the receiver buffer with NULL char
			RxBuffer[RxIndex] = '\0' ;
			RxMessageLength = RxIndex+1 ;
			RxIndex = 0 ;
		}
	}

	else
	{
		//check for overflow
		if (RxIndex > MAX_BUFFER_LENGTH)
		{
			RxIndex = 0 ;
		}

		// put the current char in the local buffer then increase the index to refer to the next one
		localBuffer[RxIndex] = RxChar ;
		RxIndex++ ;
	}
}

void USART1_GPIO_Config (void)
{
	// enable GPIOA clock in RCC Register
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN ;

	// select AF mode for pins 9 , 10
	GPIOA->MODER &= ~(GPIO_MODER_MODER9   | GPIO_MODER_MODER10  )  ;
	GPIOA->MODER |=  (GPIO_MODER_MODER9_1 | GPIO_MODER_MODER10_1)  ;

	//select output type for TX (PushPull)
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT_9 ;

	//select medium speed for TX
	GPIOA->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR9 ;
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR9_0 ;

	// select PULLUP for pin 9 , 10
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD9 | GPIO_PUPDR_PUPD10);
	GPIOA->PUPDR |= (GPIO_PUPDR_PUPD9_0 | GPIO_PUPDR_PUPD10_0) ;

	// select AF7 for pin 9 , 10
	GPIOA->AFR[1] &= ~(GPIO_AFRH_AFRH1   | GPIO_AFRH_AFRH2  ) ;
	GPIOA->AFR[1] |=  (GPIO_AFRH_AFRH1   | GPIO_AFRH_AFRH2  ) ;
	GPIOA->AFR[1] &= ~(GPIO_AFRH_AFRH1_3 | GPIO_AFRH_AFRH2_3) ;

}


void USART_Init (void)
{
	// enable usart1 in RCC register
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN ;

	//select OverSampling by 16
	USART1->CR1  &= ~USART_CR1_OVER8 ;

	//select OneBit mode
	USART1->CR3  |= USART_CR3_ONEBIT ;

	//select 1 start bit 9 data bit n stop bit
	USART1->CR1  |= USART_CR1_M ;

	//select 1 stop bit
	USART1->CR2  &= ~USART_CR2_STOP ;

	USART1->CR1  |= USART_CR1_PCE ;
	USART1->CR1  |= USART_CR1_PS ;

	//set Baud rate to 9600
	USART1 ->BRR = 0x683 ;


}

void USART1_Enable (void)
{
	//enable receiver
	USART1->CR1  |= USART_CR1_RE ;

	//enable transmitter
	USART1->CR1  |= USART_CR1_TE ;

	//enable usart
	USART1->CR1  |= USART_CR1_UE ;

	//receiver buffer not empty flag interrupt enable
	USART1->CR1  |= USART_CR1_RXNEIE ;

	//parity error interrupt enable
	USART1->CR1  |= USART_CR1_PEIE ;
}


void USART_IRQ_CallBack (void)
{
	//check for parity error
	if ((USART1->SR & USART_SR_PE) == USART_SR_PE)
	{
		//wait for RXNE flag to be set
		while ((USART1->SR & USART_SR_RXNE) != USART_SR_RXNE) ;

		//read data register to clear parity error bit
		USART1->DR ;

		currentIRQStatus = USART1_PARITY_ERROR ;
	}

	//check if the receiver buffer not empty
	if((USART1->SR & USART_SR_RXNE) == USART_SR_RXNE)
	{
		//read the data register
		RxChar = USART1->DR ;

		currentIRQStatus = USART1_CHAR_RECEIVED ;
	}
}


//void USART1_Process(void)
//{
//	switch (currentIRQStatus)
//	{
//	case USART1_PARITY_ERROR :
//		//print "parity error"
//		strTransmit(parity_error, sizeof(parity_error));
//
//		//reset to idle condition
//		currentState = USART1_IDLE ;
//
//		//reset IRQ
//		currentIRQStatus = USART1_NO_IRQ ;
//		break;
//
//	case USART1_CHAR_RECEIVED :
//
//		//receive the string char by char
//		strReceive() ;
//
//		//reset IRQ
//		currentIRQStatus = USART1_NO_IRQ ;
//		break ;
//
//	case USART1_NO_IRQ :
//		break ;
//
//	default :
//		break ;
//	}
//
//
//	switch (currentState)
//	{
//	case USART1_IDLE :
//		strTransmit(hello_world, sizeof(hello_world)) ;
//		currentState = USART1_WAIT_FOR_RESPONCE ;
//		break ;
//
//	case USART1_WAIT_FOR_RESPONCE :
//		if (RxMessageLength != 0)
//		{
//			RxMessageLength = 0 ;
//			currentState = USART1_ASK_FOR_NAME ;
//		}
//		break ;
//
//	case USART1_ASK_FOR_NAME :
//		strTransmit(ask_for_name, sizeof(ask_for_name)) ;
//		currentState = USART1_WAIT_FOR_NAME ;
//		break ;
//
//	case USART1_WAIT_FOR_NAME :
//		if(RxMessageLength != 0)
//		{
//			strTransmit(hi, sizeof(hi)) ;
//			strTransmit(RxBuffer, RxMessageLength) ;
//			strTransmit(ask_for_command, sizeof(ask_for_command)) ;
//			strTransmit(ask_for_command_ex, sizeof(ask_for_command_ex)) ;
//
//			RxMessageLength = 0 ;
//
//			currentState = USART1_WAIT_FOR_COMMAND ;
//		}
//		break ;
//
//	case USART1_WAIT_FOR_COMMAND:
//	      /* Check if new message received */
//	      if(0 != RxMessageLength)
//	      {
//	        /* Reset message length */
//	        RxMessageLength = 0;
//
//	        /* String compare results */
//	        strCmpReturnType isMatch_01 = STR_NOT_EQUAL;
//	        strCmpReturnType isMatch_02 = STR_NOT_EQUAL;
//	        strCmpReturnType isMatch_03 = STR_NOT_EQUAL;
//	        strCmpReturnType isMatch_04 = STR_NOT_EQUAL;
//
//	        /* Compare with turn on green led command */
//	        isMatch_01 =  StrCmp(turn_on_green_led, RxBuffer,
//	            sizeof(turn_on_green_led));
//
//	        /* Check return status */
//	        if(STR_EQUAL == isMatch_01)
//	        {
//	          /* Turn on green led */
//	          GPIO_TurnON_Led(Green_led);
//
//	          /* Transmit data */
//	          strTransmit(done, sizeof(done));
//	        }
//	        else
//	        {
//	          /* Compare with turn on red led command */
//	          isMatch_02 =  StrCmp(turn_on_red_led, RxBuffer,
//	              sizeof(turn_on_red_led));
//	        }
//
//	        /* Check return status */
//	        if(STR_EQUAL == isMatch_02)
//	        {
//	          /* Turn on red led */
//	        	GPIO_TurnON_Led(Red_led);
//
//	          /* Transmit data */
//	          strTransmit(done, sizeof(done));
//	        }
//	        else if(STR_NOT_EQUAL == isMatch_01)
//	        {
//	          /* Compare with turn off green led command */
//	          isMatch_03 =  StrCmp(turn_off_green_led, RxBuffer,
//	              sizeof(turn_off_green_led));
//	        }
//	        else
//	        {
//	          /* Do nothing */
//	        }
//
//	        /* Check return status */
//	        if(STR_EQUAL == isMatch_03)
//	        {
//	          /* Turn off green led */
//	          GPIO_TurnOFF_Led(Green_led);
//
//	          /* Transmit data */
//	          strTransmit(done, sizeof(done));
//	        }
//	        else if((STR_NOT_EQUAL == isMatch_02)
//	            && (STR_NOT_EQUAL == isMatch_01))
//	        {
//	          /* Compare with turn off red led command */
//	          isMatch_04 =  StrCmp(turn_off_red_led, RxBuffer,
//	              sizeof(turn_off_red_led));
//	        }
//	        else
//	        {
//	          /* Do nothing */
//	        }
//
//	        /* Check return status */
//	        if(STR_EQUAL == isMatch_04)
//	        {
//	          /* Turn off red led */
//	          GPIO_TurnOFF_Led(Red_led);
//
//	          /* Transmit data */
//	          strTransmit(done, sizeof(done));
//	        }
//	        else if((STR_NOT_EQUAL == isMatch_03)
//	            && (STR_NOT_EQUAL == isMatch_02)
//	            && (STR_NOT_EQUAL == isMatch_01))
//	        {
//	          /* Transmit data */
//	          strTransmit(wrong_command, sizeof(wrong_command));
//	        }
//	        else
//	        {
//	          /* Do nothing */
//	        }
//	      }
//	      else
//	      {
//	        /* Nothing received yet */
//	      }
//	      break;
//
//	    default:
//	      break;
//
//	}
//}


void USART1_Process(void)
{
	switch (currentIRQStatus)
	{
	case USART1_PARITY_ERROR :
		//print "parity error"
		strTransmit(parity_error, sizeof(parity_error));

		//reset to idle condition
		currentState = USART1_IDLE ;

		//reset IRQ
		currentIRQStatus = USART1_NO_IRQ ;
		break;

	case USART1_CHAR_RECEIVED :

		//receive the string char by char
		strReceive() ;

		//reset IRQ
		currentIRQStatus = USART1_NO_IRQ ;
		break ;

	case USART1_NO_IRQ :
		break ;

	default :
		break ;
	}


	switch (currentState)
	{
	case USART1_IDLE :
		strTransmit(hello_world, sizeof(hello_world)) ;
		currentState =  USART1_WAIT_FOR_COMMAND;
		break ;



	case USART1_WAIT_FOR_COMMAND:
	      /* Check if new message received */
	      if(0 != RxMessageLength)
	      {
	        /* Reset message length */
	        RxMessageLength = 0;

	        /* String compare results */
	        strCmpReturnType isMatch_01 = STR_NOT_EQUAL;
	        strCmpReturnType isMatch_02 = STR_NOT_EQUAL;
	        strCmpReturnType isMatch_03 = STR_NOT_EQUAL;
	        strCmpReturnType isMatch_04 = STR_NOT_EQUAL;

	        /* Compare with turn on green led command */
	        isMatch_01 =  StrCmp(turn_on_green_led, RxBuffer,
	            sizeof(turn_on_green_led));

	        /* Check return status */
	        if(STR_EQUAL == isMatch_01)
	        {
	          /* Turn on green led */
	          GPIO_TurnON_Led(Green_led);

	          /* Transmit data */
	          strTransmit(done, sizeof(done));
	        }
	        else
	        {
	          /* Compare with turn on red led command */
	          isMatch_02 =  StrCmp(turn_on_red_led, RxBuffer,
	              sizeof(turn_on_red_led));
	        }

	        /* Check return status */
	        if(STR_EQUAL == isMatch_02)
	        {
	          /* Turn on red led */
	        	GPIO_TurnON_Led(Red_led);

	          /* Transmit data */
	          strTransmit(done, sizeof(done));
	        }
	        else if(STR_NOT_EQUAL == isMatch_01)
	        {
	          /* Compare with turn off green led command */
	          isMatch_03 =  StrCmp(turn_off_green_led, RxBuffer,
	              sizeof(turn_off_green_led));
	        }
	        else
	        {
	          /* Do nothing */
	        }

	        /* Check return status */
	        if(STR_EQUAL == isMatch_03)
	        {
	          /* Turn off green led */
	          GPIO_TurnOFF_Led(Green_led);

	          /* Transmit data */
	          strTransmit(done, sizeof(done));
	        }
	        else if((STR_NOT_EQUAL == isMatch_02)
	            && (STR_NOT_EQUAL == isMatch_01))
	        {
	          /* Compare with turn off red led command */
	          isMatch_04 =  StrCmp(turn_off_red_led, RxBuffer,
	              sizeof(turn_off_red_led));
	        }
	        else
	        {
	          /* Do nothing */
	        }

	        /* Check return status */
	        if(STR_EQUAL == isMatch_04)
	        {
	          /* Turn off red led */
	          GPIO_TurnOFF_Led(Red_led);

	          /* Transmit data */
	          strTransmit(done, sizeof(done));
	        }
	        else if((STR_NOT_EQUAL == isMatch_03)
	            && (STR_NOT_EQUAL == isMatch_02)
	            && (STR_NOT_EQUAL == isMatch_01))
	        {
	          /* Transmit data */
	          strTransmit(wrong_command, sizeof(wrong_command));
	        }
	        else
	        {
	          /* Do nothing */
	        }
	      }
	      else
	      {
	        /* Nothing received yet */
	      }
	      break;

	    default:
	      break;

	}
}



