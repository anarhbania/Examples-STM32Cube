#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stm32f1xx.h"

volatile uint64_t tickGet; // variable for save time

uint16_t interval = 500; // variable for set interval

int main(void)
{
	SysTick_Config(8000000 / 1000); // set SysTick, for example (speed CPU / divider time)

	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; // enable clock for GPIOA

	GPIOA->CRL |= GPIO_CRL_MODE2; // set mode for GPIOA-2 (output)
	GPIOA->CRL &= ~(GPIO_CRL_CNF2); // set config for GPIOA-2 (output)

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
				GPIOA->BSRR |= GPIO_BSRR_BS2; // set high state in GPIOA-2
			}
			else
			{
				GPIOA->BSRR |= GPIO_BSRR_BR2; // set low state in GPIOA-2
			}
		}
	}
}

void SysTick_Handler(void)
{
	tickGet++; // count time
}
