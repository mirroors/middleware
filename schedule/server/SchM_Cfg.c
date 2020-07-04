/**********************************************************************************/
/* Copyright (C), 2019, DIAS Automotive Electronic Systems Co; Ltd.               */
/* File Name:    SchM_Cfg.c                                                      */
/* Description:  进程任务初始化                                                   */
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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sched.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/types.h>
#include <signal.h>
#include <semaphore.h>
#include "SchM_Cfg.h"


struct SchM_LC_Process stg_SchM_LCProc =
{
        
    0,
    "life_Cycle_Service",
    
    "sem_init_to_operat",
    "sem_init_to_operat_c",
    "sem_run_to_operat",
    "sem_run_to_operat_c",
    "sem_afterrun_to_operat",
    "sem_afterrun_to_operat_c",
    "sem_reinit_to_operat",
    "sem_reinit_to_operat_c",
    
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    "life_Cycle_Service_done_sem",
    "life_Cycle_Service_hold_sem",
    "life_Cycle_Service_rehold_sem"    
};

struct SchM_Per_Process stg_SchM_PerProc =
{
        
    0,
    "per",
    "per_done_sem"
};

struct SchM_Process stg_SchM_ProcCore1[SCHM_PROC_CORE1_CNT] =        
{        
    {    
        0,
        "CtApTransformer",
        "CtApTransformer_init_sem",
        "CtApTransformer_shtdwn_sem",
        "CtApTransformer_done_sem",
        NULL,
        NULL,
        "CtApTransformer_hold_sem",
        "CtApTransformer_rehold_sem",
        "CtApTransformer_reinit_sem",
        "CtApTransformer_reinit_done_sem",
        NULL,
        NULL

    },    
    {    
        0,
        "CpApMatchPosition",
        "CpApMatchPositi_init_sem",
        "CpApMatchPositi_shtdwn_sem",
        "CpApMatchPositi_done_sem",
        NULL,
        NULL,
        "CpApMatchPositi_hold_sem",
        "CpApMatchPositi_rehold_sem",
        "CpApMatchPositi_reinit_sem",
        "CpApMatchPositi_reinit_done_sem",
        NULL,
        NULL
    },    
    {    
        0,
        "CtApAppFicmProxy",
        "CtApAppFicmProx_init_sem",
        "CtApAppFicmProx_shtdwn_sem",
        "CtApAppFicmProx_done_sem",
        NULL,
        NULL,
        "CtApAppFicmProx_hold_sem",
        "CtApAppFicmProx_rehold_sem",
        "CtApAppFicmProx_reinit_sem",
        "CtApAppFicmProx_reinit_done_sem",
        NULL,
        NULL
    },    
    {    
        0,
        "CtApMatch",
        "CtApMatch_init_sem",
        "CtApMatch_shtdwn_sem",
        "CtApMatch_done_sem",
        NULL,
        NULL,
        "CtApMatch_hold_sem",
        "CtApMatch_rehold_sem",
        "CtApMatch_reinit_sem",
        "CtApMatch_reinit_done_sem",
        NULL,
        NULL
    },    
    {    
        0,
        "CtApPrediction",
        "CtApPrediction_init_sem",
        "CtApPrediction_shtdwn_sem",
        "CtApPrediction_done_sem",
        NULL,
        NULL,
        "CtApPrediction_hold_sem",
        "CtApPrediction_rehold_sem",
        "CtApPrediction_reinit_sem",
        "CtApPrediction_reinit_done_sem",
        NULL,
        NULL
    },    
    {    
        0,
        "CtApUWBProcess",
        "CtApUWBProcess_init_sem",
        "CtApUWBProcess_shtdwn_sem",
        "CtApUWBProcess_done_sem",
        NULL,
        NULL,
        "CtApUWBProcess_hold_sem",
        "CtApUWBProcess_rehold_sem",
        "CtApUWBProcess_reinit_sem",
        "CtApUWBProcess_reinit_done_sem",
        NULL,
        NULL
    }
    
};                
        
struct SchM_Task stg_SchM_Tsk10msCore1[SCHM_TASK_10MS_CORE1_CNT] =         
{  
 
    {    
        1,
        10,
        "Per_Write_10ms",
        "per",
        0,
        0,
        34,
        NULL,
        NULL,
        NULL,
        1000000,
        500000,
        0
    },	
    {    
        1,
        10,
        "CtApTransformer_RX_10ms",
        "CtApTransformer",
        0,
        0,
        35,
        NULL,
        NULL,
        NULL,
        500000,
        500000,
        0
    },    
    {    
        2,
        10,
        "CtApTransformer_TX_10ms",
        "CtApTransformer",
        0,
        0,
        36,
        NULL,
        NULL,
        NULL,
        800000,
        500000,
        0
    },    
    {    
        1,
        10,
        "CpApMatchPosition_RS_10ms",
        "CpApMatchPosition",
        0,
        0,
        35,
        NULL,
        NULL,
        NULL,
        1000000,
        500000,
        0
    },    
    {    
        1,
        10,
        "CtApAppFicmProxy_10ms",
        "CtApAppFicmProxy",
        0,
        0,
        35,
        NULL,
        NULL,
        NULL,
        1000000,
        500000,
        0
    },    
    {    
        1,
        10,
        "CtApPrediction_10ms",
        "CtApPrediction",
        0,
        0,
        35,
        NULL,
        NULL,
        NULL,
        500000,
        500000,
        0
    },    
    {    
        2,
        10,
        "CtApPrediction_Rx_10ms",
        "CtApPrediction",
        0,
        0,
        36,
        NULL,
        NULL,
        NULL,
        500000,
        500000,
        0
    }
 
};            
    
struct SchM_Task stg_SchM_Tsk20msCore1[SCHM_TASK_20MS_CORE1_CNT] =         
{ 
     {    
        1,
        20,
        "life_Cycle_Service_50ms",
        "life_Cycle_Service",
        0,
        0,
        35,
        NULL,
        NULL,
        NULL,
        1000000,
		500000,
        0
    },     
    {    
        2,
        20,
        "CpApMatchPosition_RS_20ms",
        "CpApMatchPosition",
        0,
        0,
        36,
        NULL,
        NULL,
        NULL,
        1000000,
        500000,
        0
    },    
    {    
        1,
        20,
        "CtApUWBProcess_RS_20ms",
        "CtApUWBProcess",
        0,
        0,
        35,
        NULL,
        NULL,
        NULL,
        1000000,
        500000,
        0
    }    
};        

struct SchM_Task stg_SchM_Tsk50msCore1[SCHM_TASK_50MS_CORE1_CNT] =         
{ 
      
    {    
        2,
        50,
        "CtApAppFicmProxy_50ms",
        "CtApAppFicmProxy",
        0,
        0,
        36,
        NULL,
        NULL,
        NULL,
        1000000,
        500000,
        0
    },    
    {    
        3,
        50,
        "CtApPrediction_50ms",
        "CtApPrediction",
        0,
        0,
        37,
        NULL,
        NULL,
        NULL,
        1000000,
        500000,
        0
    },    
    {    
        4,
        50,
        "CtApPrediction_RS_50ms",
        "CtApPrediction",
        0,
        0,
        38,
        NULL,
        NULL,
        NULL,
        1000000,
        500000,
        0 
    }    
};        

struct SchM_Task stg_SchM_Tsk100msCore1[SCHM_TASK_100MS_CORE1_CNT] =         
{        
    {    
        3,
        100,
        "CtApAppFicmProxy_100ms",
        "CtApAppFicmProxy",
        0,
        0,
        37,
        NULL,
        NULL,
        NULL,
        1000000,
        500000,
        0
    }

};                    

struct SchM_Task stg_SchM_Tsk300msCore1[SCHM_TASK_300MS_CORE1_CNT] =         
{        
    {    
        1,
        300,
        "CtApMatch_300ms",
        "CtApMatch",
        0,
        0,
        35,
        NULL,
        NULL,
        NULL,
        1000000,
        500000,
        0
    }    
};                
                    

