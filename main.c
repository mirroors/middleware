/*************************************************************************
    > File Name: main.c
    > Author: yangrongxiang
    > Mail: 347551179@qq.com
    > Created Time: Mon 30 Jul 2018 06:26:48 PM PDT
 ************************************************************************/
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include "task.h"

void init_event(Event *ev);
void event_1ms(int cnt);
void event_5ms(int cnt);
void event_10ms(int cnt);
void CanTp_TimeCount(int cnt);
void event_common(void);

Event ti_event;

int main(void)
{

	init_event(&ti_event);
	envent_callback(&ti_event);
    cycle_task(1, 20);
	cycle_task(5, 30);
	cycle_task(10, 40);
	task_creat();
	pause();
	return 0;
}

void init_event(Event *ev)
{
	ev->envent_1ms = event_1ms;
	ev->envent_5ms = event_5ms;
	ev->envent_10ms = event_10ms;
	ev->envent_common = event_common;
}

void event_1ms(int cnt)
{
	//CanTp_TimeCount(++cnt);
}

void event_5ms(int cnt)
{

}

void event_10ms(int cnt)
{
//	CanTp_TimeCount(cnt);
}

void CanTp_TimeCount(int cnt)
{
	printf("cnt:%d\n", cnt);
}

void event_common(void)
{
	while(1)
	{
		printf("event common!\n");
		sleep(1);
	}
}


