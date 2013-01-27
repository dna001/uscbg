/******************************************************************************
Copyright (c) 2013, All Rights Reserved.
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file mm_hsm.c
\brief The Shadows over Camelot Hierarchical State Machine implementation. */
/*---------------------------------------------------------------------------*/
/* INCLUDE FILES *************************************************************/
#include "sys_def.h"
#include "sys_assert.h"
#include "slnk.h"
#include "trc.h"
#include "pbuf.h"
#include "net.h"
#include "net_us.h"
#include "net_client.h"
#include "core.h"
#include "main_hsm.h"
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

/* CONSTANTS / MACROS ********************************************************/

/* LOCAL DATATYPES ***********************************************************/

/* LOCAL FUNCTION PROTOTYPES *************************************************/
static net_evt_cb_fn_t net_client_evt_cb_fn;
static void net_client_parse_command(int sock, void* data, int len);

/* MODULE CONSTANTS / VARIABLES **********************************************/
SYS_ASSERT_FILE;
SYS_DBC_FILE;  /*!< Defines the name of this source file once for all */

TRC_DEF(net_client);

static net_cfg_t net_cfg = {
   .is_server = FALSE,
   .port = 5050,
   .poll = TRUE,
   .evt_fn = net_client_evt_cb_fn
};

static int server_sock;

/* GLOBAL CONSTANTS / VARIABLES **********************************************/

/* GLOBAL FUNCTIONS **********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void net_client_init(void)
{
   TRC_REG(net_client, TRC_ERROR | TRC_DEBUG);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void net_client_start(char* name, char* ip, int port)
{
   player_t* p_player = (player_t*)calloc(1, sizeof(player_t));
   REQUIRE(p_player != NULL);
   strncpy(net_cfg.addr, ip, 15);
   p_player->id = 0;
   strncpy(p_player->name, name, MAX_CLIENT_NAME_LEN);
   core_add_player(p_player);
   net_start(&net_cfg);
}

/* LOCAL FUNCTIONS ***********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void net_client_evt_cb_fn(int evt, int sock, void* data, int len)
{
   if (evt == NET_EVT_NEW_CONNECTION)
   { /* Send client data */
      server_sock = sock;
      net_client_send_cmd(NET_CMD_CLIENT_PLAYER_NAME, 0);
   }
   else if (evt == NET_EVT_DISCONNECTED)
   { /* Disconnected from server */

   }
   else if (evt == NET_EVT_RX)
   {
      net_client_parse_command(sock, data, len);
   }
   else
   {
      TRC_ERR(net_client, "Error: Unknown net event");
   }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void net_client_parse_command(int sock, void* data, int len)
{
   uint8_t* p_data = (uint8_t*)data;
   net_us_cmd_t cmd = (p_data[0] << 8) + p_data[1];
   TRC_DBG(net_client, "Command received: %s (%d) ",
      net_us_cmd_to_str(cmd), cmd);
   switch (cmd)
   {
      case NET_CMD_SERVER_PLAYER_INFO:
      {
         int id;
         player_t* p_player;
         pbuf_unpack(&p_data[2], "w", &id);
         if (id == 0)
         { /* This player is always first in the list */
            p_player = SLNK_NEXT(player_t, &core_get()->players_head);
         }
         else
         {
            p_player = core_find_player(id);
         }
         if (p_player == NULL)
         { /* New player */
            p_player = (player_t*)calloc(1, sizeof(player_t));
            REQUIRE(p_player != NULL);
            core_add_player(p_player);
         }
         pbuf_unpack(&p_data[2], "wsdw", &id, MAX_CLIENT_NAME_LEN,
            p_player->name, &p_player->id);
         main_hsm_evt(HSM_EVT_NET_UPDATE_PLAYERS);
         break;
      }
      case NET_CMD_SERVER_PLAYER_REMOVE:
      {
         int id;
         player_t* p_player;
         pbuf_unpack(&p_data[2], "w", &id);
         p_player = core_find_player(id);
         REQUIRE(p_player != NULL);
         core_rm_player(p_player);
         main_hsm_evt(HSM_EVT_NET_UPDATE_PLAYERS);
         break;
      }
      case NET_CMD_SERVER_PLAYER_UPDATE:
      { /* Used for any other player updates during the game */
         player_t* p_player;
         int id;
         int pos = 2;
         pos += pbuf_unpack(&p_data[2], "w", &id);
         p_player = core_find_player(id);
         REQUIRE(p_player != NULL);
         pos += pbuf_unpack(&p_data[pos], "bbbwww", &p_player->color,
            &p_player->ap, &p_player->politicians, &p_player->vocations,
            &p_player->wealth, &p_player->prestige);
         /* Cards */
         core_dbg_dump_player_data(p_player);
         main_hsm_evt(HSM_EVT_NET_UPDATE_PLAYERS);
         break;
      }
      case NET_CMD_SERVER_BOARD_CARDS_UPDATE:
      {
#if 0
         int pos = 2;
         uint8_t id;
         int i;
         for (i=0;i<5;i++)
         {
            card_t* p_card;
            pos += pbuf_unpack(&p_data[pos], "b", &id);
            p_card = cards_find(&core_get()->cards_head, id);
            core_get()->board_cards[i] = p_card;
            TRC_DBG(net_client, "card id %d (%p)", id, p_card);
         }
         main_hsm_evt(HSM_EVT_NET_UPDATE_BOARD_CARDS);
#endif
         break;
      }
      case NET_CMD_SERVER_START_GAME:
      {
         main_hsm_evt(HSM_EVT_NET_START_GAME);
         break;
      }
      case NET_CMD_SERVER_SELECT_COLOR:
      {
         pbuf_unpack(&p_data[2], "b", &core_get()->available_colors);
         main_hsm_evt(HSM_EVT_NET_SELECT_COLOR);
         break;
      }
      case NET_CMD_SERVER_BLOCK_UPDATE:
      {
         block_t* p_blk;
         uint8_t id;
         int pos = 2;
         int i;
         pos += pbuf_unpack(&p_data[pos], "b", &id);
         REQUIRE(id < MAX_BOARD_BLOCKS);
         p_blk = &core_get()->board_blocks[id];
         pos += pbuf_unpack(&p_data[pos], "bw",
            &p_blk->n_buildings, &p_blk->value);
         for (i=0;i<4;i++)
         {
            pos += pbuf_unpack(&p_data[pos], "bbbb",
               &p_blk->buildings[i].zone, &p_blk->buildings[i].size,
               &p_blk->buildings[i].owner, &p_blk->buildings[i].block_pos);
         }
         TRC_DBG(net_client, "Block: id %d, n_buildings %d, value %d",
            p_blk->id, p_blk->n_buildings, p_blk->value);
         }
         break;
      case NET_CMD_SERVER_SELECT_BOARD_LOT:
         main_hsm_evt(HSM_EVT_NET_SELECT_BOARD_LOT);
         break;
      case NET_CMD_SERVER_SELECT_BOARD_CARD:
         main_hsm_evt(HSM_EVT_NET_SELECT_BOARD_CARD);
         break;
      case NET_CMD_SERVER_SELECT_PLAYER_CARD:
         main_hsm_evt(HSM_EVT_NET_SELECT_PLAYER_CARD);
         break;
      case NET_CMD_SERVER_SELECT_CARD_CHOICE:
         main_hsm_evt(HSM_EVT_NET_SELECT_CARD_CHOICE);
         break;
      case NET_CMD_SERVER_ACTIVE_PLAYER:
      {
         int id;
         player_t* p_player;
         pbuf_unpack(&p_data[2], "w", &id);
         p_player = core_find_player(id);
         REQUIRE(p_player != NULL);
         core_get()->active_player = p_player;
         main_hsm_evt(HSM_EVT_NET_UPDATE_ACTIVE_PLAYER);
         break;
      }
      case NET_CMD_SERVER_PHASE_UPDATE:
      {
         core_t* p_core = core_get();
         pbuf_unpack(&p_data[2], "bb", &p_core->current_round,
            &p_core->state);
         main_hsm_evt(HSM_EVT_NET_UPDATE_PHASE);
         break;
      }
      case NET_CMD_SERVER_LOG_ENTRY:
      {
         int id;
         char* log_entry;
         core_t* p_core = core_get();
         int pos = 2;
         pos += pbuf_unpack(&p_data[2], "w", &id);
         log_entry = (char*)&p_data[pos];
         p_core->log_entry.p_player = core_find_player(id);
         snprintf(p_core->log_entry.text, MAX_CORE_LOG_ENTRY, "%s",log_entry);
         main_hsm_evt(HSM_EVT_NET_UPDATE_LOG);
         break;
      }
      default:
         TRC_ERR(net_client, "Error: Unknown command %d", cmd);
         break;
   }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void net_client_send_cmd(net_us_cmd_t cmd, uint32_t data)
{
   uint8_t packet[MAX_PACKET_SZ];
   int len = 2;
   TRC_DBG(net_client, "Sending command %s (%d) ",
      net_us_cmd_to_str(cmd), cmd);
   packet[0] = cmd >> 8;
   packet[1] = cmd & 0xff;
   switch (cmd)
   {
      case NET_CMD_CLIENT_PLAYER_NAME:
      {
         player_t* p_player = core_find_player(0);
         REQUIRE(p_player != NULL);
         memcpy((char*)&packet[2], p_player->name, MAX_CLIENT_NAME_LEN);
         len += MAX_CLIENT_NAME_LEN;
         break;
      }
      case NET_CMD_CLIENT_START_GAME:
         break;
      case NET_CMD_CLIENT_LOAD_GAME:
         break;
      case NET_CMD_CLIENT_SELECT_COLOR:
      {
         uint8_t color = (uint8_t)data;
         core_get()->color_selection = color;
         TRC_DBG(net_client, "Color selected %d", color);
         len += pbuf_pack(&packet[2], "b", color);
         break;
      }
      case NET_CMD_CLIENT_SELECT_BOARD_LOT:
      {
         len += pbuf_pack(&packet[2], "b", core_get()->board_lot_selection);
         break;
      }
      case NET_CMD_CLIENT_SELECT_BOARD_CARD:
      case NET_CMD_CLIENT_SELECT_PLAYER_CARD:
      {
         len += pbuf_pack(&packet[2], "b", core_get()->card_selection);
         break;
      }
      case NET_CMD_CLIENT_SELECT_CARD_CHOICE:
      {
         //core_get()->card_choice = (uint8_t)data;
         //len += pbuf_pack(&packet[2], "b", core_get()->card_choice);
         break;
      }
      case NET_CMD_CLIENT_PASS:
         break;
      case NET_CMD_CLIENT_DONE:
         break;
      default:
         TRC_ERR(net_client, "Error: Unknown command %d", cmd);
         return;
   }
   net_write_packet(server_sock, packet, len);
}

/* END OF FILE ***************************************************************/
