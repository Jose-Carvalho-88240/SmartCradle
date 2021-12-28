#include "../inc/utils.h"
#include <linux/module.h>
#include <linux/types.h>

void SetGPIOFunction(struct GpioRegisters *s_pGpioRegisters, int GPIO, int functionCode) {
	int registerIndex = GPIO / 10;
	int bit = (GPIO % 10) * 3;

	unsigned oldValue = s_pGpioRegisters->GPFSEL[registerIndex];
	unsigned mask = 7 << bit;

	pr_alert("%s: register index is %d\n",__FUNCTION__,registerIndex);
	pr_alert("%s: mask is 0x%x\n",__FUNCTION__,mask);
	pr_alert("%s: update value is 0x%x\n",__FUNCTION__,((functionCode << bit) & mask));
	
	s_pGpioRegisters->GPFSEL[registerIndex] = (oldValue & ~mask) | ((functionCode << bit) & mask);
}

void SetGPIOValue(struct GpioRegisters *s_pGpioRegisters, int GPIO, bool outputValue) {

	pr_alert("%s: register value is 0x%x\n",__FUNCTION__,(1<<(GPIO %32)));
			 
	if (outputValue)
		s_pGpioRegisters->GPSET[GPIO / 32] = (1 << (GPIO % 32));
	else
		s_pGpioRegisters->GPCLR[GPIO / 32] = (1 << (GPIO % 32));
}

int GetGPIOValue(struct GpioRegisters *s_pGpioRegisters, int GPIO) {
	pr_alert("%s: register value is 0x%x\n",__FUNCTION__,s_pGpioRegisters->GPLEV[GPIO / 32]);

	unsigned mask = (1 << (GPIO % 32));

	return (s_pGpioRegisters->GPLEV[GPIO / 32] & mask) >> (GPIO % 32);
}

void SetGPIOPull(struct GpioRegisters *s_pGpioRegisters, int GPIO, int pullCode)
{
	int registerIndex = GPIO / 16;
	int bit = (GPIO % 16) * 2;

	unsigned oldValue = s_pGpioRegisters->GPIO_PUP_PDN_CNTRL_REG[registerIndex];
	unsigned mask = 7 << bit;

	pr_alert("%s: register index is %d\n",__FUNCTION__,registerIndex);
	pr_alert("%s: mask is 0x%x\n",__FUNCTION__,mask);
	pr_alert("%s: update value is 0x%x\n",__FUNCTION__,((pullCode << bit) & mask));
	pr_alert("%s: old register value is 0x%x\n",__FUNCTION__,oldValue);

	s_pGpioRegisters->GPIO_PUP_PDN_CNTRL_REG[registerIndex] = (oldValue & ~mask) | ((pullCode << bit) & mask);

	pr_alert("%s: new register value is 0x%x\n",__FUNCTION__,s_pGpioRegisters->GPIO_PUP_PDN_CNTRL_REG[registerIndex]);
}