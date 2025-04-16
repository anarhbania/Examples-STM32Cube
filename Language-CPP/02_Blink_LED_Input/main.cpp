#include <cstdint>
#include <cstdlib>

#include "stm32f1xx.h"

volatile uint64_t tickGet; // variable for save time

uint16_t interval = 500; // variable for set interval

int main(void)
{
	SysTick_Config(8000000 / 1000); // set SysTick, for example (speed CPU / divider time)

	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN; // enable clock for GPIOA, GPIOB

	GPIOA->CRL |= GPIO_CRL_MODE2; // set mode for GPIOA-2 (output)
	GPIOA->CRL &= ~(GPIO_CRL_CNF2); // set config for GPIOA-2 (output)

	GPIOB->CRH &= ~(GPIO_CRH_MODE10); // set mode for GPIOB-10 (input)
	GPIOB->CRH |= GPIO_CRH_CNF10_0; // set config for GPIOB-10 (input)
	GPIOB->CRH &= ~(GPIO_CRH_CNF10_1); // set config for GPIOB-10 (input)

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
				if(!(GPIOB->IDR & GPIO_IDR_IDR10)) // if low state in GPIOB-10
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

extern "C" void SysTick_Handler(void)
{
	tickGet++; // count time
}
