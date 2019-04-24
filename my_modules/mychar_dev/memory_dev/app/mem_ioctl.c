#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "../driver/memdev.h"
int main()
{
	int fd = 0;

	fd = open("/dev/memdev0",O_RDWR);
	ioctl(fd,MEM_SET,115200);

	ioctl(fd,MEM_RESTART);
	close(fd);

	return 0;
}