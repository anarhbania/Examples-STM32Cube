#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stm32f1xx.h"

volatile uint64_t tickGet; // variable for save time

uint16_t interval = 500; // variable for set interval
uint16_t value[2]; // variable for ADC read

int main(void)
{
	SysTick_Config(8000000 / 1000); // set SysTick, for example (speed CPU / divider time)

	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_ADC1EN; // enable clock for GPIOA, ADC
	RCC->CFGR |= RCC_CFGR_ADCPRE_DIV8; // set divider for ADC
	RCC->AHBENR |= RCC_AHBENR_DMA1EN; // enable clock for DMA

	GPIOA->CRL |= GPIO_CRL_MODE2; // set mode for GPIOA-2 (output)
	GPIOA->CRL &= ~(GPIO_CRL_CNF2); // set config for GPIOA-2 (output)

	GPIOA->CRL &= ~(GPIO_CRL_MODE1); // set mode GPIOA-1 (ADC)
	GPIOA->CRL &= ~(GPIO_CRL_MODE3); // set mode GPIOA-3 (ADC)

	ADC1->SQR1 |= 1 << ADC_SQR1_L_Pos; // set how many channel ADC (how many - 1)
	ADC1->SQR3 |= (3 << ADC_SQR3_SQ2_Pos) | (1 << ADC_SQR3_SQ1_Pos); // set regular sequence for ADC
	ADC1->SMPR2 |= ADC_SMPR2_SMP3 | ADC_SMPR2_SMP1; // set sample time for ADC

	ADC1->CR2 |= ADC_CR2_ADON; // enable AD converter
	ADC1->CR1 |= ADC_CR1_SCAN; // enable scan mode
	ADC1->CR2 |= ADC_CR2_DMA; // enable DMA for ADC

	DMA1_Channel1->CPAR = (uint32_t)&ADC1->DR; // set peripheral address
	DMA1_Channel1->CMAR = (uint32_t)value; // set memory address
	DMA1_Channel1->CNDTR = 2; // number of data to transfer

	DMA1_Channel1->CCR |= DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0 | DMA_CCR_MINC | DMA_CCR_TCIE; // set memory data size, peripheral data size, enable memory increment mode, transfer complete interrupt

	NVIC_EnableIRQ(DMA1_Channel1_IRQn); // enable interrupt for ADC

	DMA1_Channel1->CCR |= DMA_CCR_EN; // enable DMA
	ADC1->CR2 |= ADC_CR2_ADON; // enable ADC

	while(1)
	{
		static bool state; // variable for save state
		static uint64_t tickLast; // variable for save last time

		if(tickGet - tickLast >= interval) // if difference between time and last time is more then interval
		{
			tickLast = tickGet; // save time to last time

			state =! state; // change state

			if(state)
			{
				if(value[0] >= 2000) // if GPIOA-1 (ADC) have more then 2000
				{
					GPIOA->BSRR |= GPIO_BSRR_BS2; // set high state in GPIOA-2
				}
				else
				{
					GPIOA->BSRR |= GPIO_BSRR_BR2; // set low state in GPIOA-2
				}
			}
			else
			{
				GPIOA->BSRR |= GPIO_BSRR_BR2; // set low state in GPIOA-2
			}
		}
	}
}

void DMA1_Channel1_IRQHandler(void)
{
    if(DMA1->ISR & DMA_ISR_TCIF1) // if interrupt transfer complete
    {
		DMA1->IFCR |= DMA_IFCR_CTCIF1; // clear interrupt transfer complete

		DMA1_Channel1->CCR &= ~DMA_CCR_EN; // disable DMA
		DMA1_Channel1->CNDTR = 2; // number of data to transfer
		DMA1_Channel1->CCR |= DMA_CCR_EN; // enable DMA

		ADC1->CR2 |= ADC_CR2_ADON; // enable ADC
    }
}

void SysTick_Handler(void)
{
	tickGet++; // count time
}
