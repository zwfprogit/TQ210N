#ifndef __va_start_arg_end_h_
#define __va_start_arg_end_h_

#ifdef __cplusplus
extern "C"{
#endif

#define NULL	((void*)0)

typedef char *myva_list;/* _VALIST */

/* Storage alignment properties */
#define _AUPBND		(sizeof(int)-1)
#define _ADNBND		(sizeof(int)-1)
/* Variable argument list macro definitions */
#define _bnd(X,bnd)	((sizeof(X)+(bnd))& (~(bnd)))
/* 要理解这几个宏需要对C语言如何传递参数有一定了解。与PASCAL相反，与stdcall相同，C语言传递参数时是用push指令从右到左将参数逐个压栈，因此C语言里通过栈指针来访问参数。虽然X86的push一次可以压2，4或8个字节入栈，C语言在压参数入栈时仍然是机器字的size为最小单位的，也就是说参数的地址都是字对齐的，这就是_bnd(X,bnd)存在的原因。另外补充一点常识，不管是汇编还是C，编译出的X86函数一般在进入函数体后立即执行 
*/
#define myva_arg(ap,T)	(*(T*)(((ap)+=(_bnd(T,_AUPBND)))-(_bnd(T,_ADNBND))))
#define myva_end(ap)	(ap=(myva_list)NULL)
/*va_start(ap,A) ，初始化参数指针ap，将函数参数A右边第一个参数的地址赋给ap。 A必须是一个参数的指针，所以此种类型函数至少要有一个普通的参数啊。像下面的例子函数，就是将第二个参数的指针赋给ap。*/
#define myva_start(ap,A) (void)((ap)=((char*)&(A))+(_bnd(A,_AUPBND)))



#ifdef __cplusplus
}
#endif
#endif
