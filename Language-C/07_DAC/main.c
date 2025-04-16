#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stm32f1xx.h"

volatile uint64_t tickGet; // variable for save time

uint16_t interval = 100; // variable for set interval
uint16_t value[2]; // variable for DAC

int main(void)
{
	SysTick_Config(8000000 / 1000); // set SysTick, for example (speed CPU / divider time)

	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; // enable clock for GPIOA
	RCC->APB1ENR |= RCC_APB1ENR_DACEN; // enable clock for DAC

	DAC->CR |= DAC_CR_EN1 | DAC_CR_EN2; // enable channel for DAC

	while(1)
	{
		static uint64_t tickLast; // variable for save last time

		if(tickGet - tickLast >= interval) // if difference between time and last time is more then interval
		{
			tickLast = tickGet; // save time to last time

			if(value[0] < 4096 || value[1] < 4096) // if variable is less 4096
			{
				value[0]++; // add one to value[0]
				value[1]++; // add one to value[1]
			}

			DAC->DHR12R1 = value[0]; // set DAC value[0]
			DAC->DHR12R2 = value[1]; // set DAC value[1]
		}
	}
}

void SysTick_Handler(void)
{
	tickGet++; // count time
}
