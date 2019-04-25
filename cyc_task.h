/*************************************************************************
    > File Name: timer.h
    > Author: yangrongxiang
    > Mail:  yangrongxiang@dias.com.cn
    > Created Time: Tue 23 Apr 2019 10:28:26 PM PDT
 ************************************************************************/
//void timer_task(timer_t *timer, int msec, int priorty);

#define _1ms 1
#define _5ms 5
#define _10ms 10

typedef struct timer_event
{
	void (*envent_1ms)(int);
	void (*envent_5ms)(int);
	void (*envent_10ms)(int);
}timer_event;

extern void timer_envent_callback(timer_event *ev);
extern void cycle_task(int msec, int priorty);
