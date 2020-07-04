#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

typedef signed   char           sint8;
typedef signed   short          sint16;
typedef signed   int            sint32;
typedef signed   long           sint64;

typedef unsigned char           uint8;
typedef unsigned short          uint16;
typedef unsigned int            uint32;
typedef unsigned long           uint64;

/******************************************************************************
*  函数名      | Comm_GetProcNme
*  处理内容    | 获得进程名称
*  参数        | uint8 *u8t_Comm_ProcNme : 存放进程名的字符串数组指针
*              | 
*  返回值      | sint8 -1：获取进程名失败；0：获取进程名成功
*  是否同步    | 是
*  是否可重入  | 是
*  注意事项    | 无
******************************************************************************/
sint8 Comm_GetProcNme(uint8 *u8t_Comm_ProcNme)
{
    pid_t   s8t_Comm_ProcPid        = 0;
    uint8   u8t_Comm_StatPath[1024] = "";
    FILE  * ptt_Comm_FileFp         = NULL;
    uint8   u8t_Comm_BufTmp[1024]   = "";
    void  * ptt_Comm_Rtn            = NULL;

    if(u8t_Comm_ProcNme == NULL)
    {
        printf("%s :%d : %s:Parameter can not be NULL\n",__FILE__, __LINE__, __FUNCTION__);
    }

    s8t_Comm_ProcPid = getpid();
    
    sprintf(u8t_Comm_StatPath, "/proc/%d/status", s8t_Comm_ProcPid);

    ptt_Comm_FileFp = fopen(u8t_Comm_StatPath, "r");
    if(NULL != ptt_Comm_FileFp)
    {
        ptt_Comm_Rtn = fgets(u8t_Comm_BufTmp, 1024-1, ptt_Comm_FileFp);
        
        if( ptt_Comm_Rtn == NULL )
        {           
            fclose(ptt_Comm_FileFp);
            printf("%s :%d : %s:Fail to get process name\n",__FILE__, __LINE__, __FUNCTION__);
            return -1;
        }
        else
        {
            fclose(ptt_Comm_FileFp);
            sscanf(u8t_Comm_BufTmp, "%*s %s", u8t_Comm_ProcNme);    
        }
    }   
    else
    {
        printf("%s :%d : %s:Fail to open proc dir\n",__FILE__, __LINE__, __FUNCTION__);
        return -1;
    }
    
    return 0;
    
}

/******************************************************************************
*  函数名      | Comm_KeyProduce
*  处理内容    | 键值生成函数
*  参数        | uint8  *ptt_Per_Clt_SrcNme : 需要转换的字符串源地址
*              | uint8  *ptt_Per_Clt_KeyNme ：转换为键值后存放的字符串数组指针
*              | 
*  返回值      | 
*  是否同步    | 是
*  是否可重入  | 是
*  注意事项    | 无
******************************************************************************/
void Comm_KeyProduce(uint8  *ptt_Per_Clt_SrcNme, uint8  *ptt_Per_Clt_KeyNme)
{
    uint8   *ptt_Per_Clt_ScrtKey        = "lipengyu@dias.com.cn";
    uint8    u8t_Per_Clt_ScrtKeyLen     = 0;
    
    if(ptt_Per_Clt_SrcNme || ptt_Per_Clt_KeyNme)
    {
        printf("%s :%d : %s:Parameter can not be NULL\n",__FILE__, __LINE__, __FUNCTION__);
    }
    
    u8t_Per_Clt_ScrtKeyLen = strlen(ptt_Per_Clt_ScrtKey);
    
    while(*ptt_Per_Clt_SrcNme) 
    {
        for(uint8 i = 0; i < u8t_Per_Clt_ScrtKeyLen; i++)
        {
            *ptt_Per_Clt_KeyNme = *ptt_Per_Clt_SrcNme ^ (int)ptt_Per_Clt_ScrtKey[i];
        }
        ptt_Per_Clt_KeyNme++;
        ptt_Per_Clt_SrcNme++;
    }
 
}
