#include <linux/cdev.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/init.h>
#include <linux/module.h>
#include <asm/io.h>
#include <linux/timer.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/mm.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include "../inc/dht.h"

#define DEVICE_NAME "dht110"
#define CLASS_NAME "dht11Class"

MODULE_LICENSE("GPL");

/* Device variables */
static struct class* dht11Device_class = NULL;
static struct device* dht11Device_device = NULL;
static dev_t dht11Device_majorminor;
static struct cdev c_dev;  // Character device structure

static struct class *s_pDeviceClass;
static struct device *s_pDeviceObject;
struct GpioRegisters *s_pGpioRegisters;

static const int DATA = 25;

static char buffer[5];

static int wait(struct GpioRegisters *s_pGpioRegisters, int GPIO, bool level, uint8_t timeout)
{
    int cnt = 0;
    while (level == GetGPIOValue(s_pGpioRegisters,GPIO)) {
        if (cnt > timeout) {
            return 0;
        }
        cnt++;
        udelay(1);
    }
    return 1;
}

ssize_t dht11_device_read(struct file *pfile, char __user *p_buff,size_t len, loff_t *poffset){
	struct GpioRegisters *pdev; 

	int dht11_data[5]={0,0,0,0,0};
	bool currentState = 0;
	bool lastState = 1;
	uint8_t bitCounter = 1;
	uint8_t timeCounter = 0;

	pr_alert("%s: called (%u)\n",__FUNCTION__,len);
	
	if(unlikely(pfile->private_data == NULL))
		return -EFAULT;

	pdev = (struct GpioRegisters *)pfile->private_data;

	// if(!wait(pdev, DATA, 0,255))
	// {
	// 	pr_alert("%s: Bus is busy.\n",__FUNCTION__,len);
	// 	return 0;
	// }

	SetGPIOFunction(pdev, DATA, OUTPUT); //Output
	SetGPIOValue(pdev,DATA,LOW);
	mdelay(20);
	SetGPIOValue(pdev,DATA,HIGH);
	udelay(40);
	SetGPIOFunction(pdev, DATA, INPUT); //Input
	
	currentState=GetGPIOValue(pdev,DATA);

	// while(wait(pdev, DATA, 1,100))
	// {
	// 	//pr_alert("%s: Timeout. No ACK from sensor.\n",__FUNCTION__,len);
	// 	//return 0;
	// }

	// while(!wait(pdev, DATA, 1,100))
	// {
	// 	//pr_alert("%s: Timeout.\n",__FUNCTION__,len);
	// 	//return 0;
	// }

	int i;
	for(i = 0; i < 80; i++)
	{
		timeCounter=0;
		
		while(currentState == lastState)
		{
			timeCounter++;
			udelay(1);
			if(timeCounter == 255)
			{
				pr_alert("%s: Sampling error.\n",__FUNCTION__,len);
				return 0;
			}
			currentState=GetGPIOValue(pdev,DATA);
		}
		lastState = currentState;

		if( (i>=0) && (i % 2 != 0) )
		{
			pr_alert("%s: Data bit received @ i = %d.\n",__FUNCTION__,i);
			dht11_data[bitCounter / 8] <<= 1;
			if(timeCounter > 50)
				dht11_data[bitCounter / 8] |= 1;
			bitCounter++;
		}
	}
	pr_alert("%s: Bits received @ bits = %d.\n",__FUNCTION__,bitCounter);
	if( bitCounter >= 40)
	{
		uint8_t checksum = 0;

		for(i = 0; i < 4; i++)
			checksum = checksum + dht11_data[i];
		checksum = checksum && 0xFF;

		if(checksum != dht11_data[4])
		{
			pr_alert("%s: Sampling error. Checksum mismatch.\n",__FUNCTION__,len);
			return 0;
		}
		pr_alert("Temperature: %d.%dºC\nHumidity: %d.%d\nChecksum: %d\n",dht11_data[0],dht11_data[1],dht11_data[2],dht11_data[3],dht11_data[4]);
		
		sprintf(buffer,"%d%d%d%d%d",dht11_data[0],dht11_data[1],dht11_data[2],dht11_data[3],dht11_data[4]);
		
		copy_to_user(p_buff,buffer,5);

		return len;
	}

	return 0;
}

int dht11_device_close(struct inode *p_inode, struct file * pfile){
	
	pr_alert("%s: called\n",__FUNCTION__);
	pfile->private_data = NULL;
	return 0;
}

int dht11_device_open(struct inode* p_indode, struct file *p_file){

	pr_alert("%s: called\n",__FUNCTION__);
	p_file->private_data = (struct GpioRegisters *) s_pGpioRegisters;
	return 0;
}

static struct file_operations dht11Device_fops = {
	.owner = THIS_MODULE,
	.read = dht11_device_read,
	.release = dht11_device_close,
	.open = dht11_device_open,
};

static int __init dht11Module_init(void) {
	int ret;
	struct device *dev_ret;

	pr_alert("%s: called\n",__FUNCTION__);

	if ((ret = alloc_chrdev_region(&dht11Device_majorminor, 0, 1, DEVICE_NAME)) < 0) {
		return ret;
	}

	if (IS_ERR(dht11Device_class = class_create(THIS_MODULE, CLASS_NAME))) {
		unregister_chrdev_region(dht11Device_majorminor, 1);
		return PTR_ERR(dht11Device_class);
	}
	if (IS_ERR(dev_ret = device_create(dht11Device_class, NULL, dht11Device_majorminor, NULL, DEVICE_NAME))) {
		class_destroy(dht11Device_class);
		unregister_chrdev_region(dht11Device_majorminor, 1);
		return PTR_ERR(dev_ret);
	}

	cdev_init(&c_dev, &dht11Device_fops);
	c_dev.owner = THIS_MODULE;
	if ((ret = cdev_add(&c_dev, dht11Device_majorminor, 1)) < 0) {
		printk(KERN_NOTICE "Error %d adding device", ret);
		device_destroy(dht11Device_class, dht11Device_majorminor);
		class_destroy(dht11Device_class);
		unregister_chrdev_region(dht11Device_majorminor, 1);
		return ret;
	}

	s_pGpioRegisters = (struct GpioRegisters *)ioremap(GPIO_BASE, sizeof(struct GpioRegisters));

	SetGPIOFunction(s_pGpioRegisters, DATA, INPUT); //Input
	SetGPIOPull(s_pGpioRegisters,DATA,PULL_UP);

	pr_alert("map to virtual adresse: 0x%x\n", (unsigned)s_pGpioRegisters);

	return 0;
}

static void __exit dht11Module_exit(void) {
	
	pr_alert("%s: called\n",__FUNCTION__);

	iounmap(s_pGpioRegisters);
	cdev_del(&c_dev);
	device_destroy(dht11Device_class, dht11Device_majorminor);
	class_destroy(dht11Device_class);
	unregister_chrdev_region(dht11Device_majorminor, 1);
}

module_init(dht11Module_init);
module_exit(dht11Module_exit);