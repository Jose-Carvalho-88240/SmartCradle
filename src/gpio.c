/**
 * @file utils.c
 * @author José Carvalho, João Carneiro
 * @brief Utilities module implementation
 * @date 2022-01-13
 * 
 * @copyright Copyright (c) 2022
 */

#include "../inc/gpio.h"
#include <linux/module.h>
#include <linux/types.h>

void SetGPIOFunction(struct GpioRegisters *s_pGpioRegisters, int GPIO, int functionCode) {
	int registerIndex = GPIO / 10;
	int bit = (GPIO % 10) * 3;

	unsigned oldValue = s_pGpioRegisters->GPFSEL[registerIndex];
	unsigned mask = 7 << bit;

	s_pGpioRegisters->GPFSEL[registerIndex] = (oldValue & ~mask) | ((functionCode << bit) & mask);
}

void SetGPIOValue(struct GpioRegisters *s_pGpioRegisters, int GPIO, _Bool outputValue) {

	if (outputValue)
		s_pGpioRegisters->GPSET[GPIO / 32] = (1 << (GPIO % 32));
	else
		s_pGpioRegisters->GPCLR[GPIO / 32] = (1 << (GPIO % 32));
}

int GetGPIOValue(struct GpioRegisters *s_pGpioRegisters, int GPIO) {
	unsigned mask = (1 << (GPIO % 32));

	return (s_pGpioRegisters->GPLEV[GPIO / 32] & mask) >> (GPIO % 32);
}

void SetGPIOPull(struct GpioRegisters *s_pGpioRegisters, int GPIO, int pullCode)
{
	int registerIndex = GPIO / 16;
	int bit = (GPIO % 16) * 2;

	unsigned oldValue = s_pGpioRegisters->GPIO_PUP_PDN_CNTRL_REG[registerIndex];
	unsigned mask = 7 << bit;

	s_pGpioRegisters->GPIO_PUP_PDN_CNTRL_REG[registerIndex] = (oldValue & ~mask) | ((pullCode << bit) & mask);
}
