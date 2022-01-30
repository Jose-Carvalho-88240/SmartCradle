/**
 * @file utils.c
 * @author José Carvalho, João Carneiro
 * @brief Utilities module implementation
 * @date 2022-01-13
 * 
 * @copyright Copyright (c) 2022
 */

#include <linux/module.h>
#include <linux/types.h>
#include "../inc/gpio.h"

void SetGPIOFunction(struct GpioRegisters *pGPIORegisters, int GPIO, int functionCode) {
	int registerIndex = GPIO / 10;
	int bit = (GPIO % 10) * 3;

	unsigned oldValue = pGPIORegisters->GPFSEL[registerIndex];
	unsigned mask = 7 << bit;

	pGPIORegisters->GPFSEL[registerIndex] = (oldValue & ~mask) | ((functionCode << bit) & mask);
}

void SetGPIOValue(struct GpioRegisters *pGPIORegisters, int GPIO, _Bool outputValue) {

	if (outputValue)
		pGPIORegisters->GPSET[GPIO / 32] = (1 << (GPIO % 32));
	else
		pGPIORegisters->GPCLR[GPIO / 32] = (1 << (GPIO % 32));
}

int GetGPIOValue(struct GpioRegisters *pGPIORegisters, int GPIO) {
	unsigned mask = (1 << (GPIO % 32));

	return (pGPIORegisters->GPLEV[GPIO / 32] & mask) >> (GPIO % 32);
}

void SetGPIOPull(struct GpioRegisters *pGPIORegisters, int GPIO, int pullCode)
{
	int registerIndex = GPIO / 16;
	int bit = (GPIO % 16) * 2;

	unsigned oldValue = pGPIORegisters->GPIO_PUP_PDN_CNTRL_REG[registerIndex];
	unsigned mask = 7 << bit;

	pGPIORegisters->GPIO_PUP_PDN_CNTRL_REG[registerIndex] = (oldValue & ~mask) | ((pullCode << bit) & mask);
}
