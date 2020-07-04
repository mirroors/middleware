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

#include "SchMApp.h"
#include "SchCommon.h"

#define DBG1(...) 		fprintf(stderr,__VA_ARGS__) 

#define SchM_APP_WORK1TIME 1000
#define SchM_APP_WORK2TIME 1000
#define SchM_APP_WORK3TIME 1000

sint32 CtApAppFicmProxy_App_10msWork()
{

    sint32            u32t_CtApAppFicmProxy_TimCont = 0;
	
	struct timespec stt_CtApAppFicmProxy_TimStart, stt_CtApAppFicmProxy_TimEnd;
	clock_gettime(CLOCK_REALTIME, &stt_CtApAppFicmProxy_TimStart);	

	#ifdef SCHM_APP_LOGON
	
		printf("%s : [%ld.%ld] time log1\n", __FUNCTION__, stt_CtApAppFicmProxy_TimStart.tv_sec, stt_CtApAppFicmProxy_TimStart.tv_nsec);

		sprintf(u8g_SchM_Clt_LogBuf, "%s%ld.%ld;%s1 SA\n",u8g_SchM_Clt_LogBuf, stt_CtApAppFicmProxy_TimStart.tv_sec, stt_CtApAppFicmProxy_TimStart.tv_nsec, ptg_SchM_Clit_ProcNme);
	
	#endif
	
    while(1)
    {
		
        clock_gettime(CLOCK_REALTIME, &stt_CtApAppFicmProxy_TimEnd);
        u32t_CtApAppFicmProxy_TimCont = ((stt_CtApAppFicmProxy_TimEnd.tv_nsec)/1000 - (stt_CtApAppFicmProxy_TimStart.tv_nsec)/1000);
        if(u32t_CtApAppFicmProxy_TimCont < 0)
            u32t_CtApAppFicmProxy_TimCont += 1000000;
        if((u32t_CtApAppFicmProxy_TimCont) > SchM_APP_WORK1TIME)
            break;
		
    }

	#ifdef SCHM_APP_LOGON
	
		clock_gettime(CLOCK_REALTIME, &stt_CtApAppFicmProxy_TimEnd);

		sprintf(u8g_SchM_Clt_LogBuf, "%s%ld.%ld;%s1 EA\n",u8g_SchM_Clt_LogBuf, stt_CtApAppFicmProxy_TimEnd.tv_sec, stt_CtApAppFicmProxy_TimEnd.tv_nsec, ptg_SchM_Clit_ProcNme);
	
	#endif

	return 0;

}

sint32 CtApAppFicmProxy_App_50msWork()
{

    sint32            u32t_CtApAppFicmProxy_TimCont = 0;

	struct timespec stt_CtApAppFicmProxy_TimStart, stt_CtApAppFicmProxy_TimEnd;
	clock_gettime(CLOCK_REALTIME, &stt_CtApAppFicmProxy_TimStart);	
	
	#ifdef SCHM_APP_LOGON

		printf("%s : [%ld.%ld] time log2\n", __FUNCTION__, stt_CtApAppFicmProxy_TimStart.tv_sec, stt_CtApAppFicmProxy_TimStart.tv_nsec);

		sprintf(u8g_SchM_Clt_LogBuf, "%s%ld.%ld;%s2 SA\n",u8g_SchM_Clt_LogBuf, stt_CtApAppFicmProxy_TimStart.tv_sec, stt_CtApAppFicmProxy_TimStart.tv_nsec, ptg_SchM_Clit_ProcNme);
	
	#endif
	
    while(1)
    {
		
        clock_gettime(CLOCK_REALTIME, &stt_CtApAppFicmProxy_TimEnd);
        u32t_CtApAppFicmProxy_TimCont = ((stt_CtApAppFicmProxy_TimEnd.tv_nsec)/1000 - (stt_CtApAppFicmProxy_TimStart.tv_nsec)/1000);
        if(u32t_CtApAppFicmProxy_TimCont < 0)
            u32t_CtApAppFicmProxy_TimCont += 1000000;
        if((u32t_CtApAppFicmProxy_TimCont) > SchM_APP_WORK2TIME)
            break;
		
    }

	#ifdef SCHM_APP_LOGON
	
		clock_gettime(CLOCK_REALTIME, &stt_CtApAppFicmProxy_TimEnd);

		sprintf(u8g_SchM_Clt_LogBuf, "%s%ld.%ld;%s2 EA\n",u8g_SchM_Clt_LogBuf, stt_CtApAppFicmProxy_TimEnd.tv_sec, stt_CtApAppFicmProxy_TimEnd.tv_nsec, ptg_SchM_Clit_ProcNme);
	
	#endif
	
	return 0;

}

sint32 CtApAppFicmProxy_App_100msWork()
{

    sint32            u32t_CtApAppFicmProxy_TimCont = 0;
	
	struct timespec stt_CtApAppFicmProxy_TimStart, stt_CtApAppFicmProxy_TimEnd;
	clock_gettime(CLOCK_REALTIME, &stt_CtApAppFicmProxy_TimStart);	
	
	#ifdef SCHM_APP_LOGON
	
		printf("%s : [%ld.%ld] time log3\n", __FUNCTION__, stt_CtApAppFicmProxy_TimStart.tv_sec, stt_CtApAppFicmProxy_TimStart.tv_nsec);

		sprintf(u8g_SchM_Clt_LogBuf, "%s%ld.%ld;%s3 SA\n",u8g_SchM_Clt_LogBuf, stt_CtApAppFicmProxy_TimStart.tv_sec, stt_CtApAppFicmProxy_TimStart.tv_nsec, ptg_SchM_Clit_ProcNme);
	
	#endif
	
    while(1)
    {
		
        clock_gettime(CLOCK_REALTIME, &stt_CtApAppFicmProxy_TimEnd);
        u32t_CtApAppFicmProxy_TimCont = ((stt_CtApAppFicmProxy_TimEnd.tv_nsec)/1000 - (stt_CtApAppFicmProxy_TimStart.tv_nsec)/1000);
        if(u32t_CtApAppFicmProxy_TimCont < 0)
            u32t_CtApAppFicmProxy_TimCont += 1000000;
        if((u32t_CtApAppFicmProxy_TimCont) > SchM_APP_WORK3TIME)
            break;
		
    }

	#ifdef SCHM_APP_LOGON
	
		clock_gettime(CLOCK_REALTIME, &stt_CtApAppFicmProxy_TimEnd);

		sprintf(u8g_SchM_Clt_LogBuf, "%s%ld.%ld;%s3 EA\n",u8g_SchM_Clt_LogBuf, stt_CtApAppFicmProxy_TimEnd.tv_sec, stt_CtApAppFicmProxy_TimEnd.tv_nsec, ptg_SchM_Clit_ProcNme);
	
	#endif

	return 0;

}

sint32 CtApAppFicmProxy_App_Init()
{
	DBG1("CtApAppFicmProxy_App_Init done\n");
	return 0;
}

sint32 CtApAppFicmProxy_App_ShutDown()
{
	DBG1("CtApAppFicmProxy_App_ShutDown done\n");
	return 0;
}

sint32 CtApAppFicmProxy_App_ReInit()
{
	DBG1("CtApAppFicmProxy_App_ShutDown done\n");
	return 0;
}

sint32 CtApAppFicmProxy_App_10msInit()
{
	DBG1("CtApAppFicmProxy_App_10msInit done\n");
	return 0;
}

sint32 CtApAppFicmProxy_App_10msShutDown()
{
	DBG1("CtApAppFicmProxy_App_10msShutDown done\n");
	return 0;
}

sint32 CtApAppFicmProxy_App_50msInit()
{
	DBG1("CtApAppFicmProxy_App_50msInit done\n");
	return 0;
}

sint32 CtApAppFicmProxy_App_50msShutDown()
{
	DBG1("CtApAppFicmProxy_App_50msShutDown done\n");
	return 0;
}

sint32 CtApAppFicmProxy_App_100msInit()
{
	DBG1("CtApAppFicmProxy_App_100msInit done\n");
	return 0;
}

sint32 CtApAppFicmProxy_App_100msShutDown()
{
	DBG1("CtApAppFicmProxy_App_100msShutDown done\n");
	return 0;
}

void SchM_Clt_TaskRgst(struct SchM_Clt_AppOps * ptt_SchM_Client_OpsTmp)
{
	
	ptt_SchM_Client_OpsTmp->Sch_Clt_ThrdNumber = 3;
	
	ptt_SchM_Client_OpsTmp->Sch_Clt_AppInit 	= (void *)CtApAppFicmProxy_App_Init;
	ptt_SchM_Client_OpsTmp->Sch_Clt_AppShut 	= (void *)CtApAppFicmProxy_App_ShutDown;
	ptt_SchM_Client_OpsTmp->Sch_Clt_AppReInit 	= (void *)CtApAppFicmProxy_App_ReInit;
	
	ptt_SchM_Client_OpsTmp->Sch_Clt_ThrdInit[0] = (void *)CtApAppFicmProxy_App_10msInit;
	ptt_SchM_Client_OpsTmp->Sch_Clt_ThrdWork[0] = (void *)CtApAppFicmProxy_App_10msWork;
	ptt_SchM_Client_OpsTmp->Sch_Clt_ThrdShut[0] = (void *)CtApAppFicmProxy_App_10msShutDown;
	
	ptt_SchM_Client_OpsTmp->Sch_Clt_ThrdInit[1] = (void *)CtApAppFicmProxy_App_50msInit;
	ptt_SchM_Client_OpsTmp->Sch_Clt_ThrdWork[1] = (void *)CtApAppFicmProxy_App_50msWork;
	ptt_SchM_Client_OpsTmp->Sch_Clt_ThrdShut[1] = (void *)CtApAppFicmProxy_App_50msShutDown;
	
	ptt_SchM_Client_OpsTmp->Sch_Clt_ThrdInit[2] = (void *)CtApAppFicmProxy_App_100msInit;
	ptt_SchM_Client_OpsTmp->Sch_Clt_ThrdWork[2] = (void *)CtApAppFicmProxy_App_100msWork;
	ptt_SchM_Client_OpsTmp->Sch_Clt_ThrdShut[2] = (void *)CtApAppFicmProxy_App_100msShutDown;
	
}

