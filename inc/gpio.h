/**
 * @file utils.h
 * @author José Carvalho, João Carneiro
 * @brief Utilities module header
 * @date 2022-01-13
 * 
 * @copyright Copyright (c) 2022
 */

#ifndef GPIO_H
#define GPIO_H

#include <linux/types.h>

#define BCM2711_PERI_BASE 0xfe000000
#define GPIO_BASE (BCM2711_PERI_BASE + 0x200000) // GPIO controller

// PIN FUNCTION
#define INPUT 0
#define OUTPUT 1

// PIN VALUE
#define HIGH 1
#define LOW 0

// PIN PULL
#define NO_PULL 0
#define PULL_UP 1
#define PULL_DOWN 2

/**
 * @brief Structure that contains GPIO Registers
 * 
 *		GPFSEL : Set pin function
 *		GPSET : Set pin HIGH
 *		GPCLR : Set pin LOW
 *		GPLEV : Pin value
 *		GPIO_PUP_PDN_CNTRL_REG : Set pin pull
 */
struct GpioRegisters
{
	unsigned int GPFSEL[6];
	unsigned int Reserved1;
	unsigned int GPSET[2];
	unsigned int Reserved2;
	unsigned int GPCLR[2];
	unsigned int Reserved3;
	unsigned int GPLEV[2];
	unsigned int Reserved4;
	unsigned int GPEDS[2];
	unsigned int Reserved5;
	unsigned int GPREN[2];
	unsigned int Reserved6;
	unsigned int GPFEN[2];
	unsigned int Reserved7;
	unsigned int GPHEN[2];
	unsigned int Reserved8;
	unsigned int GPLEN[2];
	unsigned int Reserved9;
	unsigned int GPAREN[2];
	unsigned int Reserved10;
	unsigned int GPAFEN[2];
	unsigned int Reserved11[21];
	unsigned int GPIO_PUP_PDN_CNTRL_REG[4];
};

/**
 * @brief Set GPIO Pin function
 * 
 * @param pGPIORegisters : Register structure
 * @param GPIO : GPIO Pin
 * @param functionCode : Function (Input, Output)
 */
void SetGPIOFunction(struct GpioRegisters *pGPIORegisters, int GPIO, int functionCode);

/**
 * @brief Set GPIO Pin pull mode
 * 
 * @param pGPIORegisters : Register structure
 * @param GPIO : GPIO Pin
 * @param pullCode : Pull mode (No pull, Pull-up, Pull-down)
 */
void SetGPIOPull(struct GpioRegisters *pGPIORegisters, int GPIO, int pullCode);

/**
 * @brief Set GPIO Pin value
 * 
 * @param pGPIORegisters : Register structure
 * @param GPIO : GPIO Pin
 * @param outputValue : Value desired for pin
 */
void SetGPIOValue(struct GpioRegisters *pGPIORegisters, int GPIO, _Bool outputValue);

/**
 * @brief Get GPIO Pin value
 * 
 * @param pGPIORegisters : Register structure
 * @param GPIO : GPIO Pin
 * @return int : GPIO Pin value
 */
int GetGPIOValue(struct GpioRegisters *pGPIORegisters, int GPIO);

#endif
