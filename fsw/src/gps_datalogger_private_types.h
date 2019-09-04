/*=======================================================================================
** File Name:  gps_datalogger_private_types.h
**
** Title:  Type Header File for GPS_DATALOGGER Application
**
** $Author:    Jacob Killelea
** $Revision: 1.1 $
** $Date:      2019-08-29
**
** Purpose:  This header file contains declarations and definitions of all GPS_DATALOGGER's private
**           data structures and data types.
**
** Modification History:
**   Date | Author | Description
**   ---------------------------
**   2019-08-29 | Jacob Killelea | Build #: Code Started
**
**=====================================================================================*/
    
#ifndef _GPS_DATALOGGER_PRIVATE_TYPES_H_
#define _GPS_DATALOGGER_PRIVATE_TYPES_H_

/*
** Pragmas
*/

/*
** Include Files
*/
#include "cfe.h"

/*
** Local Defines
*/

/*
** Local Structure Declarations
*/

typedef struct
{
    uint8  ucCmdHeader[CFE_SB_CMD_HDR_SIZE];
} GPS_DATALOGGER_NoArgCmd_t;


typedef struct
{
    uint32  counter;

    /* TODO:  Add input data to this application here, such as raw data read from I/O
    **        devices or data subscribed from other apps' output data.
    */

} GPS_DATALOGGER_InData_t;

typedef struct
{
    uint8   ucTlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint32  uiCounter;
} GPS_DATALOGGER_OutData_t;

/* TODO:  Add more private structure definitions here, if necessary. */

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

#endif /* _GPS_DATALOGGER_PRIVATE_TYPES_H_ */

/*=======================================================================================
** End of file gps_datalogger_private_types.h
**=====================================================================================*/
    