/********************************************************************************************************/
/* Copyright (C), 2018, DIAS Automotive Electronic Systems Co; Ltd.                                     */
/* File Name:    SchM_Clt.h                                                                             */
/* Description:  IECU midware task                                                                      */
/*                                                                                                      */
/* Others:                                                                                              */
/*                                                                                                      */
/* Processor:    R-CAR H3                                                                               */
/* Compiler:     aarch64-poky-linux                                                                     */
/************************************** 修改历史 ********************************************************/
/* Date                 Version        Author          Description                                      */
/* 2019-07-20         V1.01.01.01.00   李鹏宇          基线创建                                         */
/* 2020-01-20         V1.01.02.01.00   李鹏宇          代码优化                                         */
/* 2020-02-28         V1.01.02.02.00   陈俊            代码优化&bug修复                                 */
/********************************************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/syscall.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#define __USE_GNU 
#include <sched.h>
#include <pthread.h>

#include "SchMClient.h"
#include "SchCommon.h"

/*是否显示调试Log*/
//#define SCHM_Clt_LOGON

/*-----------------------------------------------------*/
/*以下三个宏定义的参数可以在之后的优化中使用读取配置文件的方式*/
/*分别为线程运行的cpu,调度方法,优先级*/
#define SCHM_CLT_CPUSELECT 2
#define SCHM_CLT_SCHEDMETHOD SCHED_FIFO
#define SCHM_CLT_SCHEDPRIORITY 96
/*----------------------------------------------------*/

/*每个进程最大周期任务个数，可配置*/
#define  SCHM_CLT_MAXTSKCNT   5  

#define LIB_TSKDONEWAIT  0    /*任务执行完成进入等待状态，会发送信号量通知*/
#define LIB_TSKWAIT      1    /*任务其他等待状态，不会发送信号量*/

/*唤醒和打断信号标志位，分别为接受到唤醒信号和接收到挂起信号或进入等待状态*/
#define LIB_IRRUN      1
#define LIB_IRSTOP     0

/*保存的log调试信息缓冲*/  
uint8   u8g_SchM_Clt_LogBuf[1024*1024] = {0};
uint8   u8g_SchM_Clt_TskNo[SCHM_CLT_MAXTSKCNT] = {0, 0, 0, 0, 0};
/*运行状态变量数组*/
uint8   u8g_SchM_Clt_RunState[SCHM_CLT_MAXTSKCNT] = {0, 0, 0, 0, 0};

/*任务收到的运行或挂起的请求，收到运行请求++，收到挂起或执行完成挂起--*/
sint8   s8g_SchM_Clt_RunReq[SCHM_CLT_MAXTSKCNT] = {0, 0, 0, 0, 0};

/*log文件的文件描述符*/
FILE    *ptg_SchM_Clt_FD;

/*是否reinit的标志位*/
uint8   u8g_SchM_Clt_ReInitOn = 0;

/*线程挂起与恢复功能所需要的条件变量初始化*/
pthread_mutex_t stg_SchM_Clt_Mutex[SCHM_CLT_MAXTSKCNT] = 
{
    PTHREAD_MUTEX_INITIALIZER, 
    PTHREAD_MUTEX_INITIALIZER, 
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER, 
    PTHREAD_MUTEX_INITIALIZER
};

pthread_cond_t stg_SchM_Clt_Cond[SCHM_CLT_MAXTSKCNT] = 
{
    PTHREAD_COND_INITIALIZER, 
    PTHREAD_COND_INITIALIZER, 
    PTHREAD_COND_INITIALIZER, 
    PTHREAD_COND_INITIALIZER, 
    PTHREAD_COND_INITIALIZER
};

#define  SCHM_CLT_MAXSEMNAMELEN    30
/*存放应用名以及信号量名称的数组*/
uint8   ptg_SchM_Clit_ProcNme[SCHM_CLT_MAXSEMNAMELEN]           = "";
uint8   ptg_SchM_Clit_SemInitNme[SCHM_CLT_MAXSEMNAMELEN]        = "";
uint8   ptg_SchM_Clit_SemHoldNme[SCHM_CLT_MAXSEMNAMELEN]        = "";
uint8   ptg_SchM_Clit_SemDownNme[SCHM_CLT_MAXSEMNAMELEN]        = "";
uint8   ptg_SchM_Clit_SemShutDownNme[SCHM_CLT_MAXSEMNAMELEN]    = "";
uint8   ptg_SchM_Clit_SemReHoldNme[SCHM_CLT_MAXSEMNAMELEN]      = "";
uint8   ptg_SchM_Clit_SemReInitNme[SCHM_CLT_MAXSEMNAMELEN]      = "";
uint8   ptg_SchM_Clit_SemReInitDoneNme[SCHM_CLT_MAXSEMNAMELEN]  = "";

/*log文件保存的文件名数组*/
uint8   ptg_SchM_Clit_LogFleNme[SCHM_CLT_MAXSEMNAMELEN]         = "";

/*通知调度程序使用的信号量*/
sem_t   *ptg_SchM_Clt_SemInit   = NULL;
sem_t   *ptg_SchM_Clt_SemHold   = NULL;
sem_t   *ptg_SchM_Clt_SemDone   = NULL;
sem_t   *ptg_SchM_Clt_SemShtdwn = NULL;
sem_t   *ptg_SchM_Clt_SemReHold = NULL;
sem_t   *ptg_SchM_Clt_SemReInit = NULL;
sem_t   *ptg_SchM_Clt_SemReInitDone = NULL;

/*函数指针的全局结构体*/
struct SchM_Clt_AppOps *ptg_SchM_Clt_LibOps;

/******************************************************************************
*  函数名      | SchM_Clt_SemNmePrdc
*  处理内容    | 获得进程名称并生成信号量名称
*  参数        | 无
*              | 
*  返回值      | sint8 : -1,信号量名称生成失败; 0,信号量名称生成成功
*  是否同步    | 是
*  是否可重入  | 否
*  注意事项    | 无
******************************************************************************/
sint8 SchM_Clt_SemNmePrdc()
{
	sint8  u8t_SchM_Clt_NmePrdcRet = 0;
	
    u8t_SchM_Clt_NmePrdcRet = Comm_GetProcNme(ptg_SchM_Clit_ProcNme);
	if(u8t_SchM_Clt_NmePrdcRet < 0 )
	{
		printf("%s :%d : %s:Comm_GetProcNme error\n",__FILE__, __LINE__, __FUNCTION__);
		return -1;
	}
    /*生成创建信号量时所需的信号量字符串名称以及Log文件名称*/
    strcpy(ptg_SchM_Clit_SemInitNme, ptg_SchM_Clit_ProcNme);
    strcpy(ptg_SchM_Clit_SemHoldNme, ptg_SchM_Clit_ProcNme);
    strcpy(ptg_SchM_Clit_SemDownNme, ptg_SchM_Clit_ProcNme);
    strcpy(ptg_SchM_Clit_SemShutDownNme, ptg_SchM_Clit_ProcNme);
    strcpy(ptg_SchM_Clit_SemReHoldNme, ptg_SchM_Clit_ProcNme);
    strcpy(ptg_SchM_Clit_SemReInitNme, ptg_SchM_Clit_ProcNme);
    strcpy(ptg_SchM_Clit_SemReInitDoneNme, ptg_SchM_Clit_ProcNme);
    strcpy(ptg_SchM_Clit_LogFleNme, ptg_SchM_Clit_ProcNme);
    
    strcat(ptg_SchM_Clit_SemInitNme, "_init_sem");
    strcat(ptg_SchM_Clit_SemHoldNme, "_hold_sem");
    strcat(ptg_SchM_Clit_SemDownNme, "_done_sem");
    strcat(ptg_SchM_Clit_SemShutDownNme, "_shtdwn_sem");
    strcat(ptg_SchM_Clit_SemReHoldNme, "_rehold_sem");
    strcat(ptg_SchM_Clit_SemReInitNme, "_reinit_sem");
    strcat(ptg_SchM_Clit_SemReInitDoneNme, "_reinit_done_sem");
    strcat(ptg_SchM_Clit_LogFleNme, ".log");
	
	return 0;

}

/******************************************************************************
*  函数名      | SchM_Clt_SemCrt
*  处理内容    | 通信所需要的信号量的创建
*  参数        | 无
*              | 
*  返回值      | sint8 : -1,信号量创建异常; 0,信号量创建正常
*  是否同步    | 是
*  是否可重入  | 否
*  注意事项    | 无
******************************************************************************/
sint8 SchM_Clt_SemCrt()
{
	
    /*打开调度进程所创建的信号量，初始化完成以及挂起等动作完毕所需要的信号量通知*/       
    ptg_SchM_Clt_SemInit = sem_open(ptg_SchM_Clit_SemInitNme, O_CREAT|O_RDWR, 0666, 0);
    if(ptg_SchM_Clt_SemInit == SEM_FAILED)
    {
        fprintf(stderr, "%s,%s,%d SemInit open error\n", ptg_SchM_Clit_ProcNme, __FILE__, __LINE__);
        return -1 ;
    }
    
    ptg_SchM_Clt_SemHold = sem_open(ptg_SchM_Clit_SemHoldNme, O_CREAT|O_RDWR, 0666, 0);
    if(ptg_SchM_Clt_SemHold == SEM_FAILED)
    {
        fprintf(stderr, "%s,%s,%d SemHold open error\n", ptg_SchM_Clit_ProcNme, __FILE__, __LINE__);
        return -1 ;
    }
    
    ptg_SchM_Clt_SemDone = sem_open(ptg_SchM_Clit_SemDownNme, O_CREAT|O_RDWR, 0666, 0);
    if(ptg_SchM_Clt_SemDone == SEM_FAILED)
    {
        fprintf(stderr, "%s,%s,%d SemDown open error\n", ptg_SchM_Clit_ProcNme, __FILE__, __LINE__);
        return -1 ;
    }
    
    ptg_SchM_Clt_SemShtdwn = sem_open(ptg_SchM_Clit_SemShutDownNme, O_CREAT|O_RDWR, 0666, 0);
    if(ptg_SchM_Clt_SemShtdwn == SEM_FAILED)
    {
        fprintf(stderr, "%s,%s,%d SemShutDown open error\n", ptg_SchM_Clit_ProcNme, __FILE__, __LINE__);
        return -1 ;
    }
    
    ptg_SchM_Clt_SemReHold = sem_open(ptg_SchM_Clit_SemReHoldNme, O_CREAT|O_RDWR, 0666, 0);
    if(ptg_SchM_Clt_SemReHold == SEM_FAILED)
    {
        fprintf(stderr, "%s,%s,%d SemReHold open error\n", ptg_SchM_Clit_ProcNme, __FILE__, __LINE__);
        return -1 ;
    }
	
    ptg_SchM_Clt_SemReInit = sem_open(ptg_SchM_Clit_SemReInitNme, O_CREAT|O_RDWR, 0666, 0);
    if(ptg_SchM_Clt_SemReInit == SEM_FAILED)
    {
        fprintf(stderr, "%s,%s,%d SemReInit open error\n", ptg_SchM_Clit_ProcNme, __FILE__, __LINE__);
        return -1 ;
    }
	
    ptg_SchM_Clt_SemReInitDone = sem_open(ptg_SchM_Clit_SemReInitDoneNme, O_CREAT|O_RDWR, 0666, 0);
    if(ptg_SchM_Clt_SemReInitDone == SEM_FAILED)
    {
        fprintf(stderr, "%s,%s,%d SemReInitDone open error\n", ptg_SchM_Clit_ProcNme, __FILE__, __LINE__);
        return -1 ;
    }
	
    return 0;
}

/******************************************************************************
*  函数名      | SchM_Clt_CpuSet
*  处理内容    | 设置程序运行的Cpu
*  参数        | 无
*              | 
*  返回值      | sint8 : -1,信号量创建异常; 0,信号量创建正常
*  是否同步    | 是
*  是否可重入  | 是
*  注意事项    | 无
******************************************************************************/
sint8 SchM_Clt_CpuSet()
{
    
    struct sched_param  stt_SchM_Clt_SchParm;
    sint8  u8t_SchM_Clt_CpuSetRet = 0;
    
    /*设置进程运行的cpu*/
    cpu_set_t cpu_info;
    CPU_ZERO(&cpu_info);
    CPU_SET(SCHM_CLT_CPUSELECT, &cpu_info);
    u8t_SchM_Clt_CpuSetRet = sched_setaffinity(0, sizeof(cpu_set_t), &cpu_info);
    if (0 != u8t_SchM_Clt_CpuSetRet)
    {
        fprintf(stderr, "%s,%s,%d set affinity failed: %s\n", ptg_SchM_Clit_ProcNme, __FILE__, __LINE__, strerror(u8t_SchM_Clt_CpuSetRet));
        return -1;
    }
    
    /*设置进程运行的优先级*/
    stt_SchM_Clt_SchParm.sched_priority = SCHM_CLT_SCHEDPRIORITY;
    
    /*设置进程运行时所使用的调度方式*/
    u8t_SchM_Clt_CpuSetRet = sched_setscheduler(getpid(), SCHM_CLT_SCHEDMETHOD, &stt_SchM_Clt_SchParm);
    if(0 != u8t_SchM_Clt_CpuSetRet)
    {
        fprintf(stderr, "%s,%s,%d sched_setscheduler failed: %s\n", ptg_SchM_Clit_ProcNme, __FILE__, __LINE__, strerror(u8t_SchM_Clt_CpuSetRet));
        return -1;
    }
}

/******************************************************************************
*  函数名      | SchM_Clt_SigWaitNotify
*  处理内容    | 线程运行完成的等待挂起函数
*  参数        | uint8 u8t_SchM_Clt_WaitType ：    等待挂起类型标志位
*              | uint8 u8t_SchM_Clt_TherdNum ：    需要挂起的线程标号
*              |
*  返回值      | uint8 : 1,挂起操作处理异常; 0,挂起操作处理正常
*  是否同步    | 否
*  是否可重入  | 是
*  注意事项    | 无
******************************************************************************/
uint8 SchM_Clt_SigWaitNotify(uint8 u8t_SchM_Clt_WaitType, uint8 u8t_SchM_Clt_TherdNum)
{
    
    sint8               u8t_SchM_Clt_SigWaitNotifyRet = 0;
    uint8               u8t_SchM_Clt_WakeUp = 0;
	
	//struct timespec stg_SchM_Task_StartTime;
	//clock_gettime(CLOCK_REALTIME, &stg_SchM_Task_StartTime);
	
	u8t_SchM_Clt_SigWaitNotifyRet = pthread_mutex_trylock(&stg_SchM_Clt_Mutex[u8t_SchM_Clt_TherdNum]);
	if(u8t_SchM_Clt_SigWaitNotifyRet == 0)
	{
		/*获得锁之后，在pthread_cond_wait释放锁之前收到挂起信号会进入case5的情况*/
		u8g_SchM_Clt_RunState[u8t_SchM_Clt_TherdNum] = LIB_IRSTOP;
		if(u8t_SchM_Clt_WaitType == LIB_TSKDONEWAIT)
		{
			/*任务执行完成后进入此分支，发送完成信号量*/
			u8t_SchM_Clt_SigWaitNotifyRet = sem_post(ptg_SchM_Clt_SemDone);
		}
		else
		{
			/*挂起不发送信号量*/
			//printf("wait and not send sem, task_req = %d\n", task_req);
		}
		u8t_SchM_Clt_SigWaitNotifyRet = pthread_cond_wait(&stg_SchM_Clt_Cond[u8t_SchM_Clt_TherdNum], &stg_SchM_Clt_Mutex[u8t_SchM_Clt_TherdNum]);
		/*此处收到挂起信号，会进入case3的情况*/
		u8g_SchM_Clt_RunState[u8t_SchM_Clt_TherdNum] = LIB_IRRUN;
		
		//sprintf(u8g_SchM_Clt_LogBuf, "%s%ld.%ld client app %s after pthread_cond_wait and before unlock, u8t_SchM_Clt_HoldTypeFlag = %d, u8t_SchM_Clt_TherdNum = %d, u8g_SchM_Clt_RunState[u8t_SchM_Clt_TherdNum] = %d, tid = %d\n",  u8g_SchM_Clt_LogBuf, stg_SchM_Task_StartTime.tv_sec, stg_SchM_Task_StartTime.tv_nsec, ptg_SchM_Clit_ProcNme, u8t_SchM_Clt_WaitType, u8t_SchM_Clt_TherdNum, u8g_SchM_Clt_RunState[u8t_SchM_Clt_TherdNum], syscall(SYS_gettid));	
		
	}
	else
	{
		printf("unknow status, task_wait cannot get lock\n");
		return 1;
	}
	pthread_mutex_unlock(&stg_SchM_Clt_Mutex[u8t_SchM_Clt_TherdNum]);
	/*从解锁后，一直到任务完成后调用此函数获得锁之前，收到挂起信号会进入case4的情况*/
	
	if(s8g_SchM_Clt_RunReq[u8t_SchM_Clt_TherdNum] < 1)
	{
		/*此分支在case1，case2，case3的处理后进入，使线程再度挂起，但不会发送信号量*/
		SchM_Clt_SigWaitNotify(LIB_TSKWAIT, u8t_SchM_Clt_TherdNum);
	}	

    return 0;
    
}
/******************************************************************************
*  函数名      | SchM_Clt_SigHoldProc
*  处理内容    | 线程挂起处理函数
*  参数        | u8t_SchM_Clt_pNum：挂起的线程
*              | 
*  返回值      | 0：正确  1：错误
*  是否同步    | 否
*  是否可重入  | 是
*  注意事项    | 无
******************************************************************************/
uint8 SchM_Clt_HoldProc(uint8 u8t_SchM_Clt_pNum)
{
	sint32 s32t_ret;
	s32t_ret = pthread_mutex_trylock(&stg_SchM_Clt_Mutex[u8t_SchM_Clt_pNum]);
	if(s32t_ret == 0)
	{
		s8g_SchM_Clt_RunReq[u8t_SchM_Clt_pNum] --;
		
		if(u8g_SchM_Clt_RunState[u8t_SchM_Clt_pNum] == LIB_IRRUN)
		{
		    /*case4处理，应用任务正在执行中，收到挂起信号*/			/*case6处理，上个周期是case4的情况，本周期收到运行信号后且应用执行完成前，又收到挂起信号*/
			s32t_ret = sem_post(ptg_SchM_Clt_SemHold);
			s32t_ret = pthread_cond_wait(&stg_SchM_Clt_Cond[u8t_SchM_Clt_pNum], &stg_SchM_Clt_Mutex[u8t_SchM_Clt_pNum]);			
		}
		else if(u8g_SchM_Clt_RunState[u8t_SchM_Clt_pNum] == LIB_IRSTOP)
		{
		    /*case1和case2处理，收到开始运行信号，但运行信号的信号处理函数正在执行中时收到挂起信号*/
			s32t_ret = sem_post(ptg_SchM_Clt_SemReHold);
		}
		else
		{
		    /*错误判断*/
			printf("unknow status, task_status = %d\n", u8g_SchM_Clt_RunState[u8t_SchM_Clt_pNum]);
			return 1;
		}
		pthread_mutex_unlock(&stg_SchM_Clt_Mutex[u8t_SchM_Clt_pNum]);
	}
	else
	{
		/*case3处理，线程收到运行信号后，从pthread_cond_wait挂起恢复后，执行pthread_mutex_unlock之前收到挂起信号*/
		if(s8g_SchM_Clt_RunReq[u8t_SchM_Clt_pNum] > 0)
		{
			s8g_SchM_Clt_RunReq[u8t_SchM_Clt_pNum] --;
		}
		/*case5处理，应用任务完成后，进入自动挂起函数，获得锁，但在pthread_cond_wait之前收到挂起信号*/
		else
		{

		}	
		s32t_ret = sem_post(ptg_SchM_Clt_SemReHold);
	}
	return 0;
}

/******************************************************************************
*  函数名      | SchM_Clt_ThreadHoldNotify
*  处理内容    | 线程运行的挂起信号通知函数
*  参数        | uint32 u32l_SchM_Clt_SigNub：收到的信号的值
*              | 
*  返回值      | 无
*  是否同步    | 否
*  是否可重入  | 是
*  注意事项    | 无
******************************************************************************/
void SchM_Clt_ThreadHoldNotify(uint32 u32l_SchM_Clt_SigNub)//, siginfo_t *ptt_SchM_Clt_SigInfo, void *ptl_SchM_Clt_Reservd)
{
	sint8      u8t_SchM_Clt_HoldProcRet = 0;
	uint8      u8t_SchM_Clt_pNum;
	u8t_SchM_Clt_pNum = u32l_SchM_Clt_SigNub - SIGRTMIN - 1;
    u8t_SchM_Clt_HoldProcRet = SchM_Clt_HoldProc(u8t_SchM_Clt_pNum);
	if(u8t_SchM_Clt_HoldProcRet < 0)
	{
		fprintf(stderr, "%s,%s,%d SchM_Clt_SigWaitNotify error\n", ptg_SchM_Clit_ProcNme, __FILE__, __LINE__);
	}

}

/******************************************************************************
*  函数名      | SchM_Clt_ThreadHandle
*  处理内容    | 周期任务的线程函数
*  参数        | 无
*              | 
*  返回值      | 无
*  是否同步    | 否
*  是否可重入  | 否
*  注意事项    | 无
******************************************************************************/
void * SchM_Clt_ThreadHandle(void* ptt_SchM_Clt_pNum)
{

    uint8               u8l_SchM_Clt_ThreadHandleRet = 0;
	uint8               i;
    struct sigaction    stt_SchM_Clt_Act;

    static  uint8       u8l_SchM_Clt_PrtCnt          = 0 ;
    uint8 u8t_SchM_Clt_pNum;
	u8t_SchM_Clt_pNum = *(uint8*)ptt_SchM_Clt_pNum;
	sprintf(u8g_SchM_Clt_LogBuf, "%sSchM_Clt_ThreadHandle, u8t_SchM_Clt_pNum = %d, ptt_SchM_Clt_pNum = %d, tid = %d\n",  u8g_SchM_Clt_LogBuf, u8t_SchM_Clt_pNum, *(uint8*)ptt_SchM_Clt_pNum, syscall(SYS_gettid));
    /*设置线程的信号屏蔽*/
    sigemptyset(&stt_SchM_Clt_Act.sa_mask);
    for(i = 0; i <= ptg_SchM_Clt_LibOps->Sch_Clt_ThrdNumber; i++)
	{
		if(i == u8t_SchM_Clt_pNum + 1)
		{
			continue;
		}
		else
		{
			sigaddset(&stt_SchM_Clt_Act.sa_mask, SIGRTMIN+i);
		}
	}
    
    pthread_sigmask(SIG_BLOCK, &stt_SchM_Clt_Act.sa_mask, NULL);

    /*设置信号处理函数,线程将自己挂起时需要调用该函数*/
    stt_SchM_Clt_Act.sa_flags = SA_SIGINFO;
    stt_SchM_Clt_Act.sa_sigaction = SchM_Clt_ThreadHoldNotify;
    
    u8l_SchM_Clt_ThreadHandleRet = sigaction(SIGRTMIN + u8t_SchM_Clt_pNum + 1, &stt_SchM_Clt_Act,NULL);
    if(u8l_SchM_Clt_ThreadHandleRet == -1)
    {
        fprintf(stderr, "%s,%s,%d sigaction error: %s\n", ptg_SchM_Clit_ProcNme, __FILE__, __LINE__, strerror(u8l_SchM_Clt_ThreadHandleRet));
        exit(EXIT_FAILURE);
    }

	/*调用该线程的初始化函数，具体的内容由应用填写*/
    if(ptg_SchM_Clt_LibOps->Sch_Clt_ThrdInit[u8t_SchM_Clt_pNum] != NULL)
    {
        ptg_SchM_Clt_LibOps->Sch_Clt_ThrdInit[u8t_SchM_Clt_pNum]();
    }else
    {
        fprintf(stderr, "%s,%s,%d Thread 5 init error\n", ptg_SchM_Clit_ProcNme, __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    /*线程初始化完成之后需要首先将自己挂起,等待调度程序的运行信号*/
    SchM_Clt_SigWaitNotify(LIB_TSKWAIT, u8t_SchM_Clt_pNum);

    while(1)
    {
        /*周期任务所需要处理的具体工作*/  
        if(ptg_SchM_Clt_LibOps->Sch_Clt_ThrdWork[u8t_SchM_Clt_pNum] != NULL)
        {
            ptg_SchM_Clt_LibOps->Sch_Clt_ThrdWork[u8t_SchM_Clt_pNum]();
        }else
        {
            fprintf(stderr, "%s,%s,%d Thread 5 work error\n", ptg_SchM_Clit_ProcNme, __FILE__, __LINE__);
            exit(EXIT_FAILURE);
        }
        
		s8g_SchM_Clt_RunReq[u8t_SchM_Clt_pNum]--;
        #ifdef SCHM_CLT_LOGON
    
        if (u8l_SchM_Clt_PrtCnt ++ >= 1000 )
        {
            u8l_SchM_Clt_PrtCnt = 0 ;
            fwrite(u8g_SchM_Clt_LogBuf, sizeof(char), sizeof(u8g_SchM_Clt_LogBuf), ptg_SchM_Clt_FD);
            fclose(ptg_SchM_Clt_FD);
        }else
        {
            
        }
    
        #endif  

        /*线程初始化完成之后需要首先将自己挂起,等待调度程序的运行信号*/
        SchM_Clt_SigWaitNotify(LIB_TSKDONEWAIT,u8t_SchM_Clt_pNum);
    }

    return NULL;

}
/******************************************************************************
*  函数名      | SchM_Clt_ThrdCrt
*  处理内容    | 创建所需要的线程
*  参数        | pthread_attr_t stt_SchM_Clt_Attr ：线程属性结构体
*              | 
*  返回值      | sint8 : -1,线程创建失败; 0,线程创建成功
*  是否同步    | 否
*  是否可重入  | 否
*  注意事项    | 无
******************************************************************************/
sint8 SchM_Clt_ThrdCrt(pthread_attr_t stt_SchM_Clt_Attr)
{
    
    sint8               u8l_SchM_Clt_ThrdCrtRet = 0;
    
	/*创建线程所需要的变量*/
    pthread_t           u64t_SchM_Clt_Thread[5];
	uint8 i;
	for(i = 0; i < ptg_SchM_Clt_LibOps->Sch_Clt_ThrdNumber; i++)
	{
		u8g_SchM_Clt_TskNo[i] = i;
		/*创建带参数的线程时，不能直接用i作为参数传递，需要先将i赋值给全局变量*/
        u8l_SchM_Clt_ThrdCrtRet = pthread_create(&u64t_SchM_Clt_Thread[i], &stt_SchM_Clt_Attr, SchM_Clt_ThreadHandle, (void*)&u8g_SchM_Clt_TskNo[i]);
        if (u8l_SchM_Clt_ThrdCrtRet != 0)
        {
            fprintf(stderr, "%s,%s,%d pthread_create5 failed: %s\n", ptg_SchM_Clit_ProcNme, __FILE__, __LINE__, strerror(u8l_SchM_Clt_ThrdCrtRet));
            return -1 ;
        }		
	}

    return 0;
        
}

/******************************************************************************
*  函数名      | SchM_Clt_ThreadRunNotify
*  处理内容    | 线程运行的通知函数
*  参数        | 无
*              | 
*  返回值      | 无
*  是否同步    | 否
*  是否可重入  | 否
*  注意事项    | 无
******************************************************************************/
void SchM_Clt_ThreadRunNotify(uint8 u8t_SchM_Clt_WakeNum)
{

    sint8               u8t_SchM_Clt_RunNotifyRet = 0;
	
	/*pthread_cond_signal之前收到挂起信号为case1情况*/
	s8g_SchM_Clt_RunReq[u8t_SchM_Clt_WakeNum]++;
	u8t_SchM_Clt_RunNotifyRet = pthread_cond_signal(&stg_SchM_Clt_Cond[u8t_SchM_Clt_WakeNum]);
	/*pthread_cond_signal之后收到挂起信号为case2情况*/

    return ;

}

/******************************************************************************
*  函数名      | SchM_Clt_ThreadRunSigHand
*  处理内容    | 主进程的信号处理函数,通过主进程将子线程拉起,使其恢复正常的运行
*  参数        | uint32 u32l_SchM_Clt_SigNub：		信号的编号
*              | siginfo_t *ptl_SchM_Clt_SigInfo：	信号信息结构体，保存了传递的参数
*              | void *ptl_SchM_Clt_Reservd：		保留的指针
*              |
*  返回值      | 无
*  是否同步    | 否
*  是否可重入  | 否
*  注意事项    | 无
******************************************************************************/
void SchM_Clt_ThreadRunSigHand(uint32 u32l_SchM_Clt_SigNub, siginfo_t *ptt_SchM_Clt_SigInfo, void *ptl_SchM_Clt_Reservd)
{

    uint8                   u8t_SchM_Clt_ThreadNum = 0;

    /*获得信号传递的参数*/
    u8t_SchM_Clt_ThreadNum = ptt_SchM_Clt_SigInfo->si_int;
	
    #ifdef CTAPPREDICTION_LIB_LOGON
    
        printf("%s,%s,%d receive the data from siqueue and ptt_SchM_Clt_SigInfo->si_int is %d\n", ptg_SchM_Clit_ProcNme, __FILE__, __LINE__, ptt_SchM_Clt_SigInfo->si_int);
    
    #endif
	/*add by cj*/
	struct timespec stg_SchM_Task_StartTime;
	clock_gettime(CLOCK_REALTIME, &stg_SchM_Task_StartTime);
	if(u8t_SchM_Clt_ThreadNum != 20)
	{
		sprintf(u8g_SchM_Clt_LogBuf, "%ld.%ld client app %s get running signal, argu = %d, tid = %d\n",  stg_SchM_Task_StartTime.tv_sec, stg_SchM_Task_StartTime.tv_nsec, ptg_SchM_Clit_ProcNme, u8t_SchM_Clt_ThreadNum, syscall(SYS_gettid));
	}
	/***/
    /*根据参数的值来判断当前需要让哪一个线程运行*/
	if(u8t_SchM_Clt_ThreadNum > 0 && u8t_SchM_Clt_ThreadNum < 6)
	{
		SchM_Clt_ThreadRunNotify(u8t_SchM_Clt_ThreadNum-1);
	}
	else if(u8t_SchM_Clt_ThreadNum == 10)
	{
		/*参数10 为Shtdwn的处理，调用应用的Shtdwn函数*/
		if(ptg_SchM_Clt_LibOps->Sch_Clt_AppShut != NULL)
		{
			ptg_SchM_Clt_LibOps->Sch_Clt_AppShut();
			sem_post(ptg_SchM_Clt_SemShtdwn);
		}else
		{
			fprintf(stderr, "%s,%s,%d Sch_Clt_AppShut error\n", ptg_SchM_Clit_ProcNme, __FILE__, __LINE__);
		}
		u8g_SchM_Clt_ReInitOn = 1;		
	}
	/*调试用*/
	else if(u8t_SchM_Clt_ThreadNum == 20)
	{
		printf("log: %s\n", u8g_SchM_Clt_LogBuf);
	}
	else
	{
		fprintf(stderr, "%s,%s,%d wrong parameter\n", ptg_SchM_Clit_ProcNme, __FILE__, __LINE__);
	}

}

/******************************************************************************
*  函数名      | SchM_Clt_TaskThreadInit
*  处理内容    | 初始化函数
*  参数        | 无
*              | 
*  返回值      | sint8 : -1,线程初始化异常; 0,线程初始化正常
*  是否同步    | 是
*  是否可重入  | 否
*  注意事项    | 无
******************************************************************************/
sint8 SchM_Clt_TaskThreadInit()
{
    sint8                   u8t_SchM_Clt_TskThrdInitRet = 0;

    struct sigaction    stt_SchM_Clt_Act;

    pthread_attr_t stt_SchM_Clt_Attr;
 
    pthread_attr_init(&stt_SchM_Clt_Attr);
    
    ptg_SchM_Clt_LibOps = (struct SchM_Clt_AppOps *)malloc(sizeof(struct SchM_Clt_AppOps));
    
	/*应用层实现的注册函数，主要作用是将函数指针传递给客户端*/
    SchM_Clt_TaskRgst(ptg_SchM_Clt_LibOps);
    
	/*生成与调度程序通信所需要的信号量的名称字符串*/
    u8t_SchM_Clt_TskThrdInitRet = SchM_Clt_SemNmePrdc();
	if(u8t_SchM_Clt_TskThrdInitRet < 0 )
	{
        fprintf(stderr, "%s,%s,%d Sem creat error!\n", ptg_SchM_Clit_ProcNme, __FILE__, __LINE__);
        return -1;		
	}

	/*创建与调度程序通信所需要的信号量*/
    u8t_SchM_Clt_TskThrdInitRet = SchM_Clt_SemCrt();
    if(u8t_SchM_Clt_TskThrdInitRet < 0)
    {
        fprintf(stderr, "%s,%s,%d Sem creat error!\n", ptg_SchM_Clit_ProcNme, __FILE__, __LINE__);
        return -1;
    }

	/*设置线程运行绑定的CPU*/
    u8t_SchM_Clt_TskThrdInitRet = SchM_Clt_CpuSet();
    if(u8t_SchM_Clt_TskThrdInitRet < 0)
    {
        fprintf(stderr, "%s,%s,%d Cpu set error!\n", ptg_SchM_Clit_ProcNme, __FILE__, __LINE__);
        return -1;
    }
        
    /*设置主进程的信号处理函数，用做唤醒挂起的线程*/
    sigemptyset(&stt_SchM_Clt_Act.sa_mask); 
    stt_SchM_Clt_Act.sa_flags = SA_SIGINFO;
    stt_SchM_Clt_Act.sa_sigaction = (void *) SchM_Clt_ThreadRunSigHand;
    if(sigaction(SIGRTMIN,&stt_SchM_Clt_Act,NULL) == -1){
        fprintf(stderr, "%s,%s,%d cycle_thread_init sigaction error!\n", ptg_SchM_Clit_ProcNme, __FILE__, __LINE__);
        return -1 ;
    }
	
	/*创建各个任务的线程*/
    u8t_SchM_Clt_TskThrdInitRet = SchM_Clt_ThrdCrt(stt_SchM_Clt_Attr);
    if(u8t_SchM_Clt_TskThrdInitRet < 0)
    {
        fprintf(stderr, "%s,%s,%d Thread creat error!\n", ptg_SchM_Clit_ProcNme, __FILE__, __LINE__);
        return -1;
    }
    
	/*根据线程的数量来进行信号的屏蔽*/
	uint8 i;

	for(i = 0; i < ptg_SchM_Clt_LibOps->Sch_Clt_ThrdNumber; i++)
	{
		sigaddset(&stt_SchM_Clt_Act.sa_mask, SIGRTMIN+i+1);
	}
 
    pthread_sigmask(SIG_BLOCK, &stt_SchM_Clt_Act.sa_mask, NULL);

    /*初始化完成之后给调度进程发送信号量，作为初始化完毕的通知*/
    sem_post(ptg_SchM_Clt_SemInit);

    return 0;

}

/******************************************************************************
*  函数名      | main
*  处理内容    | 主函数
*  参数        | 无
*              | 
*  返回值      | sint8
*  是否同步    | 是
*  是否可重入  | 否
*  注意事项    | 无
******************************************************************************/
sint8 main()
{

    uint8                 u8l_SchM_Clt_MainRet = 0;

    /*主进程初始化*/
    u8l_SchM_Clt_MainRet = SchM_Clt_TaskThreadInit();
    if(u8l_SchM_Clt_MainRet < 0)
    {
        fprintf(stderr, "%s,%s,%d task thread init error!\n", ptg_SchM_Clit_ProcNme, __FILE__, __LINE__);
        return u8l_SchM_Clt_MainRet;
    }

    /*应用层初始化函数调用*/
    if(ptg_SchM_Clt_LibOps->Sch_Clt_AppInit != NULL)
    {
        u8l_SchM_Clt_MainRet = (uint8) ptg_SchM_Clt_LibOps->Sch_Clt_AppInit();
        if(u8l_SchM_Clt_MainRet < 0)
        {
            fprintf(stderr, "%s,%s,%d task app init error!\n", ptg_SchM_Clit_ProcNme, __FILE__, __LINE__);
            return -1;
        }       
    }
    else
    {
        fprintf(stderr, "%s,%s,%d Sch_Clt_AppInit error!\n", ptg_SchM_Clit_ProcNme, __FILE__, __LINE__);
        return -1;
    }

    #ifdef SCHM_CLT_LOGON
    
        if( (ptg_SchM_Clt_FD =fopen(ptg_SchM_Clit_SemReHoldNme, "w+") ) == NULL)
        {
            printf("fopen,error\n");
        }
        
    #endif      

    /*主进程循环函数暂时不做任何处理*/
    while(1)
    {
        /*判断是否进行重新初始化的操作，该标志位在运行信号的参数10内设置*/
		if(u8g_SchM_Clt_ReInitOn == 1)
		{
			sem_wait(ptg_SchM_Clt_SemReInit);
			if(ptg_SchM_Clt_LibOps->Sch_Clt_AppReInit != NULL)
			{
				ptg_SchM_Clt_LibOps->Sch_Clt_AppReInit();
				sem_post(ptg_SchM_Clt_SemReInitDone);
			}else
			{
				fprintf(stderr, "%s,%s,%d AppReInit is NULL\n", ptg_SchM_Clit_ProcNme, __FILE__, __LINE__);
			}
			
			u8g_SchM_Clt_ReInitOn = 0;
		}else
		{
			sleep(1);
		}
		
        //printf("%s,%s,%d Main run!\n", ptg_SchM_Clit_ProcNme, __FILE__, __LINE__);
        
        #ifdef SCHM_CLT_LOGON
    
            printf("%s,%s,%d Main run!\n", ptg_SchM_Clit_ProcNme, __FILE__, __LINE__);
        
        #endif  

    }

    return u8l_SchM_Clt_MainRet;

}