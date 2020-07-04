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

#define SchM_APP_WORK1TIME 500
#define SchM_APP_WORK2TIME 500
#define SchM_APP_WORK3TIME 1000
#define SchM_APP_WORK4TIME 1000

sint32 CtApPrediction_App_10msFstWork()
{

    sint32            u32t_CtApPrediction_TimCont = 0;
	
	struct timespec stt_CtApPrediction_TimStart, stt_CtApPrediction_TimEnd;
	clock_gettime(CLOCK_REALTIME, &stt_CtApPrediction_TimStart);	

	#ifdef SCHM_APP_LOGON
	
		printf("%s : [%ld.%ld] time log1\n", __FUNCTION__, stt_CtApPrediction_TimStart.tv_sec, stt_CtApPrediction_TimStart.tv_nsec);

		sprintf(u8g_SchM_Clt_LogBuf, "%s%ld.%ld;%s1 SA\n",u8g_SchM_Clt_LogBuf, stt_CtApPrediction_TimStart.tv_sec, stt_CtApPrediction_TimStart.tv_nsec, ptg_SchM_Clit_ProcNme);
	
	#endif
	
    while(1)
    {
		
        clock_gettime(CLOCK_REALTIME, &stt_CtApPrediction_TimEnd);
        u32t_CtApPrediction_TimCont = ((stt_CtApPrediction_TimEnd.tv_nsec)/1000 - (stt_CtApPrediction_TimStart.tv_nsec)/1000);
        if(u32t_CtApPrediction_TimCont < 0)
            u32t_CtApPrediction_TimCont += 1000000;
        if((u32t_CtApPrediction_TimCont) > SchM_APP_WORK1TIME)
            break;
		
    }

	#ifdef SCHM_APP_LOGON
	
		clock_gettime(CLOCK_REALTIME, &stt_CtApPrediction_TimEnd);

		sprintf(u8g_SchM_Clt_LogBuf, "%s%ld.%ld;%s1 EA\n",u8g_SchM_Clt_LogBuf, stt_CtApPrediction_TimEnd.tv_sec, stt_CtApPrediction_TimEnd.tv_nsec, ptg_SchM_Clit_ProcNme);
	
	#endif

	return 0;

}

sint32 CtApPrediction_App_10msSndWork()
{

    sint32            u32t_CtApPrediction_TimCont = 0;

	struct timespec stt_CtApPrediction_TimStart, stt_CtApPrediction_TimEnd;
	clock_gettime(CLOCK_REALTIME, &stt_CtApPrediction_TimStart);	
	
	#ifdef SCHM_APP_LOGON

		printf("%s : [%ld.%ld] time log2\n", __FUNCTION__, stt_CtApPrediction_TimStart.tv_sec, stt_CtApPrediction_TimStart.tv_nsec);

		sprintf(u8g_SchM_Clt_LogBuf, "%s%ld.%ld;%s2 SA\n",u8g_SchM_Clt_LogBuf, stt_CtApPrediction_TimStart.tv_sec, stt_CtApPrediction_TimStart.tv_nsec, ptg_SchM_Clit_ProcNme);
	
	#endif
	
    while(1)
    {
		
        clock_gettime(CLOCK_REALTIME, &stt_CtApPrediction_TimEnd);
        u32t_CtApPrediction_TimCont = ((stt_CtApPrediction_TimEnd.tv_nsec)/1000 - (stt_CtApPrediction_TimStart.tv_nsec)/1000);
        if(u32t_CtApPrediction_TimCont < 0)
            u32t_CtApPrediction_TimCont += 1000000;
        if((u32t_CtApPrediction_TimCont) > SchM_APP_WORK2TIME)
            break;
		
    }

	#ifdef SCHM_APP_LOGON
	
		clock_gettime(CLOCK_REALTIME, &stt_CtApPrediction_TimEnd);

		sprintf(u8g_SchM_Clt_LogBuf, "%s%ld.%ld;%s2 EA\n",u8g_SchM_Clt_LogBuf, stt_CtApPrediction_TimEnd.tv_sec, stt_CtApPrediction_TimEnd.tv_nsec, ptg_SchM_Clit_ProcNme);
	
	#endif

	return 0;

}

sint32 CtApPrediction_App_50msFstWork()
{

    sint32            u32t_CtApPrediction_TimCont = 0;
	
	struct timespec stt_CtApPrediction_TimStart, stt_CtApPrediction_TimEnd;
	clock_gettime(CLOCK_REALTIME, &stt_CtApPrediction_TimStart);	
	
	#ifdef SCHM_APP_LOGON
	
		printf("%s : [%ld.%ld] time log3\n", __FUNCTION__, stt_CtApPrediction_TimStart.tv_sec, stt_CtApPrediction_TimStart.tv_nsec);

		sprintf(u8g_SchM_Clt_LogBuf, "%s%ld.%ld;%s3 SA\n",u8g_SchM_Clt_LogBuf, stt_CtApPrediction_TimStart.tv_sec, stt_CtApPrediction_TimStart.tv_nsec, ptg_SchM_Clit_ProcNme);
	
	#endif
	
    while(1)
    {
		
        clock_gettime(CLOCK_REALTIME, &stt_CtApPrediction_TimEnd);
        u32t_CtApPrediction_TimCont = ((stt_CtApPrediction_TimEnd.tv_nsec)/1000 - (stt_CtApPrediction_TimStart.tv_nsec)/1000);
        if(u32t_CtApPrediction_TimCont < 0)
            u32t_CtApPrediction_TimCont += 1000000;
        if((u32t_CtApPrediction_TimCont) > SchM_APP_WORK3TIME)
            break;
		
    }

	#ifdef SCHM_APP_LOGON
	
		clock_gettime(CLOCK_REALTIME, &stt_CtApPrediction_TimEnd);

		sprintf(u8g_SchM_Clt_LogBuf, "%s%ld.%ld;%s3 EA\n",u8g_SchM_Clt_LogBuf, stt_CtApPrediction_TimEnd.tv_sec, stt_CtApPrediction_TimEnd.tv_nsec, ptg_SchM_Clit_ProcNme);
	
	#endif

	return 0;

}

sint32 CtApPrediction_App_50msSndWork()
{

    sint32            u32t_CtApPrediction_TimCont = 0;
	
	struct timespec stt_CtApPrediction_TimStart, stt_CtApPrediction_TimEnd;
	clock_gettime(CLOCK_REALTIME, &stt_CtApPrediction_TimStart);	
	
	#ifdef SCHM_APP_LOGON
	
		printf("%s : [%ld.%ld] time log4\n", __FUNCTION__, stt_CtApPrediction_TimStart.tv_sec, stt_CtApPrediction_TimStart.tv_nsec);

		sprintf(u8g_SchM_Clt_LogBuf, "%s%ld.%ld;%s4 SA\n",u8g_SchM_Clt_LogBuf, stt_CtApPrediction_TimStart.tv_sec, stt_CtApPrediction_TimStart.tv_nsec, ptg_SchM_Clit_ProcNme);
	
	#endif
	
    while(1)
    {
		
        clock_gettime(CLOCK_REALTIME, &stt_CtApPrediction_TimEnd);
        u32t_CtApPrediction_TimCont = ((stt_CtApPrediction_TimEnd.tv_nsec)/1000 - (stt_CtApPrediction_TimStart.tv_nsec)/1000);
        if(u32t_CtApPrediction_TimCont < 0)
            u32t_CtApPrediction_TimCont += 1000000;
        if((u32t_CtApPrediction_TimCont) > SchM_APP_WORK4TIME)
            break;
		
    }

	#ifdef SCHM_APP_LOGON
	
		clock_gettime(CLOCK_REALTIME, &stt_CtApPrediction_TimEnd);

		sprintf(u8g_SchM_Clt_LogBuf, "%s%ld.%ld;%s4 EA\n",u8g_SchM_Clt_LogBuf, stt_CtApPrediction_TimEnd.tv_sec, stt_CtApPrediction_TimEnd.tv_nsec, ptg_SchM_Clit_ProcNme);
	
	#endif

	return 0;

}

sint32 CtApPrediction_App_Init()
{
	DBG1("CtApPrediction_App_Init done\n");
	return 0;
}

sint32 CtApPrediction_App_ShutDown()
{
	DBG1("CtApPrediction_App_ShutDown done\n");
	return 0;
}

sint32 CtApPrediction_App_ReInit()
{
	DBG1("CtApPrediction_App_ShutDown done\n");
	return 0;
}

sint32 CtApPrediction_App_10msFstInit()
{
	DBG1("CtApPrediction_App_10msFstInit done\n");
	return 0;
}

sint32 CtApPrediction_App_10msFstShutDown()
{
	DBG1("CtApPrediction_App_10msFstShutDown done\n");
	return 0;
}

sint32 CtApPrediction_App_10msSndInit()
{
	DBG1("CtApPrediction_App_10msSndInit done\n");
	return 0;
}

sint32 CtApPrediction_App_10msSndShutDown()
{
	DBG1("CtApPrediction_App_10msSndShutDown done\n");
	return 0;
}

sint32 CtApPrediction_App_50msFstInit()
{
	DBG1("CtApPrediction_App_50msFstInit done\n");
	return 0;
}

sint32 CtApPrediction_App_50msFstShutDown()
{
	DBG1("CtApPrediction_App_50msFstShutDown done\n");
	return 0;
}

sint32 CtApPrediction_App_50msSndInit()
{
	DBG1("CtApPrediction_App_50msSndInit done\n");
	return 0;
}

sint32 CtApPrediction_App_50msSndShutDown()
{
	DBG1("CtApPrediction_App_50msSndShutDown done\n");
	return 0;
}

void SchM_Clt_TaskRgst(struct SchM_Clt_AppOps * ptt_SchM_Client_OpsTmp)
{
	
	ptt_SchM_Client_OpsTmp->Sch_Clt_ThrdNumber = 4;
	
	ptt_SchM_Client_OpsTmp->Sch_Clt_AppInit 	= (void *)CtApPrediction_App_Init;
	ptt_SchM_Client_OpsTmp->Sch_Clt_AppShut 	= (void *)CtApPrediction_App_ShutDown;
	ptt_SchM_Client_OpsTmp->Sch_Clt_AppReInit 	= (void *)CtApPrediction_App_ReInit;
	
	ptt_SchM_Client_OpsTmp->Sch_Clt_ThrdInit[0] = (void *)CtApPrediction_App_10msFstInit;
	ptt_SchM_Client_OpsTmp->Sch_Clt_ThrdWork[0] = (void *)CtApPrediction_App_10msFstWork;
	ptt_SchM_Client_OpsTmp->Sch_Clt_ThrdShut[0] = (void *)CtApPrediction_App_10msFstShutDown;
	
	ptt_SchM_Client_OpsTmp->Sch_Clt_ThrdInit[1] = (void *)CtApPrediction_App_10msSndInit;
	ptt_SchM_Client_OpsTmp->Sch_Clt_ThrdWork[1] = (void *)CtApPrediction_App_10msSndWork;
	ptt_SchM_Client_OpsTmp->Sch_Clt_ThrdShut[1] = (void *)CtApPrediction_App_10msSndShutDown;
	
	ptt_SchM_Client_OpsTmp->Sch_Clt_ThrdInit[2] = (void *)CtApPrediction_App_50msFstInit;
	ptt_SchM_Client_OpsTmp->Sch_Clt_ThrdWork[2] = (void *)CtApPrediction_App_50msFstWork;
	ptt_SchM_Client_OpsTmp->Sch_Clt_ThrdShut[2] = (void *)CtApPrediction_App_50msFstShutDown;
	
	ptt_SchM_Client_OpsTmp->Sch_Clt_ThrdInit[3] = (void *)CtApPrediction_App_50msSndInit;
	ptt_SchM_Client_OpsTmp->Sch_Clt_ThrdWork[3] = (void *)CtApPrediction_App_50msSndWork;
	ptt_SchM_Client_OpsTmp->Sch_Clt_ThrdShut[3] = (void *)CtApPrediction_App_50msSndShutDown;

}

