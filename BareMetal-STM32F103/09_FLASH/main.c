#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stm32f1xx.h"

volatile uint64_t tickGet; // variable for save time

#define ADDRESS 0x0803F800 // set FLASH address where save variable

uint16_t data = 0x1234; // variable for save in FLASH
uint16_t dataRead; // variable read from FLASH

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

	/////////// ERASE FLASH

	while(FLASH->SR & FLASH_SR_BSY); // check if FLASH busy

	if(FLASH->CR & FLASH_CR_LOCK) // check if FLASH lock
	{
		FLASH->KEYR = 0x45670123; // write protection key
		FLASH->KEYR = 0xCDEF89AB; // write protection key
	}

	FLASH->CR |= FLASH_CR_PER; // enable erase mode
	FLASH->AR = 0x0803F800; // set address
	FLASH->CR |= FLASH_CR_STRT; // start erase FLASH
	while(FLASH->SR & FLASH_SR_BSY); // check if FLASH busy
	FLASH->CR &= ~FLASH_CR_PER; // disable erase mode

	while(FLASH->SR & FLASH_SR_BSY); // check if FLASH busy

	FLASH->CR |= FLASH_CR_LOCK; // lock FLASH

	/////////// WRITE FLASH

	while(FLASH->SR & FLASH_SR_BSY); // check if FLASH busy

	if(FLASH->CR & FLASH_CR_LOCK) // check if FLASH lock
	{
		FLASH->KEYR = 0x45670123; // write protection key
		FLASH->KEYR = 0xCDEF89AB; // write protection key
	}

	FLASH->CR |= FLASH_CR_PG; // enable programming mode
	*(uint16_t *)ADDRESS = data; // set variable to address
	while(FLASH->SR & FLASH_SR_BSY); // check if FLASH busy
	FLASH->CR &= ~FLASH_CR_PG; // disable programming mode

	while(FLASH->SR & FLASH_SR_BSY); // check if FLASH busy

	FLASH->CR |= FLASH_CR_LOCK; // lock FLASH

	/////////// READ FLASH
	
	dataRead = *(volatile uint16_t *)ADDRESS; // read variable from address
	
	while(1)
	{

	}
}

void SysTick_Handler(void)
{
	tickGet++; // count time
}
