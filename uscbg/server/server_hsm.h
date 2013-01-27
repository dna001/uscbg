/******************************************************************************
Copyright (c) 2013, All Rights Reserved.
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file server_hsm.h
\brief The Server Hierarchical State Machine interface. */
/*---------------------------------------------------------------------------*/
#ifndef SERVER_HSM_H
#define SERVER_HSM_H
/* INCLUDE FILES *************************************************************/
#include "hsm.h"

/* EXPORTED DEFINES **********************************************************/

/* EXPORTED DATA TYPES *******************************************************/
enum
{
   HSM_EVT_NET_CONNECTED = HSM_EVT_USER,
   HSM_EVT_NET_DISCONNECTED,
   HSM_EVT_NET_START_GAME,
   HSM_EVT_NET_LOAD_GAME,
   HSM_EVT_NET_SAVE_GAME,
   HSM_EVT_NET_SELECT_COLOR,
   HSM_EVT_NET_SELECT_ACTION,
   HSM_EVT_NET_SELECT_BOARD_LOT,
   HSM_EVT_NET_SELECT_BOARD_CARD,
   HSM_EVT_NET_SELECT_PLAYER_CARD,
   HSM_EVT_NET_SELECT_CARD_CHOICE,
   HSM_EVT_NET_PASS,
   HSM_EVT_NET_DONE,
   HSM_EVT_STOP,
   HSM_EVT_TIMER
};

/* GLOBAL VARIABLES **********************************************************/

/* INTERFACE FUNCTIONS *******************************************************/

/*---------------------------------------------------------------------------*/
/*! \brief Initialize. */
/*---------------------------------------------------------------------------*/
void srv_hsm_init(
   void
   );

/*---------------------------------------------------------------------------*/
/*! \brief Start state machine. */
/*---------------------------------------------------------------------------*/
void srv_hsm_start(
   void
   );

/*---------------------------------------------------------------------------*/
/*! \brief Stop state machine. */
/*---------------------------------------------------------------------------*/
void srv_hsm_stop(
   void
   );

/*---------------------------------------------------------------------------*/
/*! \brief Send event to srv hsm. */
/*---------------------------------------------------------------------------*/
void srv_hsm_evt(
   int evt
   );

#endif /* #ifndef SERVER_HSM_H */
/* END OF FILE ***************************************************************/
