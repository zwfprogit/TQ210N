#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define IOCTL_GPIO_ON	1
#define IOCTL_GPIO_OFF	0

int main(int argc, char *argv[])
{
     int fd;
     int cmd;
	 int arg;
     
     if (argc <2 )
     {
         printf("please enter the second para!\n");
         return 0;	
     }
     
     cmd = atoi(argv[1]); 
     arg = atoi(argv[2]);
     fd = open("/dev/led",O_RDWR);
     
     if (cmd == 1)
         ioctl(fd,IOCTL_GPIO_ON,arg);
     else
         ioctl(fd,IOCTL_GPIO_OFF,arg);	


     return 0;
}
