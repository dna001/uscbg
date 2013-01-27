/******************************************************************************
Copyright (c) 2013, All Rights Reserved.
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file main_hsm.c
\brief The Urban Sprawl Hierarchical State Machine implementation. */
/*---------------------------------------------------------------------------*/
/* INCLUDE FILES *************************************************************/
#include "sys_def.h"
#include "sys_assert.h"
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "slnk.h"
#include "dlnk.h"
#include "trc.h"
#include "hsm.h"
#include "core.h"
#include "net.h"
#include "net_us.h"
#include "net_client.h"
#include "gui.h"
#include "gui_gbwnd.h"
#include "gui_piwnd.h"
#include "gui_logwnd.h"
#include "cfg.h"
#include "main_hsm.h"

/* CONSTANTS / MACROS ********************************************************/

/* LOCAL DATATYPES ***********************************************************/
typedef struct
{
   hsm_t super;
   hsm_state_t mainmenu;                  /*!< Main Menu */
   hsm_state_t newgame;                   /*!< New game menu */
   hsm_state_t lobby;                     /*!< Lobby */
   hsm_state_t game;                      /*!< Game */
      hsm_state_t select_color;           /*!< Select color */
      hsm_state_t select_action;          /*!< Select action */
      hsm_state_t select_board_card;      /*!< Select board card */
      hsm_state_t select_player_card;     /*!< Select player card */
      hsm_state_t select_board_lot;       /*!< Select board lot */
      hsm_state_t waitnet;                /*!< Wait for net event */
   hsm_state_t loadgame;                  /*!< Load Game */
   hsm_state_t savegame;                  /*!< Save Game */
   bool_t started;
   gui_wnd_t* p_gmenu;
   gui_wnd_t* p_gb;
   gui_wnd_t* p_gap;
   gui_wnd_t* p_gstat;
   gui_wnd_t* p_gpi[4];
   gui_wnd_t* p_glog;
   gui_wnd_t* p_gcmd;
} main_hsm_t;            /*!< Main state machine states */

/* LOCAL FUNCTION PROTOTYPES *************************************************/
STATIC void main_hsm_ctor(main_hsm_t* p_me);
STATIC hsm_evt_hnd_t main_entry_hnd;
STATIC hsm_msg_t const* main_mainmenu_hnd(main_hsm_t* p_hsm, hsm_msg_t const* p_msg);
STATIC hsm_msg_t const* main_newgame_hnd(main_hsm_t* p_hsm, hsm_msg_t const* p_msg);
STATIC hsm_msg_t const* main_lobby_hnd(main_hsm_t* p_hsm, hsm_msg_t const* p_msg);
STATIC hsm_msg_t const* main_game_hnd(main_hsm_t* p_hsm, hsm_msg_t const* p_msg);
STATIC hsm_msg_t const* main_waitnet_hnd(main_hsm_t* p_hsm, hsm_msg_t const* p_msg);
STATIC hsm_msg_t const* main_select_color_hnd(main_hsm_t* p_hsm, hsm_msg_t const* p_msg);
STATIC hsm_msg_t const* main_select_action_hnd(main_hsm_t* p_hsm, hsm_msg_t const* p_msg);
STATIC hsm_msg_t const* main_select_board_card_hnd(main_hsm_t* p_hsm, hsm_msg_t const* p_msg);
STATIC hsm_msg_t const* main_select_player_card_hnd(main_hsm_t* p_hsm, hsm_msg_t const* p_msg);
STATIC hsm_msg_t const* main_select_board_lot_hnd(main_hsm_t* p_hsm, hsm_msg_t const* p_msg);
//STATIC hsm_msg_t const* main_loadgame_hnd(main_hsm_t* p_hsm, hsm_msg_t const* p_msg);

STATIC void mainmenu_btn_add(const char* name, int evt, int pos);
STATIC void mainmenu_label_add(const char* name, char* text, int pos, int w);
STATIC void mainmenu_text_add(const char* name, char* text, int pos, int offs);
STATIC void mainmenu_reset(void);
STATIC void info_update(char* text);

static void main_hsm_gui_event(gui_widget_t* p_wgt, char* event);

/* MODULE CONSTANTS / VARIABLES **********************************************/
SYS_ASSERT_FILE;
SYS_DBC_FILE;  /*!< Defines the name of this source file once for all */

TRC_DEF(main_hsm);

static const char* player_str[4] = {
   "Player 1", "Player 2", "Player 3", "Player 4"
};

static const char* color_str[4] = {
   "Black", "Green", "Pink", "White"
};

static char* state_str[CORE_STATE_LAST] ={
   "None",
   "Setup",
   "Investments",
   "Actions",
   "Spend AP",
   "Planning Card",
   "Build",
   "Favor",
   "End of Turn",
   "End of Turn Event",
   "Reset",
   "Game end"
};

static main_hsm_t main_hsm;

/* GLOBAL CONSTANTS / VARIABLES **********************************************/

/* GLOBAL FUNCTIONS **********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void main_hsm_init(void)
{
   TRC_REG(main_hsm, TRC_ERROR | TRC_DEBUG);
   main_hsm.started = FALSE;
   main_hsm_ctor(&main_hsm);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void main_hsm_start(void)
{
   if (!main_hsm.started)
   {
      HSM_START(&main_hsm);
      main_hsm.started = TRUE;
      //gui_dlg_attach_evt_cb(main_hsm.p_dlg, main_hsm_gui_event);
   }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void main_hsm_stop(void)
{
   hsm_msg_t msg;

   msg.evt = HSM_EVT_STOP;
   HSM_EVT(&main_hsm, &msg);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void main_hsm_evt(int evt)
{
   hsm_msg_t msg;

   msg.evt = evt;
   HSM_EVT(&main_hsm, &msg);
}

/* LOCAL FUNCTIONS ***********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
STATIC void main_hsm_ctor(main_hsm_t* p_me)
{
   HSM_CTOR(p_me, "main", main_entry_hnd);
   HSM_STATE_CTOR(&p_me->mainmenu, "mainmenu", main_mainmenu_hnd,
      &p_me->super.top);
   HSM_STATE_CTOR(&p_me->newgame, "newgame", main_newgame_hnd,
      &p_me->super.top);
   HSM_STATE_CTOR(&p_me->lobby, "lobby", main_lobby_hnd, &p_me->game);
   HSM_STATE_CTOR(&p_me->game, "game", main_game_hnd, &p_me->super.top);
   HSM_STATE_CTOR(&p_me->waitnet, "waitnet", main_waitnet_hnd, &p_me->game);
   HSM_STATE_CTOR(&p_me->select_color, "select_color",
      main_select_color_hnd, &p_me->game);
   HSM_STATE_CTOR(&p_me->select_action, "select_action",
      main_select_action_hnd, &p_me->game);
   HSM_STATE_CTOR(&p_me->select_board_card, "select_board_card",
      main_select_board_card_hnd, &p_me->game);
   HSM_STATE_CTOR(&p_me->select_player_card, "select_player_card",
      main_select_player_card_hnd, &p_me->game);
   HSM_STATE_CTOR(&p_me->select_board_lot, "select_board_lot",
      main_select_board_lot_hnd, &p_me->game);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
STATIC hsm_msg_t const* main_entry_hnd(hsm_t* p_hsm, hsm_msg_t const* p_msg)
{
   main_hsm_t* p_h = REINTERPRET_CAST(main_hsm_t*, p_hsm);

   switch(HSM_EVT_GET(p_msg))
   {
   case HSM_EVT_INIT:
   {
      gui_widget_t* p_wgt;
      int i;
      /* Create gui components */
      /* Game board */
      p_h->p_gb = gui_gbwnd_create();
      REQUIRE(p_h->p_gb != NULL);
      p_wgt = p_h->p_gb->find_widget(p_h->p_gb, "board");
      p_wgt->evt_cb = main_hsm_gui_event;
      gui_wnd_add(p_h->p_gb);
      /* Player info windows */
      /*for (i=0;i<6;i++)
      {
         p_h->p_gpi[i] = gui_piwnd_create(glx_get()->w - 300*((5-i)/3+1),
            glx_get()->h - 40*(3 - i%3), 300, 40, main_hsm_gui_event);
         //p_h->p_gpi[i]->visible = FALSE;
         gui_wnd_add(p_h->p_gpi[i]);
      }
      p_h->p_gpi[0]->visible = TRUE;*/
      /* Menu window */
      p_h->p_gmenu = gui_wnd_create(NULL, "Menu", (glx_get()->w-200)/2, 150,
         200, 20, 0);
      REQUIRE(p_h->p_gmenu != NULL);
      p_h->p_gmenu->set_cfg(p_h->p_gmenu, "font",
         glx_load_font("fonts/eufm10.ttf", 20));
      p_h->p_gmenu->set_cfg(p_h->p_gmenu, "caption", "Main Menu");
      p_h->p_gmenu->set_cfg(p_h->p_gmenu, "border", (void*)TRUE);
      gui_wnd_add(p_h->p_gmenu);
      /* Active player window */
      /*p_h->p_gap = gui_wnd_create(NULL, "Active_Player", 0, 0,
         40, 40, 0);
      REQUIRE(p_h->p_gap != NULL);
      p_h->p_gap->set_cfg(p_h->p_gap, "bg_image", animal_image_path[5]);
      gui_wnd_add(p_h->p_gap);*/
      /* Status window */
      p_h->p_gstat = gui_wnd_create(NULL, "Status", 40, 0,
         200, 40, 0);
      REQUIRE(p_h->p_gstat != NULL);
      gui_wnd_add(p_h->p_gstat);
      p_wgt = gui_widget_create("state", "text", 5, 1, 190, 20);
      p_wgt->set_cfg(p_wgt, "text", "Setting up");
      p_wgt->set_cfg(p_wgt, "border", (void*)FALSE);
      p_wgt->set_cfg(p_wgt, "edit", (void*)FALSE);
      p_wgt->set_cfg(p_wgt, "center", (void*)FALSE);
      p_h->p_gstat->add_widget(p_h->p_gstat, p_wgt);
      p_wgt = gui_widget_create("info", "text", 5, 21, 190, 20);
      //p_wgt->set_cfg(p_wgt, "text", "");
      p_wgt->set_cfg(p_wgt, "border", (void*)FALSE);
      p_wgt->set_cfg(p_wgt, "edit", (void*)FALSE);
      p_wgt->set_cfg(p_wgt, "center", (void*)FALSE);
      p_h->p_gstat->add_widget(p_h->p_gstat, p_wgt);
      /* Log window */
      p_h->p_glog = gui_logwnd_create(10, glx_get()->h - 150,
         glx_get()->w - 600, 140, main_hsm_gui_event);
      REQUIRE(p_h->p_glog != NULL);
      gui_wnd_add(p_h->p_glog);
      /* Command window */
      p_h->p_gcmd = gui_wnd_create(NULL, "CommandMenu", (glx_get()->w - 200),
         glx_get()->h - 20, 200, 20, 0);
      REQUIRE(p_h->p_gcmd != NULL);
      p_wgt = gui_widget_create("done", "button", 0, 0, 50, 20);
      p_wgt->set_cfg(p_wgt, "cb_fn", main_hsm_gui_event);
      p_wgt->set_cfg(p_wgt, "text", (void*)"Done");
      p_wgt->set_cfg(p_wgt, "data", (void*)HSM_EVT_MENU_BTN_DONE);
      p_h->p_gcmd->add_widget(p_h->p_gcmd, p_wgt);
      p_wgt = gui_widget_create("center", "button", 50, 0, 50, 20);
      p_wgt->set_cfg(p_wgt, "cb_fn", main_hsm_gui_event);
      p_wgt->set_cfg(p_wgt, "text", (void*)"Center");
      p_wgt->set_cfg(p_wgt, "data", (void*)HSM_EVT_MENU_BTN_CENTER);
      p_h->p_gcmd->add_widget(p_h->p_gcmd, p_wgt);
      p_wgt = gui_widget_create("coordinates", "button", 100, 0, 50, 20);
      p_wgt->set_cfg(p_wgt, "cb_fn", main_hsm_gui_event);
      p_wgt->set_cfg(p_wgt, "text", (void*)"Coords");
      p_wgt->set_cfg(p_wgt, "data", (void*)HSM_EVT_MENU_BTN_COORDS);
      p_h->p_gcmd->add_widget(p_h->p_gcmd, p_wgt);
      p_wgt = gui_widget_create("exit", "button", 150, 0, 50, 20);
      p_wgt->set_cfg(p_wgt, "cb_fn", main_hsm_gui_event);
      p_wgt->set_cfg(p_wgt, "text", (void*)"Exit");
      p_wgt->set_cfg(p_wgt, "data", (void*)HSM_EVT_MENU_BTN_EXIT);
      p_h->p_gcmd->add_widget(p_h->p_gcmd, p_wgt);
      gui_wnd_add(p_h->p_gcmd);
      HSM_STATE_INIT(p_hsm, &p_h->newgame);
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
STATIC hsm_msg_t const* main_mainmenu_hnd(main_hsm_t* p_hsm,
   hsm_msg_t const* p_msg)
{
   switch(HSM_EVT_GET(p_msg))
   {
   case HSM_EVT_ENTRY:
      mainmenu_btn_add("New Game", HSM_EVT_MENU_BTN_NEWGAME, 0);
      mainmenu_btn_add("Exit", HSM_EVT_MENU_BTN_EXIT, 1);
      p_msg = HSM_MSG_PROCESSED;
      break;
   case HSM_EVT_MENU_BTN_NEWGAME: /* New Game */
   {
      HSM_STATE_TRAN(p_hsm, &p_hsm->newgame);
      p_msg = HSM_MSG_PROCESSED;
      break;
   }
   case HSM_EVT_MENU_BTN_EXIT:
   {
      p_msg = HSM_MSG_PROCESSED;
      break;
   }
   case HSM_EVT_EXIT:
      mainmenu_reset();
      p_msg = HSM_MSG_PROCESSED;
      break;
   default:
      break;
   }
   return p_msg;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
STATIC hsm_msg_t const* main_newgame_hnd(main_hsm_t* p_hsm,
   hsm_msg_t const* p_msg)
{
   char txt[40];

   switch(HSM_EVT_GET(p_msg))
   {
   case HSM_EVT_ENTRY:
      mainmenu_btn_add("Connect", HSM_EVT_MENU_BTN_CONNECT, 0);
      mainmenu_label_add("NameLbl", "Name:", 1, 50);
      /* Read Name from config file */
      if (cfg_get_key("ds.ini", "player_name", txt, 40))
      {
         mainmenu_text_add("Name", txt, 1, 50);
         TRC_DBG(main_hsm, "Name %s read from config file", txt);
      }
      else
      {
         mainmenu_text_add("Name", "Player", 1, 50);
      }
      mainmenu_label_add("IPLbl", "IP:", 2, 50);
      /* Read Server IP from config file */
      if (cfg_get_key("ds.ini", "server_ip", txt, 40))
      {
         mainmenu_text_add("IP", txt, 2, 50);
         TRC_DBG(main_hsm, "Server IP %s read from config file", txt);
      }
      else
      {
         mainmenu_text_add("IP", "127.0.0.1", 2, 50);
      }
      mainmenu_btn_add("Back", HSM_EVT_MENU_BTN_BACK, 3);
      p_msg = HSM_MSG_PROCESSED;
      break;
   case HSM_EVT_MENU_BTN_CONNECT:
   {
      gui_wnd_t* p_wnd = main_hsm.p_gmenu;
      gui_widget_t* p_wgt;
      char* name;
      char* ip;
      ALLEGE((p_wgt = p_wnd->find_widget(p_wnd, "Name")) != NULL);
      name = (char*)p_wgt->get_cfg(p_wgt, "text");
      ALLEGE((p_wgt = p_wnd->find_widget(p_wnd, "IP")) != NULL);
      ip = (char*)p_wgt->get_cfg(p_wgt, "text");
      net_client_start(name, ip, 0);
      HSM_STATE_TRAN(p_hsm, &p_hsm->lobby);
      p_msg = HSM_MSG_PROCESSED;
      break;
   }
   case HSM_EVT_MENU_BTN_BACK:
   {
      HSM_STATE_TRAN(p_hsm, &p_hsm->mainmenu);
      p_msg = HSM_MSG_PROCESSED;
      break;
   }
   case HSM_EVT_EXIT:
      mainmenu_reset();
      p_msg = HSM_MSG_PROCESSED;
      break;
   default:
      break;
   }
   return p_msg;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
STATIC hsm_msg_t const* main_lobby_hnd(main_hsm_t* p_hsm,
   hsm_msg_t const* p_msg)
{
   main_hsm_t* p_h = REINTERPRET_CAST(main_hsm_t*, p_hsm);

   switch(HSM_EVT_GET(p_msg))
   {
   case HSM_EVT_ENTRY:
   {
      player_t* p_player = SLNK_NEXT(player_t, &core_get()->players_head);
      int n = 0;
      mainmenu_label_add("Lobby", "LOBBY", 0, 180);
      /* Players in lobby */
      while (p_player != NULL)
      {
         REQUIRE(n < 6);
         mainmenu_label_add(player_str[n], p_player->name, n+1, 180);
         p_player = SLNK_NEXT(player_t, p_player);
         n++;
      }
      if (n >= 2)
      {
         mainmenu_btn_add("Start", HSM_EVT_MENU_BTN_START, n+1);
         mainmenu_btn_add("Load", HSM_EVT_MENU_BTN_LOADGAME, n+2);
      }
      p_h->p_gmenu->visible = TRUE;
      p_msg = HSM_MSG_PROCESSED;
      break;
   }
   case HSM_EVT_NET_UPDATE_PLAYERS:
   {
      HSM_STATE_TRAN(p_hsm, &p_hsm->lobby);
      p_msg = HSM_MSG_PROCESSED;
      break;
   }
   case HSM_EVT_MENU_BTN_START:
   {
      net_client_send_cmd(NET_CMD_CLIENT_START_GAME, 0);
      p_msg = HSM_MSG_PROCESSED;
      break;
   }
   case HSM_EVT_MENU_BTN_LOADGAME:
   {
      net_client_send_cmd(NET_CMD_CLIENT_LOAD_GAME, 0);
      p_msg = HSM_MSG_PROCESSED;
      break;
   }
   case HSM_EVT_NET_START_GAME:
   {
      HSM_STATE_TRAN(p_hsm, &p_hsm->waitnet);
      p_msg = HSM_MSG_PROCESSED;
      break;
   }
   case HSM_EVT_NET_UPDATE_LOG:
   {
      gui_wnd_t* p_wnd = p_hsm->p_glog;
      gui_widget_t* p_wgt;
      core_log_entry_t* p_clog = &core_get()->log_entry;
      p_wgt = p_wnd->find_widget(p_wnd, "log");
      p_wgt->set_cfg(p_wgt, "add_log_entry", p_clog);
      p_msg = HSM_MSG_PROCESSED;
      break;
   }
   case HSM_EVT_EXIT:
      mainmenu_reset();
      p_h->p_gmenu->visible = FALSE;
      p_msg = HSM_MSG_PROCESSED;
      break;
   default:
      break;
   }
   return p_msg;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
STATIC hsm_msg_t const* main_game_hnd(main_hsm_t* p_hsm,
   hsm_msg_t const* p_msg)
{
   switch(HSM_EVT_GET(p_msg))
   {
   case HSM_EVT_ENTRY:
   {
      //gfw_tmr_evt(0);
      p_msg = HSM_MSG_PROCESSED;
      break;
   }
   case HSM_EVT_NET_UPDATE_ACTIVE_PLAYER:
   {
      player_t* p_player = core_get()->active_player;
      /*if (p_player->color < PLAYER_COLOR_LAST)
      {
         p_hsm->p_gap->set_cfg(p_hsm->p_gap, "bg_image",
            animal_image_path[p_player->color]);
      }*/
   }
   case HSM_EVT_NET_UPDATE_PLAYERS:
   {
      /*player_t* p_player = SLNK_NEXT(player_t, &core_get()->players_head);
      while(p_player != NULL)
      {
         int color = p_player->color;
         if (color < PLAYER_COLOR_LAST)
         {
            p_hsm->p_gpi[color]->set_cfg(p_hsm->p_gpi[color],
               "update", p_player);
         }
         p_player = SLNK_NEXT(player_t, p_player);
      }*/
      p_msg = HSM_MSG_PROCESSED;
      break;
   }
   case HSM_EVT_NET_UPDATE_BOARD_CARDS:
   {
      gui_wnd_t* p_wnd = p_hsm->p_gb;
      gui_widget_t* p_wgt = p_wnd->find_widget(p_wnd, "board");
      REQUIRE(p_wgt != NULL);
      p_wgt->set_cfg(p_wgt, "update", NULL);
      p_msg = HSM_MSG_PROCESSED;
      break;
   }
   case HSM_EVT_NET_UPDATE_PHASE:
   {
      core_t* p_core = core_get();
      gui_wnd_t* p_wnd = p_hsm->p_gstat;
      gui_widget_t* p_wgt;
      char status[129];
      p_wgt = p_wnd->find_widget(p_wnd, "state");
      snprintf(status, 128, "Round %d, %s", p_core->current_round,
         state_str[p_core->state]);
      p_wgt->set_cfg(p_wgt, "text", status);
      p_msg = HSM_MSG_PROCESSED;
      break;
   }
   case HSM_EVT_NET_UPDATE_LOG:
   {
      gui_wnd_t* p_wnd = p_hsm->p_glog;
      gui_widget_t* p_wgt;
      core_log_entry_t* p_clog = &core_get()->log_entry;
      p_wgt = p_wnd->find_widget(p_wnd, "log");
      p_wgt->set_cfg(p_wgt, "add_log_entry", p_clog);
      p_msg = HSM_MSG_PROCESSED;
      break;
   }
   case HSM_EVT_MENU_BTN_DONE:
   {
      net_client_send_cmd(NET_CMD_CLIENT_DONE, 0);
      HSM_STATE_TRAN(p_hsm, &p_hsm->waitnet);
      p_msg = HSM_MSG_PROCESSED;
      break;
   }
   case HSM_EVT_TIMER:
   {
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
STATIC hsm_msg_t const* main_waitnet_hnd(main_hsm_t* p_hsm,
   hsm_msg_t const* p_msg)
{
   switch(HSM_EVT_GET(p_msg))
   {
   case HSM_EVT_ENTRY:
      info_update("Waiting for other player");
      p_msg = HSM_MSG_PROCESSED;
      break;
   case HSM_EVT_NET_SELECT_COLOR:
      HSM_STATE_TRAN(p_hsm, &p_hsm->select_color);
      p_msg = HSM_MSG_PROCESSED;
      break;
   case HSM_EVT_NET_SELECT_BOARD_CARD:
      HSM_STATE_TRAN(p_hsm, &p_hsm->select_board_card);
      p_msg = HSM_MSG_PROCESSED;
      break;
   case HSM_EVT_NET_SELECT_PLAYER_CARD:
      HSM_STATE_TRAN(p_hsm, &p_hsm->select_player_card);
      p_msg = HSM_MSG_PROCESSED;
      break;
   case HSM_EVT_NET_SELECT_BOARD_LOT:
      HSM_STATE_TRAN(p_hsm, &p_hsm->select_board_lot);
      p_msg = HSM_MSG_PROCESSED;
      break;
   case HSM_EVT_NET_SELECT_CARD_CHOICE:
      //HSM_STATE_TRAN(p_hsm, &p_hsm->select_card_choice);
      p_msg = HSM_MSG_PROCESSED;
      break;
   case HSM_EVT_MENU_BTN_DONE:
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
STATIC hsm_msg_t const* main_select_color_hnd(main_hsm_t* p_hsm,
   hsm_msg_t const* p_msg)
{
   switch(HSM_EVT_GET(p_msg))
   {
   case HSM_EVT_ENTRY:
   {
      int n = 0;
      int i;
      info_update("Select color");
      for (i=0;i<PLAYER_COLOR_LAST;i++)
      {
         if (core_get()->available_colors & (1u << i))
         {
            mainmenu_btn_add(color_str[i], HSM_EVT_MENU_BTN_1 + i, n++);
         }
      }
      if (core_get()->state == CORE_STATE_NONE)
      {
         mainmenu_btn_add("Random", HSM_EVT_MENU_BTN_RANDOM, n++);
      }
      p_hsm->p_gmenu->visible = TRUE;
      p_msg = HSM_MSG_PROCESSED;
      break;
   }
   case HSM_EVT_MENU_BTN_1:
   case HSM_EVT_MENU_BTN_2:
   case HSM_EVT_MENU_BTN_3:
   case HSM_EVT_MENU_BTN_4:
   {
      net_client_send_cmd(NET_CMD_CLIENT_SELECT_COLOR,
         (HSM_EVT_GET(p_msg) - HSM_EVT_MENU_BTN_1));
      HSM_STATE_TRAN(p_hsm, &p_hsm->waitnet);
      p_msg = HSM_MSG_PROCESSED;
      break;
   }
   case HSM_EVT_MENU_BTN_RANDOM:
   {
      net_client_send_cmd(NET_CMD_CLIENT_SELECT_COLOR, PLAYER_COLOR_LAST);
      HSM_STATE_TRAN(p_hsm, &p_hsm->waitnet);
      p_msg = HSM_MSG_PROCESSED;
      break;
   }
   case HSM_EVT_EXIT:
      mainmenu_reset();
      p_hsm->p_gmenu->visible = FALSE;
      p_msg = HSM_MSG_PROCESSED;
      break;
   default:
      break;
   }
   return p_msg;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
STATIC hsm_msg_t const* main_select_action_hnd(main_hsm_t* p_hsm,
   hsm_msg_t const* p_msg)
{
   switch(HSM_EVT_GET(p_msg))
   {
   case HSM_EVT_ENTRY:
   {
      int n = 0;
      info_update("Select action");
      mainmenu_btn_add("Take card (planning/favor)", HSM_EVT_MENU_BTN_1, n++);
      mainmenu_btn_add("Build contract", HSM_EVT_MENU_BTN_2, n++);
      mainmenu_btn_add("Back", HSM_EVT_MENU_BTN_BACK, n++);
      p_hsm->p_gmenu->visible = TRUE;
      p_msg = HSM_MSG_PROCESSED;
      break;
   }
   case HSM_EVT_MENU_BTN_1:
   case HSM_EVT_MENU_BTN_2:
   {
      net_client_send_cmd(NET_CMD_CLIENT_SELECT_ACTION,
         (HSM_EVT_GET(p_msg) - HSM_EVT_MENU_BTN_1));
      HSM_STATE_TRAN(p_hsm, &p_hsm->waitnet);
      p_msg = HSM_MSG_PROCESSED;
      break;
   }
   case HSM_EVT_EXIT:
      mainmenu_reset();
      p_hsm->p_gmenu->visible = FALSE;
      p_msg = HSM_MSG_PROCESSED;
      break;
   default:
      break;
   }
   return p_msg;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
STATIC hsm_msg_t const* main_select_board_card_hnd(main_hsm_t* p_hsm,
   hsm_msg_t const* p_msg)
{
   switch(HSM_EVT_GET(p_msg))
   {
   case HSM_EVT_ENTRY:
   {
      info_update("Select board card");
      //core_board_cards_mark();
      p_msg = HSM_MSG_PROCESSED;
      break;
   }
   case HSM_EVT_CARD:
   {
      net_client_send_cmd(NET_CMD_CLIENT_SELECT_BOARD_CARD, 0);
      HSM_STATE_TRAN(p_hsm, &p_hsm->waitnet);
      p_msg = HSM_MSG_PROCESSED;
      break;
   }
   case HSM_EVT_MENU_BTN_DONE:
      p_msg = HSM_MSG_PROCESSED;
      break;
   case HSM_EVT_EXIT:
      //core_board_cards_clear();
      p_msg = HSM_MSG_PROCESSED;
      break;
   default:
      break;
   }
   return p_msg;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
STATIC hsm_msg_t const* main_select_player_card_hnd(main_hsm_t* p_hsm,
   hsm_msg_t const* p_msg)
{
   switch(HSM_EVT_GET(p_msg))
   {
   case HSM_EVT_ENTRY:
   {
      info_update("Select player card");
      //core_board_cards_mark();
      p_msg = HSM_MSG_PROCESSED;
      break;
   }
   case HSM_EVT_CARD:
   {
      net_client_send_cmd(NET_CMD_CLIENT_SELECT_PLAYER_CARD, 0);
      HSM_STATE_TRAN(p_hsm, &p_hsm->waitnet);
      p_msg = HSM_MSG_PROCESSED;
      break;
   }
   case HSM_EVT_MENU_BTN_DONE:
      p_msg = HSM_MSG_PROCESSED;
      break;
   case HSM_EVT_EXIT:
      //core_board_cards_clear();
      p_msg = HSM_MSG_PROCESSED;
      break;
   default:
      break;
   }
   return p_msg;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
STATIC hsm_msg_t const* main_select_board_lot_hnd(main_hsm_t* p_hsm,
   hsm_msg_t const* p_msg)
{
   switch(HSM_EVT_GET(p_msg))
   {
   case HSM_EVT_ENTRY:
   {
      info_update("Select board lot");
      core_board_lots_mark();
      p_msg = HSM_MSG_PROCESSED;
      break;
   }
   case HSM_EVT_BOARD_LOT:
   {
      net_client_send_cmd(NET_CMD_CLIENT_SELECT_BOARD_LOT, 0);
      HSM_STATE_TRAN(p_hsm, &p_hsm->waitnet);
      p_msg = HSM_MSG_PROCESSED;
      break;
   }
   case HSM_EVT_EXIT:
      core_board_lots_clear();
      p_msg = HSM_MSG_PROCESSED;
      break;
   default:
      break;
   }
   return p_msg;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
STATIC void mainmenu_btn_add(const char* name, int evt, int pos)
{
   gui_widget_t* p_wgt;
   gui_wnd_t* p_wnd = main_hsm.p_gmenu;
   p_wgt = gui_widget_create(name, "button", 10, 10+pos*40, 180, 35);
   p_wgt->set_cfg(p_wgt, "cb_fn", main_hsm_gui_event);
   p_wgt->set_cfg(p_wgt, "text", (void*)name);
   p_wgt->set_cfg(p_wgt, "data", (void*)evt);
   p_wnd->add_widget(p_wnd, p_wgt);
   p_wnd->h = p_wnd->caption_offset_y + 10 + (pos + 1)*40;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
STATIC void mainmenu_label_add(const char* name, char* text, int pos, int w)
{
   gui_widget_t* p_wgt;
   gui_wnd_t* p_wnd = main_hsm.p_gmenu;
   p_wgt = gui_widget_create(name, "text", 10, 10+pos*40, w, 35);
   p_wgt->set_cfg(p_wgt, "text", text);
   p_wgt->set_cfg(p_wgt, "border", (void*)FALSE);
   p_wgt->set_cfg(p_wgt, "edit", (void*)FALSE);
   p_wnd->add_widget(p_wnd, p_wgt);
   p_wnd->h = p_wnd->caption_offset_y + 10 + (pos + 1)*40;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
STATIC void mainmenu_text_add(const char* name, char* text, int pos, int offs)
{
   gui_widget_t* p_wgt;
   gui_wnd_t* p_wnd = main_hsm.p_gmenu;
   p_wgt = gui_widget_create(name, "text", 10+offs, 10+pos*40, 180-offs, 35);
   p_wgt->set_cfg(p_wgt, "text", text);
   p_wgt->set_cfg(p_wgt, "border", (void*)TRUE);
   p_wgt->set_cfg(p_wgt, "edit", (void*)TRUE);
   p_wnd->add_widget(p_wnd, p_wgt);
   p_wnd->h = p_wnd->caption_offset_y + 10 + (pos + 1)*40;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
STATIC void mainmenu_reset(void)
{
   gui_wnd_t* p_wnd = main_hsm.p_gmenu;
   gui_widget_t* p_wgt = DLNK_NEXT(gui_widget_t, &p_wnd->widget_lst);
   /* Remove all widgets and set initial height */
   while (&p_wgt->dlnk != &p_wnd->widget_lst)
   {
      p_wnd->rm_widget(p_wnd, p_wgt);
      p_wgt = DLNK_NEXT(gui_widget_t, &p_wnd->widget_lst);
   }
   p_wnd->h = 20;
}

STATIC void info_update(char* text)
{
   gui_wnd_t* p_wnd = main_hsm.p_gstat;
   gui_widget_t* p_wgt;
   char status[129];
   p_wgt = p_wnd->find_widget(p_wnd, "info");
   snprintf(status, 128, "%s", text);
   p_wgt->set_cfg(p_wgt, "text", status);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void main_hsm_gui_event(gui_widget_t* p_wgt, char* event)
{
   if (strcmp(p_wgt->type, "button") == 0) {
      if (strcmp(event, "clicked") == 0) {
         hsm_msg_t msg;
         msg.evt = (int)p_wgt->get_cfg(p_wgt, "data");
         HSM_EVT(&main_hsm, &msg);
      }
   } else if (strcmp(p_wgt->type, "board") == 0) {
      if (strcmp(event, "board_lot") == 0) {
         hsm_msg_t msg;
         msg.evt = HSM_EVT_BOARD_LOT;
         HSM_EVT(&main_hsm, &msg);
      } else if (strcmp(event, "card") == 0) {
         hsm_msg_t msg;
         msg.evt = HSM_EVT_CARD;
         HSM_EVT(&main_hsm, &msg);
      }
   /*} else if (p_wgt->p_owner == main_hsm.p_gpi[0]) {
      hsm_msg_t msg;
      msg.evt = 0;
      if (msg.evt != 0)
         HSM_EVT(&main_hsm, &msg);*/
   }
}

/* END OF FILE ***************************************************************/
