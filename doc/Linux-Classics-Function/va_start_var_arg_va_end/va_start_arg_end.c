#include"va_start_arg_end.h"
#include <stdio.h>
#include<string.h>
#include<math.h>


char *myitoa(int i, char *str)
{
    int mod, div = fabs(i), index = 0;
    char *start, *end, temp;

    do
    {
        mod = div % 10;
        str[index++] = '0' + mod;
        div = div / 10;
    }while(div != 0);

    if (i < 0)
        str[index++] = '-';

    str[index] = '\0';

    for (start = str, end = str + strlen(str) - 1;
        start < end; start++, end--)
    {
        temp    = *start;
        *start    = *end;
        *end    = temp;
    }
    
    return str;
}

void myprint(const char *fmt, ...)
{
    char str[100];
    unsigned int len, i, index;
    int iTemp;
    char *strTemp;
    myva_list args;

    myva_start(args, fmt);
    len = strlen(fmt);
    for(i=0, index=0; i<len; i++)
    {
        if (fmt[i] != '%')    /* 非格式化参数 */
        {
            str[index++] = fmt[i];
        }
        else                /* 格式化参数 */
        {
            switch(fmt[i+1])
            {
            case 'd':        /* 整型 */
            case 'D':
                iTemp = myva_arg(args, int);
                strTemp = myitoa(iTemp, str+index);
                index += strlen(strTemp);
                i++;
                break;
            case 's':        /* 字符串 */
            case 'S':
                strTemp = myva_arg(args, char*);
                //strcpy(str + index, strTemp);
                index += strlen(strTemp);
                i++;
                break;
            default:
                str[index++] = fmt[i];
            }
        }
    }
    str[index] = '\0';
    myva_end(args);

    printf("%s\n",str);        
}

int main()
{
    myprint("Version: %d; Modifier: %s\n", -958, "lingd");
    return 0;
}