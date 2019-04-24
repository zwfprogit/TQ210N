#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include "memdev.h"

int dev1_registers[5];	//模拟设备1
int dev2_registers[5];	//模拟设备2

struct cdev cdev; 
dev_t devno;

/*文件打开函数*/
int mem_open(struct inode *inode, struct file *filp)
{
    
    /*获取次设备号，通过次设备号区分在操作的是哪一个设备*/
    int num = MINOR(inode->i_rdev);
    
    if (num==0)
        filp->private_data = dev1_registers;
    else if(num == 1)
        filp->private_data = dev2_registers;
    else
        return -ENODEV;  //无效的次设备号
    
    return 0; 
}

/*文件释放函数*/
int mem_release(struct inode *inode, struct file *filp)
{
	//此处不是对真实的硬件操作，关闭设备时无事可做，直接返回
	return 0;
}

/*读函数*/
static ssize_t mem_read(struct file *filp, char __user *buf, size_t size, loff_t *ppos)
{
  unsigned long p =  *ppos;
  unsigned int count = size;
  int ret = 0;
  int *register_addr = filp->private_data; /*获取设备的寄存器基地址*/

  /*判断读位置是否有效*/
  if (p >= 5*sizeof(int))
    return 0;
  if (count > 5*sizeof(int) - p)
    count = 5*sizeof(int) - p;

  /*读数据到用户空间*/
  if (copy_to_user(buf, register_addr+p, count))
  {
    ret = -EFAULT;
  }
  else
  {
    *ppos += count;
    ret = count;
  }

  return ret;
}

/*写函数*/
static ssize_t mem_write(struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{
  unsigned long p =  *ppos;
  unsigned int count = size;
  int ret = 0;
  int *register_addr = filp->private_data; /*获取设备的寄存器地址*/
  
  /*分析和获取有效的写长度*/
  if (p >= 5*sizeof(int))
    return 0;
  if (count > 5*sizeof(int) - p)
    count = 5*sizeof(int) - p;
    
  /*从用户空间写入数据*/
  if (copy_from_user(register_addr + p, buf, count))
    ret = -EFAULT;
  else
  {
    *ppos += count;
    ret = count;
  }

  return ret;
}

/* seek文件定位函数 whence表示起始位置*/
static loff_t mem_llseek(struct file *filp, loff_t offset, int whence)
{ 
    loff_t newpos;

    switch(whence) {
      case SEEK_SET: 
        newpos = offset;
        break;

      case SEEK_CUR: 
        newpos = filp->f_pos + offset;
        break;

      case SEEK_END: 
        newpos = 5*sizeof(int)-1 + offset;
        break;

      default: 
        return -EINVAL;
    }
    if ((newpos<0) || (newpos>5*sizeof(int)))
    	return -EINVAL;
    	
    filp->f_pos = newpos;
    return newpos;

}

long mem_ioctl(struct file *filep, unsigned int cmd, unsigned long arg)
{
	switch(cmd)
	{
		case MEM_RESTART:
			printk("restart device\n");
			break;
		case MEM_SET:
			printk("set arg is %d\n",arg);
			break;
		default:
			return -EINVAL;
	}
	return 0;
}


/*文件操作结构体*/
static const struct file_operations mem_fops =
{
  .llseek = mem_llseek,
  .read = mem_read,
  .write = mem_write,
  .open = mem_open,
  .release = mem_release,
  .unlocked_ioctl = mem_ioctl,
};


/*设备驱动模块加载函数*/
static int memdev_init(void)
{
  /*初始化cdev结构*/
  cdev_init(&cdev, &mem_fops);
  
  /* 注册字符设备 */
  /* 实际分配出来的是设备号，包括主设备号和次设备号，主设备号可以分配好，但次设备号需要指定，第二个
  参数表示起始次设备号是0，第三个参数表示需要分配多少个次设备号，第四个参数表示哪一个设备在分配，就是
  驱动的名字 */
  alloc_chrdev_region(&devno, 0, 2, "memdev");
  cdev_add(&cdev, devno, 2);
}

/*模块卸载函数*/
static void memdev_exit(void)
{
  cdev_del(&cdev);   /*注销设备*/
  unregister_chrdev_region(devno, 2); /*释放设备号*/
}

MODULE_LICENSE("GPL");

module_init(memdev_init);
module_exit(memdev_exit);
