#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/io.h>

#include "led.h"

#define LED_GPC0CON	0xE0200060
#define LED_GPC0DAT	0xE0200064

unsigned int *led_config; 
unsigned int *led_data; 

struct cdev cdev;
dev_t devno;

int led_open (struct inode *node, struct file *filp)
{
	int tmp;
	led_config = ioremap(LED_GPC0CON,4);
	tmp = readl(led_config) & (~0xff000);
	writel(tmp | 0x11000,led_config);
	led_data = ioremap(LED_GPC0DAT,4);
	return 0;
}

long led_ioctl (struct file *filp, unsigned int cmd, unsigned long args)
{
	int tmp;
	tmp = readl(led_data);
	switch (cmd)
	{
	    case LED_ON:		
	        writel(tmp|0x18,led_data);
			tmp = readl(led_data);
			printk("LED_ON %x\n",tmp & 0x18);
	        break;
	    case LED_OFF:
	        writel(tmp & ~0x18,led_data);
			tmp = readl(led_data);
			printk("LED_OFF %x\n",tmp & 0x18);
	        break;	    
	    default:
	    	return -EINVAL;
	}
	return 0;
}

const struct file_operations led_fops = {
	.open = led_open,
	.unlocked_ioctl = led_ioctl,
};

static int led_init(void)
{
	cdev_init(&cdev, &led_fops);
	alloc_chrdev_region(&devno, 0, 1,"myled");
	cdev_add(&cdev, devno, 1);
	return 0;
}

static void led_exit(void)
{
	cdev_del(&cdev);
	unregister_chrdev_region(devno, 1);
}

MODULE_LICENSE("GPL");
module_init(led_init);
module_exit(led_exit);