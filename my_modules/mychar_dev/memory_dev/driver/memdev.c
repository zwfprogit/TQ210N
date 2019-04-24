#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include "memdev.h"

int dev1_registers[5];	//ģ���豸1
int dev2_registers[5];	//ģ���豸2

struct cdev cdev; 
dev_t devno;

/*�ļ��򿪺���*/
int mem_open(struct inode *inode, struct file *filp)
{
    
    /*��ȡ���豸�ţ�ͨ�����豸�������ڲ���������һ���豸*/
    int num = MINOR(inode->i_rdev);
    
    if (num==0)
        filp->private_data = dev1_registers;
    else if(num == 1)
        filp->private_data = dev2_registers;
    else
        return -ENODEV;  //��Ч�Ĵ��豸��
    
    return 0; 
}

/*�ļ��ͷź���*/
int mem_release(struct inode *inode, struct file *filp)
{
	//�˴����Ƕ���ʵ��Ӳ���������ر��豸ʱ���¿�����ֱ�ӷ���
	return 0;
}

/*������*/
static ssize_t mem_read(struct file *filp, char __user *buf, size_t size, loff_t *ppos)
{
  unsigned long p =  *ppos;
  unsigned int count = size;
  int ret = 0;
  int *register_addr = filp->private_data; /*��ȡ�豸�ļĴ�������ַ*/

  /*�ж϶�λ���Ƿ���Ч*/
  if (p >= 5*sizeof(int))
    return 0;
  if (count > 5*sizeof(int) - p)
    count = 5*sizeof(int) - p;

  /*�����ݵ��û��ռ�*/
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

/*д����*/
static ssize_t mem_write(struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{
  unsigned long p =  *ppos;
  unsigned int count = size;
  int ret = 0;
  int *register_addr = filp->private_data; /*��ȡ�豸�ļĴ�����ַ*/
  
  /*�����ͻ�ȡ��Ч��д����*/
  if (p >= 5*sizeof(int))
    return 0;
  if (count > 5*sizeof(int) - p)
    count = 5*sizeof(int) - p;
    
  /*���û��ռ�д������*/
  if (copy_from_user(register_addr + p, buf, count))
    ret = -EFAULT;
  else
  {
    *ppos += count;
    ret = count;
  }

  return ret;
}

/* seek�ļ���λ���� whence��ʾ��ʼλ��*/
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


/*�ļ������ṹ��*/
static const struct file_operations mem_fops =
{
  .llseek = mem_llseek,
  .read = mem_read,
  .write = mem_write,
  .open = mem_open,
  .release = mem_release,
  .unlocked_ioctl = mem_ioctl,
};


/*�豸����ģ����غ���*/
static int memdev_init(void)
{
  /*��ʼ��cdev�ṹ*/
  cdev_init(&cdev, &mem_fops);
  
  /* ע���ַ��豸 */
  /* ʵ�ʷ�����������豸�ţ��������豸�źʹ��豸�ţ����豸�ſ��Է���ã������豸����Ҫָ�����ڶ���
  ������ʾ��ʼ���豸����0��������������ʾ��Ҫ������ٸ����豸�ţ����ĸ�������ʾ��һ���豸�ڷ��䣬����
  ���������� */
  alloc_chrdev_region(&devno, 0, 2, "memdev");
  cdev_add(&cdev, devno, 2);
}

/*ģ��ж�غ���*/
static void memdev_exit(void)
{
  cdev_del(&cdev);   /*ע���豸*/
  unregister_chrdev_region(devno, 2); /*�ͷ��豸��*/
}

MODULE_LICENSE("GPL");

module_init(memdev_init);
module_exit(memdev_exit);
