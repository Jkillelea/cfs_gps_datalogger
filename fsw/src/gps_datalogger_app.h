/*=======================================================================================
** File Name:  gps_datalogger_app.h
**
** Title:  Header File for GPS_DATALOGGER Application
**
** $Author:    Jacob Killelea
** $Revision: 1.1 $
** $Date:      2019-08-29
**
** Purpose:  To define GPS_DATALOGGER's internal macros, data types, global variables and
**           function prototypes
**
** Modification History:
**   Date | Author | Description
**   ---------------------------
**   2019-08-29 | Jacob Killelea | Build #: Code Started
**
**=====================================================================================*/
    
#ifndef _GPS_DATALOGGER_APP_H_
#define _GPS_DATALOGGER_APP_H_

/*
** Pragmas
*/

/*
** Include Files
*/
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "gps_datalogger_platform_cfg.h"
#include "gps_datalogger_mission_cfg.h"
#include "gps_datalogger_private_ids.h"
#include "gps_datalogger_private_types.h"
#include "gps_datalogger_perfids.h"
#include "gps_datalogger_msgids.h"
#include "gps_datalogger_msg.h"

#include "osapi.h"



/*
** Local Defines
*/
#define GPS_DATALOGGER_TIMEOUT_MSEC    1000

/*
** Local Structure Declarations
*/
typedef struct
{
    /* CFE Event table */
    CFE_EVS_BinFilter_t  EventTbl[GPS_DATALOGGER_EVT_CNT];

    /* CFE scheduling pipe */
    CFE_SB_PipeId_t  SchPipeId; 
    uint16           usSchPipeDepth;
    char             cSchPipeName[OS_MAX_API_NAME];

    /* CFE command pipe */
    CFE_SB_PipeId_t  CmdPipeId;
    uint16           usCmdPipeDepth;
    char             cCmdPipeName[OS_MAX_API_NAME];
    
    /* CFE telemetry pipe */
    CFE_SB_PipeId_t  TlmPipeId;
    uint16           usTlmPipeDepth;
    char             cTlmPipeName[OS_MAX_API_NAME];

    /* Task-related */
    uint32  uiRunStatus;
    
    /* Input data - from I/O devices or subscribed from other apps' output data.
       Data structure should be defined in gps_datalogger/fsw/src/gps_datalogger_private_types.h */
    GPS_DATALOGGER_InData_t   InData;

    /* Output data - to be published at the end of a Wakeup cycle.
       Data structure should be defined in gps_datalogger/fsw/src/gps_datalogger_private_types.h */
    GPS_DATALOGGER_OutData_t  OutData;

    /* Housekeeping telemetry - for downlink only.
       Data structure should be defined in gps_datalogger/fsw/src/gps_datalogger_msg.h */
    GPS_DATALOGGER_HkTlm_t  HkTlm;

    /* TODO:  Add declarations for additional private data here */
    int32 RawDataLogFileId;
    int32 FilteredDataLogFileId;
} GPS_DATALOGGER_AppData_t;

/*
** External Global Variables
*/

/*
** Global Variables
*/

/*
** Local Variables
*/

/*
** Local Function Prototypes
*/
int32  GPS_DATALOGGER_InitApp(void);
int32  GPS_DATALOGGER_InitEvent(void);
int32  GPS_DATALOGGER_InitData(void);
int32  GPS_DATALOGGER_InitPipe(void);

void  GPS_DATALOGGER_AppMain(void);

void  GPS_DATALOGGER_CleanupCallback(void);

int32  GPS_DATALOGGER_RcvMsg(int32 iBlocking);

void  GPS_DATALOGGER_ProcessNewData(void);
void  GPS_DATALOGGER_ProcessNewCmds(void);
void  GPS_DATALOGGER_ProcessNewAppCmds(CFE_SB_Msg_t*);

void  GPS_DATALOGGER_ReportHousekeeping(void);
void  GPS_DATALOGGER_SendOutData(void);

boolean  GPS_DATALOGGER_VerifyCmdLength(CFE_SB_Msg_t*, uint16);

int32 GPS_DATALOGGER_FindNextLogFileName(const char *filePredicate,
        char *nextAvailableName,
        uint32 nextAvailableNameLength);

#endif /* _GPS_DATALOGGER_APP_H_ */

/*=======================================================================================
** End of file gps_datalogger_app.h
**=====================================================================================*/
    
