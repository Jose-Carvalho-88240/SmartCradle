#include <linux/types.h>

#define BCM2711_PERI_BASE       0xfe000000
#define GPIO_BASE (BCM2711_PERI_BASE + 0x200000) // GPIO controller

#define INPUT 0
#define OUTPUT 1

#define HIGH 1
#define LOW 0

#define NO_PULL 0
#define PULL_UP 1
#define PULL_DOWN 2

struct GpioRegisters
{
	uint32_t GPFSEL[6];
	uint32_t Reserved1;
	uint32_t GPSET[2];
	uint32_t Reserved2;
	uint32_t GPCLR[2];
	uint32_t Reserved3;
	uint32_t GPLEV[2];
	uint32_t Reserved4[42];
	uint32_t GPIO_PUP_PDN_CNTRL_REG[4];
};

void SetGPIOFunction(struct GpioRegisters *s_pGpioRegisters, int GPIO, int functionCode);
void SetGPIOPull(struct GpioRegisters *s_pGpioRegisters, int GPIO, int pullCode);
void SetGPIOValue(struct GpioRegisters *s_pGpioRegisters, int GPIO, bool outputValue);
int GetGPIOValue(struct GpioRegisters *s_pGpioRegisters, int GPIO);