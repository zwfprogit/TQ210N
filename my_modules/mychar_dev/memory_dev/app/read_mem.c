#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

int main()
{
     int fd = 0;
     int dst = 0;
     fd = open("/dev/memdev0",O_RDWR);   //打开文件，权限可读可写
     read(fd,&dst,sizeof(int));          //
	 printf("dst is %d\n",dst);
     close(fd);
	 
     return 0;
}

