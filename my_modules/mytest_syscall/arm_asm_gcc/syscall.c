/**
方法一：通过syscall实现系统调用：
syscall - indirect(间接) system call
#define _GNU_SOURCE         // See feature_test_macros(7) 
#include <unistd.h>
#include <sys/syscall.h>   // For SYS_xxx definitions 

long syscall(long number, ...);
number:用具体的系统调序号，具体的参数通过可变参数传递
return value：
0：indicates success
-1：indicates an error，error code is stored in errno.
*/
#if 0 //此方法验证可以实现系统调用

#include <syscall.h>
#include <sys/types.h>
#include <stdio.h>

int main()
{
	long id1 = 0;
	//CALL(sys_myself_call),自己实现的系统调用序号是380，传递参数190
	id1 = syscall(380,190);
	printf("%ld\n",id1);

	return 0;
}
#endif

/**
方法二：通过内联汇编实现系统调用：
swi{cond}#immed
*/
#if 0
#include <stdio.h>
#include <stdlib.h>

int main()
{
	int a,b = 190;
	asm volatile(
		"ldr r7,=380\n\t"
		"mov r0,%1\n\t"	//参数传递
		"swi #0\n\t"
		"mov %0,r0\n\t"
		:"=r"(a)
		:"r"(b)
		:"memory"
	);
	printf("swi sys_myself_call result=%d\n",a);
	return 0;
}
#endif

/**
方法二：通过内联汇编实现系统调用：
svc{cond}#immed
*/
#if 1
#include <stdio.h>
#include <stdlib.h>

int main()
{
	int a,b = 190;
	asm volatile(
		"ldr r7,=380\n\t"
		"mov r0,%1\n\t"	//参数传递
		"svc #0\n\t"
		"mov %0,r0\n\t"
		:"=r"(a)
		:"r"(b)
		:"memory"
	);
	printf("svc sys_myself_call result=%d\n",a);
	return 0;
}
#endif