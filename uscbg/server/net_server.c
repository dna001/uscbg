/******************************************************************************
Copyright (c) 2013, All Rights Reserved.
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file net_server.c
\brief Dominant Species net server implementation. */
/*---------------------------------------------------------------------------*/
/* INCLUDE FILES *************************************************************/
#include "sys_def.h"
#include "sys_assert.h"
#include "slnk.h"
#include "trc.h"
#include "pbuf.h"
#include "net.h"
#include "net_us.h"
#include "net_server.h"
#include "core.h"
#include "server_hsm.h"
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

/* CONSTANTS / MACROS ********************************************************/

/* LOCAL DATATYPES ***********************************************************/

/* LOCAL FUNCTION PROTOTYPES *************************************************/
static net_evt_cb_fn_t net_server_evt_cb_fn;
static void net_server_parse_command(int sock, void* data, int len);

/* MODULE CONSTANTS / VARIABLES **********************************************/
SYS_ASSERT_FILE;
SYS_DBC_FILE;  /*!< Defines the name of this source file once for all */

TRC_DEF(net_server);

static net_cfg_t net_cfg = {
   .is_server = TRUE,
   .port = 5050,
   .max_connections = 4,
   .poll = FALSE,
   .evt_fn = net_server_evt_cb_fn
};

/* GLOBAL CONSTANTS / VARIABLES **********************************************/

/* GLOBAL FUNCTIONS **********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void net_server_init(void)
{
   TRC_REG(net_server, TRC_ERROR | TRC_DEBUG);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void net_server_start(void)
{
   net_start(&net_cfg);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void net_server_send_cmd(int sock, int cmd, void* data)
{
   uint8_t packet[MAX_PACKET_SZ];
   int len = 2;
   packet[0] = cmd >> 8;
   packet[1] = cmd & 0xff;
   TRC_DBG(net_server, "Sending command %s (%d) ",
      net_us_cmd_to_str(cmd), cmd);
   switch (cmd)
   {
      case NET_CMD_SERVER_PLAYER_INFO:
      { /* Only name (fixed values) */
         player_t* p_player = (player_t*)data;
         int id = (p_player->id == sock)?0:p_player->id;
         len += pbuf_pack(&packet[2], "wsdw", id, MAX_CLIENT_NAME_LEN,
            p_player->name, p_player->id);
         break;
      }
      case NET_CMD_SERVER_PLAYER_REMOVE:
      {
         int id = (int)data;
         len += pbuf_pack(&packet[2], "w", id);
         break;
      }
      case NET_CMD_SERVER_PLAYER_UPDATE:
      { /* Used for any other player updates during the game */
         player_t* p_player = (player_t*)data;
         card_t* p_card = SLNK_NEXT(card_t, &p_player->cards_head);
         int i;
         len += pbuf_pack(&packet[2], "w", p_player->id);
         len += pbuf_pack(&packet[len], "bbbwww", p_player->color,
            p_player->ap, p_player->politicians, p_player->vocations,
            p_player->wealth, p_player->prestige);
         /* Cards */
         for (i=0;i<6;i++)
         {
            uint8_t id = (p_card)?p_card->id:0;
            len += pbuf_pack(&packet[len], "b", id);
            if (p_card)
            {
               p_card = SLNK_NEXT(card_t, p_card);
            }
         }
         break;
      }
      case NET_CMD_SERVER_BOARD_CARDS_UPDATE:
      {
         int i;
         for (i=0;i<5;i++)
         {
            card_t* p_card = core_get()->board_planning_cards[i];
            uint8_t id = (p_card)?p_card->id:0;
            len += pbuf_pack(&packet[len], "b", id);
         }
         for (i=0;i<8;i++)
         {
            card_t* p_card = core_get()->board_contract_cards[i];
            uint8_t id = (p_card)?p_card->id:0;
            len += pbuf_pack(&packet[len], "b", id);
         }
         break;
      }
      case NET_CMD_SERVER_START_GAME:
         break;
      case NET_CMD_SERVER_SELECT_COLOR:
         len += pbuf_pack(&packet[2], "b", core_get()->available_colors);
         break;
      case NET_CMD_SERVER_BLOCK_UPDATE:
      {
         block_t* p_blk = (block_t*)data;
         int i;
         len += pbuf_pack(&packet[len], "bbw", p_blk->id,
            p_blk->n_buildings, p_blk->value);
         for (i=0;i<4;i++)
         {
            len += pbuf_pack(&packet[len], "bbbb",
               p_blk->buildings[i].zone, p_blk->buildings[i].size,
               p_blk->buildings[i].owner, p_blk->buildings[i].block_pos);
         }
         TRC_DBG(net_server, "Block: id %d, n_buildings %d, value %d",
            p_blk->id, p_blk->n_buildings, p_blk->value);
         break;
      }
      case NET_CMD_SERVER_ACTIVE_PLAYER:
      {
         int id = core_get()->active_player->id;
         len += pbuf_pack(&packet[2], "w", id);
         break;
      }
      case NET_CMD_SERVER_SELECT_ACTION:
         break;
      case NET_CMD_SERVER_SELECT_BOARD_LOT:
         break;
      case NET_CMD_SERVER_SELECT_BOARD_CARD:
         break;
      case NET_CMD_SERVER_SELECT_PLAYER_CARD:
         break;
      case NET_CMD_SERVER_SELECT_CARD_CHOICE:
         break;
      case NET_CMD_SERVER_PHASE_UPDATE:
      {
         core_t* p_core = core_get();
         len += pbuf_pack(&packet[2], "bb", p_core->current_round,
            p_core->state);
         break;
      }
      case NET_CMD_SERVER_LOG_ENTRY:
      {
         core_log_entry_t* p_clog = &core_get()->log_entry;
         int id = (p_clog->p_player)?p_clog->p_player->id:0;
         len += pbuf_pack(&packet[2], "wsd", id,
               strlen(p_clog->text) + 1, p_clog->text);
         break;
      }
      default:
         TRC_ERR(net_server, "Error: Unknown command %d", cmd);
         return;
   }
   net_write_packet(sock, packet, len);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void net_server_broadcast_cmd(int cmd, void* data)
{
   player_t* p_player = SLNK_NEXT(player_t, &core_get()->players_head);
   while(p_player != NULL)
   {
      net_server_send_cmd(p_player->id, cmd, data);
      p_player = SLNK_NEXT(player_t, p_player);
   }
}

/* LOCAL FUNCTIONS ***********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void net_server_evt_cb_fn(int evt, int sock, void* data, int len)
{
   if (evt == NET_EVT_NEW_CONNECTION)
   { /* Don't update other clients until name is sent */
      /* Create new player */
      player_t* p_player = (player_t*)calloc(1, sizeof(player_t));
      REQUIRE(p_player != NULL);
      p_player->id = sock;
      core_add_player(p_player);
      TRC_DBG(net_server, "New connection on socket %d", sock);
      //net_server_send_cmd(sock, );
   }
   else if (evt == NET_EVT_DISCONNECTED)
   { /* Client disconnected from server */
      player_t* p_player = core_find_player(sock);
      TRC_DBG(net_server, "Client disconnected (socket %d)", sock);
      if (p_player != NULL)
      {
         /* Remove client/player */
         core_rm_player(p_player);
         /*  Update other clients */
         p_player = SLNK_NEXT(player_t, &core_get()->players_head);
         while(p_player != NULL)
         {
            net_server_send_cmd(p_player->id, NET_CMD_SERVER_PLAYER_REMOVE,
               (void*)sock);
            p_player = SLNK_NEXT(player_t, p_player);
         }
      }
   }
   else if (evt == NET_EVT_RX)
   {
      net_server_parse_command(sock, data, len);
   }
   else
   {
      TRC_ERR(net_server, "Error: Unknown net event");
   }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void net_server_parse_command(int sock, void* data, int len)
{
   uint8_t* p_data = (uint8_t*)data;
   net_us_cmd_t cmd = (p_data[0] << 8) + p_data[1];
   core_t* p_core = core_get();
   TRC_DBG(net_server, "Command received: %s (%d) ",
      net_us_cmd_to_str(cmd), cmd);
   switch (cmd)
   {
      case NET_CMD_CLIENT_PLAYER_NAME:
      { /* Update player name and send player info to clients */
         player_t* p_player = core_find_player(sock);
         player_t* p_player2;
         memset(p_player->name, 0, MAX_CLIENT_NAME_LEN);
         memcpy(p_player->name, &p_data[2], len-2);
         p_player2 = SLNK_NEXT(player_t, &p_core->players_head);
         while(p_player2 != NULL)
         {
            /* Update new player with all players info */
            net_server_send_cmd(p_player->id, NET_CMD_SERVER_PLAYER_INFO,
               p_player2);
            /* Update players with new player info */
            if (p_player->id != p_player2->id)
            {
               net_server_send_cmd(p_player2->id, NET_CMD_SERVER_PLAYER_INFO,
                  p_player);
            }
            p_player2 = SLNK_NEXT(player_t, p_player2);
         }
         /* Send greeting to new player */
         p_core->log_entry.p_player = NULL;
         strcpy(p_core->log_entry.text, "Welcome!");
         //core_log(NULL, "Welcome!");
         net_server_send_cmd(p_player->id, NET_CMD_SERVER_LOG_ENTRY, NULL);
         break;
      }
      case NET_CMD_CLIENT_START_GAME:
      {
         srv_hsm_evt(HSM_EVT_NET_START_GAME);
         break;
      }
      case NET_CMD_CLIENT_LOAD_GAME:
      {
         srv_hsm_evt(HSM_EVT_NET_LOAD_GAME);
         break;
      }
      case NET_CMD_CLIENT_SELECT_COLOR:
      {
         pbuf_unpack(&p_data[2], "b", &core_get()->color_selection);
         REQUIRE(core_get()->color_selection <= PLAYER_COLOR_LAST);
         srv_hsm_evt(HSM_EVT_NET_SELECT_COLOR);
         break;
      }
      case NET_CMD_CLIENT_SELECT_ACTION:
      {
         pbuf_unpack(&p_data[2], "b", &core_get()->action_selection);
         srv_hsm_evt(HSM_EVT_NET_SELECT_ACTION);
         break;
      }
      case NET_CMD_CLIENT_SELECT_BOARD_LOT:
      {
         pbuf_unpack(&p_data[2], "b", &core_get()->board_lot_selection);
         //REQUIRE(core_get()->board_lot_selection < MAX_BOARD_LOTS);
         srv_hsm_evt(HSM_EVT_NET_SELECT_BOARD_LOT);
         break;
      }
      case NET_CMD_CLIENT_SELECT_BOARD_CARD:
      {
         pbuf_unpack(&p_data[2], "b", &core_get()->card_selection);
         srv_hsm_evt(HSM_EVT_NET_SELECT_BOARD_CARD);
         break;
      }
      case NET_CMD_CLIENT_SELECT_PLAYER_CARD:
      {
         pbuf_unpack(&p_data[2], "b", &core_get()->card_selection);
         srv_hsm_evt(HSM_EVT_NET_SELECT_PLAYER_CARD);
         break;
      }
      case NET_CMD_CLIENT_SELECT_CARD_CHOICE:
      {
         //pbuf_unpack(&p_data[2], "b", &core_get()->card_choice);
         //srv_hsm_evt(HSM_EVT_NET_SELECT_CARD_CHOICE);
         break;
      }
      case NET_CMD_CLIENT_PASS:
      {
         core_log(p_core->active_player, "passed");
         p_core->active_player->passed = TRUE;
         srv_hsm_evt(HSM_EVT_NET_PASS);
         break;
      }
      case NET_CMD_CLIENT_DONE:
      {
         core_log(p_core->active_player, "done");
         //p_core->active_player->done = TRUE;
         srv_hsm_evt(HSM_EVT_NET_DONE);
         break;
      }
      default:
         TRC_ERR(net_server, "Error: Unknown command %d", cmd);
         break;
   }
}

/* END OF FILE ***************************************************************/

