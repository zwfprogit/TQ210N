#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define BEEP_ON  1
#define BEEP_OFF 0

int main(int argc, char *argv[])
{
     int fd;
     int cmd;
	 int value;
     
     if (argc <2 )
     {
         printf("please enter the second para!\n");
         return 0;	
     }
     cmd = atoi(argv[1]); 
	 value = atoi(argv[2]);

     fd = open("/dev/beep",O_RDWR);
     
     /*if (cmd == 1)
         ioctl(fd,BEEP_ON,value);
     else
         ioctl(fd,BEEP_OFF,value);	
	*/
	for(;;){
		ioctl(fd,BEEP_ON,value);
		sleep(1);
		ioctl(fd,BEEP_OFF,value);
		sleep(1);
	}

     return 0;
}
