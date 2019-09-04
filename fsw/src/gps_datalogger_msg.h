/*=======================================================================================
** File Name:  gps_datalogger_msg.h
**
** Title:  Message Definition Header File for GPS_DATALOGGER Application
**
** $Author:    Jacob Killelea
** $Revision: 1.1 $
** $Date:      2019-08-29
**
** Purpose:  To define GPS_DATALOGGER's command and telemetry message defintions 
**
** Modification History:
**   Date | Author | Description
**   ---------------------------
**   2019-08-29 | Jacob Killelea | Build #: Code Started
**
**=====================================================================================*/
    
#ifndef _GPS_DATALOGGER_MSG_H_
#define _GPS_DATALOGGER_MSG_H_

/*
** Pragmas
*/

/*
** Include Files
*/



/*
** Local Defines
*/

/*
** GPS_DATALOGGER command codes
*/
#define GPS_DATALOGGER_NOOP_CC                 0
#define GPS_DATALOGGER_RESET_CC                1

/*
** Local Structure Declarations
*/
typedef struct
{
    uint8              TlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint8              usCmdCnt;
    uint8              usCmdErrCnt;

    /* TODO:  Add declarations for additional housekeeping data here */

} GPS_DATALOGGER_HkTlm_t;


#endif /* _GPS_DATALOGGER_MSG_H_ */

/*=======================================================================================
** End of file gps_datalogger_msg.h
**=====================================================================================*/
    