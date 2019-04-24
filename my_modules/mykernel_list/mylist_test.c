#include<linux/init.h>
#include<linux/module.h>
#include<linux/list.h>

struct score
{
	int num;
	int englist;
	int math;
	struct list_head list;
};

struct list_head score_head; 
struct score stu1,stu2,stu3;
struct list_head *pos;
struct score *tmp;

static int  mylist_module_init()
{
	INIT_LIST_HEAD(&score_head);//创建节点
	stu1.num = 1;
	stu1.englist = 90;
	stu1.math = 98;
	list_add_tail(&stu1.list,&score_head);

	stu2.num = 2;
	stu2.englist = 89;
	stu2.math = 97;
	list_add_tail(&stu2.list,&score_head);

	stu3.num = 3;
	stu3.englist = 88;
	stu3.math = 96;
	list_add_tail(&stu3.list,&score_head);

	//遍历链表
	list_for_each(pos,&score_head){
		tmp = list_entry(pos,struct score,list);
		printk("num:%d English:%d Math:%d\n",tmp->num,tmp->englist,tmp->math);
	}

	return 0;
}

static void mylist_module_exit()
{
	list_del(&stu1.list);
	list_del(&stu2.list);
}
module_init(mylist_module_init);
module_exit(mylist_module_exit);
MODULE_LICENSE("GPL");