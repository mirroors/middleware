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
#include "cyc_task.h"

void timer_thread(union sigval sigval_value);
void timer_task(timer_t *timer, int msec, int priorty);

timer_t timer_1ms, timer_5ms, timer_10ms;
timer_event *event;

/******************************************************
*******************************************************

	@timer_event:	if policy = SCHED_OTHER,priority = 0

*******************************************************/
void timer_envent_callback(timer_event *ev)
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

