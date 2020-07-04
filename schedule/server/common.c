/**********************************************************************************/
/* Copyright (C), 2019, DIAS Automotive Electronic Systems Co; Ltd.               */
/* File Name:    common.c                                                         */
/* Description:  中间件公共函数                                                   */
/*                                                                                */
/* Others:       中间件公共函数                                                   */
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
#include "common.h"

/******************************************************************************
*  函数名       | Comm_TimeAdd
*  处理内容     | 计算时间偏移 
*  参数         | struct timespec stt_Comm_CrrTime:当前时间
                | uint64 u64t_TimeOffset:偏移时间
*  返回值       | struct timespec：返回偏移后时间
*  是否同步     | 是
*  是否可重入   | 否
*  注意事项     | 正向偏移 
******************************************************************************/
struct timespec Comm_TimeAdd(struct timespec stt_Comm_CrrTime, uint64 u64t_TimeOffset)
{
    struct timespec stt_Comm_Timeout;
    stt_Comm_Timeout.tv_nsec = stt_Comm_CrrTime.tv_nsec + u64t_TimeOffset;
    if(stt_Comm_Timeout.tv_nsec >= SCHM_SECOND)
    {
        stt_Comm_Timeout.tv_sec = stt_Comm_CrrTime.tv_sec + stt_Comm_Timeout.tv_nsec / SCHM_SECOND;
        stt_Comm_Timeout.tv_nsec = stt_Comm_Timeout.tv_nsec % SCHM_SECOND;
    }
    else
    {
        stt_Comm_Timeout.tv_sec = stt_Comm_CrrTime.tv_sec;
    }
    return stt_Comm_Timeout; 
}


/******************************************************************************
*  函数名       | Comm_GetTimeDiff
*  处理内容     | 计算时间差
*  参数         | struct timespec stt_Comm_CrrTime:当前时间
                | struct timespec stt_Comm_StartTime:开始时间
*  返回值       | uint64：时间差，单位：ns
*  是否同步     | 是
*  是否可重入   | 否
*  注意事项     | 
******************************************************************************/
uint64 Comm_GetTimeDiff(struct timespec stt_Comm_CrrTime, struct timespec stt_Comm_StartTime)
{
    uint64 u64t_timeDiff_ns = 0;
    uint64 u64t_timeDiff_s = 0;
    if(stt_Comm_CrrTime.tv_sec - stt_Comm_StartTime.tv_sec < 0)
    {
        printf("current time is earlier than start time.\n");
    }
    else
    {
        u64t_timeDiff_s = stt_Comm_CrrTime.tv_sec - stt_Comm_StartTime.tv_sec;
        if(u64t_timeDiff_s == 0)
        {
            if(stt_Comm_CrrTime.tv_nsec - stt_Comm_StartTime.tv_nsec < 0)
            {
                printf("current time is earlier than start time.\n");
            }
            else
            {
                u64t_timeDiff_ns = stt_Comm_CrrTime.tv_nsec - stt_Comm_StartTime.tv_nsec;
            }
        }
        else
        {
            u64t_timeDiff_ns = stt_Comm_CrrTime.tv_nsec + u64t_timeDiff_s * SCHM_SECOND - stt_Comm_StartTime.tv_nsec;
        }    
    }

    return u64t_timeDiff_ns; 
}