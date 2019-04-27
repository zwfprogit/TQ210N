#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/io.h>
#include <linux/device.h>
/*static inline unsigned long __must_check copy_from_user(void *to, const void __user *from, unsigned long n)
static inline unsigned long __must_check copy_to_user(void __user *to, const void *from, unsigned long n)
头文件：#include <asm/uaccess.h>
*/
#include <asm/uaccess.h>
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

ssize_t led_write(struct file *file, const char __user *buf, size_t count, loff_t *offset)
{
	int cmd;
	int tmp;
	if(copy_from_user(&cmd,buf,count)){//拷贝用户调用write传进来的值
		return -EINVAL;
	}
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
	.write = led_write,
};

static int led_init(void)
{
#if 0
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
#else
	printk(KERN_INFO "led /dev entries driver\n");
	devno = register_chrdev(0,"myled",&led_fops);
	/*执行字符设备注册函数，只不过是在内核数组里面把led_fops结构填充进去，
	但是还不会生成系统信息，那么系统信息使怎么生成的呢？调用class_create创建一个类，
	对应/sys/class/myclass下的类，再调用device_create该类下创建设备，
	对应/sys/class/myclass/myled设备，然后busybox下的mdev就会根据系统信息
	自动在/dev/myled设备节点*/
	printk(KERN_INFO "led major num:%d\n",devno);

	cls = class_create(THIS_MODULE,"myclass");
	if(IS_ERR(cls)){
		class_destroy(cls);
		return PTR_ERR(cls);
	}
	mydev = device_create(cls,NULL, MKDEV(devno, 0),NULL,"myled");
	if(IS_ERR(mydev)){
		unregister_chrdev(MKDEV(devno,0),"myled");
		return PTR_ERR(mydev);;
	}
	return 0;
#endif
}

static void led_exit(void)
{
#if 0
	cdev_del(&cdev);
	device_destroy(cls,devno);
	class_destroy(cls);
	unregister_chrdev_region(devno, 1);
#else
	device_destroy(cls,MKDEV(devno,0));
	class_destroy(cls);
	unregister_chrdev(MKDEV(devno,0),"myled");
#endif
	printk("led_exit\n");
}

MODULE_LICENSE("GPL");
module_init(led_init);
module_exit(led_exit);