#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include "../driver/led.h"

int main(int argc, char *argv[])
{
     int fd;
     int cmd;
     
     if (argc <2 )
     {
         printf("please enter the second para!\n");
         return 0;	
     }
     
     cmd = atoi(argv[1]); 
     
     fd = open("/dev/myled",O_RDWR);
	 if(fd < 0){
		printf("open dev fail!\n"); 
		exit(1);
	 }
     
     if (cmd == 1)
         ioctl(fd,LED_ON);
     else
         ioctl(fd,LED_OFF);	


     return 0;
}
