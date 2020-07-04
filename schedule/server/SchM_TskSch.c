/**********************************************************************************/
/* Copyright (C), 2019, DIAS Automotive Electronic Systems Co; Ltd.               */
/* File Name:    schedule.c                                                      */
/* Description:  任务调度处理                                                     */
/*                                                                                */
/* Others:       处理任务执行，挂起                                               */
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
#include "SchM_TskSch.h"
#include "SchM_MainFunc.h"
#include "SchM_Queue.h"
#include "SchM_Cfg.h"


#define SCHM_TASK_RUNNING_SUCCESS 0
#define SCHM_TASK_RUNNING_NOT_DONE 1
#define SCHM_TASK_RUNNING_ERROR 2

#define SCHM_TASK_NEEDGETTASKLIST    1
#define SCHM_TASK_NOTNEEDGETTASKLIST 0

struct SchM_TskQueue stg_SchM_RunTaskQueue;
struct SchM_TskQueue stg_SchM_NewTaskQueue;
pthread_t stg_SchM_SchPid;
pthread_mutex_t stg_SchM_Mutex = PTHREAD_MUTEX_INITIALIZER;
uint8 u8g_SchM_ErrCnt = 0;
uint8 u8g_SchM_SemToMainFlg = 1;    /*afterrun状态，调度线程任务执行后发送给主线的信号量flag*/

static struct timespec stg_SchM_Task_StartTime;
static struct timespec stg_SchM_Task_TimeOut;
static uint8 u8g_SchM_GetTasklistFlg = SCHM_TASK_NOTNEEDGETTASKLIST;
static sem_t* ptt_SemRunTaskQueueStart;
static uint8 u8g_SchM_TskLastStat = SchM_TASK_IDLE;
/******************************************************************************
*  函数名         | SchM_GetTaskRestTime
*  处理内容       | 获取任务剩余执行时间
*  参数           | struct SchM_Task*：任务指针
*                 | struct timespec：挂起的时间
*  返回值         | void
*  是否同步       | 是
*  是否可重入     | 否
*  注意事项       | 
*****************************************************************************/
static void SchM_GetTaskRestTime(struct SchM_Task* ptt_Task,  struct timespec stt_SchM_HoldReq_T)
{
    /*任务挂起的时间与任务超时时间比较*/
    if((stg_SchM_Task_TimeOut.tv_sec > stt_SchM_HoldReq_T.tv_sec) || 
       ((stg_SchM_Task_TimeOut.tv_sec == stt_SchM_HoldReq_T.tv_sec) && (stg_SchM_Task_TimeOut.tv_nsec > stt_SchM_HoldReq_T.tv_nsec)))
    {
        ptt_Task->u64t_TskRstT = Comm_GetTimeDiff(stg_SchM_Task_TimeOut, stt_SchM_HoldReq_T);
    }
    else
    {
        ptt_Task->u64t_TskRstT = 0;
    }
}

/******************************************************************************
*  函数名         | SchM_HoldTask
*  处理内容       | 任务挂起通知函数
*  参数           | struct SchM_Task* ptt_Task:任务结构体指针
*  返回值         | void
*  是否同步       | 是
*  是否可重入     | 否
*  注意事项       | 
******************************************************************************/
void SchM_HoldTask(struct SchM_Task* ptt_Task)
{
    union sigval un_task_notify_sigval;
    un_task_notify_sigval.sival_int = 20;          //挂起信号参数，调试用
    pid_t s32t_hold_pid;
    int u8t_hold_signal, ret;
    struct timespec  stt_t_end;
    struct timespec  stt_SchM_HoldReq_T;
    struct timespec  stt_SchM_WaitHoldBack_T;
    s32t_hold_pid = ptt_Task->s32t_TskPid;
        
    u8t_hold_signal = ptt_Task->u8t_TskIrqSig;
    
    /*判断挂起的任务是否是10ms任务*/
    if(ptt_Task->u8t_TskPer == 10)
    {
        /* error handle */
        clock_gettime(CLOCK_REALTIME, &stt_t_end);
        printf("[%ld.%ld]%s 10ms not finish, 10ms cnt = %d\n", stt_t_end.tv_sec, stt_t_end.tv_nsec, ptt_Task->ptt_TskName, u64g_SchM_Cnt10ms);
        printf("stg_SchM_NewTaskQueue.tail = %d\n", stg_SchM_NewTaskQueue.u8t_TskTail);
    }
    /*任务是否为正在运行状态，如果是则通知应用挂起*/
    if(ptt_Task->enm_TskSt == SchM_TASK_RUNNING)
    {
        
        ret = sigqueue(s32t_hold_pid, u8t_hold_signal, un_task_notify_sigval);
        if(ret != 0)
        {
            printf("send hold signal error cnt = %d\n", u64g_SchM_Cnt10ms);
        }
        
        clock_gettime(CLOCK_REALTIME, &stt_SchM_HoldReq_T);
        uint64 u64t_SchM_TskHold_t;
        sprintf(u8g_SchM_Log, "%s %ld.%ld %s will be hold, 10ms cnt = %d, s32t_hold_pid = %d, u8t_hold_signal = %d\n", u8g_SchM_Log, stt_SchM_HoldReq_T.tv_sec, stt_SchM_HoldReq_T.tv_nsec, ptt_Task->ptt_TskName, u64g_SchM_Cnt10ms, s32t_hold_pid, u8t_hold_signal);
        while(1)
        {
            clock_gettime(CLOCK_REALTIME, &stt_SchM_WaitHoldBack_T);
            /*判断是否接收到挂起的信号量*/
            if(sem_trywait(ptt_Task->ptt_TskSemHold) == 0)
            { 
                ret = pthread_mutex_lock(&stg_SchM_Mutex);
                ptt_Task->enm_TskSt = SchM_TASK_HOLD;
                ret = pthread_mutex_unlock(&stg_SchM_Mutex);
                SchM_GetTaskRestTime(ptt_Task, stt_SchM_HoldReq_T);
                //printf("[%ld.%ld]%s hold success, 10ms cnt = %d, Task->enm_TskSt = %d\n", stt_SchM_WaitHoldBack_T.tv_sec, stt_SchM_WaitHoldBack_T.tv_nsec, ptt_Task->ptt_TskName,u64g_SchM_Cnt10ms, ptt_Task->enm_TskSt);
                break;
            }    
            /*判断是否接收到无需挂起的信号量*/
            else if(sem_trywait(ptt_Task->ptt_TskSemRehold) == 0)
            {       
                /*上个周期为hold状态，收到rehold表示任务还未开始执行就收到
                挂起信号，此时仍将任务置位挂起状态，剩余运行时间同上个周期计算*/
                ret = pthread_mutex_lock(&stg_SchM_Mutex);
                if(u8g_SchM_TskLastStat == SchM_TASK_HOLD)
                {
                    ptt_Task->enm_TskSt = SchM_TASK_HOLD;    
                }
                else
                {                    
                    ptt_Task->enm_TskSt = SchM_TASK_IDLE;                    
                }
                ret = pthread_mutex_unlock(&stg_SchM_Mutex);
                printf("[%ld.%ld]%s not need hold, 10ms cnt = %d, Task->enm_TskSt = %d\n", stt_SchM_WaitHoldBack_T.tv_sec, stt_SchM_WaitHoldBack_T.tv_nsec, ptt_Task->ptt_TskName, u64g_SchM_Cnt10ms, ptt_Task->enm_TskSt);
                break;
            }
            /*以上两个信号量接收超时*/
            else
            {
                u64t_SchM_TskHold_t = Comm_GetTimeDiff(stt_SchM_WaitHoldBack_T, stt_SchM_HoldReq_T);
                if(u64t_SchM_TskHold_t > SCHM_HOLD_SEM_DELAY)
                {
                    /* error handle */
                    printf("[%ld.%ld]sem wait time out.  task->name = %s, 10ms cnt = %d\n", stt_SchM_WaitHoldBack_T.tv_sec, stt_SchM_WaitHoldBack_T.tv_nsec, ptt_Task->ptt_TskName, u64g_SchM_Cnt10ms);
                    printf("[%ld.%ld]hold info time\n", stt_SchM_HoldReq_T.tv_sec, stt_SchM_HoldReq_T.tv_nsec);
                    timer_delete(stg_SchM_Timer_10ms);
                    printf("%s\n", u8g_SchM_Log);
                    while(1)
                    {
                        if(sem_trywait(ptt_Task->ptt_TskSemHold) == 0)
                        {
                            clock_gettime(CLOCK_REALTIME, &stt_SchM_WaitHoldBack_T);
                            printf("[%ld.%ld]%s hold success but cs, 10ms cnt = %d, Task->enm_TskSt = %d\n", stt_SchM_WaitHoldBack_T.tv_sec, stt_SchM_WaitHoldBack_T.tv_nsec, ptt_Task->ptt_TskName,u64g_SchM_Cnt10ms, ptt_Task->enm_TskSt);
                            break;
                        }
                        else if(sem_trywait(ptt_Task->ptt_TskSemRehold) == 0)
                        {
                            clock_gettime(CLOCK_REALTIME, &stt_SchM_WaitHoldBack_T);
                            printf("[%ld.%ld]%s not need hold but cs, 10ms cnt = %d, Task->enm_TskSt = %d\n", stt_SchM_WaitHoldBack_T.tv_sec, stt_SchM_WaitHoldBack_T.tv_nsec, ptt_Task->ptt_TskName, u64g_SchM_Cnt10ms, ptt_Task->enm_TskSt);
                            break;
                        }
                    }
                    //pause();
                    //break;
                }
            }
        }
    }
    else
    {
        printf("%s has been done, hold signal not send, 10ms cnt = %d\n", ptt_Task->ptt_TskName, u64g_SchM_Cnt10ms);
    }
    
}

/******************************************************************************
*  函数名          | SchM_GetNewTasklist
*  处理内容        | 获取本周期内新的任务队列
*  参数            | void
*  返回值          | void
*  是否同步        | 是
*  是否可重入      | 否
*  注意事项        | 
******************************************************************************/
void SchM_GetNewTasklist(void)
{
    SchM_InitQueue(&stg_SchM_NewTaskQueue);
    int i, ret;
    /*状态机是running状态才运行应用任务，否则只运行状态机应用*/
    if(u8g_SchM_LCState == SCHM_LC_STATE_RUNNING)
    {
        for(i = 0; i < SCHM_TASK_10MS_CORE1_CNT; i++)
        {
            //printf("task_10ms_core0[i].name = %s\n", task_10ms_core1[i].name);
            ret = SchM_PushQueue(&stg_SchM_NewTaskQueue, &stg_SchM_Tsk10msCore1[i]);
            if(ret != RET_OK)
            {
                printf("queue_push error 10ms\n");
            }
        }
        /*add 20ms task*/
        if((u64g_SchM_Cnt10ms-1) % 2 == 0)
        {
            for(i = 0; i < SCHM_TASK_20MS_CORE1_CNT; i++)
            {
                ret = SchM_PushQueue(&stg_SchM_NewTaskQueue, &stg_SchM_Tsk20msCore1[i]);
                if(ret != RET_OK)
                {
                    printf("queue_push error 10ms\n");
                }
            }        
        }
        /*add 50ms task*/
        if((u64g_SchM_Cnt10ms-1) % 5 == 0)
        {
            for(i = 0; i < SCHM_TASK_50MS_CORE1_CNT; i++)
            {
                ret = SchM_PushQueue(&stg_SchM_NewTaskQueue, &stg_SchM_Tsk50msCore1[i]);
                if(ret != RET_OK)
                {
                    printf("queue_push error 10ms\n");
                }
            }        
        }
        /*add 100ms task*/
        if((u64g_SchM_Cnt10ms-1) % 10 == 0)
        {
            for(i = 0; i < SCHM_TASK_100MS_CORE1_CNT; i++)
            {
                ret = SchM_PushQueue(&stg_SchM_NewTaskQueue, &stg_SchM_Tsk100msCore1[i]);
                if(ret != RET_OK)
                {
                    printf("queue_push error 10ms\n");
                }
            }
                    
        }
        /*add 300ms task*/
        if((u64g_SchM_Cnt10ms-1) % 30 == 0)
        {
            for(i = 0; i < SCHM_TASK_300MS_CORE1_CNT; i++)
            {
                ret = SchM_PushQueue(&stg_SchM_NewTaskQueue, &stg_SchM_Tsk300msCore1[i]);
                if(ret != RET_OK)
                {
                    printf("queue_push error 10ms\n");
                }
            }        
        }
    }
    else
    {
        ret = SchM_PushQueue(&stg_SchM_NewTaskQueue, &stg_SchM_Tsk10msCore1[0]);
        if(ret != RET_OK)
        {
            printf("queue_push error 10ms\n");
        }	
        if((u64g_SchM_Cnt10ms-1) % 2 == 0)
        {
            ret = SchM_PushQueue(&stg_SchM_NewTaskQueue, &stg_SchM_Tsk20msCore1[0]);
            if(ret != RET_OK)
            {
                printf("queue_push error 10ms\n");
            }
        }
    }
}

/******************************************************************************
*  函数名       | SchM_SetThreadAffinity
*  处理内容     | 获取本周期内新的任务队列
*  参数         | uint8 u8t_core_id：CPU核
*  返回值       | sint8： 0：执行成功
                          -1：执行错误   
*  是否同步     | 是
*  是否可重入   | 否
*  注意事项     | 
******************************************************************************/
static sint8 SchM_SetThreadAffinity(uint8 u8t_core_id)
{
    cpu_set_t stt_mask;  
    CPU_ZERO(&stt_mask);  
    CPU_SET(u8t_core_id, &stt_mask);
    //if (0 != sched_setaffinity(0, sizeof(mask),&mask))
    if(-1 == pthread_setaffinity_np(pthread_self() ,sizeof(stt_mask),&stt_mask))  
    {
        printf("pthread_setaffinity_np error");
        return RET_NOT_OK;  
    }
    return RET_OK;
}

/******************************************************************************
*  函数名       | SchM_TimerSigAction
*  处理内容     | 定时器线程信号处理函数
*  参数         | void
*  返回值       | void
*  是否同步     | 否
*  是否可重入   | 否
*  注意事项     | 
******************************************************************************/
static void SchM_TimerSigAction(void)
{
    u8g_SchM_GetTasklistFlg = SCHM_TASK_NEEDGETTASKLIST;
    
    sem_post(ptt_SemRunTaskQueueStart);
}

/******************************************************************************
*  函数名       | SchM_GetProcTasklist
*  处理内容     | 获取本周期需执行的任务队列
*  参数         | void
*  返回值       | void
*  是否同步     | 否
*  是否可重入   | 否
*  注意事项     | 
******************************************************************************/
static void SchM_GetProcTasklist(void)
{
    
    if(SchM_CheckQueueEmpty(&stg_SchM_RunTaskQueue) == 0)
    {
        //pthread_mutex_lock(&stg_SchM_Mutex);
        SchM_MergeQueue(&stg_SchM_NewTaskQueue, &stg_SchM_RunTaskQueue);    
        //pthread_mutex_unlock(&stg_SchM_Mutex);
    }    
    stg_SchM_RunTaskQueue = stg_SchM_NewTaskQueue;    
    u8g_SchM_TimerStartFlg = SCHM_TIMER_NOT_TRIGGER;
    /*LOG */
    if(u64g_SchM_Cnt10ms <= 5)
    {
        printf("stg_SchM_RunTaskQueue.tail = %d\n", stg_SchM_RunTaskQueue.u8t_TskTail);
    }
}


/******************************************************************************
*  函数名       | SchM_RunTask
*  处理内容     | 通知任务执行
*  参数         | struct SchM_Task *stt_task：任务指针
*  返回值       | uint8: 0: 任务执行成功
                         1：无须执行
                         2：错误
*  是否同步     | 是
*  是否可重入   | 否
*  注意事项     | 
******************************************************************************/
static uint8 SchM_RunTask(struct SchM_Task *stt_task)
{
    
    union sigval un_task_notify_sigval;
    un_task_notify_sigval.sival_int = stt_task->u8t_TskId;
    int ret;
    uint8 u8t_SchM_TaskCurrStat;
    ret = pthread_mutex_trylock(&stg_SchM_Mutex);
    u8g_SchM_TskLastStat = stt_task->enm_TskSt;
    /*获得锁，而且定时器未触发*/
    if((ret == 0) && (u8g_SchM_TimerStartFlg != SCHM_TIMER_TRIGGER))
    {
        ret = sigqueue(stt_task->s32t_TskPid, SIGRTMIN, un_task_notify_sigval);
        if(ret != 0)
        {
            pthread_mutex_unlock(&stg_SchM_Mutex);
            printf("task notify signal send error, task->name = %s, task->pid = %d\n", stt_task->ptt_TskName, stt_task->s32t_TskPid);
            return SCHM_TASK_RUNNING_ERROR;
        }
        stt_task->enm_TskSt = SchM_TASK_RUNNING;     
        pthread_mutex_unlock(&stg_SchM_Mutex);
    }
    else
    {
        /*获得锁，但定时器已触发*/
        if(ret == 0)
        {
            pthread_mutex_unlock(&stg_SchM_Mutex);
            printf("timer has been triggered\n");
        }
        /*未获得锁*/
        else
        {
            printf("task not get lock\n");
        }    
        return SCHM_TASK_RUNNING_NOT_DONE;
    }
    
    clock_gettime(CLOCK_REALTIME, &stg_SchM_Task_StartTime);
    //printf("%ld.%ld inform  task %s to run\n",  stg_SchM_Task_StartTime.tv_sec, stg_SchM_Task_StartTime.tv_nsec, stt_task->ptt_TskName);
    sprintf(u8g_SchM_Log, "%s %ld.%ld send sig to task %s \n", u8g_SchM_Log, stg_SchM_Task_StartTime.tv_sec, stg_SchM_Task_StartTime.tv_nsec, stt_task->ptt_TskName);
    
    /*获取任务超时时间*/
    if(u8g_SchM_TskLastStat == SchM_TASK_IDLE)
    {      
        stg_SchM_Task_TimeOut = Comm_TimeAdd(stg_SchM_Task_StartTime, stt_task->u64t_TskWcet + stt_task->u64t_TskDL);
    }
    else if(u8g_SchM_TskLastStat == SchM_TASK_HOLD)
    {
        stg_SchM_Task_TimeOut = Comm_TimeAdd(stg_SchM_Task_StartTime, stt_task->u64t_TskRstT + stt_task->u64t_TskDL);
        //stg_SchM_Task_TimeOut = Comm_TimeAdd(stg_SchM_Task_StartTime, stt_task->u64t_TskWcet + stt_task->u64t_TskDL);
    }

    /*等待应用任务完成的信号量通知*/
    ret = sem_timedwait(stt_task->ptt_TskSemDone, &stg_SchM_Task_TimeOut);
    struct timespec stt_t_end;
    clock_gettime(CLOCK_REALTIME, &stt_t_end);
    /*表示收到应用任务完成的通知信号量*/
    if(ret == 0)
    {
        stt_task->enm_TskSt = SchM_TASK_IDLE;
        //printf("%ld.%ld get task done %s sem \n",  stt_t_end.tv_sec, stt_t_end.tv_nsec, stt_task->ptt_TskName);
        sprintf(u8g_SchM_Log, "%s %ld.%ld get task done %s sem \n", u8g_SchM_Log, stt_t_end.tv_sec, stt_t_end.tv_nsec, stt_task->ptt_TskName);        
        return SCHM_TASK_RUNNING_SUCCESS;        
    }    
    /*表示任务运行超时或者定时器线程发送给本线程信号后唤醒*/
    else
    {        
        pthread_mutex_lock(&stg_SchM_Mutex);
        u8t_SchM_TaskCurrStat = stt_task->enm_TskSt;
        pthread_mutex_unlock(&stg_SchM_Mutex);
        
        /*定时器线程发送给本线程信号后唤醒
        u8t_SchM_TaskCurrStat == SchM_TASK_HOLD 表示应用任务挂起，定时器收到挂起信号量
        u8t_SchM_TaskCurrStat == SchM_TASK_IDLE 表示应用任务无需挂起，定时器收到rehold信号量
        */
        if(u8t_SchM_TaskCurrStat == SchM_TASK_HOLD || u8t_SchM_TaskCurrStat == SchM_TASK_IDLE)
        {    
            //stt_task->enm_TskSt = SchM_TASK_IDLE;
            return SCHM_TASK_RUNNING_NOT_DONE;
        }
        /*运行超时*/
        else
        {
            /*error handle*/
            u8g_SchM_ErrCnt++;
            wait_task_done_flag++;
            int semvalue;
            sem_getvalue(stt_task->ptt_TskSemDone, &semvalue);
            //clock_gettime(CLOCK_REALTIME, &stt_t_end);
            printf("[%ld.%ld]error occurred sem_value = %d, task->name = %s, error cnt = %d, 10ms cnt = %d, task->enm_TskSt = %d\n", stt_t_end.tv_sec, stt_t_end.tv_nsec, semvalue, stt_task->ptt_TskName, u8g_SchM_ErrCnt, u64g_SchM_Cnt10ms, stt_task->enm_TskSt);
            printf("[%ld.%ld]task start, 10ms cnt = %d, task->name = %s, last state = %d\n", stg_SchM_Task_StartTime.tv_sec, stg_SchM_Task_StartTime.tv_nsec, u64g_SchM_Cnt10ms, stt_task->ptt_TskName, u8g_SchM_TskLastStat);
            printf("%s\n", u8g_SchM_Log);
            timer_delete(stg_SchM_Timer_10ms);
            stg_SchM_RunTaskQueue.u8t_TskHead = 0;
            stg_SchM_RunTaskQueue.u8t_TskTail = 0;
            ret = sem_wait(stt_task->ptt_TskSemDone);
            if(ret == 0)
            {
                clock_gettime(CLOCK_REALTIME, &stt_t_end);
                printf("[%ld.%ld]timeout done, 10ms cnt = %d, task->name = %s\n", stt_t_end.tv_sec, stt_t_end.tv_nsec,u64g_SchM_Cnt10ms, stt_task->ptt_TskName);
                //pause();
            }
            return SCHM_TASK_RUNNING_ERROR;             
        }        
    }
  
}

/******************************************************************************
*  函数名       | SchM_Start
*  处理内容     | 调度线程开始
*  参数         | void
*  返回值       | void
*  是否同步     | 否
*  是否可重入   | 否
*  注意事项     | 
******************************************************************************/
void SchM_Start(void)
{
    
    sint32 s32t_ret;
    uint64 u64t_period_rest_time;
    struct timespec stt_curr_t;
    /*线程绑定核2*/
    s32t_ret = SchM_SetThreadAffinity(2);
     if(s32t_ret != 0)
    {
        printf("set core Affinity error\n");
    }
    
    struct sigaction stt_sa_timertrg;
    stt_sa_timertrg.sa_flags = SA_SIGINFO;
    stt_sa_timertrg.sa_sigaction = (void *)SchM_TimerSigAction;
    if(sigaction(SIGRTMIN,&stt_sa_timertrg,NULL)<0)
    {
        printf("install sigal error\n");
    }
    
    ptt_SemRunTaskQueueStart = sem_open("SchM_RunTaskQueueStart", O_CREAT|O_RDWR, 0666, 0);
    while(1)
    {
        //clock_gettime(CLOCK_REALTIME, &stt_curr_t);   
        /*判断是否需要生成新的运行队列，在本线程收到定时器信号的信号处理函数后条件满足*/
        if(u8g_SchM_GetTasklistFlg == SCHM_TASK_NEEDGETTASKLIST)
        {
            SchM_GetProcTasklist(); 
            u8g_SchM_GetTasklistFlg = SCHM_TASK_NOTNEEDGETTASKLIST;
        }        
        if((u8g_SchM_TimerStartFlg == SCHM_TIMER_NOT_TRIGGER) && (SchM_CheckQueueEmpty(&stg_SchM_RunTaskQueue) != SCHM_QUEUE_EMPTY))
        {
		    /*表示在afterrun状态，执行的任务是应用任务，同时也不是上个周期被挂起的任务，此时直接删除任务*/
            if((u8g_SchM_LCState == SCHM_LC_STATE_AFTERRUN) && stg_SchM_RunTaskQueue.u8t_TskHead >= 1 && stg_SchM_RunTaskQueue.sts_TskList[stg_SchM_RunTaskQueue.u8t_TskHead].enm_TskSt != SchM_TASK_HOLD && (strcmp(stg_SchM_RunTaskQueue.sts_TskList[stg_SchM_RunTaskQueue.u8t_TskHead].ptt_TskParentName, "life_Cycle_Service") != 0))
			{
			    SchM_PopQueue(&stg_SchM_RunTaskQueue);
			}
			else
			{
                s32t_ret = SchM_RunTask(&(stg_SchM_RunTaskQueue.sts_TskList[stg_SchM_RunTaskQueue.u8t_TskHead]));
                if(s32t_ret == SCHM_TASK_RUNNING_SUCCESS)
                {
                    SchM_PopQueue(&stg_SchM_RunTaskQueue);
                }
                else if(s32t_ret == SCHM_TASK_RUNNING_ERROR)
                {
                    /*error handle*/
                    SchM_PopQueue(&stg_SchM_RunTaskQueue);
                }
                /*表示SCHM_TASK_RUNNING_NOT_DONE*/
                else
                {
                }
			}
        }
        else
        {

		    /*当前是afterrun状态并且已经没有任务，才通知主线程，并且只通知一次*/
		    if((u8g_SchM_SemToMainFlg == 1) && (u8g_SchM_LCState == SCHM_LC_STATE_AFTERRUN) && (SchM_CheckQueueEmpty(&stg_SchM_RunTaskQueue) == SCHM_QUEUE_EMPTY))
			{
			    sem_post(ptg_SemAppTskDone);
				u8g_SchM_SemToMainFlg = 0;
			}
            /*等待运行队列开始执行的信号量，while防止误唤醒*/
            do
            {
				
                s32t_ret = sem_wait(ptt_SemRunTaskQueueStart);
            }
            while(s32t_ret != 0);
        
        }
        
    }
    
}

/******************************************************************************
*  函数名         | SchM_Start
*  处理内容       | 调度线程开始
*  参数           | void
*  返回值         | void
*  是否同步       | 是
*  是否可重入     | 否
*  注意事项       | 
******************************************************************************/
void SchM_CreateSchThread(void)
{
    int ret;
    pthread_attr_t stt_attr;
    struct sched_param stt_param;
    pthread_attr_init(&stt_attr);
    stt_param.sched_priority = 96;
    pthread_attr_setschedpolicy(&stt_attr, SCHED_FIFO); //线程调度 先进先出
    pthread_attr_setschedparam(&stt_attr, &stt_param);        //线程优先级
    ret = pthread_create(&stg_SchM_SchPid, &stt_attr, (void *)&SchM_Start, NULL);
    //pthread_join(stg_SchM_SchPid, NULL);
}