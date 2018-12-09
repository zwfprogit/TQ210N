#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BL1_SIZE			    (16*1024)
#define BL_HEADER_INFO			"                "		//16个空格
#define BL_HEADER_SIZE			16


int main(int argc,char *argv[])
{
	FILE *fp = NULL;
	int file_len = 0;
	unsigned char *buff = NULL,data = 0;    /* 这里要定义成unsigned 否则,否则data加出来的会有负数,导致校验和错误,这里用三星官方给的有点小坑 */
	int buf_len = 0;
	unsigned int checksum = 0,count;
	int i = 0;	
	unsigned int nbytes = 0;

    /* 输入三个参数,分别是自己可执行文件,uboot.bin,输出的16k文件 */
	if(3 != argc)
	{
		printf("argc paramenter number is %d,not 3\n",argc);
		printf("%s <source file> <destination file>\n",argv[0]);
		return -1;
	}	

	buff = (char *)malloc(BL1_SIZE);
	if(NULL == buff)
	{
		printf("malloc error\n");
		return -1;
	}

	/* clear the buff memary */
	memset(buff, 0, BL1_SIZE);

	fp = fopen(argv[1],"rb");
	if(NULL == fp)
	{
		printf("open source file error\n");
		return -1;
	}
	/* 定位到文件尾 */
	fseek(fp,0,SEEK_END);
	/* 得到文件长度 */
	file_len = ftell(fp);	
	/* 定位到文件头 */
	fseek(fp,0,SEEK_SET);
	/* get write BL1 size */
	count = (file_len<(BL1_SIZE- BL_HEADER_SIZE)) ? file_len : (BL1_SIZE - BL_HEADER_SIZE);
	/* write BL1 header info */
	memcpy(buff, BL_HEADER_INFO, BL_HEADER_SIZE);
	/* read fp file count size of buff +... */
	nbytes = fread(buff + BL_HEADER_SIZE, 1, count , fp);
	if(count != nbytes)
	{
		printf("fread %s faile\n  ",argv[1]);
		free(buff);
		fclose(fp);
		return -1;
	}
	fclose(fp);
	fp = NULL;

	/* calculate checksum */
	for(i = 0; i < count; i++ )
	{
		data = *(volatile unsigned char *)(buff + BL_HEADER_SIZE + i);
		checksum += data;
	}

	*(volatile unsigned int *)buff = BL1_SIZE;
	*(volatile unsigned int *)(buff + 8) = checksum;

	fp = fopen(argv[2],"wb");
	if(NULL == fp)
	{
		printf("open aim file error\n");
		return -1;
	}

	nbytes = fwrite(buff, 1, BL1_SIZE, fp);
	if(BL1_SIZE != nbytes)
	{
		printf("write aim file faile\n");
		free(buff);
		fclose(fp);
		return -1;
	}
	
	fclose(fp);
	free(buff);

	return 0;
}
