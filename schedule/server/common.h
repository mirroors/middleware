/**********************************************************************************/
/* Copyright (C), 2019, DIAS Automotive Electronic Systems Co; Ltd.               */
/* File Name:    task_queue.h                                                     */
/* Description:  任务队列                                                         */
/*                                                                                */
/* Others:                                                                        */
/*                                                                                */
/* Processor:    RCAR H3                                                          */
/* Compiler:     AARCH64-POKY-LINUX                                               */
/********************************** 修改历史 **************************************/
/* Date            Version        Author          Description                     */
/* 2019-08-20      V1.0.0.0       陈俊            创建                            */
/**********************************************************************************/

#ifndef  __COMMON_H__
#define  __COMMON_H__

#include <sys/time.h>
#include <time.h>

typedef unsigned char	    uint8;
typedef unsigned short int	uint16;
typedef unsigned int	    uint32;
typedef unsigned long int	uint64;
typedef signed char	    	sint8;
typedef signed short int	sint16;
typedef signed int	    	sint32;
typedef signed long int	    sint64;

#define RET_OK          0
#define RET_NOT_OK      !RET_OK

#define SCHM_SECOND     1000000000          //1s = 1000000000ns
#define SCHM_MILLSECOND 1000000

extern struct timespec Comm_TimeAdd(struct timespec stt_Comm_CrrTime, uint64 u64t_TimeOffset);
extern uint64 Comm_GetTimeDiff(struct timespec stt_Comm_CrrTime, struct timespec stt_Comm_StartTime);
#endif