/******************************************************************************
Copyright (c) 2013, All Rights Reserved.
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file net_us.c
\brief The net_us implementation. */
/*---------------------------------------------------------------------------*/
/* INCLUDE FILES *************************************************************/
#include "sys_def.h"
#include "sys_assert.h"
#include "net_us.h"

/* CONSTANTS / MACROS ********************************************************/

/* LOCAL DATATYPES ***********************************************************/

/* LOCAL FUNCTION PROTOTYPES *************************************************/

/* MODULE CONSTANTS / VARIABLES **********************************************/
/*** Remove this comment if you want to use an ASSERT
SYS_ASSERT_FILE;
***/
SYS_DBC_FILE;  /*!< Defines the name of this source file once for all */

/* GLOBAL CONSTANTS / VARIABLES **********************************************/
char* net_cmd_to_str[NET_CMD_LAST] =
{
   "None",
   "Server Player Info", /* NET_CMD_SERVER_PLAYER_INFO */
   "Server Player Remove",
   "Server Start Game",
   "Server Select Color",
   "Server Select Action",
   "Server Select Building Rotation",
   "Server Select Board Lot",
   "Server Select Board Card",
   "Server Select Player Card",
   "Server Select Card Choice",
   "Server Player Update",
   "Server Board Cards Update",
   "Server Block Update",
   "Server Card Update",
   "Server Active Player",
   "Server Phase Update",
   "Server Log Entry",
   "Server Game End",
   "Client Player Name", /* NET_CMD_CLIENT_PLAYER_NAME */
   "Client Start Game",
   "Client Load Game",
   "Client Save Game",
   "Client Select Color",
   "Client Select Action",
   "Client Select Building Rotation",
   "Client Select Board Lot",
   "Client Select Board Card",
   "Client Select Player Card",
   "Client Select Card Choice",
   "Client Pass",
   "Client Done",
   "Client Back"
};

/* GLOBAL FUNCTIONS **********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void net_us_init(void)
{
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
const char* net_us_cmd_to_str(net_us_cmd_t cmd)
{
   char* cmd_str = NULL;
   if (cmd < NET_CMD_LAST)
   {
      cmd_str = net_cmd_to_str[cmd];
   }
   return cmd_str;
}

/* LOCAL FUNCTIONS ***********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/

/* END OF FILE ***************************************************************/
