/**********************************************************************************/
/* Copyright (C), 2019, DIAS Automotive Electronic Systems Co; Ltd.               */
/* File Name:    timer.c                                                          */
/* Description:  定时器                                                           */
/*                                                                                */
/* Others:       定时器创建及处理                                                 */
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
#include "SchM_Queue.h"
#include "SchM_TskSch.h"
#include "SchM_Timer.h"

timer_t stg_SchM_Timer_10ms;
uint64 u64g_SchM_Cnt10ms;
uint8 u8g_SchM_TimerStartFlg = SCHM_TIMER_TRIGGER;
struct timespec stg_SchM_Timer_StartTime;
struct timespec sts_SchM_LastTimerStart;

uint8 u8g_SchM_Log[1024*1024] = {0};
uint8 wait_task_done_flag = 0;
/******************************************************************************
*  函数名       | SchM_Timer_Sig_Proc
*  处理内容     | 定时器触发处理函数
*  参数         | void
*  返回值       | void
*  是否同步     | 否
*  是否可重入   | 否
*  注意事项     | 
******************************************************************************/
void SchM_Timer_Sig_Proc(void)
{
    u8g_SchM_TimerStartFlg = SCHM_TIMER_TRIGGER;
    u64g_SchM_Cnt10ms++;
    int ret;
    uint8 u8t_TskCurrStat;
    struct timespec st_t_end ;
    clock_gettime(CLOCK_REALTIME, &stg_SchM_Timer_StartTime);
	

	
    /* log */
    sprintf(u8g_SchM_Log, "%s%ld.%ld 10ms timer cnt = %d  \n", u8g_SchM_Log, stg_SchM_Timer_StartTime.tv_sec, stg_SchM_Timer_StartTime.tv_nsec, u64g_SchM_Cnt10ms);
    /*
    if(u64g_SchM_Cnt10ms % 1000 == 0)
    {
        printf("[%ld.%ld]SchM: 10s pass. timer cnt = %ld\n", stg_SchM_Timer_StartTime.tv_sec, stg_SchM_Timer_StartTime.tv_nsec, u64g_SchM_Cnt10ms);
    }
    */
    /***********************计算两次10ms任务的触发时间间隔/***********************/
    long int u64t_delay_10ms;
    u64t_delay_10ms = stg_SchM_Timer_StartTime.tv_nsec - sts_SchM_LastTimerStart.tv_nsec;
    if(u64t_delay_10ms < 0)
    {
        u64t_delay_10ms += SCHM_SECOND;
    }
    if((u64t_delay_10ms/SCHM_MILLSECOND < 9) || (u64t_delay_10ms/SCHM_MILLSECOND > 11) && u64g_SchM_Cnt10ms > 1)
    {
        /*error handle*/
        printf("10ms delay, cnt = %d\n", u64g_SchM_Cnt10ms);
        printf("%ld.%ld last start time\n",  sts_SchM_LastTimerStart.tv_sec, sts_SchM_LastTimerStart.tv_nsec, u64g_SchM_Cnt10ms);
        printf("%ld.%ld current start time\n",  stg_SchM_Timer_StartTime.tv_sec, stg_SchM_Timer_StartTime.tv_nsec, u64g_SchM_Cnt10ms);
    }
    else
    {    
    }
    
    ret = timer_getoverrun(stg_SchM_Timer_10ms);
    if(ret > 0)
    {
        printf("overrun = %d\n",ret);
        /*error handle*/
    }
    /* log */
    if(u64g_SchM_Cnt10ms < 2)
    {
        printf("%ld.%ld start time\n", stg_SchM_Timer_StartTime.tv_sec, stg_SchM_Timer_StartTime.tv_nsec);
    }
    
    /*调试用*/
    if(wait_task_done_flag > 0)
    {
        printf("wait timeout task running done, u64g_SchM_Cnt10ms = %d\n", u64g_SchM_Cnt10ms);    
        //pause();
        return;    
    }
    
    pthread_mutex_lock(&stg_SchM_Mutex);
    u8t_TskCurrStat = stg_SchM_RunTaskQueue.sts_TskList[stg_SchM_RunTaskQueue.u8t_TskHead].enm_TskSt;
    pthread_mutex_unlock(&stg_SchM_Mutex);
    
    if(u8t_TskCurrStat == SchM_TASK_RUNNING)
    {
        SchM_HoldTask(&stg_SchM_RunTaskQueue.sts_TskList[stg_SchM_RunTaskQueue.u8t_TskHead]);                
    }
    else
    {    
    }
    SchM_GetNewTasklist();
    ret = pthread_kill(stg_SchM_SchPid, SIGRTMIN);
    if(ret != 0)
    {
        printf("kill error SchM_pid = %d\n", stg_SchM_SchPid);
        timer_delete(stg_SchM_Timer_10ms);
        pause();
    }
    
    /* log */
    sts_SchM_LastTimerStart = stg_SchM_Timer_StartTime;
    clock_gettime(CLOCK_REALTIME, &st_t_end);    
    sprintf(u8g_SchM_Log, "%ld.%ld current  cnt = %d\n",  st_t_end.tv_sec, st_t_end.tv_nsec, u64g_SchM_Cnt10ms);
    sprintf(u8g_SchM_Log, "%s %ld.%ld last start time\n",  u8g_SchM_Log,sts_SchM_LastTimerStart.tv_sec, sts_SchM_LastTimerStart.tv_nsec, u64g_SchM_Cnt10ms);
    //sprintf(u8g_SchM_Log, "%s current timer runtime = %ld\n", u8g_SchM_Log, st_t_end.tv_nsec - stg_SchM_Timer_StartTime.tv_nsec);
}

 /******************************************************************************
*  函数名       | SchM_CreateTimer
*  处理内容     | 创建定时器
*  参数         | timer_t *ptt_timer：定时器指针
*               | uint32 u32t_period：定时器周期,1bit=1ms
*  返回值       | void
*  是否同步     | 否
*  是否可重入   | 否
*  注意事项     | 
******************************************************************************/
void SchM_CreateTimer(timer_t *ptt_timer, uint32 u32t_period)//, int priority)
{
    struct sigevent stt_Event;
    struct sigaction stt_Action;
    sigemptyset(&stt_Action.sa_mask);
    stt_Action.sa_flags =  SA_SIGINFO;
    stt_Action.sa_handler = SchM_Timer_Sig_Proc;
    if(sigaction(SIGRTMAX, &stt_Action, NULL) != 0)
    {
        perror("sigaction  failed .\n");
        return;        
    }
    
    memset(&stt_Event, 0, sizeof(stt_Event));
    stt_Event.sigev_notify = SIGEV_SIGNAL;
    stt_Event.sigev_signo = SIGRTMAX;
    if (timer_create(CLOCK_REALTIME, &stt_Event, ptt_timer) == -1)
    {
        perror("fail to timer_create");
        exit(-1);
    }

    /* timer control*/
    struct itimerspec stt_it;
    stt_it.it_interval.tv_sec = 0;
    stt_it.it_interval.tv_nsec = SCHM_MILLSECOND * u32t_period;
    /*定时器延迟1s开始*/
    stt_it.it_value.tv_sec = 1; 
    stt_it.it_value.tv_nsec = 0;                 
    if (timer_settime(*ptt_timer, 0, &stt_it, NULL) == -1)
    {
        perror("fail to timer_settime");
        exit(-1);
    }
    
}

