#include "types.h"

#define MP0_1CON  		(*(volatile u32 *)0xE02002E0)
#define	MP0_3CON  		(*(volatile u32 *)0xE0200320)
#define	MP0_6CON 		(*(volatile u32 *)0xE0200380)

#define	NFCONF  		(*(volatile u32 *)0xB0E00000) 
#define	NFCONT  		(*(volatile u32 *)0xB0E00004) 	
#define	NFCMMD  		(*(volatile u32 *)0xB0E00008) 
#define	NFADDR  		(*(volatile u32 *)0xB0E0000C)
#define	NFDATA  		(*(volatile u8 *)0xB0E00010)
#define	NFSTAT  		(*(volatile u32 *)0xB0E00028)

#define	NFMECCD0  		(*(volatile u32 *)0xB0E00014)
#define	NFMECCD1  		(*(volatile u32 *)0xB0E00018)
#define	NFECCPRGECC0 	(*(volatile u32 *)0xB0E20090)
#define	NFECCPRGECC1 	(*(volatile u32 *)0xB0E20094)
#define	NFECCPRGECC2 	(*(volatile u32 *)0xB0E20098)
#define	NFECCPRGECC3 	(*(volatile u32 *)0xB0E2009C)
#define	NFECCCONF  		(*(volatile u32 *)0xB0E20000)
#define	NFECCCONT  		(*(volatile u32 *)0xB0E20020)
#define	NFECCSTAT  		(*(volatile u32 *)0xB0E20030)

#define NFECCSECSTAT	(*(volatile u32 *)0xB0E20040)
#define NFECCERL0		(*(volatile u32 *)0xB0E200C0)
#define NFECCERL1		(*(volatile u32 *)0xB0E200C4)
#define NFECCERL2		(*(volatile u32 *)0xB0E200C8)
#define NFECCERL3		(*(volatile u32 *)0xB0E200CC)
#define NFECCERP0		(*(volatile u32 *)0xB0E200F0)
#define NFECCERP1		(*(volatile u32 *)0xB0E200F4)
#define NFECCERP2		(*(volatile u32 *)0xB0E200F8)
#define NFECCERP3		(*(volatile u32 *)0xB0E200FC)

#define PAGE_SIZE		2048
#define PAGE_PER_BLK	64
#define BLOCK_SIZE		(PAGE_SIZE * (PAGE_PER_BLK))

#define WRITE_ECC		0
#define READ_ECC		1

#define NF8_ReadPage_Adv(a,b,c) (((int(*)(u32, u32, u8*))(*((u32 *)0xD0037F90)))(a,b,c))

/* 等待NAND准备好 */
static void inline nand_wait_ready()
{
	while(!(NFSTAT & (1 << 0)));
}

/* 片选 */
void inline nand_select_chip()
{
	NFCONT &= ~(1 << 1);
}

/* 取消片选 */
void inline nand_deselect_chip()
{
	NFCONT |= (1 << 1);
}

/* 发命令 */
static void inline nand_cmd(u32 cmd)
{
	NFCMMD = cmd;
}

/* 发地址（5个周期） */
static void nand_addr(u32 addr)
{
	u32 col = addr % PAGE_SIZE;	/* 页内偏移 */
	u32 row = addr / PAGE_SIZE;	/* 页地址 */
	
	NFADDR = col & 0xFF;
	NFADDR = (col >> 8) & 0xF;
	NFADDR = row & 0xFF;
	NFADDR = (row >> 8) & 0xFF;
	NFADDR = (row >> 16) & 0x07;
}

/* 读1byte数据 */
static u8 inline nand_read()
{
	return NFDATA;
}

/* 写1byte数据 */
static void inline nand_write(u8 data)
{
	NFDATA = data;
}

/* 读size byte数据 */
static void nand_read_buf(u8 *buf, int size)
{
	int i = 0;
	for (; i < size; i++)
		buf[i] =  NFDATA;
}

/* 写size byte数据 */
static void nand_write_buf(u8 *buf, int size)
{
	int i = 0;
	for (; i < size; i++)
		NFDATA = buf[i];
}

/* 复位NAND */
static void nand_reset()
{
	nand_select_chip();
	nand_cmd(0xFF);
	nand_wait_ready();
	nand_deselect_chip();
}

/* NAND初始化 */
void nand_init()
{
	/*
	** Port Map
	** CE1->Xm0CSn2-> MP01_2
	** CE2->Xm0CSn3-> MP01_3
	** CE3->Xm0CSn4-> MP01_4
	** CE4->Xm0CSn5-> MP01_5
	** CLE->Xm0FCLE-> MP03_0
	** ALE->Xm0FALE-> MP03_1
	** WE->Xm0FWEn->  MP03_2
	** RE->Xm0FREn->  MP03_3
	** RB1->Xm0FRnB0->MP03_4
	** RB2->Xm0FRnB1->MP03_5
	** RB3->Xm0FRnB2->MP03_6
	** RB4->Xm0FRnB3->MP03_7
	** IO[7:0]->Xm0DATA[7:0]->MP0_6[7:0]
	*/
	MP0_1CON &= ~(0xFFFF << 8);
	MP0_1CON |= (0x3333 << 8);
	MP0_3CON = 0x22222222;
	MP0_6CON = 0x22222222;
	
		/* HCLK_PSYS=133MHz(7.5ns) */
	NFCONF =	(0x3 << 23) |	/* Disable 1-bit and 4-bit ECC */
				/* 下面3个时间参数稍微比计算出的值大些（我这里依次加1），否则读写不稳定 */
				(0x3 << 12) |	/* 7.5ns * 2 > 12ns tALS tCLS */
				(0x2 << 8) | 	/* (1+1) * 7.5ns > 12ns (tWP) */
				(0x1 << 4) | 	/* (0+1) * 7.5 > 5ns (tCLH/tALH) */
				(0x0 << 3) | 	/* SLC NAND Flash */
				(0x0 << 2) |	/* 2KBytes/Page */
				(0x1 << 1);		/* 5 address cycle */
				
	/* 
	** The setting all nCE[3:0] zero can not be allowed. Only 
	** one nCE can be asserted to enable external NAND flash 
	** memory. The lower bit has more priority when user set all 
	** nCE[3:0] zeros. 
	*/
	NFCONT =	(0x0 << 16)|	/* Disable Soft Lock */
				(0x1 << 1) |	/* Disable chip select */
				(0x1 << 0);		/* Enable NAND Flash Controller */
				
	nand_reset();
}

/* 读NAND ID */
void nand_read_id(u8 id[])
{
	int i;
	nand_select_chip();
	nand_cmd(0x90);
	NFADDR = 0x00;
	for (i = 0; i < 5; i++)
		id[i] = nand_read();

	nand_deselect_chip();
}

/* 擦除一个块 */
void nand_erase(u32 addr)
{	
	if (addr & (BLOCK_SIZE - 1))
	{
		printf("not block align\n");
		return;
	}
	u32 row = addr / 2048;
	
	nand_select_chip();
	nand_cmd(0x60);
	
	NFADDR = row & 0xFF;			
	NFADDR = (row >> 8) & 0xFF;
	NFADDR = (row >> 16) & 0x07;
	
	nand_cmd(0xD0);
	nand_wait_ready();
	nand_deselect_chip();
}

/* 初始化ECC模块 */
void nand_init_hwecc_8bit(u8 mode)
{	
	if (mode == READ_ECC)
	{
		NFECCCONT &= ~(0x1 << 16);	/* set 8/12/16bit Ecc direction to Encoding */
		NFECCSTAT |= (1 << 24);		/* clear 8/12/16bit ecc encode done */
	}
	else
	{
		NFECCCONT |= (0x1 << 16);	/* set 8/12/16bit Ecc direction to Encoding */
		NFECCSTAT |= (1 << 25);		/* clear 8/12/16bit ecc encode done */
	}	
	NFCONT |= (1 << 5);			/* Initialize main area ECC decoder/encoder */
			  
	NFECCCONF = (511 << 16) |	/* The ECC message size(For 512-byte message, you should set 511) */
				(0x3 << 0);		/* 8-bit ECC/512B */
				
	NFECCCONT |= (0x1 << 2);	/* Initialize main area ECC decoder/ encoder */
	NFCONT &= ~(1 << 7);		/* Unlock Main area ECC   */
}

void nand_calculate_ecc_8bit(u8 *ecc_calc, u8 mode)
{
	int i;
	NFCONT |= (1 << 7);		/* Lock Main area ECC */
	
	if (mode == READ_ECC)
	{
		/* ECC decoding is completed  */
		while (!(NFECCSTAT & (1 << 24)));
	}
	else
	{
		/* ECC encoding is completed  */
		while (!(NFECCSTAT & (1 << 25)));

		u32 nfeccprgecc0 = 0, nfeccprgecc1 = 0, nfeccprgecc2 = 0, nfeccprgecc3 = 0;
			
		/* 读取13 Byte的Ecc Code */
		nfeccprgecc0 = NFECCPRGECC0;
		nfeccprgecc1 = NFECCPRGECC1;
		nfeccprgecc2 = NFECCPRGECC2;
		nfeccprgecc3 = NFECCPRGECC3;

		ecc_calc[0] = nfeccprgecc0 & 0xFF;
		ecc_calc[1] = (nfeccprgecc0 >> 8) & 0xFF;
		ecc_calc[2] = (nfeccprgecc0 >> 16) & 0xFF;
		ecc_calc[3] = (nfeccprgecc0 >> 24) & 0xFF;
		ecc_calc[4] = nfeccprgecc1 & 0xFF;
		ecc_calc[5] = (nfeccprgecc1 >> 8) & 0xFF;
		ecc_calc[6] = (nfeccprgecc1 >> 16) & 0xFF;
		ecc_calc[7] = (nfeccprgecc1 >> 24) & 0xFF;
		ecc_calc[8] = nfeccprgecc2 & 0xFF;
		ecc_calc[9] = (nfeccprgecc2 >> 8) & 0xFF;
		ecc_calc[10] = (nfeccprgecc2 >> 16) & 0xFF;
		ecc_calc[11] = (nfeccprgecc2 >> 24) & 0xFF;
		ecc_calc[12] = nfeccprgecc3 & 0xFF;
	}
}

int nand_correct_data_8bit(u8 *dat)
{
	int ret = 0;
	u32 errNo;
	u32 erl0, erl1, erl2, erl3, erp0, erp1;

	/* Wait until the 8-bit ECC decoding engine is Idle */
	while (NFECCSTAT & (1 << 31));
	/*Note: If 8-bit ECC is used, the valid number of error is until
	8. If the number exceeds the supported error number, it
	means that uncorrectable error occurs*/
	errNo = NFECCSECSTAT & 0x1F;
	erl0 = NFECCERL0;//Error byte location of 1st or 2nd bit error
	erl1 = NFECCERL1;//Error byte location of 3st or 4nd bit error
	erl2 = NFECCERL2;//Error byte location of 5st or 6nd bit error
	erl3 = NFECCERL3;//Error byte location of 7st or 8nd bit error
	//printf("NFECCERL0 = %X\n", erl0);
	//printf("NFECCERL1 = %X\n", erl1);
	//printf("NFECCERL2 = %X\n", erl2);
	//printf("NFECCERL3 = %X\n", erl3);
	
	erp0 = NFECCERP0;//1st-4th Error bit pattern
	erp1 = NFECCERP1;//5st-8th Error bit pattern
	//printf("NFECCERP0 = %X\n", erp0);
	//printf("NFECCERP1 = %X\n", erp1);
	
	if (errNo)
		printf("errNo = %d\n", errNo);
	
	switch (errNo)
	{
	case 8:
		dat[(erl3 >> 16) & 0x3FF] ^= (erp1 >> 24) & 0xFF;
	case 7:
		dat[erl3 & 0x3FF] ^= (erp1 >> 16) & 0xFF;
	case 6:
		dat[(erl2 >> 16) & 0x3FF] ^= (erp1 >> 8) & 0xFF;
	case 5:
		dat[erl2 & 0x3FF] ^= erp1 & 0xFF;
	case 4:
		dat[(erl1 >> 16) & 0x3FF] ^= (erp0 >> 24) & 0xFF;
	case 3:
		dat[erl1 & 0x3FF] ^= (erp0 >> 16) & 0xFF;
	case 2:
		dat[(erl0 >> 16) & 0x3FF] ^= (erp0 >> 8) & 0xFF;
	case 1:
		dat[erl0 & 0x3FF] ^= erp0 & 0xFF;
	case 0:
		break;
	default:
		ret = -1;
		printf("ECC uncorrectable error detected\n");
		break;
	}
	
	return ret;
}

/* 随机读:从任意地址读任意字节的数据 */
void nand_read_random(u8 *buf, u32 addr, u32 size)
{	
	nand_cmd(0);
	nand_addr(addr);
	nand_cmd(0x30);
	nand_wait_ready();
	
	int i;
	u32 col = addr % PAGE_SIZE;	/* 页内偏移 */

	for(i = col; i < size + col; i++)
	{
		nand_cmd(0x05);
		NFADDR = i & 0xFF;
		NFADDR = (i >> 8) & 0xF;
		nand_cmd(0xE0);
		*buf++ = nand_read();
	}
}

void nand_read_oob(u8 *buf, u32 addr, u32 size)
{	
	nand_cmd(0);
	
	u32 col = 2048;	/* 页内偏移 */
	u32 row = addr / PAGE_SIZE;	/* 页地址 */
	
	NFADDR = col & 0xFF;
	NFADDR = (col >> 8) & 0xF;
	NFADDR = row & 0xFF;
	NFADDR = (row >> 8) & 0xFF;
	NFADDR = (row >> 16) & 0x07;
	
	nand_cmd(0x30);
	nand_wait_ready();

	nand_read_buf(buf, size);
}

/* 写一页数据 */
void nand_write_page(u8 *buf, u32 addr, u8 *oob)
{
	if (addr & (PAGE_SIZE - 1))
	{
		printf("not page align\n");
		return;
	}
	
	nand_cmd(0x80);
	nand_addr(addr);
	nand_wait_ready();
	
	nand_write_buf(buf, PAGE_SIZE);
	
	nand_write_buf(oob, 64);
	
	nand_cmd(0x10);
	nand_wait_ready();
}

/* 写一页数据，同时将Ecc Code写入OOB区 */
void nand_write_page_8bit(u8 *buf, u32 addr, u8 *oob)
{
	if (addr & (PAGE_SIZE - 1))
	{
		printf("not page align\n");
		return;
	}
	
	nand_cmd(0x80);
	nand_addr(addr);
	nand_wait_ready();
	
	int i;
	int eccsize = 512;
	int eccbytes = 13;
	int eccsteps = PAGE_SIZE / eccsize;
	int ecctotal = eccsteps * eccbytes;
	
	for (i = 0; i < 12; i++)
		oob[i] = 0xFF;
	
	/* 每次写eccsize个字节，分eccsteps次写完一页数据 */
	for (i = 0; eccsteps; eccsteps--, i += eccbytes, buf += eccsize)
	{
		nand_init_hwecc_8bit(WRITE_ECC);					/* 初始化ECC写 */
		nand_write_buf(buf, eccsize);
		nand_calculate_ecc_8bit(oob + i + 12, WRITE_ECC);	/* 计算ECC校验码 */
	}
	
	nand_write_buf(oob, 64);					/* 将计算出的ECC写入OOB */
	
#if 1
	printf("--------write Ecc---------\n");
	for (i = 0; i < 64; i++)
	{
		if (i % 8 == 0)
			putchar('\n');
		printf("%X ", oob[i]);
	}
	putchar('\n');
#endif	
	
	nand_cmd(0x10);
	nand_wait_ready();
}

int nand_read_page_8bit(u8 *buf, u32 addr)
{
	int err = 0;
	if (addr & (PAGE_SIZE - 1))
	{
		printf("not page align\n");
		return;
	}
	
	u8 oob[64];
	nand_read_oob(oob, addr, 64);

	int i;
#if 1
	printf("--------read Ecc---------\n");
	for (i = 0; i < 64; i++)
	{
		if (i % 8 == 0)
			putchar('\n');
		printf("%X ", oob[i]);
	}
	putchar('\n');
#endif	
	
	nand_cmd(0);
	nand_addr(addr);
	nand_cmd(0x30);
	nand_wait_ready();
	
	int col;
	int eccsize = 512;
	int eccbytes = 13;
	int eccsteps = PAGE_SIZE / eccsize;
	int ecctotal = eccsteps * eccbytes;
	
	/* 每次写eccsize个字节，分eccsteps次写完一页数据 */
	for (i = 0, col = 0; eccsteps; eccsteps--, i += eccbytes, buf += eccsize, col += eccsize)
	{
		/* 页内偏移 */
		nand_cmd(0x05);
		NFADDR = col & 0xFF;
		NFADDR = (col >> 8) & 0xF;
		nand_cmd(0xE0);
		
		nand_init_hwecc_8bit(READ_ECC);			/* 初始化ECC读 */
		nand_read_buf(buf, eccsize);
		/* 下面两种方式都可以 */
#if 0
		nand_cmd(0x05);
		NFADDR = (2048 + i + 12)& 0xFF;
		NFADDR = ((2048 + i + 12) >> 8) & 0xF;
		nand_cmd(0xE0);
		nand_read_buf(oob, eccbytes);
#else
		nand_write_buf(oob + 12 + i, eccbytes);
#endif
		
		nand_calculate_ecc_8bit(0, READ_ECC);	/* 计算ECC校验码 */
		if (nand_correct_data_8bit(buf) < 0)
			err++;
	}
	
	return err;
}

/*
** 三星提供的固化在iROM里的函数
** 使用8位硬件ECC读取1页数据
** 成功返回0，否则返回1
*/
int NF8_ReadPage_8ECC(u32 addr, u8 *buf)
{
	if (addr & (PAGE_SIZE - 1))
	{
		printf("not page align\n");
		return;
	}
	
	int page = addr / PAGE_SIZE;
	int ret = NF8_ReadPage_Adv(page / PAGE_PER_BLK, page % PAGE_PER_BLK, buf);
	return ret;
}
