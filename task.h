/*************************************************************************
    > File Name: timer.h
    > Author: yangrongxiang
    > Mail:  yangrongxiang@dias.com.cn
    > Created Time: Tue 23 Apr 2019 10:28:26 PM PDT
 ************************************************************************/
//void timer_task(timer_t *timer, int msec, int priorty);
#include "platform_types.h"

#define _1ms 1
#define _5ms 5
#define _10ms 10

#define OS_TEMPTASKMAXNUM 10

typedef struct Event
{
	void (*envent_1ms)(int);
	void (*envent_5ms)(int);
	void (*envent_10ms)(int);
	void (*envent_common)(void);
}Event;

typedef struct
{
    void (* pt_task_run)(void);
}OsTaskStackType;

extern void envent_callback(Event *ev);
extern void cycle_task(int msec, int priorty);

extern uint8 OsStack_AddTempTask( void * pt_task);
extern void OsStack_Init();
extern void OsStack_Run();
extern void task_creat();
