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

sint32 CtApMatch_App_300msWork()
{ 

    sint32            u32t_CtApMatch_TimCont = 0;
	
	struct timespec stt_CtApMatch_TimStart, stt_CtApMatch_TimEnd;
	clock_gettime(CLOCK_REALTIME, &stt_CtApMatch_TimStart);	

	#ifdef SCHM_APP_LOGON
	
		printf("%s : [%ld.%ld] time log1\n", __FUNCTION__, stt_CtApMatch_TimStart.tv_sec, stt_CtApMatch_TimStart.tv_nsec);

		sprintf(u8g_SchM_Clt_LogBuf, "%s%ld.%ld;%s1 SA\n",u8g_SchM_Clt_LogBuf, stt_CtApMatch_TimStart.tv_sec, stt_CtApMatch_TimStart.tv_nsec, ptg_SchM_Clit_ProcNme);
	
	#endif
	
    while(1)
    {
		
        clock_gettime(CLOCK_REALTIME, &stt_CtApMatch_TimEnd);
        u32t_CtApMatch_TimCont = ((stt_CtApMatch_TimEnd.tv_nsec)/1000 - (stt_CtApMatch_TimStart.tv_nsec)/1000);
        if(u32t_CtApMatch_TimCont < 0)
            u32t_CtApMatch_TimCont += 1000000;
        if((u32t_CtApMatch_TimCont) > SchM_APP_WORK1TIME)
            break;
		
    }

	#ifdef SCHM_APP_LOGON
	
		clock_gettime(CLOCK_REALTIME, &stt_CtApMatch_TimEnd);

		sprintf(u8g_SchM_Clt_LogBuf, "%s%ld.%ld;%s1 EA\n",u8g_SchM_Clt_LogBuf, stt_CtApMatch_TimEnd.tv_sec, stt_CtApMatch_TimEnd.tv_nsec, ptg_SchM_Clit_ProcNme);
	
	#endif

	return 0;

}

sint32 CtApMatch_App_Init()
{
	DBG1("CtApMatch_App_Init done\n");
	return 0;
}

sint32 CtApMatch_App_ShutDown()
{
	DBG1("CtApMatch_App_ShutDown done\n");
	return 0;
}

sint32 CtApMatch_App_ReInit()
{
	DBG1("CtApMatch_App_ShutDown done\n");
	return 0;
}

sint32 CtApMatch_App_300msInit()
{
	DBG1("CtApMatch_App_300msInit done\n");
	return 0;
}

sint32 CtApMatch_App_300msShutDown()
{
	DBG1("CtApMatch_App_300msShutDown done\n");
	return 0;
}

void SchM_Clt_TaskRgst(struct SchM_Clt_AppOps * ptt_SchM_Client_OpsTmp)
{
	
	ptt_SchM_Client_OpsTmp->Sch_Clt_ThrdNumber = 1;
	
	ptt_SchM_Client_OpsTmp->Sch_Clt_AppInit 	= (void *)CtApMatch_App_Init;
	ptt_SchM_Client_OpsTmp->Sch_Clt_AppShut 	= (void *)CtApMatch_App_ShutDown;
	ptt_SchM_Client_OpsTmp->Sch_Clt_AppReInit 	= (void *)CtApMatch_App_ReInit;
	
	ptt_SchM_Client_OpsTmp->Sch_Clt_ThrdInit[0] = (void *)CtApMatch_App_300msInit;
	ptt_SchM_Client_OpsTmp->Sch_Clt_ThrdWork[0] = (void *)CtApMatch_App_300msWork;
	ptt_SchM_Client_OpsTmp->Sch_Clt_ThrdShut[0] = (void *)CtApMatch_App_300msShutDown;
	
}

