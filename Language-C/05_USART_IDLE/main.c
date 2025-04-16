#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stm32f1xx.h"

volatile uint64_t tickGet; // variable for save time

bool idle; // variable for flag idle

uint8_t i; // variable for count RX data
uint8_t data[64]; // table variable for save RX data

void RS232_Write(uint8_t message); // prototype
void RS232_Print(char *message); // prototype
bool RS232_ReadAvailable(void); // prototype
uint8_t RS232_Read(void); // prototype

int main(void)
{
	SysTick_Config(8000000 / 1000); // set SysTick, for example (speed CPU / divider time)

	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_USART1EN; // enable clock for GPIOA, USART1

	GPIOA->CRH |= GPIO_CRH_MODE9; // set mode for GPIOA-9 (TX)
	GPIOA->CRH &= ~(GPIO_CRH_CNF9_0); // set config for GPIOA-9 (TX)
	GPIOA->CRH |= GPIO_CRH_CNF9_1; // set config for GPIOA-9 (TX)

	GPIOA->CRH &= ~(GPIO_CRH_MODE10); // set mode for GPIOA-10 (RX)
	GPIOA->CRH |= GPIO_CRH_CNF10_0; // set config for GPIOA-10 (RX)
	GPIOA->CRH &= ~(GPIO_CRH_CNF10_1); // set config for GPIOA-10 (RX)

	USART1->BRR = 8000000 / 9600; // set baud, for example (speed CPU / baud)
	USART1->CR1 |= USART_CR1_TE | USART_CR1_RE; // enable TX, RX
	USART1->CR1 |= USART_CR1_IDLEIE; // enable IDLE interrupt
	NVIC_EnableIRQ(USART1_IRQn); // enable USART1 interrupt
	USART1->CR1 |= USART_CR1_UE; // enable USART1

	while(1)
	{
		static uint64_t tickLast; // variable for save last time

		if(tickGet - tickLast >= 1000) // if difference between time and last time is more then interval
		{
			tickLast = tickGet; // save time to last time

			RS232_Print("Hello World\r\n"); // send const data

			if(i > 0) // if count RX data is more then 0
			{
				RS232_Print((char *)data); // send RX data
				RS232_Print("\r\n"); // send return, new line

				memset(data, 0, 64); // clear table variable for save RX data
				i = 0; // set 0 for count RX data

				if(idle) // if idle
				{
					RS232_Print("Detect IDLE\r\n"); // send const data

					idle = 0; // cleat flag idle
				}
			}
		}

		if(RS232_ReadAvailable()) // if data ready to read
		{
			data[i] = RS232_Read(); // read data to table variable

			if(i > 63) // if count RX data is max
			{
				i = 0; // set 0 for count RX data
			}
			else
			{
				i++; // add 1 for count RX data
			}
		}
	}
}

void RS232_Write(uint8_t message)
{
	while(!(USART1->SR & USART_SR_TXE)); // if transmit data register empty

	USART1->DR = message; // prepare data to send

	while(!(USART1->SR & USART_SR_TC)); // if transmission complete
}

void RS232_Print(char *message)
{
	while(*message != '\0') // variable isn't 0
	{
		RS232_Write(*message++); // next table variable
	}
}

bool RS232_ReadAvailable(void)
{
	return (USART1->SR & USART_SR_RXNE); // if read data register not empty
}

uint8_t RS232_Read(void)
{
	return USART1->DR; // prepare data to read
}

void USART1_IRQHandler(void)
{
	if(USART1->SR & USART_SR_IDLE) // if IDLE detect
	{
		(void)USART1->DR; // clear data to read

		idle = 1; // set flag idle
	}
}

void SysTick_Handler(void)
{
	tickGet++; // count time
}
