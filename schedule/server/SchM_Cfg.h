/**********************************************************************************/
/* Copyright (C), 2019, DIAS Automotive Electronic Systems Co; Ltd.               */
/* File Name:    task_init.h                                                      */
/* Description:  任务初始化                                                       */
/*                                                                                */
/* Others:       other description                                                */
/*                                                                                */
/* Processor:    RCAR H3                                                          */
/* Compiler:     AARCH64-POKY-LINUX                                               */
/********************************** 修改历史 **************************************/
/* Date            Version        Author          Description                     */
/* 2019-08-20      V1.0.0.0       陈俊            创建                            */
/**********************************************************************************/

#ifndef  __SCHM_CFG_H__
#define  __SCHM_CFG_H__

#include <semaphore.h>
#include "common.h"

#define SCHM_APP_TASK_DONE_NAME   "SchM_AppTaskDone_Sem"
#define SCHM_PROC_CORE1_CNT       6
#define SCHM_TASK_10MS_CORE1_CNT  7
#define SCHM_TASK_20MS_CORE1_CNT  3
#define SCHM_TASK_50MS_CORE1_CNT  3
#define SCHM_TASK_100MS_CORE1_CNT 1
#define SCHM_TASK_300MS_CORE1_CNT 1
#define SCHM_MW_TASK_CNT 2

#define SCHM_MAINTHREAD_PRIORITY  99

#define SCHM_HOLD_SEM_DELAY 500000

enum SchM_Task_State
{
	SchM_TASK_IDLE = 0,
	SchM_TASK_RUNNING ,
	SchM_TASK_HOLD    
};

enum SchM_LC_State
{
	SCHM_LC_STATE_NOTRUN = 0,
	SCHM_LC_STATE_RUNNING,  
	SCHM_LC_STATE_AFTERRUN,
	SCHM_LC_STATE_REINIT
};

struct SchM_LC_Process 
{
	pid_t  s32t_ProcPid;
	uint8* ptt_ProcName;
	
	uint8* ptt_SemInitAppName;       //接收
	uint8* ptt_SemInitAppDoneName;   //发送
	uint8* ptt_SemInit2RunName;      //接收
	uint8* ptt_SemInit2RunDoneName;  //发送
	uint8* ptt_SemAfterrunName;      //接收
	uint8* ptt_SemAfterrunDoneName;  //发送
	uint8* ptt_SemRenit2RunName;	 //接收
	uint8* ptt_SemRenit2RunDoneName; //发送
	
	sem_t* ptt_SemInitApp;     	
	sem_t* ptt_SemInitAppDone;   
	sem_t* ptt_SemInit2Run;      
	sem_t* ptt_SemInit2RunDone;
	sem_t* ptt_SemAfterrun;      
	sem_t* ptt_SemAfterrunDone;  
	sem_t* ptt_SemRenit2Run;
	sem_t* ptt_SemRenit2RunDone;
	
	uint8* ptt_SemTaskDoneName;
	uint8* ptt_SemTaskHoldName;
	uint8* ptt_SemTaskReholdName;
};

struct SchM_Per_Process 
{
	pid_t  s32t_ProcPid;
	uint8* ptt_ProcName;
	uint8* ptt_SemTaskDoneName;
};

struct SchM_Process 
{
	pid_t  s32t_ProcPid;
	uint8* ptt_ProcName;	 
	uint8* ptt_SemInitName;
	uint8* ptt_SemShtdwnName;
	uint8* ptt_SemTaskDoneName;
	sem_t* ptt_SemInit;
	sem_t* ptt_SemShtdwn;	
	uint8* ptt_SemTaskHoldName;
	uint8* ptt_SemTaskReholdName;
	uint8* ptt_SemReinitName;
	uint8* ptt_SemReinitDoneName;
	sem_t* ptt_SemReinit;
	sem_t* ptt_SemReinitDone;
};

struct SchM_Task 
{
	uint8 u8t_TskId;
	uint16 u8t_TskPer;
	uint8* ptt_TskName;
	uint8* ptt_TskParentName;
	pid_t s32t_TskPid;
	enum SchM_Task_State enm_TskSt; 
	uint8 u8t_TskIrqSig;
	sem_t *ptt_TskSemDone;
	sem_t *ptt_TskSemHold;
	sem_t *ptt_TskSemRehold;
	uint64 u64t_TskWcet;          /* 任务最坏运行时间 */
	uint64 u64t_TskDL; 
	uint64 u64t_TskRstT; 
};

extern struct SchM_LC_Process stg_SchM_LCProc;
extern struct SchM_Per_Process stg_SchM_PerProc;
extern struct SchM_Process stg_SchM_ProcCore1[SCHM_PROC_CORE1_CNT];
extern struct SchM_Task stg_SchM_Tsk10msCore1[SCHM_TASK_10MS_CORE1_CNT];
extern struct SchM_Task stg_SchM_Tsk20msCore1[SCHM_TASK_20MS_CORE1_CNT];
extern struct SchM_Task stg_SchM_Tsk50msCore1[SCHM_TASK_50MS_CORE1_CNT];
extern struct SchM_Task stg_SchM_Tsk100msCore1[SCHM_TASK_100MS_CORE1_CNT];
extern struct SchM_Task stg_SchM_Tsk300msCore1[SCHM_TASK_300MS_CORE1_CNT];

#endif