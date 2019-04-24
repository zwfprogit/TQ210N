//测试程序代码：
#if 1
#include <stdio.h>
#include <fcntl.h>

int main(){
	int fd = open("/dev/buttons", O_RDWR);
	if(fd < 0){
		printf("open error");;
		return 0;
	}

	unsigned char key;
	while(1){
		read(fd, &key, 1);
		printf("The key = %x\n", key);
	}

	close(fd);
}
#else
//相比轮询方式的按键驱动程序，中断方式编写的按键驱动程序可以很大程度上节省CPU资源，因此，推荐使用中断方式。

//但是，这种方式有个弊端，如果一直接收不到按键，程序就会永远阻塞在这里，幸运的是，linux内核提供了poll机制，可以设置延迟时间，如果在这个时间内受到按键消息则取得键值，反之则超时退出。使内核支持poll非常简单，为file_operations的poll成员提供poll处理函数即可。

 

//使内核支持poll还需要以下几步：

//添加poll头文件


//编写poll处理函数：

//static unsigned buttons_poll(struct file *file, poll_table *wait){
//	unsigned int mask = 0;
//	poll_wait(file, &button_waitq, wait);
//
//	if (pressed)
//		mask |= POLLIN | POLLRDNORM;
//
//	return mask;
//}
//将poll处理函数添加给file_operations：

 

 

//.poll    = buttons_poll,
//这样，驱动程序就支持poll机制了。下面是poll方式的测试程序：

 

 

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <poll.h>

int main(int argc, char **argv){
	int fd;
	unsigned char key_val;
	int ret;

	struct pollfd fds[1];
	
	fd = open("/dev/buttons", O_RDWR);
	if (fd < 0){
		printf("can't open!\n");
	}

	fds[0].fd     = fd;
	fds[0].events = POLLIN;
	while (1){
		ret = poll(fds, 1, 5000);
		if (ret == 0){
			printf("time out\n");
		}
		else{
			read(fd, &key_val, 1);
			printf("key_val = 0x%x\n", key_val);
		}
	}
	
	return 0;
}
//这样按键驱动程序就完成了。

#endif