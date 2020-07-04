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

#ifndef  __SCHM_QUEUE_H__
#define  __SCHM_QUEUE_H__

#include <semaphore.h>
#include "common.h"
#include "SchM_Cfg.h"

#define SCHM_QUEUE_FULL 1
#define SCHM_QUEUE_NOT_FULL 0
#define SCHM_QUEUE_EMPTY 1
#define SCHM_QUEUE_NOT_EMPTY 0
#define SCHM_TASKLIST_MAX_SIZE 20

struct SchM_TskQueue 
{
	struct SchM_Task sts_TskList[SCHM_TASKLIST_MAX_SIZE];
	uint8 u8t_TskHead; 
	uint8 u8t_TskTail; 		  
};
extern void SchM_InitQueue(struct SchM_TskQueue *ptt_TskQueue);
//extern uint8 queue_size(struct SchM_TskQueue *ptt_TskQueue) ;
extern uint8 SchM_CheckQueueFull(struct SchM_TskQueue *ptt_TskQueue);
extern uint8 SchM_CheckQueueEmpty(struct SchM_TskQueue *ptt_TskQueue);
extern uint8 SchM_PushQueue(struct SchM_TskQueue *ptt_TskQueue, struct SchM_Task *ptt_task);
extern uint8 SchM_PopQueue(struct SchM_TskQueue *ptt_TskQueue);
extern uint8 SchM_MergeQueue(struct SchM_TskQueue *ptt_TargQueue_target, struct SchM_TskQueue *ptt_CurrQueue) ;
extern uint8 SchM_GetQueueTaskCnt(struct SchM_TskQueue *ptt_TskQueue);
#endif