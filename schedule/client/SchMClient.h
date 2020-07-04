/********************************************************************************************************/
/* Copyright (C), 2018, DIAS Automotive Electronic Systems Co; Ltd.                                     */
/* File Name:    SchM_Client.h                                                                 */
/* Description:  IECU midware task                                                                      */
/*                                                                                                      */
/* Others:                                                                                              */
/*                                                                                                      */
/* Processor:    R-CAR H3                                                                               */
/* Compiler:     aarch64-poky-linux                                                                     */
/************************************** 修改历史 ********************************************************/
/* Date                 Version        Author          Description                                      */
/* 2019-07-20         V1.01.01.01.00   李鹏宇          基线创建                                         */
/********************************************************************************************************/

#ifndef _SCHMCLIENT_H
#define _SCHMCLIENT_H

#include "SchCommon.h"

/*初始化、执行任务和关闭的函数指针结构体，应用也需要使用同样的结构体*/
struct SchM_Clt_AppOps
{
	/*应用总的初始化、关闭和重新初始化函数*/
    int (*Sch_Clt_AppInit)();
    int (*Sch_Clt_AppShut)();
    int (*Sch_Clt_AppReInit)();	
	
	/*单个线程的初始化、工作和关闭函数。*/
	/*因为一个进程中最多有五个线程，所以用了函数指针数组*/
    int (*Sch_Clt_ThrdInit[5])();
    int (*Sch_Clt_ThrdWork[5])();
    int (*Sch_Clt_ThrdShut[5])();
	
	/*进程中线程的数量，有应用在注册函数中设置*/
	/*该线程数量的设置必须由应用保证正确*/
    int Sch_Clt_ThrdNumber;
	
};

extern void SchM_Clt_TaskRgst(struct SchM_Clt_AppOps *ptt_SchM_Client_OpsTmp);

#endif