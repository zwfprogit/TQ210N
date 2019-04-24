#include<linux/module.h>
#include<linux/init.h>
#include<linux/cdev.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/gpio.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/sched.h> 
#include <linux/wait.h>
#include <linux/uaccess.h>

static dev_t devno;
static struct cdev cdev;
static struct class* buttons_class;
static struct device* buttons_device;

static wait_queue_head_t button_waitq;

static volatile int pressed = 0;
static unsigned char key_val;

struct key_desc{
	unsigned int  pin;
	unsigned char value;
};

static struct key_desc key_descs[8] = {
	[0] = {
		.pin = S5PV210_GPH0(0),
		.value = 0x00,
	},

	[1] = {
		.pin = S5PV210_GPH0(1),
		.value = 0x01,
	},

	[2] = {
		.pin = S5PV210_GPH0(2),
		.value = 0x02,
	},

	[3] = {
		.pin = S5PV210_GPH0(3),
		.value = 0x03,
	},

	[4] = {
		.pin = S5PV210_GPH0(4),
		.value = 0x04,
	},

	[5] = {
		.pin = S5PV210_GPH0(5),
		.value = 0x05,
	},

	[6] = {
		.pin = S5PV210_GPH2(6),
		.value = 0x06,
	},

	[7] = {
		.pin = S5PV210_GPH2(7),
		.value = 0x07,
	},
};

static irqreturn_t buttons_irq(int irq, void *dev_id){
	volatile struct key_desc *key = (volatile struct key_desc *)dev_id;

	if(gpio_get_value(key->pin)){
		key_val = key->value|0x80;
	}
	else{
		key_val = key->value;
	}

	pressed = 1;
	wake_up_interruptible(&button_waitq);

	return IRQ_RETVAL(IRQ_HANDLED);
}

static int buttons_open(struct inode *inode, struct file *file){
	int ret;
	unsigned long flags = IRQF_SHARED|IRQF_TRIGGER_RISING|IRQF_TRIGGER_FALLING;
	printk("buttons driver buttons_open start\n");
	ret = request_irq(IRQ_EINT(0),   buttons_irq, flags, "key1", &key_descs[0]);
	if(ret)
		return ret;
	ret = request_irq(IRQ_EINT(1),   buttons_irq, flags, "key2", &key_descs[1]);
	if(ret)
		return ret;
 	ret = request_irq(IRQ_EINT(2),   buttons_irq, flags, "key3", &key_descs[2]);
	if(ret)
		return ret;
 	ret = request_irq(IRQ_EINT(3),   buttons_irq, flags, "key4", &key_descs[3]);
	if(ret)
		return ret;
	ret = request_irq(IRQ_EINT(4),   buttons_irq, flags, "key5", &key_descs[4]);
	if(ret)
		return ret;
	ret = request_irq(IRQ_EINT(5),   buttons_irq, flags, "key6", &key_descs[5]);
	if(ret)
		return ret;
	ret = request_irq(IRQ_EINT(22),  buttons_irq, flags, "key7", &key_descs[6]);
	if(ret)
		return ret;
	ret = request_irq(IRQ_EINT(23),  buttons_irq, flags, "key8", &key_descs[7]);
	if(ret)
		return ret;
	printk("buttons driver buttons_open end\n");
	return 0;
}

static ssize_t buttons_read(struct file * file, char __user *data, size_t count, loff_t *loff){
	if(count != 1){
		printk(KERN_ERR "The driver can only give one key value once!\n");
		return -ENOMEM;
	}

	wait_event_interruptible(button_waitq, pressed);
	pressed = 0;

	if(copy_to_user(data, &key_val, 1)){
		printk(KERN_ERR "The driver can not copy the data to user area!\n");
		return -ENOMEM;
	}
	
	return 0;
}

static int buttons_close(struct inode *inode, struct file *file){
	free_irq(IRQ_EINT(0),  &key_descs[0]);
	free_irq(IRQ_EINT(1),  &key_descs[1]);	
	free_irq(IRQ_EINT(2),  &key_descs[2]);
	free_irq(IRQ_EINT(3),  &key_descs[3]);
	free_irq(IRQ_EINT(4),  &key_descs[4]);
	free_irq(IRQ_EINT(5),  &key_descs[5]);
	free_irq(IRQ_EINT(22), &key_descs[6]);
	free_irq(IRQ_EINT(23), &key_descs[7]);
	return 0;
}

struct file_operations buttons_ops = {
	.open    = buttons_open,
	.read    = buttons_read,
	.release = buttons_close,
};

/*设备驱动模块加载函数*/
int buttons_init(void)
{
	int ret;
	// 1 初始化cdev结构
	cdev_init(&cdev, &buttons_ops);
	cdev.owner = THIS_MODULE;
	// 2  分配字符设备号
	/*实际分配出来的是设备号，包括主设备号和次设备号，主设备号可以分配好，但次设备号需要指定，第二个
	参数表示起始次设备号是0，第三个参数表示需要分配多少个次设备号，第四个参数表示哪一个设备在分配，就是
	驱动的名字 */
	ret = alloc_chrdev_region(&devno,0,1,"buttons");
	if(ret){
		printk(KERN_ERR "alloc char device region faild!\n");
		return ret;
	}
	// 3 添加字符设备
	ret = cdev_add(&cdev, devno, 1);
	if(ret){
		printk(KERN_ERR "add char device faild!\n");
		goto add_error;
	}
	// 4 
	buttons_class = class_create(THIS_MODULE, "buttonsdrv");
	if(IS_ERR(buttons_class)){
		printk(KERN_ERR "create class error!\n");
		goto class_error;
	}

	buttons_device = device_create(buttons_class, NULL, devno, NULL, "buttons");
	if(IS_ERR(buttons_device)){
		printk(KERN_ERR "create buttons device error!\n");
		goto device_error;
	}

	init_waitqueue_head(&button_waitq);

	return 0;
	
	
device_error:
	class_destroy(buttons_class);
class_error:
	cdev_del(&cdev);
add_error:
	unregister_chrdev_region(devno,1);

	return -ENODEV;
}

void buttons_exit(void)
{
	device_destroy(buttons_class, devno);
	class_destroy(buttons_class);
	cdev_del(&cdev);
	unregister_chrdev_region(devno, 1);
}

module_init(buttons_init);
module_exit(buttons_exit);
MODULE_LICENSE("GPL");
