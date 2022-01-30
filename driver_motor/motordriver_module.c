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
#include "../inc/gpio.h"

#define DEVICE_NAME "motordriver0"
#define CLASS_NAME "motordriverClass"

/* Device variables */
static struct class* motordriverDevice_class = NULL;
static struct device* motordriverDevice_device = NULL;
static dev_t motordriverDevice_majorminor;
static struct cdev c_dev;  // Character device structure

/* Registers */
struct GpioRegisters *pGPIORegisters;

/* Device Pins */
static const int IN3 = 23;
static const int IN4 = 24;

ssize_t motordriver_device_write(struct file *pfile, const char __user *pbuff, size_t len, loff_t *off) {
	struct GpioRegisters *pdev; 
	
	pr_alert("%s: called (%u)\n",__FUNCTION__,len);
	
	if(unlikely(pfile->private_data == NULL))
		return -EFAULT;

	pdev = (struct GpioRegisters *)pfile->private_data;

	if (pbuff[0]=='0') //Stop motor
	{
		pr_alert("%s: Motor is stopping...\n",__FUNCTION__);
		SetGPIOValue(pdev, IN3, HIGH);
		SetGPIOValue(pdev, IN4, HIGH);
	}
	else //Start motor
	{
		pr_alert("%s: Motor is running...\n",__FUNCTION__);
		SetGPIOValue(pdev, IN3, HIGH);
		SetGPIOValue(pdev, IN4, LOW);
	}	
	return len;
}

int motordriver_device_close(struct inode *p_inode, struct file * pfile){
	
	pr_alert("%s: called\n",__FUNCTION__);
	pfile->private_data = NULL;
	return 0;
}

int motordriver_device_open(struct inode* p_indode, struct file *p_file){

	pr_alert("%s: called\n",__FUNCTION__);
	p_file->private_data = (struct GpioRegisters *) pGPIORegisters;
	return 0;
	
}

static struct file_operations motordriverDevice_fops = {
	.owner = THIS_MODULE,
	.write = motordriver_device_write,
	.release = motordriver_device_close,
	.open = motordriver_device_open,
};

static int __init motordriverModule_init(void) {
	int ret;
	struct device *dev_ret;

	pr_alert("%s: called\n",__FUNCTION__);

	if ((ret = alloc_chrdev_region(&motordriverDevice_majorminor, 0, 1, DEVICE_NAME)) < 0) {
		return ret;
	}

	if (IS_ERR(motordriverDevice_class = class_create(THIS_MODULE, CLASS_NAME))) {
		unregister_chrdev_region(motordriverDevice_majorminor, 1);
		return PTR_ERR(motordriverDevice_class);
	}
	if (IS_ERR(dev_ret = device_create(motordriverDevice_class, NULL, motordriverDevice_majorminor, NULL, DEVICE_NAME))) {
		class_destroy(motordriverDevice_class);
		unregister_chrdev_region(motordriverDevice_majorminor, 1);
		return PTR_ERR(dev_ret);
	}

	cdev_init(&c_dev, &motordriverDevice_fops);
	c_dev.owner = THIS_MODULE;
	if ((ret = cdev_add(&c_dev, motordriverDevice_majorminor, 1)) < 0) {
		printk(KERN_NOTICE "Error %d adding device", ret);
		device_destroy(motordriverDevice_class, motordriverDevice_majorminor);
		class_destroy(motordriverDevice_class);
		unregister_chrdev_region(motordriverDevice_majorminor, 1);
		return ret;
	}

	pGPIORegisters = (struct GpioRegisters *)ioremap(GPIO_BASE, sizeof(struct GpioRegisters));

	pr_alert("map to virtual adresse: 0x%x\n", (unsigned)pGPIORegisters);
	
	SetGPIOFunction(pGPIORegisters, IN3, OUTPUT); //Output
	SetGPIOFunction(pGPIORegisters, IN4, OUTPUT); //Output

	return 0;
}

static void __exit motordriverModule_exit(void) {
	
	pr_alert("%s: called\n",__FUNCTION__);
	
	SetGPIOFunction(pGPIORegisters, IN4, INPUT); //Input
	SetGPIOFunction(pGPIORegisters, IN3, INPUT); //Input

	iounmap(pGPIORegisters);
	cdev_del(&c_dev);
	device_destroy(motordriverDevice_class, motordriverDevice_majorminor);
	class_destroy(motordriverDevice_class);
	unregister_chrdev_region(motordriverDevice_majorminor, 1);
}

module_init(motordriverModule_init);
module_exit(motordriverModule_exit);

MODULE_LICENSE("GPL");