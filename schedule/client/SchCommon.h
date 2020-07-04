
#ifndef _SCHMCOMMON_H
#define _SCHMCOMMON_H

typedef signed   char           sint8;
typedef signed   short          sint16;
typedef signed   int            sint32;
typedef signed   long           sint64;

typedef unsigned char           uint8;
typedef unsigned short          uint16;
typedef unsigned int            uint32;
typedef unsigned long           uint64;

extern sint8 Comm_GetProcNme(uint8  *u8t_Comm_ProcNme);

extern void Comm_KeyProduce(uint8  *ptt_Per_Clt_SrcNme, uint8  *ptt_Per_Clt_KeyNme);

#endif