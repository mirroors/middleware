/**********************************************************************************/
/* Copyright (C), 2019, DIAS Automotive Electronic Systems Co; Ltd.               */
/* File Name:    task_queue.c                                                     */
/* Description:  任务队列                                                         */
/*                                                                                */
/* Others:       任务队列相关处理                                                 */
/*                                                                                */
/* Processor:    RCAR H3                                                          */
/* Compiler:     AARCH64-POKY-LINUX                                               */
/********************************** 修改历史 **************************************/
/* Date            Version        Author          Description                     */
/* 2019-08-20      V1.0.0.0       陈俊            创建                            */
/**********************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "SchM_Queue.h"
#include "common.h"

/******************************************************************************
*  函数名       | SchM_InitQueue
*  处理内容     | 任务队列初始化
*  参数         | void
*  返回值       | void
*  是否同步     | 是
*  是否可重入   | 否
*  注意事项     | 
******************************************************************************/
void SchM_InitQueue(struct SchM_TskQueue *ptt_TskQueue)
{
    ptt_TskQueue->u8t_TskHead = 0;
    ptt_TskQueue->u8t_TskTail = 0;
}

/******************************************************************************
*  函数名       | SchM_CheckQueueFull
*  处理内容     | 判断任务队列是否已满
*  参数         | void
*  返回值       | uint8: 1：满； 0：未满                          
*  是否同步     | 是
*  是否可重入   | 否
*  注意事项     | 
******************************************************************************/
uint8 SchM_CheckQueueFull(struct SchM_TskQueue *ptt_TskQueue)
{
    return ptt_TskQueue->u8t_TskTail >= SCHM_TASKLIST_MAX_SIZE;
}

/******************************************************************************
*  函数名       | SchM_CheckQueueEmpty
*  处理内容     | 判断任务队列是否为空
*  参数         | void
*  返回值       | uint8: 1：空； 0：非空                          
*  是否同步     | 是
*  是否可重入   | 否
*  注意事项     | 
******************************************************************************/
uint8 SchM_CheckQueueEmpty(struct SchM_TskQueue *ptt_TskQueue)
{
    if(ptt_TskQueue->u8t_TskTail > ptt_TskQueue->u8t_TskHead)
    {
        return SCHM_QUEUE_NOT_EMPTY;
    }
    else
    {
        return SCHM_QUEUE_EMPTY;
    }
}

/******************************************************************************
*  函数名       | SchM_PushQueue
*  处理内容     | 任务队列添加任务
*  参数         | struct SchM_TskQueue *ptt_TskQueue：任务队列
                | struct SchM_Task *ptt_task：需要添加的任务
*  返回值       | uint8: 1：成功； 0：不成功                          
*  是否同步     | 是
*  是否可重入   | 否
*  注意事项     | 
******************************************************************************/
uint8 SchM_PushQueue(struct SchM_TskQueue *ptt_TskQueue, struct SchM_Task *ptt_task)
{
    if (SchM_CheckQueueFull(ptt_TskQueue)) 
    {
        return RET_NOT_OK;
    }
    else 
    {
        ptt_TskQueue->sts_TskList[ptt_TskQueue->u8t_TskTail] = *ptt_task;
        ptt_TskQueue->u8t_TskTail++;
        return RET_OK;
    }
}

/******************************************************************************
*  函数名       | SchM_PopQueue
*  处理内容     | 任务队列删除第一个任务
*  参数         | struct SchM_TskQueue *ptt_TskQueue：任务队列
*  返回值       | uint8: 1：成功； 0：不成功                          
*  是否同步     | 是
*  是否可重入   | 否
*  注意事项     | 
******************************************************************************/
uint8 SchM_PopQueue(struct SchM_TskQueue *ptt_TskQueue)
{
    if (SchM_CheckQueueEmpty(ptt_TskQueue)) 
    {
        return RET_NOT_OK;
    }
    else 
    {
        ptt_TskQueue->u8t_TskHead++;
        return RET_OK;
    }
}

/******************************************************************************
*  函数名       | SchM_MergeQueue
*  处理内容     | 任务队列合并
*  参数         | struct SchM_TskQueue *ptt_TskQueue：任务队列
*               | struct SchM_Task *ptt_task：需要添加的任务
*  返回值       | uint8: 1：成功； 0：不成功                          
*  是否同步     | 是
*  是否可重入   | 否
*  注意事项     | 
******************************************************************************/
uint8 SchM_MergeQueue(struct SchM_TskQueue *ptt_TargQueue_target, struct SchM_TskQueue *ptt_CurrQueue)
{
    if(SchM_CheckQueueEmpty(ptt_CurrQueue))
    {
        return RET_NOT_OK;
    }
    else
    {
        int i;
        for(i = 0; i < (ptt_CurrQueue->u8t_TskTail - ptt_CurrQueue->u8t_TskHead); i++)
        {
            SchM_PushQueue(ptt_TargQueue_target, &ptt_CurrQueue->sts_TskList[(ptt_CurrQueue->u8t_TskHead) + i]);
        }
        return RET_OK;
    }
}

uint8 SchM_GetQueueTaskCnt(struct SchM_TskQueue *ptt_TskQueue)
{
    return ptt_TskQueue->u8t_TskTail - ptt_TskQueue->u8t_TskHead;
}

