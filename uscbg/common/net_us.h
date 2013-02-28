/******************************************************************************
Copyright (c) 2013, All Rights Reserved.
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file net_us.h
\brief Shared Urban Sprawl net types/functions between server and client*/
/*---------------------------------------------------------------------------*/
#ifndef NET_US_H
#define NET_US_H
/* INCLUDE FILES *************************************************************/

/* EXPORTED DEFINES **********************************************************/

/* EXPORTED DATA TYPES *******************************************************/
typedef enum
{
   NET_CMD_NONE = 0,
   /* Server->Client messages */
   NET_CMD_SERVER_PLAYER_INFO,
   NET_CMD_SERVER_PLAYER_REMOVE,
   NET_CMD_SERVER_START_GAME,
   NET_CMD_SERVER_SELECT_COLOR,
   NET_CMD_SERVER_SELECT_ACTION,
   NET_CMD_SERVER_SELECT_BUILDING_ROTATION,
   NET_CMD_SERVER_SELECT_BOARD_LOT,
   NET_CMD_SERVER_SELECT_BOARD_CARD,
   NET_CMD_SERVER_SELECT_PLAYER_CARD,
   NET_CMD_SERVER_SELECT_CARD_CHOICE,
   NET_CMD_SERVER_PLAYER_UPDATE,
   NET_CMD_SERVER_BOARD_CARDS_UPDATE,
   NET_CMD_SERVER_BLOCK_UPDATE,
   NET_CMD_SERVER_CARD_UPDATE,
   NET_CMD_SERVER_ACTIVE_PLAYER,
   NET_CMD_SERVER_PHASE_UPDATE,
   NET_CMD_SERVER_LOG_ENTRY,
   NET_CMD_SERVER_GAME_END,
   /* Client->Server messages */
   NET_CMD_CLIENT_PLAYER_NAME,
   NET_CMD_CLIENT_START_GAME,
   NET_CMD_CLIENT_LOAD_GAME,
   NET_CMD_CLIENT_SAVE_GAME,
   NET_CMD_CLIENT_SELECT_COLOR,
   NET_CMD_CLIENT_SELECT_ACTION,
   NET_CMD_CLIENT_SELECT_BUILDING_ROTATION,
   NET_CMD_CLIENT_SELECT_BOARD_LOT,
   NET_CMD_CLIENT_SELECT_BOARD_CARD,
   NET_CMD_CLIENT_SELECT_PLAYER_CARD,
   NET_CMD_CLIENT_SELECT_CARD_CHOICE,
   NET_CMD_CLIENT_PASS,
   NET_CMD_CLIENT_DONE,
   NET_CMD_CLIENT_BACK,
   NET_CMD_LAST
} net_us_cmd_t;

/* GLOBAL VARIABLES **********************************************************/

/* INTERFACE FUNCTIONS *******************************************************/

/*---------------------------------------------------------------------------*/
/*! \brief Initialize. */
/*---------------------------------------------------------------------------*/
void net_us_init(
   void
   );

/*---------------------------------------------------------------------------*/
/*! \brief Net US command to string. */
/*---------------------------------------------------------------------------*/
const char* net_us_cmd_to_str(
   net_us_cmd_t cmd
   );

#endif /* #ifndef NET_US_H */
/* END OF FILE ***************************************************************/
