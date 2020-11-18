/*=======================================================================================
** File Name:  gps_datalogger_app.c
**
** Title:  Function Definitions for GPS_DATALOGGER Application
**
** $Author:    Jacob Killelea
** $Revision: 1.1 $
** $Date:      2019-08-29
**
** Purpose:  This source file contains all necessary function definitions to run GPS_DATALOGGER
**           application.
**
** Functions Defined:
**    Function X - Brief purpose of function X
**    Function Y - Brief purpose of function Y
**    Function Z - Brief purpose of function Z
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to all functions in the file.
**    2. List the external source(s) and event(s) that can cause the funcs in this
**       file to execute.
**    3. List known limitations that apply to the funcs in this file.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Modification History:
**   Date | Author | Description
**   ---------------------------
**   2019-08-29 | Jacob Killelea | Build #: Code Started
**
**=====================================================================================*/

/*
** Pragmas
*/

/*
** Include Files
*/
#include <string.h>

#include "cfe.h"

#include "gps_datalogger_platform_cfg.h"
#include "gps_datalogger_mission_cfg.h"
#include "gps_datalogger_app.h"
#include "gps_reader_msgs.h"
#include "gps_reader_msgids.h"
#include "gps_kalman_msgids.h"
#include "gps_kalman_msg.h"

/*
** Local Defines
*/

/*
** Local Structure Declarations
*/

/*
** External Global Variables
*/

/*
** Global Variables
*/
GPS_DATALOGGER_AppData_t  g_GPS_DATALOGGER_AppData;

/*
** Local Variables
*/

/*
** Local Function Definitions
*/

/*=====================================================================================
** Name: GPS_DATALOGGER_InitEvent
**
** Purpose: To initialize and register event table for GPS_DATALOGGER application
**
** Arguments:
**    None
**
** Returns:
**    int32 iStatus - Status of initialization
**
** Routines Called:
**    CFE_EVS_Register
**    CFE_ES_WriteToSysLog
**
** Called By:
**    GPS_DATALOGGER_InitApp
**
** Global Inputs/Reads:
**    TBD
**
** Global Outputs/Writes:
**    g_GPS_DATALOGGER_AppData.EventTbl
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to this function.
**    2. List the external source(s) and event(s) that can cause this function to execute.
**    3. List known limitations that apply to this function.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Algorithm:
**    Psuedo-code or description of basic algorithm
**
** Author(s):  Jacob Killelea
**
** History:  Date Written  2019-08-29
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
int32 GPS_DATALOGGER_InitEvent()
{
    int32  iStatus=CFE_SUCCESS;

    /* Create the event table */
    memset((void*)g_GPS_DATALOGGER_AppData.EventTbl, 0x00, sizeof(g_GPS_DATALOGGER_AppData.EventTbl));

    g_GPS_DATALOGGER_AppData.EventTbl[0].EventID = GPS_DATALOGGER_RESERVED_EID;
    g_GPS_DATALOGGER_AppData.EventTbl[1].EventID = GPS_DATALOGGER_INF_EID;
    g_GPS_DATALOGGER_AppData.EventTbl[2].EventID = GPS_DATALOGGER_INIT_INF_EID;
    g_GPS_DATALOGGER_AppData.EventTbl[3].EventID = GPS_DATALOGGER_ILOAD_INF_EID;
    g_GPS_DATALOGGER_AppData.EventTbl[4].EventID = GPS_DATALOGGER_CDS_INF_EID;
    g_GPS_DATALOGGER_AppData.EventTbl[5].EventID = GPS_DATALOGGER_CMD_INF_EID;

    g_GPS_DATALOGGER_AppData.EventTbl[ 6].EventID = GPS_DATALOGGER_ERR_EID;
    g_GPS_DATALOGGER_AppData.EventTbl[ 7].EventID = GPS_DATALOGGER_INIT_ERR_EID;
    g_GPS_DATALOGGER_AppData.EventTbl[ 8].EventID = GPS_DATALOGGER_ILOAD_ERR_EID;
    g_GPS_DATALOGGER_AppData.EventTbl[ 9].EventID = GPS_DATALOGGER_CDS_ERR_EID;
    g_GPS_DATALOGGER_AppData.EventTbl[10].EventID = GPS_DATALOGGER_CMD_ERR_EID;
    g_GPS_DATALOGGER_AppData.EventTbl[11].EventID = GPS_DATALOGGER_PIPE_ERR_EID;
    g_GPS_DATALOGGER_AppData.EventTbl[12].EventID = GPS_DATALOGGER_MSGID_ERR_EID;
    g_GPS_DATALOGGER_AppData.EventTbl[13].EventID = GPS_DATALOGGER_MSGLEN_ERR_EID;

    /* Register the table with CFE */
    iStatus = CFE_EVS_Register(g_GPS_DATALOGGER_AppData.EventTbl,
                               GPS_DATALOGGER_EVT_CNT, CFE_EVS_BINARY_FILTER);
    if (iStatus != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("GPS_DATALOGGER - Failed to register with EVS (0x%08X)\n", iStatus);
    }

    return (iStatus);
}

/*=====================================================================================
** Name: GPS_DATALOGGER_InitPipe
**
** Purpose: To initialize all message pipes and subscribe to messages for GPS_DATALOGGER application
**
** Arguments:
**    None
**
** Returns:
**    int32 iStatus - Status of initialization
**
** Routines Called:
**    CFE_SB_CreatePipe
**    CFE_SB_Subscribe
**    CFE_ES_WriteToSysLog
**
** Called By:
**    GPS_DATALOGGER_InitApp
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**    g_GPS_DATALOGGER_AppData.usSchPipeDepth
**    g_GPS_DATALOGGER_AppData.cSchPipeName
**    g_GPS_DATALOGGER_AppData.SchPipeId
**    g_GPS_DATALOGGER_AppData.usCmdPipeDepth
**    g_GPS_DATALOGGER_AppData.cCmdPipeName
**    g_GPS_DATALOGGER_AppData.CmdPipeId
**    g_GPS_DATALOGGER_AppData.usTlmPipeDepth
**    g_GPS_DATALOGGER_AppData.cTlmPipeName
**    g_GPS_DATALOGGER_AppData.TlmPipeId
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to this function.
**    2. List the external source(s) and event(s) that can cause this function to execute.
**    3. List known limitations that apply to this function.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Algorithm:
**    Psuedo-code or description of basic algorithm
**
** Author(s):  Jacob Killelea
**
** History:  Date Written  2019-08-29
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
int32 GPS_DATALOGGER_InitPipe()
{
    int32  iStatus = CFE_SUCCESS;

    /* Init schedule pipe */
    g_GPS_DATALOGGER_AppData.usSchPipeDepth = GPS_DATALOGGER_SCH_PIPE_DEPTH;
    memset((void*) g_GPS_DATALOGGER_AppData.cSchPipeName, '\0', sizeof(g_GPS_DATALOGGER_AppData.cSchPipeName));
    strncpy(g_GPS_DATALOGGER_AppData.cSchPipeName, "GPS_DATALOGGER_SCH_PIPE", OS_MAX_API_NAME-1);

    /* Subscribe to Wakeup messages */
    iStatus = CFE_SB_CreatePipe(&g_GPS_DATALOGGER_AppData.SchPipeId,
                                 g_GPS_DATALOGGER_AppData.usSchPipeDepth,
                                 g_GPS_DATALOGGER_AppData.cSchPipeName);
    if (iStatus == CFE_SUCCESS)
    {
        iStatus = CFE_SB_SubscribeEx(GPS_DATALOGGER_WAKEUP_MID, g_GPS_DATALOGGER_AppData.SchPipeId, CFE_SB_Default_Qos, 1);

        if (iStatus != CFE_SUCCESS)
        {
            CFE_ES_WriteToSysLog("GPS_DATALOGGER - Sch Pipe failed to subscribe to GPS_DATALOGGER_WAKEUP_MID. (0x%08X)\n", iStatus);
            goto GPS_DATALOGGER_InitPipe_Exit_Tag;
        }

    }
    else
    {
        CFE_ES_WriteToSysLog("GPS_DATALOGGER - Failed to create SCH pipe (0x%08X)\n", iStatus);
        goto GPS_DATALOGGER_InitPipe_Exit_Tag;
    }

    /* Init command pipe */
    g_GPS_DATALOGGER_AppData.usCmdPipeDepth = GPS_DATALOGGER_CMD_PIPE_DEPTH ;
    memset((void*)g_GPS_DATALOGGER_AppData.cCmdPipeName, '\0', sizeof(g_GPS_DATALOGGER_AppData.cCmdPipeName));
    strncpy(g_GPS_DATALOGGER_AppData.cCmdPipeName, "GPS_DATALOGGER_CMD_PIPE", OS_MAX_API_NAME-1);

    /* Subscribe to command messages */
    iStatus = CFE_SB_CreatePipe(&g_GPS_DATALOGGER_AppData.CmdPipeId,
                                 g_GPS_DATALOGGER_AppData.usCmdPipeDepth,
                                 g_GPS_DATALOGGER_AppData.cCmdPipeName);
    if (iStatus == CFE_SUCCESS)
    {
        /* Subscribe to command messages */
        iStatus = CFE_SB_Subscribe(GPS_DATALOGGER_CMD_MID, g_GPS_DATALOGGER_AppData.CmdPipeId);

        if (iStatus != CFE_SUCCESS)
        {
            CFE_ES_WriteToSysLog("GPS_DATALOGGER - CMD Pipe failed to subscribe to GPS_DATALOGGER_CMD_MID. (0x%08X)\n", iStatus);
            goto GPS_DATALOGGER_InitPipe_Exit_Tag;
        }

        iStatus = CFE_SB_Subscribe(GPS_DATALOGGER_SEND_HK_MID, g_GPS_DATALOGGER_AppData.CmdPipeId);

        if (iStatus != CFE_SUCCESS)
        {
            CFE_ES_WriteToSysLog("GPS_DATALOGGER - CMD Pipe failed to subscribe to GPS_DATALOGGER_SEND_HK_MID. (0x%08X)\n", iStatus);
            goto GPS_DATALOGGER_InitPipe_Exit_Tag;
        }

    }
    else
    {
        CFE_ES_WriteToSysLog("GPS_DATALOGGER - Failed to create CMD pipe (0x%08X)\n", iStatus);
        goto GPS_DATALOGGER_InitPipe_Exit_Tag;
    }

    /* Init telemetry pipe */
    g_GPS_DATALOGGER_AppData.usTlmPipeDepth = GPS_DATALOGGER_TLM_PIPE_DEPTH;
    memset((void*)g_GPS_DATALOGGER_AppData.cTlmPipeName, '\0', sizeof(g_GPS_DATALOGGER_AppData.cTlmPipeName));
    strncpy(g_GPS_DATALOGGER_AppData.cTlmPipeName, "GPS_DATALOGGER_TLM_PIPE", OS_MAX_API_NAME-1);

    /* Subscribe to telemetry messages on the telemetry pipe */
    iStatus = CFE_SB_CreatePipe(&g_GPS_DATALOGGER_AppData.TlmPipeId,
                                 g_GPS_DATALOGGER_AppData.usTlmPipeDepth,
                                 g_GPS_DATALOGGER_AppData.cTlmPipeName);
    if (iStatus == CFE_SUCCESS)
    {
        /* Subscribe to raw GPS data and kalman filter data */
        iStatus = CFE_SB_Subscribe(GPS_KALMAN_OUT_DATA_MID, g_GPS_DATALOGGER_AppData.TlmPipeId);
        if (iStatus != CFE_SUCCESS)
        {
            CFE_ES_WriteToSysLog(
                    "GPS_DATALOGGER - TLM Pipe failed to subscribe to GPS_KALMAN_OUT_DATA_MID. (0x%08X)\n",
                    iStatus);
            goto GPS_DATALOGGER_InitPipe_Exit_Tag;
        }

        /* Subscribe to raw GPS data and kalman filter data */
        iStatus = CFE_SB_Subscribe(GPS_READER_GPS_INFO_MSG, g_GPS_DATALOGGER_AppData.TlmPipeId);
        if (iStatus != CFE_SUCCESS)
        {
            CFE_ES_WriteToSysLog(
                    "GPS_DATALOGGER - TLM Pipe failed to subscribe to GPS_READER_GPS_INFO_MSG. (0x%08X)\n",
                    iStatus);
            goto GPS_DATALOGGER_InitPipe_Exit_Tag;
        }
    }
    else
    {
        CFE_ES_WriteToSysLog("GPS_DATALOGGER - Failed to create TLM pipe (0x%08X)\n", iStatus);
        goto GPS_DATALOGGER_InitPipe_Exit_Tag;
    }

GPS_DATALOGGER_InitPipe_Exit_Tag:
    return (iStatus);
}

/*=====================================================================================
** Name: GPS_DATALOGGER_InitData
**
** Purpose: To initialize global variables used by GPS_DATALOGGER application
**
** Arguments:
**    None
**
** Returns:
**    int32 iStatus - Status of initialization
**
** Routines Called:
**    CFE_SB_InitMsg
**
** Called By:
**    GPS_DATALOGGER_InitApp
**
** Global Inputs/Reads:
**    TBD
**
** Global Outputs/Writes:
**    g_GPS_DATALOGGER_AppData.InData
**    g_GPS_DATALOGGER_AppData.OutData
**    g_GPS_DATALOGGER_AppData.HkTlm
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to this function.
**    2. List the external source(s) and event(s) that can cause this function to execute.
**    3. List known limitations that apply to this function.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Algorithm:
**    Psuedo-code or description of basic algorithm
**
** Author(s):  Jacob Killelea
**
** History:  Date Written  2019-08-29
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
int32 GPS_DATALOGGER_InitData()
{
    int32 iStatus = CFE_SUCCESS;
    char raw_gps_log_path[512];
    char filter_gps_log_path[512];

    /* Init input data */
    memset((void*)&g_GPS_DATALOGGER_AppData.InData, 0x00, sizeof(g_GPS_DATALOGGER_AppData.InData));

    /* Init output data */
    memset((void*)&g_GPS_DATALOGGER_AppData.OutData, 0x00, sizeof(g_GPS_DATALOGGER_AppData.OutData));
    CFE_SB_InitMsg(&g_GPS_DATALOGGER_AppData.OutData,
                   GPS_DATALOGGER_OUT_DATA_MID, sizeof(g_GPS_DATALOGGER_AppData.OutData), TRUE);

    /* Init housekeeping packet */
    memset((void*)&g_GPS_DATALOGGER_AppData.HkTlm, 0x00, sizeof(g_GPS_DATALOGGER_AppData.HkTlm));
    CFE_SB_InitMsg(&g_GPS_DATALOGGER_AppData.HkTlm,
                   GPS_DATALOGGER_HK_TLM_MID, sizeof(g_GPS_DATALOGGER_AppData.HkTlm), TRUE);

    /* Second arg is unused in posix impl */
    iStatus = OS_mkdir("/cf/log/", 0);
    if (iStatus != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(GPS_DATALOGGER_INF_EID, CFE_EVS_ERROR, "Failed to open /cf/log/!");
        return (iStatus);
    }

    /* Set Permissions. */
    if (OS_chmod("/cf/log", OS_READ_WRITE) != OS_FS_SUCCESS) {
        CFE_EVS_SendEvent(GPS_DATALOGGER_INF_EID, CFE_EVS_ERROR,
                "Failed to chmod %s", "/cf/log");

        iStatus = CFE_ES_APP_ERROR;
        return (iStatus);
    }

    /* Init raw data logfile */
    iStatus = GPS_DATALOGGER_FindNextLogFileName("/cf/log/raw_gps_log", raw_gps_log_path, 512);

    if (iStatus != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(GPS_DATALOGGER_INF_EID, CFE_EVS_INFORMATION,
            "Failed to find the next available filename for /cf/log/raw_gps_log!");

        return (iStatus);
    }

    CFE_EVS_SendEvent(GPS_DATALOGGER_INF_EID, CFE_EVS_INFORMATION,
        "Next available filename for /cf/log/raw_gps_log is %s!", raw_gps_log_path);

    g_GPS_DATALOGGER_AppData.RawDataLogFileId = OS_creat(raw_gps_log_path, OS_READ_WRITE);

    /* Failed to open the file */
    if (g_GPS_DATALOGGER_AppData.RawDataLogFileId < 0)
    {
        CFE_EVS_SendEvent(GPS_DATALOGGER_INF_EID, CFE_EVS_ERROR, "Failed to open %s (%d)",
            raw_gps_log_path, g_GPS_DATALOGGER_AppData.RawDataLogFileId);

        iStatus = CFE_ES_APP_ERROR;
        return (iStatus);
    }

    /* Set Permissions. */
    if (OS_chmod(raw_gps_log_path, OS_READ_WRITE) != OS_FS_SUCCESS) {
        CFE_EVS_SendEvent(GPS_DATALOGGER_INF_EID, CFE_EVS_ERROR,
                "Failed to chmod %s", raw_gps_log_path);

        iStatus = CFE_ES_APP_ERROR;
        return (iStatus);
    }

    /* Init filtered data logfile */
    iStatus = GPS_DATALOGGER_FindNextLogFileName("/cf/log/filter_gps_log", filter_gps_log_path, 512);

    if (iStatus != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(GPS_DATALOGGER_INF_EID, CFE_EVS_INFORMATION,
            "Failed to find the next available filename for /cf/log/filter_gps_log!");
        return (iStatus);
    }

    CFE_EVS_SendEvent(GPS_DATALOGGER_INF_EID, CFE_EVS_ERROR,
        "Next available filename for /cf/log/filter_gps_log is %s!", filter_gps_log_path);

    g_GPS_DATALOGGER_AppData.FilteredDataLogFileId = OS_creat(filter_gps_log_path, OS_WRITE_ONLY);

    /* Failed to open the file */
    if (g_GPS_DATALOGGER_AppData.FilteredDataLogFileId < 0)
    {
        CFE_EVS_SendEvent(GPS_DATALOGGER_INF_EID, CFE_EVS_ERROR,
            "Failed to open %s (%d)",
            filter_gps_log_path, g_GPS_DATALOGGER_AppData.FilteredDataLogFileId);

        iStatus = CFE_ES_APP_ERROR;
        return (iStatus);
    }

    /* Set Permissions. */
    if (OS_chmod(filter_gps_log_path, OS_READ_WRITE) != OS_FS_SUCCESS) {
        CFE_EVS_SendEvent(GPS_DATALOGGER_INF_EID, CFE_EVS_ERROR,
                "Failed to chmod %s", filter_gps_log_path);

        iStatus = CFE_ES_APP_ERROR;
        return (iStatus);
    }

    /* Write headers */
    OS_write(g_GPS_DATALOGGER_AppData.RawDataLogFileId, "lattitude,longitude,speed,hdg\n",
            sizeof("lattitude,longitude,speed,hdg\n"));
    OS_write(g_GPS_DATALOGGER_AppData.FilteredDataLogFileId, "lattitude,longitude,speed,hdg\n",
            sizeof("lattitude,longitude,speed,hdg\n"));

    return (iStatus);
}

/*=====================================================================================
** Name: GPS_DATALOGGER_InitApp
**
** Purpose: To initialize all data local to and used by GPS_DATALOGGER application
**
** Arguments:
**    None
**
** Returns:
**    int32 iStatus - Status of initialization
**
** Routines Called:
**    CFE_ES_RegisterApp
**    CFE_ES_WriteToSysLog
**    CFE_EVS_SendEvent
**    OS_TaskInstallDeleteHandler
**    GPS_DATALOGGER_InitEvent
**    GPS_DATALOGGER_InitPipe
**    GPS_DATALOGGER_InitData
**
** Called By:
**    GPS_DATALOGGER_AppMain
**
** Global Inputs/Reads:
**    TBD
**
** Global Outputs/Writes:
**    TBD
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to this function.
**    2. List the external source(s) and event(s) that can cause this function to execute.
**    3. List known limitations that apply to this function.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Algorithm:
**    Psuedo-code or description of basic algorithm
**
** Author(s):  Jacob Killelea
**
** History:  Date Written  2019-08-29
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
int32 GPS_DATALOGGER_InitApp()
{
    int32  iStatus=CFE_SUCCESS;

    g_GPS_DATALOGGER_AppData.uiRunStatus = CFE_ES_APP_RUN;

    iStatus = CFE_ES_RegisterApp();
    if (iStatus != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("GPS_DATALOGGER - Failed to register the app (0x%08X)\n", iStatus);
        goto GPS_DATALOGGER_InitApp_Exit_Tag;
    }

    if ((GPS_DATALOGGER_InitEvent() != CFE_SUCCESS) ||
        (GPS_DATALOGGER_InitPipe() != CFE_SUCCESS) ||
        (GPS_DATALOGGER_InitData() != CFE_SUCCESS))
    {
        iStatus = -1;
        goto GPS_DATALOGGER_InitApp_Exit_Tag;
    }

    /* Install the cleanup callback */
    OS_TaskInstallDeleteHandler(GPS_DATALOGGER_CleanupCallback);

GPS_DATALOGGER_InitApp_Exit_Tag:
    if (iStatus == CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(GPS_DATALOGGER_INIT_INF_EID, CFE_EVS_INFORMATION,
                          "GPS_DATALOGGER - Application initialized");
    }
    else
    {
        CFE_ES_WriteToSysLog("GPS_DATALOGGER - Application failed to initialize\n");
    }

    return (iStatus);
}

/*=====================================================================================
** Name: GPS_DATALOGGER_CleanupCallback
**
** Purpose: To handle any neccesary cleanup prior to application exit
**
** Arguments:
**    None
**
** Returns:
**    None
**
** Routines Called:
**    TBD
**
** Called By:
**    TBD
**
** Global Inputs/Reads:
**    TBD
**
** Global Outputs/Writes:
**    TBD
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to this function.
**    2. List the external source(s) and event(s) that can cause this function to execute.
**    3. List known limitations that apply to this function.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Algorithm:
**    Psuedo-code or description of basic algorithm
**
** Author(s):  Jacob Killelea
**
** History:  Date Written  2019-08-29
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
void GPS_DATALOGGER_CleanupCallback()
{
    OS_close(g_GPS_DATALOGGER_AppData.RawDataLogFileId);
    OS_close(g_GPS_DATALOGGER_AppData.FilteredDataLogFileId);
}

/*=====================================================================================
** Name: GPS_DATALOGGER_RcvMsg
**
** Purpose: To receive and process messages for GPS_DATALOGGER application
**
** Arguments:
**    None
**
** Returns:
**    int32 iStatus - Status of initialization
**
** Routines Called:
**    CFE_SB_RcvMsg
**    CFE_SB_GetMsgId
**    CFE_EVS_SendEvent
**    CFE_ES_PerfLogEntry
**    CFE_ES_PerfLogExit
**    GPS_DATALOGGER_ProcessNewCmds
**    GPS_DATALOGGER_ProcessNewData
**    GPS_DATALOGGER_SendOutData
**
** Called By:
**    GPS_DATALOGGER_Main
**
** Global Inputs/Reads:
**    g_GPS_DATALOGGER_AppData.SchPipeId
**
** Global Outputs/Writes:
**    g_GPS_DATALOGGER_AppData.uiRunStatus
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to this function.
**    2. List the external source(s) and event(s) that can cause this function to execute.
**    3. List known limitations that apply to this function.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Algorithm:
**    Psuedo-code or description of basic algorithm
**
** Author(s):  Jacob Killelea
**
** History:  Date Written  2019-08-29
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
int32 GPS_DATALOGGER_RcvMsg(int32 iBlocking)
{
    int32          iStatus = CFE_SUCCESS;
    CFE_SB_Msg_t*  MsgPtr  = NULL;
    CFE_SB_MsgId_t MsgId;

    /* Stop Performance Log entry */
    CFE_ES_PerfLogExit(GPS_DATALOGGER_MAIN_TASK_PERF_ID);

    /* Wait for WakeUp messages from scheduler */
    iStatus = CFE_SB_RcvMsg(&MsgPtr, g_GPS_DATALOGGER_AppData.SchPipeId, iBlocking);

    /* Start Performance Log entry */
    CFE_ES_PerfLogEntry(GPS_DATALOGGER_MAIN_TASK_PERF_ID);

    if (iStatus == CFE_SUCCESS)
    {
        MsgId = CFE_SB_GetMsgId(MsgPtr);
        switch (MsgId)
        {
            case GPS_DATALOGGER_WAKEUP_MID:
                GPS_DATALOGGER_ProcessNewCmds();
                GPS_DATALOGGER_ProcessNewData();

                /* TODO:  Add more code here to handle other things when app wakes up */

                /* The last thing to do at the end of this Wakeup cycle should be to
                   automatically publish new output. */
                GPS_DATALOGGER_SendOutData();
                break;

            default:
                CFE_EVS_SendEvent(GPS_DATALOGGER_MSGID_ERR_EID, CFE_EVS_ERROR,
                                  "GPS_DATALOGGER - Recvd invalid SCH msgId (0x%08X)", MsgId);
        }
    }
    else if (iStatus == CFE_SB_NO_MESSAGE)
    {
        /* If there's no incoming message, you can do something here, or nothing */
    }
    else
    {
        /* This is an example of exiting on an error.
        ** Note that a SB read error is not always going to result in an app quitting.
        */
        CFE_EVS_SendEvent(GPS_DATALOGGER_PIPE_ERR_EID, CFE_EVS_ERROR,
            "GPS_DATALOGGER: SB pipe read error (0x%08X), app will exit", iStatus);
        g_GPS_DATALOGGER_AppData.uiRunStatus= CFE_ES_APP_ERROR;
    }

    return (iStatus);
}

/*=====================================================================================
** Name: GPS_DATALOGGER_ProcessNewData
**
** Purpose: To process incoming data subscribed by GPS_DATALOGGER application
**
** Arguments:
**    None
**
** Returns:
**    None
**
** Routines Called:
**    CFE_SB_RcvMsg
**    CFE_SB_GetMsgId
**    CFE_EVS_SendEvent
**    OS_write
**    snprintf
**
** Called By:
**    GPS_DATALOGGER_RcvMsg
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**    Writes to FilteredDataLogFileId and RawDataLogFileId.
**    If either of these calls fails to write as many bytes as expected,
**    g_GPS_DATALOGGER_AppData.uiRunStatus is set to CFE_EVS_ERROR
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to this function.
**    2. List the external source(s) and event(s) that can cause this function to execute.
**    3. List known limitations that apply to this function.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Algorithm:
**    Psuedo-code or description of basic algorithm
**
** Author(s):  Jacob Killelea
**
** History:  Date Written  2019-08-29
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
void GPS_DATALOGGER_ProcessNewData()
{
    int iStatus = CFE_SUCCESS;
    CFE_SB_Msg_t*   TlmMsgPtr=NULL;
    CFE_SB_MsgId_t  TlmMsgId;

    char LogDataBuffer[256];
    int32 nbytes_to_write = 0;
    int32 nbytes_written = 0;

    /* Process telemetry messages till the pipe is empty */
    while (1)
    {
        iStatus = CFE_SB_RcvMsg(&TlmMsgPtr, g_GPS_DATALOGGER_AppData.TlmPipeId, CFE_SB_POLL);
        if (iStatus == CFE_SUCCESS)
        {
            TlmMsgId = CFE_SB_GetMsgId(TlmMsgPtr);
            switch (TlmMsgId)
            {
                /* TODO: Add code to process all subscribed data here */
                case GPS_KALMAN_OUT_DATA_MID:
                    CFE_EVS_SendEvent(GPS_DATALOGGER_INF_EID, CFE_EVS_INFORMATION,
                                      "GPS_DATALOGGER - Recvd GPS_KALMAN data");

                    GPS_KALMAN_OutData_t *kalmanData = (GPS_KALMAN_OutData_t *) TlmMsgPtr;
                    nbytes_to_write = snprintf(LogDataBuffer, sizeof(LogDataBuffer),
                            /* lat,lon,spd,hdg */
                            "%lf,%lf,%lf,%lf\n",
                            kalmanData->filterLat,
                            kalmanData->filterLon,
                            kalmanData->filterVel,
                            kalmanData->filterHdg);

                    nbytes_written = OS_write(g_GPS_DATALOGGER_AppData.FilteredDataLogFileId,
                                              LogDataBuffer,
                                              nbytes_to_write);

                    if (nbytes_written != nbytes_to_write) {
                        CFE_EVS_SendEvent(GPS_DATALOGGER_INF_EID, CFE_EVS_ERROR,
                                  "Didn't write as many bytes as expected (%d of %d)",
                                  nbytes_written, nbytes_to_write);
                        g_GPS_DATALOGGER_AppData.uiRunStatus = CFE_EVS_ERROR;
                    }
                    break;

                case GPS_READER_GPS_INFO_MSG:
                    CFE_EVS_SendEvent(GPS_DATALOGGER_INF_EID, CFE_EVS_INFORMATION,
                                      "GPS_DATALOGGER - Recvd GPS_READER data");

                    GpsInfoMsg_t *rawData = (GpsInfoMsg_t *) TlmMsgPtr;

                    nbytes_to_write = snprintf(LogDataBuffer, sizeof(LogDataBuffer),
                            /* lat,lon,spd,hdg */
                            "%lf,%lf,%lf,%lf\n",
                            rawData->gpsInfo.lat,
                            rawData->gpsInfo.lon,
                            rawData->gpsInfo.speed,
                            rawData->gpsInfo.direction);

                    nbytes_written = OS_write(g_GPS_DATALOGGER_AppData.RawDataLogFileId,
                                              LogDataBuffer,
                                              nbytes_to_write);

                    if (nbytes_written != nbytes_to_write) {
                        CFE_EVS_SendEvent(GPS_DATALOGGER_INF_EID, CFE_EVS_ERROR,
                                  "Didn't write as many bytes as expected (%d of %d)",
                                  nbytes_written, nbytes_to_write);
                        g_GPS_DATALOGGER_AppData.uiRunStatus = CFE_EVS_ERROR;
                    }
                    break;

                default:
                    CFE_EVS_SendEvent(GPS_DATALOGGER_MSGID_ERR_EID, CFE_EVS_ERROR,
                                      "GPS_DATALOGGER - Recvd invalid TLM msgId (0x%08X)", TlmMsgId);
                    break;
            }
        }
        else if (iStatus == CFE_SB_NO_MESSAGE)
        {
            break;
        }
        else
        {
            CFE_EVS_SendEvent(GPS_DATALOGGER_PIPE_ERR_EID, CFE_EVS_ERROR,
                  "GPS_DATALOGGER: CMD pipe read error (0x%08X)", iStatus);
            g_GPS_DATALOGGER_AppData.uiRunStatus = CFE_ES_APP_ERROR;
            break;
        }
    }
}

/*=====================================================================================
** Name: GPS_DATALOGGER_ProcessNewCmds
**
** Purpose: To process incoming command messages for GPS_DATALOGGER application
**
** Arguments:
**    None
**
** Returns:
**    None
**
** Routines Called:
**    CFE_SB_RcvMsg
**    CFE_SB_GetMsgId
**    CFE_EVS_SendEvent
**    GPS_DATALOGGER_ProcessNewAppCmds
**    GPS_DATALOGGER_ReportHousekeeping
**
** Called By:
**    GPS_DATALOGGER_RcvMsg
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**    None
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to this function.
**    2. List the external source(s) and event(s) that can cause this function to execute.
**    3. List known limitations that apply to this function.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Algorithm:
**    Psuedo-code or description of basic algorithm
**
** Author(s):  Jacob Killelea
**
** History:  Date Written  2019-08-29
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
void GPS_DATALOGGER_ProcessNewCmds()
{
    int iStatus = CFE_SUCCESS;
    CFE_SB_Msg_t*   CmdMsgPtr=NULL;
    CFE_SB_MsgId_t  CmdMsgId;

    /* Process command messages till the pipe is empty */
    while (1)
    {
        iStatus = CFE_SB_RcvMsg(&CmdMsgPtr, g_GPS_DATALOGGER_AppData.CmdPipeId, CFE_SB_POLL);
        if(iStatus == CFE_SUCCESS)
        {
            CmdMsgId = CFE_SB_GetMsgId(CmdMsgPtr);
            switch (CmdMsgId)
            {
                case GPS_DATALOGGER_CMD_MID:
                    GPS_DATALOGGER_ProcessNewAppCmds(CmdMsgPtr);
                    break;

                case GPS_DATALOGGER_SEND_HK_MID:
                    GPS_DATALOGGER_ReportHousekeeping();
                    break;

                /* TODO:  Add code to process other subscribed commands here
                **
                ** Example:
                **     case CFE_TIME_DATA_CMD_MID:
                **         GPS_DATALOGGER_ProcessTimeDataCmd(CmdMsgPtr);
                **         break;
                */

                default:
                    CFE_EVS_SendEvent(GPS_DATALOGGER_MSGID_ERR_EID, CFE_EVS_ERROR,
                                      "GPS_DATALOGGER - Recvd invalid CMD msgId (0x%08X)", CmdMsgId);
                    break;
            }
        }
        else if (iStatus == CFE_SB_NO_MESSAGE)
        {
            break;
        }
        else
        {
            CFE_EVS_SendEvent(GPS_DATALOGGER_PIPE_ERR_EID, CFE_EVS_ERROR,
                  "GPS_DATALOGGER: CMD pipe read error (0x%08X)", iStatus);
            g_GPS_DATALOGGER_AppData.uiRunStatus = CFE_ES_APP_ERROR;
            break;
        }
    }
}

/*=====================================================================================
** Name: GPS_DATALOGGER_ProcessNewAppCmds
**
** Purpose: To process command messages targeting GPS_DATALOGGER application
**
** Arguments:
**    CFE_SB_Msg_t*  MsgPtr - new command message pointer
**
** Returns:
**    None
**
** Routines Called:
**    CFE_SB_GetCmdCode
**    CFE_EVS_SendEvent
**
** Called By:
**    GPS_DATALOGGER_ProcessNewCmds
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**    g_GPS_DATALOGGER_AppData.HkTlm.usCmdCnt
**    g_GPS_DATALOGGER_AppData.HkTlm.usCmdErrCnt
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to this function.
**    2. List the external source(s) and event(s) that can cause this function to execute.
**    3. List known limitations that apply to this function.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Algorithm:
**    Psuedo-code or description of basic algorithm
**
** Author(s):  Jacob Killelea
**
** History:  Date Written  2019-08-29
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
void GPS_DATALOGGER_ProcessNewAppCmds(CFE_SB_Msg_t* MsgPtr)
{
    uint32  uiCmdCode=0;

    if (MsgPtr != NULL)
    {
        uiCmdCode = CFE_SB_GetCmdCode(MsgPtr);
        switch (uiCmdCode)
        {
            case GPS_DATALOGGER_NOOP_CC:
                g_GPS_DATALOGGER_AppData.HkTlm.usCmdCnt++;
                CFE_EVS_SendEvent(GPS_DATALOGGER_CMD_INF_EID, CFE_EVS_INFORMATION,
                                  "GPS_DATALOGGER - Recvd NOOP cmd (%d)", uiCmdCode);
                break;

            case GPS_DATALOGGER_RESET_CC:
                g_GPS_DATALOGGER_AppData.HkTlm.usCmdCnt = 0;
                g_GPS_DATALOGGER_AppData.HkTlm.usCmdErrCnt = 0;
                CFE_EVS_SendEvent(GPS_DATALOGGER_CMD_INF_EID, CFE_EVS_INFORMATION,
                                  "GPS_DATALOGGER - Recvd RESET cmd (%d)", uiCmdCode);
                break;

            /* TODO:  Add code to process the rest of the GPS_DATALOGGER commands here */

            default:
                g_GPS_DATALOGGER_AppData.HkTlm.usCmdErrCnt++;
                CFE_EVS_SendEvent(GPS_DATALOGGER_MSGID_ERR_EID, CFE_EVS_ERROR,
                                  "GPS_DATALOGGER - Recvd invalid cmdId (%d)", uiCmdCode);
                break;
        }
    }
}

/*=====================================================================================
** Name: GPS_DATALOGGER_ReportHousekeeping
**
** Purpose: To send housekeeping message
**
** Arguments:
**    None
**
** Returns:
**    None
**
** Routines Called:
**    TBD
**
** Called By:
**    GPS_DATALOGGER_ProcessNewCmds
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**    TBD
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to this function.
**    2. List the external source(s) and event(s) that can cause this function to execute.
**    3. List known limitations that apply to this function.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Algorithm:
**    Psuedo-code or description of basic algorithm
**
** Author(s):  GSFC, Jacob Killelea
**
** History:  Date Written  2019-08-29
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
void GPS_DATALOGGER_ReportHousekeeping()
{
    /* TODO:  Add code to update housekeeping data, if needed, here.  */

    CFE_SB_TimeStampMsg((CFE_SB_Msg_t*)&g_GPS_DATALOGGER_AppData.HkTlm);
    CFE_SB_SendMsg((CFE_SB_Msg_t*)&g_GPS_DATALOGGER_AppData.HkTlm);
}

/*=====================================================================================
** Name: GPS_DATALOGGER_SendOutData
**
** Purpose: To publish 1-Wakeup cycle output data
**
** Arguments:
**    None
**
** Returns:
**    None
**
** Routines Called:
**    TBD
**
** Called By:
**    GPS_DATALOGGER_RcvMsg
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**    TBD
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to this function.
**    2. List the external source(s) and event(s) that can cause this function to execute.
**    3. List known limitations that apply to this function.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Algorithm:
**    Psuedo-code or description of basic algorithm
**
** Author(s):  Jacob Killelea
**
** History:  Date Written  2019-08-29
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
void GPS_DATALOGGER_SendOutData()
{
    /* TODO:  Add code to update output data, if needed, here.  */

    CFE_SB_TimeStampMsg((CFE_SB_Msg_t*)&g_GPS_DATALOGGER_AppData.OutData);
    CFE_SB_SendMsg((CFE_SB_Msg_t*)&g_GPS_DATALOGGER_AppData.OutData);
}

/*=====================================================================================
** Name: GPS_DATALOGGER_VerifyCmdLength
**
** Purpose: To verify command length for a particular command message
**
** Arguments:
**    CFE_SB_Msg_t*  MsgPtr      - command message pointer
**    uint16         usExpLength - expected command length
**
** Returns:
**    boolean bResult - result of verification
**
** Routines Called:
**    TBD
**
** Called By:
**    GPS_DATALOGGER_ProcessNewCmds
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**    TBD
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to this function.
**    2. List the external source(s) and event(s) that can cause this function to execute.
**    3. List known limitations that apply to this function.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Algorithm:
**    Psuedo-code or description of basic algorithm
**
** Author(s):  Jacob Killelea
**
** History:  Date Written  2019-08-29
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
boolean GPS_DATALOGGER_VerifyCmdLength(CFE_SB_Msg_t* MsgPtr,
                           uint16 usExpectedLen)
{
    boolean bResult=FALSE;
    uint16  usMsgLen=0;

    if (MsgPtr != NULL)
    {
        usMsgLen = CFE_SB_GetTotalMsgLength(MsgPtr);

        if (usExpectedLen != usMsgLen)
        {
            CFE_SB_MsgId_t MsgId = CFE_SB_GetMsgId(MsgPtr);
            uint16 usCmdCode = CFE_SB_GetCmdCode(MsgPtr);

            CFE_EVS_SendEvent(GPS_DATALOGGER_MSGLEN_ERR_EID, CFE_EVS_ERROR,
                              "GPS_DATALOGGER - Rcvd invalid msgLen: msgId=0x%08X, cmdCode=%d, "
                              "msgLen=%d, expectedLen=%d",
                              MsgId, usCmdCode, usMsgLen, usExpectedLen);
            g_GPS_DATALOGGER_AppData.HkTlm.usCmdErrCnt++;
        }
    }

    return (bResult);
}

/*=====================================================================================
** Name: GPS_DATALOGGER_AppMain
**
** Purpose: To define GPS_DATALOGGER application's entry point and main process loop
**
** Arguments:
**    None
**
** Returns:
**    None
**
** Routines Called:
**    CFE_ES_RegisterApp
**    CFE_ES_RunLoop
**    CFE_ES_PerfLogEntry
**    CFE_ES_PerfLogExit
**    CFE_ES_ExitApp
**    CFE_ES_WaitForStartupSync
**    GPS_DATALOGGER_InitApp
**    GPS_DATALOGGER_RcvMsg
**
** Called By:
**    TBD
**
** Global Inputs/Reads:
**    TBD
**
** Global Outputs/Writes:
**    TBD
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to this function.
**    2. List the external source(s) and event(s) that can cause this function to execute.
**    3. List known limitations that apply to this function.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Algorithm:
**    Psuedo-code or description of basic algorithm
**
** Author(s):  Jacob Killelea
**
** History:  Date Written  2019-08-29
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
void GPS_DATALOGGER_AppMain()
{
    /* Register the application with Executive Services */
    CFE_ES_RegisterApp();

    /* Start Performance Log entry */
    CFE_ES_PerfLogEntry(GPS_DATALOGGER_MAIN_TASK_PERF_ID);

    /* Perform application initializations */
    if (GPS_DATALOGGER_InitApp() != CFE_SUCCESS)
    {
        g_GPS_DATALOGGER_AppData.uiRunStatus = CFE_ES_APP_ERROR;
    } else {
        /* Do not perform performance monitoring on startup sync */
        CFE_ES_PerfLogExit(GPS_DATALOGGER_MAIN_TASK_PERF_ID);
        CFE_ES_WaitForStartupSync(GPS_DATALOGGER_TIMEOUT_MSEC);
        CFE_ES_PerfLogEntry(GPS_DATALOGGER_MAIN_TASK_PERF_ID);
    }

    /* Application main loop */
    while (CFE_ES_RunLoop(&g_GPS_DATALOGGER_AppData.uiRunStatus) == TRUE)
    {
        GPS_DATALOGGER_RcvMsg(CFE_SB_PEND_FOREVER);
    }

    /* Stop Performance Log entry */
    CFE_ES_PerfLogExit(GPS_DATALOGGER_MAIN_TASK_PERF_ID);

    /* Exit the application */
    CFE_ES_ExitApp(g_GPS_DATALOGGER_AppData.uiRunStatus);
}

/*=======================================================================================
** End of file gps_datalogger_app.c
**=====================================================================================*/






/*=====================================================================================
** Name: GPS_DATALOGGER_FindNextLogFileName
**
** Purpose: Find the next available logfile name. Example: rawdata0, rawdata1...
**
** Arguments:
**    None
**
** Returns:
**    None
**
** Routines Called:
**    OS_creat
**    OS_readdir
**    OS_stat
**
**
** Called By:
**    GPS_DATALOGGER_InitData
**
** Global Inputs/Reads:
**    Searches the root filesystem
**
** Global Outputs/Writes:
**
** Limitations, Assumptions, External Events, and Notes:
**    1. The filesystem is readable
**
** Algorithm:
**    Searches for the the maximum N of files named 'filePredicateN'. Returns 'filePredicate(N+1)'
**    If no file matching the predicate is found, returns 'filePredicate0'
**
** Author(s):  Jacob Killelea
**
** History:  Date Written  2019-08-29
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
int32 GPS_DATALOGGER_FindNextLogFileName(const char *filePredicate, char *nextAvailableName,
        uint32 nextAvailableNameLength)
{
    int32 iStatus = CFE_SUCCESS;
    uint32 i;
    os_fstat_t fileStatus;

    for (i = 0; i <= GPS_DATALOGGER_MAX_LOGFILE_NUMBER; i++) {
        snprintf(nextAvailableName, nextAvailableNameLength, "%s%d", filePredicate, i);

        /* Not Found */
        if (OS_stat(nextAvailableName, &fileStatus) != OS_FS_SUCCESS)
        {
            break;
        }
    }

    /* All names taken */
    if (i == GPS_DATALOGGER_MAX_LOGFILE_NUMBER)
    {
        iStatus = CFE_ES_APP_ERROR;
    }

    return(iStatus);
}
