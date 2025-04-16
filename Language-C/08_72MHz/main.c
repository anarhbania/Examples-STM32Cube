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
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN; // enable clock for GPIOC where is connect crystal

	RCC->CR |= RCC_CR_HSEON; // enable HSE clock
	while(!(RCC->CR & RCC_CR_HSERDY)); // check if HSE clock be ready

	RCC->CFGR |= RCC_CFGR_PLLMULL9 | RCC_CFGR_PLLSRC | RCC_CFGR_PPRE1_DIV2 | RCC_CFGR_PPRE2_DIV1; // set PLL multiplication, PLL entry clock source, APB low-speed prescaler, APB high-speed prescaler
	RCC->CR |= RCC_CR_PLLON; // enable PLL clock
	while(!(RCC->CR & RCC_CR_PLLRDY)); // check if PLL clock be ready

	FLASH->ACR |= FLASH_ACR_LATENCY_2 | FLASH_ACR_PRFTBE; // set latency, enable prefetch buffer

	RCC->CFGR |= RCC_CFGR_SW_PLL; // switch system clock to PLL
	while(!(RCC->CFGR & RCC_CFGR_SWS_PLL)); // check if system clock PLL be ready

	SysTick_Config(72000000 / 1000); // set SysTick, for example (speed CPU / divider time)

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
