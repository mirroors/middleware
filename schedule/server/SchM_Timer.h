/**********************************************************************************/
/* Copyright (C), 2019, DIAS Automotive Electronic Systems Co; Ltd.               */
/* File Name:    timer.h                                                          */
/* Description:  定时器                                                           */
/*                                                                                */
/* Others:       other description                                                */
/*                                                                                */
/* Processor:    RCAR H3                                                          */
/* Compiler:     AARCH64-POKY-LINUX                                               */
/********************************** 修改历史 **************************************/
/* Date            Version        Author          Description                     */
/* 2019-08-20      V1.0.0.0       陈俊            创建                            */
/**********************************************************************************/

#ifndef  __SCHM_TIMER_H__
#define  __SCHM_TIMER_H__

#include <time.h>
#include <sys/time.h>
#include "common.h"

#define SCHM_TIMER_TRIGGER 1
#define SCHM_TIMER_NOT_TRIGGER 0

extern uint64 u64g_SchM_Cnt10ms;
extern timer_t stg_SchM_Timer_10ms;
//extern struct timespec stg_SchM_Timer_StartTime;
extern uint8 u8g_SchM_TimerStartFlg;

extern uint8 u8g_SchM_Log[1024*1024];
extern uint8 wait_task_done_flag;

extern void SchM_CreateTimer(timer_t *ptt_timer, uint32 u32t_period);//, int priority);

#endif