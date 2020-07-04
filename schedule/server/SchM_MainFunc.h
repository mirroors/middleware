/**********************************************************************************/
/* Copyright (C), 2019, DIAS Automotive Electronic Systems Co; Ltd.               */
/* File Name:    common.h                                                         */
/* Description:  通用定义                                                         */
/*                                                                                */
/* Others:       other description                                                */
/*                                                                                */
/* Processor:    RCAR H3                                                          */
/* Compiler:     AARCH64-POKY-LINUX                                               */
/********************************** 修改历史 **************************************/
/* Date            Version        Author          Description                     */
/* 2019-08-20      V1.0.0.0       陈俊            创建                            */
/**********************************************************************************/
#ifndef  __SCHM_MAINFUNC_H__
#define  __SCHM_MAINFUNC_H__

#include "common.h"

extern uint8 u8g_SchM_LCState;
extern sem_t* ptg_SemAppTskDone;;

#endif