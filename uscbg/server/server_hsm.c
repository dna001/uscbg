/******************************************************************************
Copyright (c) 2013, All Rights Reserved.
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file server_hsm.c
\brief The Urban Sprawl server state machine implementation. */
/*---------------------------------------------------------------------------*/
/* INCLUDE FILES *************************************************************/
#include "sys_def.h"
#include "sys_assert.h"
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "slnk.h"
#include "trc.h"
#include "hsm.h"
#include "server_hsm.h"
#include "net_us.h"
#include "net_server.h"
#include "core.h"

/* CONSTANTS / MACROS ********************************************************/

/* LOCAL DATATYPES ***********************************************************/
typedef struct
{
   hsm_t super;
   hsm_state_t top;                       /*!< Top (all common messages) */
      hsm_state_t lobby;                  /*!< Lobby (waiting for new connections) */
      hsm_state_t select_color;           /*!< Select color */
      hsm_state_t game;                   /*!< Game */
         hsm_state_t setup;               /*!< Setup */
         hsm_state_t investments;         /*!< Investments */
         hsm_state_t select_action;       /*!< Select action */
         hsm_state_t action_take_card;    /*!< Take card action */
         hsm_state_t action_build;        /*!< Build action */
         hsm_state_t end_of_turn;         /*!< End of turn */
         hsm_state_t card;                /*!< Card (event) */
   bool_t started;
} srv_hsm_t;            /*!< Server state machine states */

/* LOCAL FUNCTION PROTOTYPES *************************************************/
STATIC void srv_hsm_ctor(srv_hsm_t* p_me);
STATIC hsm_evt_hnd_t srv_entry_hnd;
STATIC hsm_msg_t const* srv_top_hnd(srv_hsm_t* p_hsm, hsm_msg_t const* p_msg);
STATIC hsm_msg_t const* srv_lobby_hnd(srv_hsm_t* p_hsm, hsm_msg_t const* p_msg);
STATIC hsm_msg_t const* srv_select_color_hnd(srv_hsm_t* p_hsm, hsm_msg_t const* p_msg);
STATIC hsm_msg_t const* srv_game_hnd(srv_hsm_t* p_hsm, hsm_msg_t const* p_msg);
STATIC hsm_msg_t const* srv_setup_hnd(srv_hsm_t* p_hsm, hsm_msg_t const* p_msg);
STATIC hsm_msg_t const* srv_investments_hnd(srv_hsm_t* p_hsm, hsm_msg_t const* p_msg);
STATIC hsm_msg_t const* srv_select_action_hnd(srv_hsm_t* p_hsm, hsm_msg_t const* p_msg);
STATIC hsm_msg_t const* srv_action_take_card_hnd(srv_hsm_t* p_hsm, hsm_msg_t const* p_msg);
STATIC hsm_msg_t const* srv_action_build_hnd(srv_hsm_t* p_hsm, hsm_msg_t const* p_msg);
STATIC hsm_msg_t const* srv_end_of_turn_hnd(srv_hsm_t* p_hsm, hsm_msg_t const* p_msg);
STATIC hsm_msg_t const* srv_card_hnd(srv_hsm_t* p_hsm, hsm_msg_t const* p_msg);

STATIC void server_hsm_action_next_state(srv_hsm_t* p_hsm);

/* MODULE CONSTANTS / VARIABLES **********************************************/
SYS_ASSERT_FILE;
SYS_DBC_FILE;  /*!< Defines the name of this source file once for all */

TRC_DEF(srv_hsm);

static srv_hsm_t srv_hsm;

/* GLOBAL CONSTANTS / VARIABLES **********************************************/

/* GLOBAL FUNCTIONS **********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void srv_hsm_init(void)
{
   TRC_REG(srv_hsm, TRC_DEBUG | TRC_ERROR);
   srv_hsm.started = FALSE;
   srv_hsm_ctor(&srv_hsm);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void srv_hsm_start(void)
{
   if (!srv_hsm.started)
   {
      HSM_START(&srv_hsm);
      srv_hsm.started = TRUE;
   }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void srv_hsm_stop(void)
{
   hsm_msg_t msg;

   msg.evt = HSM_EVT_STOP;
   HSM_EVT(&srv_hsm, &msg);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void srv_hsm_evt(int evt)
{
   hsm_msg_t msg;

   msg.evt = evt;
   HSM_EVT(&srv_hsm, &msg);
}

/* LOCAL FUNCTIONS ***********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
STATIC void srv_hsm_ctor(srv_hsm_t* p_me)
{
   HSM_CTOR(p_me, "srv", srv_entry_hnd);
   HSM_STATE_CTOR(&p_me->top, "top", srv_top_hnd, &p_me->super.top);
   HSM_STATE_CTOR(&p_me->lobby, "lobby", srv_lobby_hnd, &p_me->top);
   HSM_STATE_CTOR(&p_me->select_color, "select_color",
      srv_select_color_hnd, &p_me->top);
   HSM_STATE_CTOR(&p_me->game, "game", srv_game_hnd, &p_me->top);
   HSM_STATE_CTOR(&p_me->setup, "setup",
      srv_setup_hnd, &p_me->game);
   HSM_STATE_CTOR(&p_me->investments, "investments",
      srv_investments_hnd, &p_me->game);
   HSM_STATE_CTOR(&p_me->select_action, "select_action",
      srv_select_action_hnd, &p_me->game);
   HSM_STATE_CTOR(&p_me->action_take_card, "action_take_card",
      srv_action_take_card_hnd, &p_me->game);
   HSM_STATE_CTOR(&p_me->action_build, "action_build",
      srv_action_build_hnd, &p_me->game);
   HSM_STATE_CTOR(&p_me->end_of_turn, "end_of_turn",
      srv_end_of_turn_hnd, &p_me->game);
   HSM_STATE_CTOR(&p_me->card, "card", srv_card_hnd, &p_me->game);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
STATIC hsm_msg_t const* srv_entry_hnd(hsm_t* p_hsm, hsm_msg_t const* p_msg)
{
   srv_hsm_t* p_h = REINTERPRET_CAST(srv_hsm_t*, p_hsm);

   switch(HSM_EVT_GET(p_msg))
   {
   case HSM_EVT_INIT:
   {
      HSM_STATE_INIT(p_hsm, &p_h->lobby);
      p_msg = HSM_MSG_PROCESSED;
      break;
   }
   default:
      break;
   }
   return p_msg;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
STATIC hsm_msg_t const* srv_top_hnd(srv_hsm_t* p_hsm, hsm_msg_t const* p_msg)
{
   switch(HSM_EVT_GET(p_msg))
   {
   case HSM_EVT_ENTRY:
      p_msg = HSM_MSG_PROCESSED;
      break;
   case HSM_EVT_EXIT:
      p_msg = HSM_MSG_PROCESSED;
      break;
   default:
      break;
   }
   return p_msg;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
STATIC hsm_msg_t const* srv_lobby_hnd(srv_hsm_t* p_hsm, hsm_msg_t const* p_msg)
{
   switch(HSM_EVT_GET(p_msg))
   {
   case HSM_EVT_ENTRY:
      p_msg = HSM_MSG_PROCESSED;
      break;
   case HSM_EVT_NET_START_GAME:
   {
      player_t* p_player;
      /* Send start game command to all clients */
      net_server_broadcast_cmd(NET_CMD_SERVER_START_GAME, NULL);
      /* Go to color selection */
      p_player = SLNK_NEXT(player_t, &core_get()->players_head);
      core_get()->active_player = p_player;
      HSM_STATE_TRAN(p_hsm, &p_hsm->select_color);
      p_msg = HSM_MSG_PROCESSED;
      break;
   }
   case HSM_EVT_NET_LOAD_GAME:
   {
      /* Load game */
      if (core_newgame(TRUE))
      {
         /* Send start game command to all clients */
         net_server_broadcast_cmd(NET_CMD_SERVER_START_GAME, NULL);
         net_server_broadcast_cmd(NET_CMD_SERVER_PHASE_UPDATE, NULL);
         //server_hsm_action_next_state(p_hsm);
      }
      else
      {
         core_log(NULL, "Load game failed");
      }
      p_msg = HSM_MSG_PROCESSED;
      break;
   }
   case HSM_EVT_EXIT:
      p_msg = HSM_MSG_PROCESSED;
      break;
   default:
      break;
   }
   return p_msg;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
STATIC hsm_msg_t const* srv_select_color_hnd(srv_hsm_t* p_hsm,
   hsm_msg_t const* p_msg)
{
   switch(HSM_EVT_GET(p_msg))
   {
   case HSM_EVT_ENTRY:
      net_server_broadcast_cmd(NET_CMD_SERVER_ACTIVE_PLAYER, NULL);
      net_server_send_cmd(core_get()->active_player->id,
         NET_CMD_SERVER_SELECT_COLOR, NULL);
      p_msg = HSM_MSG_PROCESSED;
      break;
   case HSM_EVT_NET_SELECT_COLOR:
   {
      player_t* p_player;
      core_select_color();
      p_player = SLNK_NEXT(player_t, core_get()->active_player);
      if (p_player == NULL)
      { /* All players have made a selection */
         /* Start new game */
         core_newgame(FALSE);
         /* Update phase */
         core_get()->state = CORE_STATE_SETUP;
         net_server_broadcast_cmd(NET_CMD_SERVER_PHASE_UPDATE, NULL);
         HSM_STATE_TRAN(p_hsm, &p_hsm->setup);
      }
      else
      {
         core_get()->active_player = p_player;
         HSM_STATE_TRAN(p_hsm, &p_hsm->select_color);
      }
      p_msg = HSM_MSG_PROCESSED;
      break;
   }
   case HSM_EVT_EXIT:
      p_msg = HSM_MSG_PROCESSED;
      break;
   default:
      break;
   }
   return p_msg;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
STATIC hsm_msg_t const* srv_game_hnd(srv_hsm_t* p_hsm, hsm_msg_t const* p_msg)
{
   switch(HSM_EVT_GET(p_msg))
   {
   case HSM_EVT_ENTRY:
      p_msg = HSM_MSG_PROCESSED;
      break;
   case HSM_EVT_NET_SELECT_ACTION:
   case HSM_EVT_NET_SELECT_BOARD_LOT:
   case HSM_EVT_NET_SELECT_COLOR:
   case HSM_EVT_NET_SELECT_BOARD_CARD:
   case HSM_EVT_NET_SELECT_PLAYER_CARD:
   case HSM_EVT_NET_PASS:
   case HSM_EVT_NET_DONE:
   {
      //core_action_done();
      //server_hsm_action_next_state(p_hsm);
      p_msg = HSM_MSG_PROCESSED;
   }
   case HSM_EVT_EXIT:
      p_msg = HSM_MSG_PROCESSED;
      break;
   default:
      break;
   }
   return p_msg;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
STATIC hsm_msg_t const* srv_setup_hnd(srv_hsm_t* p_hsm,
   hsm_msg_t const* p_msg)
{
   core_t* p_core = core_get();

   switch(HSM_EVT_GET(p_msg))
   {
   case HSM_EVT_ENTRY:
      net_server_broadcast_cmd(NET_CMD_SERVER_ACTIVE_PLAYER, NULL);
      net_server_send_cmd(p_core->active_player->id,
         NET_CMD_SERVER_SELECT_BOARD_LOT, NULL);
      p_msg = HSM_MSG_PROCESSED;
      break;
   case HSM_EVT_NET_SELECT_BOARD_LOT:
      core_action_build();
      p_core->startup_buildings--;
      p_core->active_player = core_get_next_player();
      if (p_core->startup_buildings > 0)
      {
         HSM_STATE_TRAN(p_hsm, &p_hsm->setup);
      }
      else
      {
         HSM_STATE_TRAN(p_hsm, &p_hsm->investments);
      }
      p_msg = HSM_MSG_PROCESSED;
      break;
   case HSM_EVT_EXIT:
      p_msg = HSM_MSG_PROCESSED;
      break;
   default:
      break;
   }
   return p_msg;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
STATIC hsm_msg_t const* srv_investments_hnd(srv_hsm_t* p_hsm,
   hsm_msg_t const* p_msg)
{
   core_t* p_core = core_get();

   switch(HSM_EVT_GET(p_msg))
   {
   case HSM_EVT_ENTRY:
      net_server_broadcast_cmd(NET_CMD_SERVER_ACTIVE_PLAYER, NULL);
      net_server_send_cmd(p_core->active_player->id,
         NET_CMD_SERVER_SELECT_PLAYER_CARD, NULL);
      p_msg = HSM_MSG_PROCESSED;
      break;
   case HSM_EVT_NET_SELECT_PLAYER_CARD:
   {
      card_t* p_card = cards_find(&p_core->active_player->cards_head,
         p_core->card_selection);
      REQUIRE(p_card != NULL);
      core_log(p_core->active_player, "selected card %d", p_card->id);
      core_invest();
      HSM_STATE_TRAN(p_hsm, &p_hsm->investments);
      p_msg = HSM_MSG_PROCESSED;
      break;
   }
   case HSM_EVT_NET_DONE:
      HSM_STATE_TRAN(p_hsm, &p_hsm->select_action);
      p_msg = HSM_MSG_PROCESSED;
      break;
   case HSM_EVT_EXIT:
      p_msg = HSM_MSG_PROCESSED;
      break;
   default:
      break;
   }
   return p_msg;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
STATIC hsm_msg_t const* srv_select_action_hnd(srv_hsm_t* p_hsm,
   hsm_msg_t const* p_msg)
{
   core_t* p_core = core_get();

   switch(HSM_EVT_GET(p_msg))
   {
   case HSM_EVT_ENTRY:
      net_server_broadcast_cmd(NET_CMD_SERVER_ACTIVE_PLAYER, NULL);
      net_server_send_cmd(p_core->active_player->id,
         NET_CMD_SERVER_SELECT_ACTION, NULL);
      p_msg = HSM_MSG_PROCESSED;
      break;
   case HSM_EVT_NET_SELECT_ACTION:
   {
      if (p_core->action_selection == 0)
      {
         HSM_STATE_TRAN(p_hsm, &p_hsm->action_take_card);
      }
      else if (p_core->action_selection == 1)
      {
         HSM_STATE_TRAN(p_hsm, &p_hsm->action_build);
      }
      p_msg = HSM_MSG_PROCESSED;
      break;
   }
   case HSM_EVT_NET_DONE:
      HSM_STATE_TRAN(p_hsm, &p_hsm->end_of_turn);
      p_msg = HSM_MSG_PROCESSED;
      break;
   case HSM_EVT_EXIT:
      p_msg = HSM_MSG_PROCESSED;
      break;
   default:
      break;
   }
   return p_msg;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
STATIC hsm_msg_t const* srv_action_take_card_hnd(srv_hsm_t* p_hsm,
   hsm_msg_t const* p_msg)
{
   core_t* p_core = core_get();

   switch(HSM_EVT_GET(p_msg))
   {
   case HSM_EVT_ENTRY:
      net_server_broadcast_cmd(NET_CMD_SERVER_ACTIVE_PLAYER, NULL);
      net_server_send_cmd(p_core->active_player->id,
         NET_CMD_SERVER_SELECT_BOARD_CARD, NULL);
      p_msg = HSM_MSG_PROCESSED;
      break;
   case HSM_EVT_NET_SELECT_BOARD_CARD:
      core_action_take_card();
      break;
   case HSM_EVT_EXIT:
      p_msg = HSM_MSG_PROCESSED;
      break;
   default:
      break;
   }
   return p_msg;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
STATIC hsm_msg_t const* srv_action_build_hnd(srv_hsm_t* p_hsm,
   hsm_msg_t const* p_msg)
{
   core_t* p_core = core_get();

   switch(HSM_EVT_GET(p_msg))
   {
   case HSM_EVT_ENTRY:
      net_server_broadcast_cmd(NET_CMD_SERVER_ACTIVE_PLAYER, NULL);
      net_server_send_cmd(p_core->active_player->id,
         NET_CMD_SERVER_SELECT_BOARD_CARD, NULL);
      p_msg = HSM_MSG_PROCESSED;
      break;
   case HSM_EVT_NET_SELECT_BOARD_CARD:
      core_action_build();
      break;
   case HSM_EVT_EXIT:
      p_msg = HSM_MSG_PROCESSED;
      break;
   default:
      break;
   }
   return p_msg;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
STATIC hsm_msg_t const* srv_end_of_turn_hnd(srv_hsm_t* p_hsm,
   hsm_msg_t const* p_msg)
{
   core_t* p_core = core_get();

   switch(HSM_EVT_GET(p_msg))
   {
   case HSM_EVT_ENTRY:
      net_server_broadcast_cmd(NET_CMD_SERVER_ACTIVE_PLAYER, NULL);
      net_server_send_cmd(p_core->active_player->id,
         NET_CMD_SERVER_SELECT_BOARD_LOT, NULL);
      p_msg = HSM_MSG_PROCESSED;
      break;
   case HSM_EVT_NET_SELECT_BOARD_LOT:
   {
      //p_core->extinction_save_tile = p_core->board_lot_selection;
      break;
   }
   case HSM_EVT_NET_DONE:
      break;
   case HSM_EVT_EXIT:
      p_msg = HSM_MSG_PROCESSED;
      break;
   default:
      break;
   }
   return p_msg;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
STATIC hsm_msg_t const* srv_card_hnd(srv_hsm_t* p_hsm, hsm_msg_t const* p_msg)
{
#if 0
   core_t* p_core = core_get();
   switch(HSM_EVT_GET(p_msg))
   {
   case HSM_EVT_ENTRY:
      net_server_broadcast_cmd(NET_CMD_SERVER_ACTIVE_PLAYER, NULL);
      net_server_send_cmd(p_core->active_player->id,
         NET_CMD_SERVER_SELECT_CARD, NULL);
      p_msg = HSM_MSG_PROCESSED;
      break;
   case HSM_EVT_NET_SELECT_CARD:
   {
      p_core->current_card = p_core->board_cards[p_core->card_selection];
      core_log(p_core->active_player, "selected card %s",
         cards_get_name(p_core->current_card));
      if (cards_use(p_core->current_card, CARD_EVT_START) != CARD_ACTION_DONE)
      {
         p_msg = HSM_MSG_PROCESSED;
      }
      break;
   }
   case HSM_EVT_NET_SELECT_ANIMAL:
      if (cards_use(p_core->current_card, CARD_EVT_ANIMAL) != CARD_ACTION_DONE)
      {
         p_msg = HSM_MSG_PROCESSED;
      }
      break;
   case HSM_EVT_NET_SELECT_ELEMENT_BOX:
      if (cards_use(p_core->current_card, CARD_EVT_ELEMENT_BOX) !=
          CARD_ACTION_DONE)
      {
         p_msg = HSM_MSG_PROCESSED;
      }
      break;
   case HSM_EVT_NET_SELECT_BOARD_TILE:
      if (cards_use(p_core->current_card, CARD_EVT_BOARD_TILE) !=
          CARD_ACTION_DONE)
      {
         p_msg = HSM_MSG_PROCESSED;
      }
      break;
   case HSM_EVT_NET_SELECT_BOARD_ELEMENT:
      if (cards_use(p_core->current_card, CARD_EVT_BOARD_ELEMENT) !=
          CARD_ACTION_DONE)
      {
         p_msg = HSM_MSG_PROCESSED;
      }
      break;
   case HSM_EVT_NET_SELECT_ELEMENT:
      if (cards_use(p_core->current_card, CARD_EVT_ELEMENT) !=
          CARD_ACTION_DONE)
      {
         p_msg = HSM_MSG_PROCESSED;
      }
      break;
   case HSM_EVT_NET_SELECT_CARD_CHOICE:
      if (cards_use(p_core->current_card, CARD_EVT_CARD_CHOICE) !=
          CARD_ACTION_DONE)
      {
         p_msg = HSM_MSG_PROCESSED;
      }
      break;
   case HSM_EVT_NET_PLACE_ACTION_PAWN:
      if (cards_use(p_core->current_card, CARD_EVT_PLACE_ACTION_PAWN) !=
         CARD_ACTION_DONE)
      {
         p_msg = HSM_MSG_PROCESSED;
      }
      break;
   case HSM_EVT_NET_DONE:
      if (cards_use(p_core->current_card, CARD_EVT_DONE) != CARD_ACTION_DONE)
      {
         p_msg = HSM_MSG_PROCESSED;
      }
      break;
   case HSM_EVT_EXIT:
      p_msg = HSM_MSG_PROCESSED;
      break;
   default:
      break;
   }
#endif
   return p_msg;
}

#if 0
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
STATIC void server_hsm_action_next_state(srv_hsm_t* p_hsm)
{
   core_state_t next_state;
   next_state = core_next_action();
   switch (next_state)
   {
   case CORE_STATE_ACTION_INITIATIVE:
      HSM_STATE_TRAN(p_hsm, &p_hsm->action_initiative);
      break;
   case CORE_STATE_ACTION_ADAPTATION:
      HSM_STATE_TRAN(p_hsm, &p_hsm->action_adaptation);
      break;
   case CORE_STATE_ACTION_REGRESSION:
      HSM_STATE_TRAN(p_hsm, &p_hsm->action_regression);
      break;
   case CORE_STATE_ACTION_ABUNDANCE:
      HSM_STATE_TRAN(p_hsm, &p_hsm->action_abundance);
      break;
   case CORE_STATE_ACTION_WASTELAND:
      HSM_STATE_TRAN(p_hsm, &p_hsm->action_wasteland);
      break;
   case CORE_STATE_ACTION_DEPLETION:
      HSM_STATE_TRAN(p_hsm, &p_hsm->action_depletion);
      break;
   case CORE_STATE_ACTION_GLACIATION:
      HSM_STATE_TRAN(p_hsm, &p_hsm->action_glaciation);
      break;
   case CORE_STATE_ACTION_SPECIATION:
      HSM_STATE_TRAN(p_hsm, &p_hsm->action_speciation);
      break;
   case CORE_STATE_ACTION_SPECIATION_INSECT:
      HSM_STATE_TRAN(p_hsm, &p_hsm->action_speciation_insect);
      break;
   case CORE_STATE_ACTION_WANDERLUST:
      HSM_STATE_TRAN(p_hsm, &p_hsm->action_wanderlust);
      break;
   case CORE_STATE_ACTION_MIGRATION:
      HSM_STATE_TRAN(p_hsm, &p_hsm->action_migration);
      break;
   case CORE_STATE_ACTION_COMPETITION:
      HSM_STATE_TRAN(p_hsm, &p_hsm->action_competition);
      break;
   case CORE_STATE_ACTION_DOMINATION:
      HSM_STATE_TRAN(p_hsm, &p_hsm->action_domination);
      break;
   case CORE_STATE_EXTINCTION:
      HSM_STATE_TRAN(p_hsm, &p_hsm->extinction);
      break;
   case CORE_STATE_RESET:
      core_extinction_check();
      core_survival_check();
      core_prepare_new_round();
      HSM_STATE_TRAN(p_hsm, &p_hsm->planning);
      break;
   case CORE_STATE_GAME_END:
      core_extinction_check();
      core_survival_check();
      core_calculate_final_vp();
      break;
   default:
      break;
   }
}
#endif

/* END OF FILE ***************************************************************/
