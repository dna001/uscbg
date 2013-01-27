/******************************************************************************
Copyright (c) 2003, All Rights Reserved.

$Author$
$Id$ $Revision$ $Date$
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file sys_ctrl.h
\brief Control bitfield definitions.  */
/*---------------------------------------------------------------------------*/
#ifndef SYS_CTRL_H
#define SYS_CTRL_H

/* EXPORTED DEFINES **********************************************************/

/*-----------------------------------------------------------------------------
The following defines enables commands for the unit it represents. This is the
place where this functionality shall be configured.
-----------------------------------------------------------------------------*/
#define SYS_CMD_OSA    0x00000001  /* Commands for the OS Abstraction. */
#define SYS_CMD_TRC    0x00000002  /* Commands for the TRC component */
#define SYS_CMD_MEM    0x00000004  /* Commands for the MEM component. */
#define SYS_CMD_CIO    0x00000008  /* Commands for the CIO component. */
#define SYS_CMD_TRL    0x00000010  /* Commands for the TRL component. */
#define SYS_CMD_SWT    0x80000000  /* Commands SoftWare Tests only. */

/* EXPORTED DATA TYPES *******************************************************/

/* GLOBAL VARIABLES **********************************************************/

/* INTERFACE FUNCTIONS *******************************************************/

#endif /* #ifndef SYS_CTRL_H */

/* END OF FILE ***************************************************************/
