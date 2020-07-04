/**********************************************************************************/
/* Copyright (C), 2019, DIAS Automotive Electronic Systems Co; Ltd.               */
/* File Name:    main.c                                                           */
/* Description:  主函数、初始化                                                   */
/*                                                                                */
/* Others:       other description                                                */
/*                                                                                */
/* Processor:    RCAR H3                                                          */
/* Compiler:     AARCH64-POKY-LINUX                                               */
/********************************** 修改历史 **************************************/
/* Date            Version        Author          Description                     */
/* 2019-08-20      V1.0.0.0       陈俊            创建                            */
/**********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <semaphore.h>
#include <sys/syscall.h>
#define __USE_GNU
#include <sched.h>
#include <pthread.h>
#include <fcntl.h>
#include "SchM_Timer.h"
#include "SchM_MainFunc.h"
#include "SchM_TskSch.h"
#include "SchM_Queue.h"

#define PROC_NAME_LEN 25

uint8 u8g_SchM_LCState = SCHM_LC_STATE_NOTRUN;
sem_t* ptg_SemAppTskDone;

/******************************************************************************
*  函数名       | SchM_InitAppProc
*  处理内容     | 初始化应用进程
*  参数         | struct SchM_Process *ptt_Proc：每个核上执行的应用的数组首地址指针
*               | uint8 proc_cnt：每个核上执行的应用个数
*  返回值       | void
*  是否同步     | 是
*  是否可重入   | 否
*  注意事项     | 
******************************************************************************/
void SchM_InitAppProc(struct SchM_Process *ptt_Proc, uint8 u8t_ProcCnt)
{
    /* printf("Enter app porc, app_name = %s\n", ptt_Proc->name); */
    pid_t s32t_pid;
    uint8 i;
    uint8 *ptt_ProcName;
    uint8 u8t_ProcPath[PROC_NAME_LEN] = "./";
    
    for(i = 0; i < u8t_ProcCnt; i++)
    {
        
        ptt_ProcName = ptt_Proc->ptt_ProcName;
        s32t_pid = fork();    
        if(s32t_pid < 0)
        {
            printf("error in fork!");
            exit(1);
        }
        else if (s32t_pid == 0) 
        {
            
            int s32t_ret;
            strcat(u8t_ProcPath, ptt_ProcName);
            s32t_ret = execl(u8t_ProcPath, "&", NULL); 
            if(s32t_ret < 0)
            {
                perror("execl error\n");
                exit(1);
            } 
        } 
        ptt_Proc->s32t_ProcPid = s32t_pid;
        ptt_Proc->ptt_SemInit = sem_open(ptt_Proc->ptt_SemInitName, O_CREAT|O_RDWR, 0666, 0);
        ptt_Proc->ptt_SemShtdwn = sem_open(ptt_Proc->ptt_SemShtdwnName, O_CREAT|O_RDWR, 0666, 0);
        
        ptt_Proc->ptt_SemReinit = sem_open(ptt_Proc->ptt_SemReinitName, O_CREAT|O_RDWR, 0666, 0);

        ptt_Proc->ptt_SemReinitDone = sem_open(ptt_Proc->ptt_SemReinitDoneName, O_CREAT|O_RDWR, 0666, 0);
        ptt_Proc++;
    }    
}

/******************************************************************************
*  函数名       | SchM_WaitAppInitComplete
*  处理内容     | 等待应用进程初始化完成
*  参数         | struct SchM_Process *ptt_Proc：每个核上执行的应用的数组首地址指针
*               | uint8 u8t_ProcCnt：每个核上执行的应用个数
*  返回值       | void
*  是否同步     | 是
*  是否可重入   | 否
*  注意事项     | 
******************************************************************************/
void SchM_WaitAppInitComplete(struct SchM_Process *ptt_Proc, uint8 u8t_ProcCnt)
{
    int i, s32t_ret;
    for(i = 0; i < u8t_ProcCnt; i++)
    {
        s32t_ret = sem_wait(ptt_Proc->ptt_SemInit);
        if(s32t_ret == 0)
        {
            ptt_Proc++;
        }
        else
        {
            /* TBD */
        }
    }
}

/******************************************************************************
*  函数名       | SchM_WaitAppReinitComplete
*  处理内容     | 通知应用进程reinit并等待其完成
*  参数         | struct SchM_Process *ptt_Proc：每个核上执行的应用的数组首地址指针
*               | uint8 u8t_ProcCnt：每个核上执行的应用个数
*  返回值       | void
*  是否同步     | 是
*  是否可重入   | 否
*  注意事项     | 
******************************************************************************/
void SchM_WaitAppReinitComplete(struct SchM_Process *ptt_Proc, uint8 u8t_ProcCnt)
{
    int i, s32t_ret;
    for(i = 0; i < u8t_ProcCnt; i++)
    {
        /*通知应用reinit*/
        sem_post(ptt_Proc->ptt_SemReinit);
        /*等待应用reinit完成信号量*/
        s32t_ret = sem_wait(ptt_Proc->ptt_SemReinitDone);
        if(s32t_ret == 0)
        {
            ptt_Proc++;
        }
        else
        {
            /* TBD */
        }
    }
}

/******************************************************************************
*  函数名       | SchM_WaitAppShtdwnComplete
*  处理内容     | 通知应用程序执行shutdown并等待应用关闭完成
*  参数         | struct SchM_Process *ptt_Proc：每个核上执行的应用的数组首地址指针
*               | uint8 u8t_ProcCnt：每个核上执行的应用个数
*  返回值       | void
*  是否同步     | 是
*  是否可重入   | 否
*  注意事项     | 
******************************************************************************/
void SchM_WaitAppShtdwnComplete(struct SchM_Process *ptt_Proc, uint8 u8t_ProcCnt)
{
    int i, s32t_ret;
    union sigval un_task_notify_sigval;
    un_task_notify_sigval.sival_int = 10;  /*参数10 为Shtdwn的处理*/
    
    for(i = 0; i < u8t_ProcCnt; i++)
    {   
        /*通知应用shutdown*/
        s32t_ret = sigqueue(ptt_Proc->s32t_ProcPid, SIGRTMIN, un_task_notify_sigval);
        if(s32t_ret == 0)
        {
            /*等待应用shutdown完成信号量通知*/
            s32t_ret = sem_wait(ptt_Proc->ptt_SemShtdwn);
            if(s32t_ret == 0)
            {
                ptt_Proc++;
            }
        }
    }

}

/******************************************************************************
*  函数名       | SchM_InitTask
*  处理内容     | 初始化任务
*  参数         | struct SchM_Task *ptt_task：每个核上执行的不同周期的任务
                | struct SchM_Process *ptt_Proc：每个核上执行的应用的数组首地址指针
*               | uint8 u8t_ProcCnt：每个核上执行的应用个数
*  返回值       | void
*  是否同步     | 是
*  是否可重入   | 否
*  注意事项     | 
******************************************************************************/
void SchM_InitTask(struct SchM_Task *ptt_task, struct SchM_Process *ptt_Proc, uint8 u8t_ProcCnt)
{
    int i;
    for(i = 0; i < u8t_ProcCnt; i++)
    {
        if(strcmp(ptt_task->ptt_TskParentName, ptt_Proc->ptt_ProcName) == 0)
        {
            ptt_task->s32t_TskPid = ptt_Proc->s32t_ProcPid;
            ptt_task->ptt_TskSemDone = sem_open(ptt_Proc->ptt_SemTaskDoneName, O_CREAT|O_RDWR, 0666, 0);
            ptt_task->ptt_TskSemHold = sem_open(ptt_Proc->ptt_SemTaskHoldName, O_CREAT|O_RDWR, 0666, 0);
            ptt_task->ptt_TskSemRehold = sem_open(ptt_Proc->ptt_SemTaskReholdName, O_CREAT|O_RDWR, 0666, 0);
            break;
        }
        ptt_Proc++;
    }
}

/******************************************************************************
*  函数名       | SchM_InitLCProc
*  处理内容     | 状态机进程初始化
*  参数         | void
*  返回值       | void
*  是否同步     | 是
*  是否可重入   | 否
******************************************************************************/
void SchM_InitLCProc(void)
{
    pid_t s32t_pid;
    uint8 u8t_ProcPath[PROC_NAME_LEN] = "./";
    int s32t_ret;
    s32t_pid = fork();
    if(s32t_pid < 0)
    {
        printf("error in fork!");
        //exit(1);
    }
    else if (s32t_pid == 0) 
    {
        strcat(u8t_ProcPath, stg_SchM_LCProc.ptt_ProcName);
        s32t_ret = execl(u8t_ProcPath, "&", NULL); 
        if(s32t_ret < 0)
        {
            printf("LC execl error\n");
            //exit(1);
        } 
    } 
    stg_SchM_LCProc.s32t_ProcPid = s32t_pid;
    
    stg_SchM_LCProc.ptt_SemInitApp = sem_open(stg_SchM_LCProc.ptt_SemInitAppName, O_CREAT|O_RDWR, 0666, 0);
    stg_SchM_LCProc.ptt_SemInitAppDone = sem_open(stg_SchM_LCProc.ptt_SemInitAppDoneName, O_CREAT|O_RDWR, 0666, 0);
    stg_SchM_LCProc.ptt_SemInit2Run = sem_open(stg_SchM_LCProc.ptt_SemInit2RunName, O_CREAT|O_RDWR, 0666, 0);
    stg_SchM_LCProc.ptt_SemInit2RunDone = sem_open(stg_SchM_LCProc.ptt_SemInit2RunDoneName, O_CREAT|O_RDWR, 0666, 0);
    stg_SchM_LCProc.ptt_SemAfterrun = sem_open(stg_SchM_LCProc.ptt_SemAfterrunName, O_CREAT|O_RDWR, 0666, 0);
    stg_SchM_LCProc.ptt_SemAfterrunDone = sem_open(stg_SchM_LCProc.ptt_SemAfterrunDoneName, O_CREAT|O_RDWR, 0666, 0);
    stg_SchM_LCProc.ptt_SemRenit2Run = sem_open(stg_SchM_LCProc.ptt_SemRenit2RunName, O_CREAT|O_RDWR, 0666, 0);
    stg_SchM_LCProc.ptt_SemRenit2RunDone = sem_open(stg_SchM_LCProc.ptt_SemRenit2RunDoneName, O_CREAT|O_RDWR, 0666, 0);
    
}

/******************************************************************************
*  函数名       | SchM_InitPerProc
*  处理内容     | 状态机进程初始化
*  参数         | void
*  返回值       | void
*  是否同步     | 是
*  是否可重入   | 否
******************************************************************************/
void SchM_InitPerProc(void)
{
    pid_t s32t_pid;
    uint8 u8t_ProcPath[PROC_NAME_LEN] = "./";
    int s32t_ret;
    s32t_pid = fork();
    if(s32t_pid < 0)
    {
        printf("error in fork!");
        //exit(1);
    }
    else if (s32t_pid == 0) 
    {
        strcat(u8t_ProcPath, stg_SchM_PerProc.ptt_ProcName);
        s32t_ret = execl(u8t_ProcPath, "&", NULL); 
        if(s32t_ret < 0)
        {
            printf("LC execl error\n");
            //exit(1);
        } 
    } 
    stg_SchM_PerProc.s32t_ProcPid = s32t_pid;
    
}

/******************************************************************************
*  函数名       | SchM_InitLCTask
*  处理内容     | 状态机周期任务初始化
*  参数         | void
*  返回值       | void
*  是否同步     | 是
*  是否可重入   | 否
*  注意事项     | 
******************************************************************************/
void SchM_InitLCTask(struct SchM_Task *ptt_task)
{

    ptt_task->s32t_TskPid = stg_SchM_LCProc.s32t_ProcPid;
    ptt_task->ptt_TskSemDone = sem_open(stg_SchM_LCProc.ptt_SemTaskDoneName, O_CREAT|O_RDWR, 0666, 0);
    ptt_task->ptt_TskSemHold = sem_open(stg_SchM_LCProc.ptt_SemTaskHoldName, O_CREAT|O_RDWR, 0666, 0);
    ptt_task->ptt_TskSemRehold = sem_open(stg_SchM_LCProc.ptt_SemTaskReholdName, O_CREAT|O_RDWR, 0666, 0);

}

/******************************************************************************
*  函数名       | SchM_InitPerTask
*  处理内容     | 状态机周期任务初始化
*  参数         | void
*  返回值       | void
*  是否同步     | 是
*  是否可重入   | 否
*  注意事项     | 
******************************************************************************/
void SchM_InitPerTask(struct SchM_Task *ptt_task)
{

    ptt_task->s32t_TskPid = stg_SchM_PerProc.s32t_ProcPid;
    ptt_task->ptt_TskSemDone = sem_open(stg_SchM_PerProc.ptt_SemTaskDoneName, O_CREAT|O_RDWR, 0666, 0);

}

void main(int argc, char**argv)
{
    sint32 i, s32t_ret;

    struct sched_param  param;
    param.sched_priority = SCHM_MAINTHREAD_PRIORITY;
    s32t_ret = sched_setscheduler(getpid(), SCHED_FIFO, &param);
    if(s32t_ret == -1)
    {
        /* error handle */
    }
    
	ptg_SemAppTskDone = sem_open(SCHM_APP_TASK_DONE_NAME, O_CREAT|O_RDWR, 0666, 0);
	/* 初始化存储模块和状态机模块 */
    SchM_InitLCProc();
    SchM_InitPerProc();
    
    /* 等待状态机发送的初始化应用的信号量 */
    s32t_ret = sem_wait(stg_SchM_LCProc.ptt_SemInitApp);
    if(s32t_ret != 0)
    {
        /*error handle*/
    }    

    SchM_InitAppProc(stg_SchM_ProcCore1, SCHM_PROC_CORE1_CNT);
    SchM_WaitAppInitComplete(stg_SchM_ProcCore1, SCHM_PROC_CORE1_CNT);
	
    SchM_InitLCTask(&stg_SchM_Tsk20msCore1[0]);
    SchM_InitPerTask(&stg_SchM_Tsk10msCore1[0]);

    for(i = 1; i < SCHM_TASK_10MS_CORE1_CNT; i++)
    {
        SchM_InitTask(&stg_SchM_Tsk10msCore1[i], stg_SchM_ProcCore1, SCHM_PROC_CORE1_CNT);   
    }
    for(i = 1; i < SCHM_TASK_20MS_CORE1_CNT; i++)
    {
        SchM_InitTask(&stg_SchM_Tsk20msCore1[i], stg_SchM_ProcCore1, SCHM_PROC_CORE1_CNT);    
    }
    for(i = 0; i < SCHM_TASK_50MS_CORE1_CNT; i++)
    {
        SchM_InitTask(&stg_SchM_Tsk50msCore1[i], stg_SchM_ProcCore1, SCHM_PROC_CORE1_CNT);    
    }
    for(i = 0; i < SCHM_TASK_100MS_CORE1_CNT; i++)
    {
        SchM_InitTask(&stg_SchM_Tsk100msCore1[i], stg_SchM_ProcCore1, SCHM_PROC_CORE1_CNT);    
    }
    for(i = 0; i < SCHM_TASK_300MS_CORE1_CNT; i++)
    {
        SchM_InitTask(&stg_SchM_Tsk300msCore1[i], stg_SchM_ProcCore1, SCHM_PROC_CORE1_CNT);    
    }
	printf("init done\n");
    /*通知状态机进程初始化完成*/
    s32t_ret = sem_post(stg_SchM_LCProc.ptt_SemInitAppDone);
    if(s32t_ret != 0)
    {
        /*error handle*/
    }

    SchM_CreateSchThread();
    /*等待状态机开始运行的信号量*/
    s32t_ret = sem_wait(stg_SchM_LCProc.ptt_SemInit2Run);
    u8g_SchM_LCState = SCHM_LC_STATE_RUNNING;
    SchM_CreateTimer(&stg_SchM_Timer_10ms, 10);
    s32t_ret = sem_post(stg_SchM_LCProc.ptt_SemInit2RunDone);
	
    while(1)
    {
        
        s32t_ret = sem_wait(stg_SchM_LCProc.ptt_SemAfterrun);
        
        if(s32t_ret == 0)
        {
		    printf("SCHM: ptt_SemAfterrun done\n");
            u8g_SchM_LCState = SCHM_LC_STATE_AFTERRUN;
            do
            {
			    /*等待调度线程任务全部执行完成通知*/
			    s32t_ret = sem_wait(ptg_SemAppTskDone);
            }
			while(s32t_ret != 0);
            printf("SCHM: app period task done\n");
            /*shutdown app */            
            SchM_WaitAppShtdwnComplete(stg_SchM_ProcCore1, SCHM_PROC_CORE1_CNT);
            /*通知状态机进程afterrun已完成*/
            sem_post(stg_SchM_LCProc.ptt_SemAfterrunDone);
            printf("SCHM: app shutdown done\n");
            while(1)
            {
                /*等待状态机进程的reinit通知*/
                s32t_ret = sem_wait(stg_SchM_LCProc.ptt_SemRenit2Run);
                
                if(s32t_ret == 0)
                {
					printf("SCHM:get reinit sem\n");
				    u8g_SchM_LCState = SCHM_LC_STATE_REINIT;
                    /*通知应用进行reinit操作*/
                    SchM_WaitAppReinitComplete(stg_SchM_ProcCore1, SCHM_PROC_CORE1_CNT);
                    
                    if(s32t_ret != 0)
                    {
                        printf("ReinitDone sem error\n");
                    }
                    /*发送状态机进程reinit完成*/            
                    s32t_ret = sem_post(stg_SchM_LCProc.ptt_SemRenit2RunDone);
                    u8g_SchM_LCState = SCHM_LC_STATE_RUNNING;
					u8g_SchM_SemToMainFlg = 1;
					u64g_SchM_Cnt10ms = 0;
					printf("SCHM: reinint change to run\n");
                    break;
                }
            }
        }
    
        //pause();
    }    
    
}