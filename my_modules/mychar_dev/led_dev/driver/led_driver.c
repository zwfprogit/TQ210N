#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/io.h>
#include <linux/device.h>

#include "led.h"

#define LED_GPC0CON	0xE0200060
#define LED_GPC0DAT	0xE0200064

static unsigned int *led_config = NULL;
static unsigned int *led_data  = NULL;

static struct cdev cdev;
static dev_t devno;
static struct class *cls;
static struct device *mydev;

int led_open (struct inode *node, struct file *filp)
{
	int tmp;
	led_config = ioremap(LED_GPC0CON,4);
	tmp = readl(led_config) & (~0xff000);
	writel(tmp | 0x11000,led_config);
	led_data = ioremap(LED_GPC0DAT,4);
	printk("Kernel led_open\n");
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
	int retval = -1;
	cdev_init(&cdev, &led_fops);// 初始化cdev结构和操纵这个字符设备文件的方法,
	// 对应： cat /proc/devices 254 myled
	alloc_chrdev_region(&devno, 0, 1,"myled");//动态获取一个主设备号和次设备号范围

	retval = cdev_add(&cdev, devno, 1);
	if (retval != 0) {
		printk("cdev_add character device fail\n");
	}
	// 对应： /sys/class/myclass
	cls = class_create(THIS_MODULE,"myclass");
	if(IS_ERR(cls)){
		unregister_chrdev_region(devno,1);
		return -EBUSY;
	}
	// 对应： /sys/class/myclass/myled
	mydev = device_create(cls,NULL,devno,NULL,"myled");
	if(IS_ERR(mydev)){
		class_destroy(cls);
		unregister_chrdev_region(devno,1);
		return -EBUSY;
	}

	return 0;
}

static void led_exit(void)
{
	cdev_del(&cdev);
	device_destroy(cls,devno);
	class_destroy(cls);
	unregister_chrdev_region(devno, 1);
	printk("led_exit\n");
}

MODULE_LICENSE("GPL");
module_init(led_init);
module_exit(led_exit);