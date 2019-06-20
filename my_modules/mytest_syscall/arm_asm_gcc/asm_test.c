#if 0
#include <stdio.h>
int main()
{
	int ou1=88,ou2=0;
	int in1=22,in2=44;
	__asm__ __volatile__(
		"mov %0,%2\n\t"
		"mov %1,%3\n\t"
		:"=r"(ou1),"=r"(ou2)
		:"r"(in2),"r"(in1)
	);
	   
	printf("sno ou1=%d,ou2=%d\n",ou1,ou2);
}

#endif

#if 1
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc,const char *argv[])
{
	int fd;
	fd = open("/dev/i2c-0",O_RDWR);
	
	close(fd); 

	return 0;
}
#endif