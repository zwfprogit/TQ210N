#include<stdio.h>

#define offset_of(type,member) \
((size_t)&((type*)0)->member)

#define container_of(ptr,type,member) ({\
const typeof(((type*)0)->member)*__mptr = (ptr); \
(type*)(((char*)__mptr)- offset_of(type,member));})


typedef struct{
	int i;
	int j;
	char k;
}test;


int main()
{
	printf("1，直接计算\n");
	test t,*tt,*ttt;
	printf("&t=%p\n",&t);
	printf("&t.k=%p\n",&t.k);
	printf("&((test*)0)->k=%ld\n",((size_t)&((test*)0)->k));
	
	printf("2，通过container_of计算\n");
	tt = container_of(&t.k,test,k);
	printf("tt=%p\n",tt);
	printf("&tt->k=%p\n",&tt->k);
	
	printf("3，直接展开container_of计算\n");
	ttt = ({typeof(((test*)0)->k) * __mptr = (&t.k);
	(test *)(((char*)__mptr) - (size_t)(&((test*)0)->k));});
	printf("ttt=%p\n",ttt);
	printf("&ttt->k=%p\n",&ttt->k);
	
	return 0;
}