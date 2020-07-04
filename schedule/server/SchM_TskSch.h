/**********************************************************************************/
/* Copyright (C), 2019, DIAS Automotive Electronic Systems Co; Ltd.               */
/* File Name:    schedule.h                                                       */
/* Description:  任务调度                                                         */
/*                                                                                */
/* Others:       other description                                                */
/*                                                                                */
/* Processor:    RCAR H3                                                          */
/* Compiler:     AARCH64-POKY-LINUX                                               */
/********************************** 修改历史 **************************************/
/* Date            Version        Author          Description                     */
/* 2019-08-20      V1.0.0.0       陈俊            创建                            */
/**********************************************************************************/

#ifndef  __SCHM_TASK_H__
#define  __SCHM_TASK_H__
#include "SchM_Cfg.h"
#include "common.h"

extern struct SchM_TskQueue stg_SchM_RunTaskQueue;
extern struct SchM_TskQueue stg_SchM_NewTaskQueue;
//extern int SchM_pid;

extern pthread_t stg_SchM_SchPid;
extern uint8 u8g_SchM_ErrCnt;
extern uint8 u8g_SchM_SemToMainFlg;

extern pthread_mutex_t stg_SchM_Mutex;
extern void SchM_CreateSchThread(void);
extern void SchM_HoldTask(struct SchM_Task* ptt_Task);
extern void SchM_GetNewTasklist(void);

#endif