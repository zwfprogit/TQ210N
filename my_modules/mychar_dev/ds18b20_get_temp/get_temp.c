#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DS18B20_DEV		"/sys/devices/w1_bus_master1/28-0115a32b24ff/w1_slave"

/* return 0 on success,or -1 on erro */
int get_temp(float *value, const char *dev)
{
	int ret;
	FILE *fp;
	char buf1[100], buf2[100];
	fp = fopen(dev, "r");
	if (fp == NULL)
	{
		perror("fopen");
		return -1;
	}
	/*
	c5 01 4b 46 7f ff 0c 10 78 : crc=78 YES
	c5 01 4b 46 7f ff 0c 10 78 t=28312
	*/
	fgets(buf1, 100, fp);
	sscanf(buf1, "%*[^:]: crc=%[^ ]%s\n", buf2);
	if (strcmp(buf2, "YES") == 0)
	{
		fgets(buf1, 100, fp);
		sscanf(buf1, "%*[^t]t=%s\n", buf2);
		*value = atoi(buf2) * 1.0 / 1000;
		ret = 0;
	}
	else
	{
		fprintf(stderr, "CRC error\n");
		ret = -1;
	}

	fclose(fp);
	return ret;
}

int main(int argc, char **argv)
{
	float value;
	if (get_temp(&value, DS18B20_DEV) == 0)
		printf("%.3f øC\n", value);
	return 0;
}
