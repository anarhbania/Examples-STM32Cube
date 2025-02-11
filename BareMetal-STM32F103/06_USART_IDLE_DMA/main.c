#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stm32f1xx.h"

volatile uint64_t tickGet; // variable for save time

bool idle; // variable for flag idle

uint8_t data[64]; // table variable for save RX data

void RS232_Write(uint8_t message); // prototype
void RS232_Print(char *message); // prototype

int main(void)
{
	SysTick_Config(8000000 / 1000); // set SysTick, for example (speed CPU / divider time)

	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_USART1EN; // enable clock for GPIOA, USART1
	RCC->AHBENR |= RCC_AHBENR_DMA1EN; // enable clock for DMA

	GPIOA->CRH |= GPIO_CRH_MODE9; // set mode for GPIOA-9 (TX)
	GPIOA->CRH &= ~(GPIO_CRH_CNF9_0); // set config for GPIOA-9 (TX)
	GPIOA->CRH |= GPIO_CRH_CNF9_1; // set config for GPIOA-9 (TX)

	GPIOA->CRH &= ~(GPIO_CRH_MODE10); // set mode for GPIOA-10 (RX)
	GPIOA->CRH |= GPIO_CRH_CNF10_0; // set config for GPIOA-10 (RX)
	GPIOA->CRH &= ~(GPIO_CRH_CNF10_1); // set config for GPIOA-10 (RX)

	USART1->BRR = 8000000 / 9600; // set baud, for example (speed CPU / baud)
	USART1->CR1 |= USART_CR1_TE | USART_CR1_RE; // enable TX, RX
	USART1->CR1 |= USART_CR1_IDLEIE; // enable IDLE interrupt
	USART1->CR3 |= USART_CR3_DMAR; // enable DMA for USART1

	DMA1_Channel5->CPAR = (uint32_t)&USART1->DR; // set peripheral address
	DMA1_Channel5->CMAR = (uint32_t)data; // set memory address
	DMA1_Channel5->CNDTR = 64; // number of data to transfer

	DMA1_Channel5->CCR |= DMA_CCR_MINC | DMA_CCR_TCIE; // enable memory increment mode, transfer complete interrupt
	DMA1_Channel5->CCR |= DMA_CCR_EN; // enable DMA

	NVIC_EnableIRQ(USART1_IRQn); // enable USART1 interrupt
	USART1->CR1 |= USART_CR1_UE; // enable USART1

	while(1)
	{
		if(idle) // if idle
		{
			idle = 0; // cleat flag idle

			RS232_Print((char *)data); // send data

			memset(data, 0, 64); // clear table variable for save RX data

			DMA1_Channel5->CNDTR = 64; // number of data to transfer
			DMA1_Channel5->CCR |= DMA_CCR_EN; // enable DMA
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

void USART1_IRQHandler(void)
{
	if(USART1->SR & USART_SR_IDLE) // if IDLE detect
	{
		(void)USART1->DR; // clear data to read

		DMA1_Channel5->CCR &= ~(DMA_CCR_EN); // disable DMA

		while(DMA1_Channel5->CCR & DMA_CCR_EN); // wait until disable DMA

		idle = 1; // set flag idle
	}
}

void DMA1_Channel5_IRQHandler(void)
{
	if(DMA1->ISR & DMA_ISR_TCIF5) // if interrupt transfer complete
	{
		DMA1->IFCR |= DMA_IFCR_CTCIF5; // clear interrupt transfer complete
	}
}

void SysTick_Handler(void)
{
	tickGet++; // count time
}
