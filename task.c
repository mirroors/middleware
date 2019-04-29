/*************************************************************************
    > File Name: timer.c
    > Author: yangrongxiang
    > Mail:  yangrongxiang@dias.com.cn
    > Created Time: Tue 23 Apr 2019 10:28:21 PM PDT
 ************************************************************************/

#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <sys/time.h>
#include <signal.h>
#include <string.h>
#include "task.h"

void timer_thread(union sigval sigval_value);
void timer_task(timer_t *timer, int msec, int priorty);

timer_t timer_1ms, timer_5ms, timer_10ms;
Event *event;

//common tasks
static OsTaskStackType OsTaskStack[OS_TEMPTASKMAXNUM];
static uint8 u8s_OsTask_StartItr;
static uint8 u8s_OsTask_EndItr;
static uint8 u8s_OsTask_OverflowFlg;

/******************************************************
*******************************************************

	@timer_event:	if policy = SCHED_OTHER,priority = 0

*******************************************************/
void envent_callback(Event *ev)
{
	event = NULL;
	if(ev)
		event = ev;
}


/******************************************************
*******************************************************
	@msec:		Cycle task elapsed time(MS)
	@priorty:	if policy = SCHED_OTHER,priority = 0
				if policy = SCHED_FIFO ,priority = 1-99
				if policy = SCHED_RR,   priority = 1-99

*******************************************************/
void cycle_task(int msec, int priority)
{
	switch(msec)
	{
		case _1ms:
			timer_task(&timer_1ms, _1ms, priority);
			break;
		case _5ms:
			timer_task(&timer_5ms, _5ms, priority);
			break;
		case _10ms:
			timer_task(&timer_10ms, _10ms, priority);
			break;
		default:
			printf("cycle_task no exist!\n");
			break;
	}
}

/******************************************************
*******************************************************
	@timer:		timer_t struct
	@msec:		Cycle task elapsed time(MS)
	@priorty:	if policy = SCHED_OTHER,priority = 0
				if policy = SCHED_FIFO ,priority = 1-99
				if policy = SCHED_RR,   priority = 1-99

*******************************************************/
void timer_task(timer_t *timer, int msec, int priority)
{
	struct sched_param param;
	param.sched_priority = priority;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setscope (&attr, PTHREAD_SCOPE_SYSTEM);
	pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);
	pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
	pthread_attr_setschedparam(&attr,&param);

	struct sigevent evp;
	memset(&evp, 0, sizeof(evp));
	evp.sigev_notify = SIGEV_THREAD;
	evp.sigev_value.sival_int = msec;
	evp.sigev_notify_function = timer_thread;
    evp.sigev_notify_attributes = &attr;
    if (timer_create(CLOCK_REALTIME, &evp, timer) == -1)
    {
        perror("fail to timer_create");
        return;
    }
	/* timer control*/
    struct itimerspec it;
	it.it_interval.tv_sec = 0;
	it.it_interval.tv_nsec = 1000000 * msec;
	it.it_value.tv_sec = 1;
	it.it_value.tv_nsec = 0;
    if (timer_settime(*timer, 0, &it, NULL) == -1)
    {
        perror("fail to timer_settime");
        return;
    }
}

/******************************************************
*******************************************************
	@sigval:		sigevent.sigval_value

*******************************************************/
void timer_thread(union sigval sigval_value)
{
	int overrun_cnt;
	if(event == NULL)
	{
		printf("event is NULL,please callback timer_envent_callback befor this!\n");
		return;
	}
	switch(sigval_value.sival_int)
	{
		case _1ms:
			overrun_cnt = timer_getoverrun(timer_5ms);
			if(event->envent_1ms)
				event->envent_1ms(overrun_cnt);
			else
				printf("envent_1ms is NULL,please callback timer_envent_callback befor this!\n");
			break;

		case _5ms:
			overrun_cnt = timer_getoverrun(timer_5ms);
			if(event->envent_5ms)
				event->envent_5ms(overrun_cnt);
			else
				printf("envent_5ms is NULL,please callback timer_envent_callback befor this!\n");
			break;

		case _10ms:
			overrun_cnt = timer_getoverrun(timer_10ms);
			if(event->envent_10ms)
				event->envent_10ms(overrun_cnt);
			else
				printf("envent_10ms is NULL,please callback timer_envent_callback befor this!\n");
			break;

		default:
			break;
	}
}

/******************************************************
*******************************************************
	parameter:	void
	describe:	Init
*******************************************************/

void OsStack_Init()
{
	u8s_OsTask_StartItr = 0;
	u8s_OsTask_EndItr = 0;
	u8s_OsTask_OverflowFlg = (uint8)OFF;
}

/******************************************************
*******************************************************
	parameter:	@pt_task:callback pointer
	describe:	add taske
*******************************************************/
uint8 OsStack_AddTempTask( void * pt_task)
{
	uint8 u8t_result;
	u8t_result = (uint8)1;
	if ( u8s_OsTask_OverflowFlg == (uint8)OFF)
	{
		OsTaskStack[u8s_OsTask_EndItr].pt_task_run = pt_task;
		u8s_OsTask_EndItr++;
		if (u8s_OsTask_EndItr == OS_TEMPTASKMAXNUM )
		{
			u8s_OsTask_EndItr = (uint8)0;
		}
		if ( u8s_OsTask_EndItr == u8s_OsTask_StartItr )
		{
			u8s_OsTask_OverflowFlg = (uint8)ON;
		}
		u8t_result = (uint8)0;
	}
	else
	{

	}
	return u8t_result;
}

/******************************************************
*******************************************************
	parameter:	void
	describe:	run taske
*******************************************************/
void OsStack_Run()
{
	if ( u8s_OsTask_StartItr != u8s_OsTask_EndItr )
	{
		OsTaskStack[u8s_OsTask_StartItr].pt_task_run();
		u8s_OsTask_StartItr++;
		if (u8s_OsTask_StartItr == OS_TEMPTASKMAXNUM )
		{
			u8s_OsTask_StartItr = (uint8)0;
		}
		if ( u8s_OsTask_OverflowFlg == (uint8)ON )
		{
			u8s_OsTask_OverflowFlg = (uint8)OFF;
		}
	}
}

/******************************************************
*******************************************************
	parameter:	void 
	describe:	creat task by thread
*******************************************************/
void task_creat()
{
	if(event->envent_common)
	{
		pthread_t tid;
		int ret = pthread_create(&tid, NULL, event->envent_common, NULL);
		if(ret < 0)
		{
			printf("pthread_create faile!\n");
			return;
		}		
		pthread_detach(tid);	
	}
	else
	{
		printf("event->envent_common is NULL.please init event->envent_common befor this!\n");
	}
}

