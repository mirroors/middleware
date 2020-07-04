#ifndef _SCHMCLIENT_H
#define _SCHMCLIENT_H

#include "SchCommon.h"

struct SchM_Clt_AppOps
{
	/*应用总的初始化、关闭和重新初始化函数*/
    sint32 (*Sch_Clt_AppInit)();
    sint32 (*Sch_Clt_AppShut)();
	sint32 (*Sch_Clt_AppReInit)();	
	
	/*单个线程的初始化、工作和关闭函数。*/
	/*因为一个进程中最多有五个线程，所以用了函数指针数组*/
    sint32 (*Sch_Clt_ThrdInit[5])();
    sint32 (*Sch_Clt_ThrdWork[5])();
    sint32 (*Sch_Clt_ThrdShut[5])();
	
	/*进程中线程的数量，有应用在注册函数中设置*/
	/*该线程数量的设置必须由应用保证正确*/	
    sint32 Sch_Clt_ThrdNumber;
	
};

extern uint8   ptg_SchM_Clit_ProcNme[30];
extern uint8   u8g_SchM_Clt_LogBuf[1024*1024];

#endif