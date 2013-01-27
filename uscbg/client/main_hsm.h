/******************************************************************************
Copyright (c) 2013, All Rights Reserved.
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file main_hsm.h
\brief The Main Hierarchical State Machine interface. */
/*---------------------------------------------------------------------------*/
#ifndef MAIN_HSM_H
#define MAIN_HSM_H
/* INCLUDE FILES *************************************************************/
#include "hsm.h"

/* EXPORTED DEFINES **********************************************************/

/* EXPORTED DATA TYPES *******************************************************/
enum
{
   HSM_EVT_MENU_BTN_NEWGAME = HSM_EVT_USER,
   HSM_EVT_MENU_BTN_CONNECT,
   HSM_EVT_MENU_BTN_LOADGAME,
   HSM_EVT_MENU_BTN_SAVEGAME,
   HSM_EVT_MENU_BTN_START,
   HSM_EVT_MENU_BTN_BACK,
   HSM_EVT_MENU_BTN_EXIT,
   HSM_EVT_MENU_BTN_PASS,
   HSM_EVT_MENU_BTN_DONE,
   HSM_EVT_MENU_BTN_1,
   HSM_EVT_MENU_BTN_2,
   HSM_EVT_MENU_BTN_3,
   HSM_EVT_MENU_BTN_4,
   HSM_EVT_MENU_BTN_5,
   HSM_EVT_MENU_BTN_6,
   HSM_EVT_MENU_BTN_RANDOM,
   HSM_EVT_MENU_BTN_CENTER,
   HSM_EVT_MENU_BTN_COORDS,
   HSM_EVT_BOARD_LOT,
   HSM_EVT_CARD,
   HSM_EVT_TOGGLE_DEV_BOARD,
   HSM_EVT_TOGGLE_CARD_WINDOW,
   HSM_EVT_NET_UPDATE_ACTIVE_PLAYER,
   HSM_EVT_NET_UPDATE_PLAYERS,
   HSM_EVT_NET_UPDATE_GAMEBOARD,
   HSM_EVT_NET_UPDATE_BOARD_CARDS,
   HSM_EVT_NET_UPDATE_PHASE,
   HSM_EVT_NET_UPDATE_LOG,
   HSM_EVT_NET_START_GAME,
   HSM_EVT_NET_SELECT_COLOR,
   HSM_EVT_NET_SELECT_BOARD_LOT,
   HSM_EVT_NET_SELECT_BOARD_CARD,
   HSM_EVT_NET_SELECT_PLAYER_CARD,
   HSM_EVT_NET_SELECT_CARD_CHOICE,
   HSM_EVT_SELECT,
   HSM_EVT_CANCEL,
   HSM_EVT_STOP,
   HSM_EVT_TIMER
};

/* GLOBAL VARIABLES **********************************************************/

/* INTERFACE FUNCTIONS *******************************************************/

/*---------------------------------------------------------------------------*/
/*! \brief Initialize. */
/*---------------------------------------------------------------------------*/
void main_hsm_init(
   void
   );

/*---------------------------------------------------------------------------*/
/*! \brief Start state machine. */
/*---------------------------------------------------------------------------*/
void main_hsm_start(
   void
   );

/*---------------------------------------------------------------------------*/
/*! \brief Stop state machine. */
/*---------------------------------------------------------------------------*/
void main_hsm_stop(
   void
   );

/*---------------------------------------------------------------------------*/
/*! \brief Send event to main hsm. */
/*---------------------------------------------------------------------------*/
void main_hsm_evt(
   int evt
   );

#endif /* #ifndef MAIN_HSM_H */
/* END OF FILE ***************************************************************/
