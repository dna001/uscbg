/******************************************************************************
Copyright (c) 2013, All Rights Reserved.
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file core.c
\brief The Core game logic implementation for Urban Sprawl. */
/*---------------------------------------------------------------------------*/
/* INCLUDE FILES *************************************************************/
#include "sys_def.h"
#include "sys_assert.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include "net_us.h"
#include "slnk.h"
#include "trc.h"
#include "pbuf.h"
#include "core.h"

/* CONSTANTS / MACROS ********************************************************/
#define MAX_PATH_LENGTH (255)

/* LOCAL DATATYPES ***********************************************************/
typedef struct
{
   uint8_t block;
   uint8_t mark;
   zone_t zone;
} startup_building_t;

/* LOCAL FUNCTION PROTOTYPES *************************************************/
//static int core_lots_setup(int n, int x, int y, int c);
static void core_prepare_players(void);
//static int core_compare_ascending(const void* a, const void* b);
static int core_compare_descending(const void* a, const void* b);
static int core_calc_block_value(int block);

/* MODULE CONSTANTS / VARIABLES **********************************************/
SYS_ASSERT_FILE;
SYS_DBC_FILE;  /*!< Defines the name of this source file once for all */

TRC_DEF(core);

/* Marked bits explanation
block: |0|1|
       |2|3|
*/
const startup_building_t startup_buildings[16] = {
   {0, BIT(3), ZONE_RES}, {5, BIT(2), ZONE_RES},
   {7, BIT(3), ZONE_IND}, {10, BIT(2), ZONE_COM},
   {14, BIT(3), ZONE_IND}, {15, BIT(2), ZONE_COM},
   {20, BIT(1), ZONE_COM}, {21, BIT(0), ZONE_CIV},
   {25, BIT(1), ZONE_COM}, {28, BIT(0), ZONE_CIV},
   {30, BIT(1), ZONE_RES}, {35, BIT(0), ZONE_RES}
};

const uint8_t bonus_vp_tbl[] =
{
   0, 1, 3, 6, 10, 15, 21, 28, 36, 45
};

static core_t core;

/* GLOBAL CONSTANTS / VARIABLES **********************************************/

/* GLOBAL FUNCTIONS **********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void core_init(core_net_send_fn_t* p_fn_send, core_net_broadcast_fn_t* p_fn_bc)
{
   int i;
   memset(&core, 0, sizeof(core_t));
   SLNK_INIT(&core.players_head);
   TRC_REG(core, TRC_ERROR | TRC_DEBUG);
   core.net_send = p_fn_send;
   core.net_broadcast = p_fn_bc;
   core.current_round = 1;
   cards_init();
   for (i=0;i<PLAYER_COLOR_LAST;i++)
   {
      core.available_colors |= 1u << i;
   }
   for (i=0;i<MAX_BOARD_BLOCKS;i++)
   {
      core.board_blocks[i].id = i;
   }
   core.prestige_markers[0].rows = BIT(2)|BIT(3); /* Prestige 1 on row 2 and 3 */
   core.prestige_markers[1].rows = BIT(4)|BIT(5); /* Prestige 2 on row 4 and 5 */
   core.prestige_markers[2].rows = BIT(0)|BIT(1); /* Prestige 3 on row 0 and 1 */
   core.wealth_markers[0].columns = BIT(0); /* Wealth 1 on column 0 */
   core.wealth_markers[1].columns = BIT(5); /* Wealth 2 on column 5 */
   core.wealth_markers[2].columns = BIT(4); /* Wealth 3 on column 4 */
   core.wealth_markers[3].columns = BIT(1); /* Wealth 4 on column 1 */
   core.wealth_markers[4].columns = BIT(2); /* Wealth 5 on column 2 */
   core.wealth_markers[5].columns = BIT(3); /* Wealth 6 on column 3 */
   core.startup_buildings = 12;
   /* Test */
   core.board_blocks[1].buildings[0].block_pos = 0x1;
   core.board_blocks[1].buildings[0].size = 1;
   core.board_blocks[1].buildings[0].zone = ZONE_CIV;
   core.board_blocks[1].n_buildings = 1;
   core.board_blocks[4].buildings[0].block_pos = 0x1;
   core.board_blocks[4].buildings[0].size = 1;
   core.board_blocks[4].buildings[0].zone = ZONE_COM;
   core.board_blocks[4].n_buildings = 1;
   core.board_blocks[18].buildings[0].block_pos = 0x1;
   core.board_blocks[18].buildings[0].size = 1;
   core.board_blocks[18].buildings[0].zone = ZONE_IND;
   core.board_blocks[18].n_buildings = 1;
   core.board_blocks[34].buildings[0].block_pos = 0x1;
   core.board_blocks[34].buildings[0].size = 1;
   core.board_blocks[34].buildings[0].zone = ZONE_RES;
   core.board_blocks[34].n_buildings = 1;
   //cards_create_deck(&core.cards_head);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void core_free(void)
{
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
core_t* core_get(void)
{
   return &core;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
bool_t core_newgame(bool_t load)
{
   bool_t ret = FALSE;
   if (load)
   {
      core.state = CORE_STATE_SETUP;
      srand((unsigned)time( NULL ));
      //cards_shuffle_deck(&core.cards_head);
      //if (core_loadgame("save.dat"))
      {
         /* Start player left most on initiative track */
         //core.initiative = 0;
         //core.current_action = CORE_AD_INITIATIVE_AP;
         //core.active_player = core_find_player_by_animal(core.ad.initiative[0]);
         ret = TRUE;
      }
   }
   else
   {
      core.state = CORE_STATE_SETUP;
      core_net_broadcast(NET_CMD_SERVER_PHASE_UPDATE, NULL);
      srand((unsigned)time( NULL ));
      //cards_create_deck(&core.cards_head);
      //cards_shuffle_deck(&core.cards_head);
      //core_prepare_players();
      /* Start player left most on initiative track */
      //core.current_action = CORE_AD_INITIATIVE_AP;
      //core.active_player = core_find_player_by_animal(core.ad.initiative[0]);
      ret = TRUE;
   }
   return ret;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void core_select_color(void)
{
   player_t* p_player = core.active_player;
   int color = 0;
   REQUIRE(core.color_selection <= 6);
   srand((unsigned)time( NULL ));
   if (core.color_selection == 6)
   {
      int i;
      int r = rand()%6;
      TRC_DBG(core, "Random number %d", r);
      for (i=0;i<PLAYER_COLOR_LAST;i++)
      {
         if ((core.available_colors & (1u << i)) && (r == 0))
         {
            color = i;
            break;
         }
         if (r > 0)
         {
            r--;
         }
      }
      TRC_DBG(core, "Random color %d", i);
   }
   else
   {
      color = core.color_selection;
   }
   p_player->color = color;
   core.available_colors &= ~(1u << color);
   TRC_DBG(core, "Available colors 0x%x", core.available_colors);
   core_net_broadcast(NET_CMD_SERVER_PLAYER_UPDATE, core.active_player);
   //core_log(p_player, "selected %s", animal_str[animal]);
}

#if 0
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
core_state_t core_next_action(void)
{
   core_state_t next_state = CORE_STATE_ACTIONS;
   {
      /* Auto save game */
      core_savegame("save.dat");
   }
   if (core.current_action == CORE_AD_AP_LAST)
   {
      if ((core_find_player_by_animal(ANIMAL_MAMAL) != NULL) &&
          (core.state != CORE_STATE_EXTINCTION))
      {
         core.active_player = core_find_player_by_animal(ANIMAL_MAMAL);
         next_state = CORE_STATE_EXTINCTION;
         core.state = next_state;
      }
      else if (core.last_round)
      {
         next_state = CORE_STATE_GAME_END;
         core.state = next_state;
      }
      else
      {
         next_state = CORE_STATE_RESET;
         core.state = next_state;
      }
   }
   core_net_broadcast(NET_CMD_SERVER_PHASE_UPDATE, NULL);
   return next_state;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void core_action_done(void)
{
   uint8_t animal = core.ad.apbox[core.current_action];
   player_t* p_player = core_find_player_by_animal(animal);
   /* Remove action pawn from action display and give back to player */
   if ((core.current_action != CORE_AD_REGRESSION_AP_REPTILE) &&
       (core.current_action != CORE_AD_SPECIATION_AP_INSECT) &&
       (core.current_action != CORE_AD_COMPETITION_AP_ARACHNID))
   {
      if (core.ad.apbox[core.current_action] != ANIMAL_LAST)
      {
         p_player->used_ap--;
         core.ad.apbox[core.current_action] = ANIMAL_LAST;
      }
   }
   /* Handle regression and wasteland mandatory parts */
   if (core.current_action == CORE_AD_REGRESSION_AP_REPTILE)
   {
      core_action_regression_mandatory();
   }
   else if (core.current_action == CORE_AD_WASTELAND_AP)
   {
      core_action_wasteland_mandatory();
   }
   core_update_earth_dominance();
   core.current_action++;
   core.element_box_selection = CORE_AD_ELEMENT_LAST;
   core.board_tile_selection = MAX_BOARD_TILES;
   core.board_element_selection = MAX_BOARD_ELEMENTS;
   core.wanderlust_tile_selection = 3;
   core.speciation_element = ELEMENT_NONE;
   core.speciation_tiles[0] = MAX_BOARD_TILES;
   core.speciation_tiles[1] = MAX_BOARD_TILES;
   core.speciation_tiles[2] = MAX_BOARD_TILES;
   core.speciation_species[0] = 0;
   core.speciation_species[1] = 0;
   core.speciation_species[2] = 0;
   core.competition_terrain[0] = TERRAIN_NONE;
   core.competition_terrain[1] = TERRAIN_NONE;
   core.competition_terrain[2] = TERRAIN_NONE;
   core.migration_points = 1;
   core.migration_tile = MAX_BOARD_TILES;
   core_net_broadcast(NET_CMD_SERVER_PLAYER_UPDATE, core.active_player);
   core_net_broadcast(NET_CMD_SERVER_ACTION_DISPLAY_UPDATE, NULL);
}
#endif

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void core_invest(void)
{
   player_t* p_player = core.active_player;
   int i;
   int wealth = 0;
   /* Discard selected planning card for wealth */
   core_net_broadcast(NET_CMD_SERVER_PLAYER_UPDATE, core.active_player);
   core_log(p_player, "recieved %d wealth", wealth);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void core_action_take_card(void)
{
   player_t* p_player = core.active_player;
   int i;
   /* Add selected card to player card list (if planning card) or favor
      (if contract card). */
   //core_log(p_player, "took card %d for %d ap(s)", card, ap);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void core_action_build(void)
{ /* Build contract card using selected planning cards on selected lot(s) */
   player_t* p_player = core.active_player;
   zone_t zone;
   int size;
   int lot = core.board_lot_selection;
   int cost = 0;
   block_t* p_blk = &core.board_blocks[lot/4];
   int i;
   if (core.state == CORE_STATE_SETUP)
   { /* Only lot selected in this state. Set size and type here. */
      int block = lot/4;
      size = 1;
      for (i=0;i<16;i++)
      {
         if (startup_buildings[i].block == block)
         {
            zone = startup_buildings[i].zone;
            break;
         }
      }
   }
   REQUIRE(p_blk->n_buildings < 4);
   p_blk->buildings[p_blk->n_buildings].owner = p_player->color;
   p_blk->buildings[p_blk->n_buildings].zone = zone;
   p_blk->buildings[p_blk->n_buildings].size = size;
   if (size == 1)
   {
      p_blk->buildings[p_blk->n_buildings].block_pos = 1u << (lot%4);
   }
   p_blk->n_buildings++;
   core_net_broadcast(NET_CMD_SERVER_BLOCK_UPDATE, p_blk);
   cost = core_calc_block_value(lot/4);
   TRC_DBG(core, "building cost=%d", cost);
   p_player->wealth -= cost;
   core_net_broadcast(NET_CMD_SERVER_PLAYER_UPDATE, p_player);
   //core_log(p_player, "built %s", p_card->name);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void core_prepare_new_round(void)
{
   //core.current_round++;
   /* Next player */
   core.active_player = SLNK_NEXT(player_t, core.active_player);
   if (core.active_player == NULL)
   {
      core.active_player = SLNK_NEXT(player_t, &core.players_head);
   }
   core.active_player = SLNK_NEXT(player_t, &core.players_head);
   core.state = CORE_STATE_INVESTMENTS;
   core_net_broadcast(NET_CMD_SERVER_PHASE_UPDATE, NULL);
   /* Auto save game */
   //core_savegame("save.dat");
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void core_use_card(void)
{
   player_t* p_player = core.active_player;
   //card_t* p_card = cards_draw(&p_player->cards_head, core.card_selection);
   //REQUIRE(p_card != NULL);
   //cards_use(p_card);
   //SLNK_ADD(&core.cards_discard_head, p_card);
   core_net_broadcast(NET_CMD_SERVER_PLAYER_UPDATE, p_player);
   core.card_selection = 0;
}

#if 0
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void core_calculate_final_vp(void)
{ /* Score all tiles one final time and determine winner */
   player_t* winner = 0;
   int most_vp = 0;
   int i;
   for (i=0;i<MAX_BOARD_TILES;i++)
   {
      tile_t* p_tile = &core.board_tiles[i];
      if (p_tile->terrain > TERRAIN_NONE)
      {
         core_domination_vp(p_tile);
      }
   }
   /* Find winner */
   for (i=ANIMAL_INSECT;i>=0;i--)
   {
      player_t* p_player = core_find_player_by_animal(i);
      if (p_player)
      {
         if (p_player->vp >= most_vp)
         {
            winner = p_player;
            most_vp = p_player->vp;
         }
      }
   }
   core_log(winner, "is the winner with %d vp", winner->vp);
}
#endif

#if 0
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
lot_t* core_find_lot(int x, int y)
{
   lot_t* p_lot = NULL;
   int i;
   for (i=0;i<MAX_BOARD_BLOCKS;i++)
   {
      p_tile = &core.board_blocks[i];
   }
   return p_tile;
}
#endif

#if 0
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
bool_t core_savegame(char* name)
{ /* Only at end of round at the moment */
   bool_t res = FALSE;
   FILE* fp;
   uint8_t buf[512];
   int len;
   int i;
   /* Backup old save */
   remove("save_bak.dat");
   rename(name, "save_bak.dat");
   fp = fopen(name, "wb");
   if (fp == NULL)
   {
      TRC_ERR(core, "Error opening save file %s", name);
      goto done;
   }
   /* Save player data */
   for (i=0;i<ANIMAL_LAST;i++)
   {
      player_t* p_player = core_find_player_by_animal(i);
      if (p_player)
      {
         len = pbuf_pack(buf, "sdbbbbwsd", MAX_NAME_LENGTH, p_player->name,
            p_player->ap, p_player->used_ap, p_player->gene_pool,
            p_player->gene_pool_max, p_player->vp, 6, p_player->elements);
         TRC_DBG(core, "player save size = %d", len);
         fwrite(buf, 1, len, fp);
      }
      else
      {
         memset(buf, 0, 54);
         fwrite(buf, 1, 54, fp);
      }
   }
   /* Save board tiles */
   for (i=0;i<MAX_BOARD_TILES;i++)
   {
      tile_t* p_tile = &core.board_tiles[i];
      len = pbuf_pack(buf, "bbsdb", p_tile->terrain, p_tile->tundra,
         6, p_tile->species, p_tile->dominance);
      fwrite(buf, 1, len, fp);
   }
   /* Save board elements */
   len = 0;
   for (i=0;i<MAX_BOARD_ELEMENTS;i++)
   {
      element_t* p_element = &core.board_elements[i];
      len += pbuf_pack(&buf[len], "b", p_element->element);
   }
   TRC_DBG(core, "board element total save size = %d", len);
   fwrite(buf, 1, len, fp);
   /* Save action display */
   len = pbuf_pack(buf, "sdsdsd", 6, core.ad.initiative, CORE_AD_AP_LAST,
      core.ad.apbox, CORE_AD_ELEMENT_LAST, core.ad.element);
   TRC_DBG(core, "action display total save size = %d", len);
   fwrite(buf, 1, len, fp);
   /* Save board cards */
   len = 0;
   for (i=0;i<5;i++)
   {
      card_t* p_card = core.board_cards[i];
      if (p_card)
      {
         len += pbuf_pack(&buf[len], "b", (uint8_t)p_card->id);
      }
      else
      {
         buf[len] = 0;
         len++;
      }
   }
   TRC_DBG(core, "board cards total save size = %d", len);
   fwrite(buf, 1, len, fp);
   /* Save used cards */
   memset(buf, 0, 512);
   {
      card_t* p_card = SLNK_NEXT(card_t, &core.cards_discard_head);
      while(p_card != NULL)
      {
         buf[p_card->id/8] |= (1u << (p_card->id%8));
         p_card = SLNK_NEXT(card_t, p_card);
      }
      TRC_DBG(core, "used cards bits %x %x %x %x", buf[0], buf[1],
         buf[2], buf[3]);
      fwrite(buf, 1, 4, fp);
   }
   /* Save element bag */
   len = pbuf_pack(buf, "sd", 6, core.element_bag);
   fwrite(buf, 1, len, fp);
   /* Save wanderlust tiles state */
   len = pbuf_pack(buf, "sdsdsd", 3, core.wanderlust_tiles, 3,
      core.wanderlust_tiles_left, 7, core.wanderlust_tile_bag);
   TRC_DBG(core, "wanderlust tiles total save size = %d", len);
   fwrite(buf, 1, len, fp);
   /* Save current round, state and action*/
   len = pbuf_pack(buf, "bbb", core.current_round, core.state,
      core.current_action);
   fwrite(buf, 1, len, fp);
   fclose(fp);
   core_log(NULL, "Game saved!");
   res = TRUE;
done:
   return res;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
bool_t core_loadgame(char* name)
{
   bool_t res = FALSE;
   FILE* fp;
   uint8_t* buf = NULL;
   int len;
   int pos = 0;
   int i;
   fp = fopen(name, "rb");
   if (fp == NULL)
   {
      TRC_ERR(core, "Error opening save file %s", name);
      goto done;
   }
   /* Get file length */
   fseek(fp, 0, SEEK_END);
   len = ftell(fp);
   TRC_DBG(core, "Save file length: %d", len);
   rewind(fp);
   buf = (uint8_t*)malloc(len);
   REQUIRE(buf != NULL);
   fread(buf, 1, len, fp);
   fclose(fp);
   /* Load player data */
   for (i=0;i<ANIMAL_LAST;i++)
   {
      player_t player;
      pos += pbuf_unpack(&buf[pos], "sdbbbbwsd", MAX_NAME_LENGTH,
         player.name, &player.ap, &player.used_ap, &player.gene_pool,
         &player.gene_pool_max, &player.vp, 6, player.elements);
      /* Find player */
      if (player.name[0] != 0)
      {
         player_t* p_player;
         p_player = core_find_player_by_name(player.name);
         if (p_player)
         {
            TRC_DBG(core, "loadgame: Found matching player name %s",
               p_player->name);
            p_player->animal = i;
            p_player->ap = player.ap;
            p_player->used_ap = player.used_ap;
            p_player->gene_pool = player.gene_pool;
            p_player->gene_pool_max = player.gene_pool_max;
            p_player->vp = player.vp;
            memcpy(p_player->elements, player.elements, 6);
            core_net_broadcast(NET_CMD_SERVER_PLAYER_UPDATE, p_player);
         }
         else
         {
            TRC_ERR(core, "loadgame: Player %s not in save file",
               player.name);
            goto done;
         }
      }
   }
   REQUIRE(pos <= len);
   /* Load board tiles */
   for (i=0;i<MAX_BOARD_TILES;i++)
   {
      tile_t* p_tile = &core.board_tiles[i];
      pos += pbuf_unpack(&buf[pos], "bbsdb", &p_tile->terrain, &p_tile->tundra,
         6, p_tile->species, &p_tile->dominance);
      core_net_broadcast(NET_CMD_SERVER_TILE_UPDATE, p_tile);
   }
   REQUIRE(pos <= len);
   /* Load board elements */
   for (i=0;i<MAX_BOARD_ELEMENTS;i++)
   {
      element_t* p_element = &core.board_elements[i];
      pos += pbuf_unpack(&buf[pos], "b", &p_element->element);
      core_net_broadcast(NET_CMD_SERVER_ELEMENT_UPDATE, p_element);
   }
   REQUIRE(pos <= len);
   /* Load action display */
   pos += pbuf_unpack(&buf[pos], "sdsdsd", 6, core.ad.initiative,
      CORE_AD_AP_LAST, core.ad.apbox, CORE_AD_ELEMENT_LAST, core.ad.element);
   core_net_broadcast(NET_CMD_SERVER_ACTION_DISPLAY_UPDATE, NULL);
   /* Load board cards */
   for (i=0;i<5;i++)
   {
      uint8_t id;
      pos += pbuf_unpack(&buf[pos], "b", &id);
      if (id > 0)
      {
         core.board_cards[i] = cards_draw(&core.cards_head, id);
      }
      else
      {
         core.board_cards[i] = NULL;
      }
   }
   core_net_broadcast(NET_CMD_SERVER_BOARD_CARDS_UPDATE, NULL);
   REQUIRE(pos <= len);
   /* Load used cards */
   {
      uint8_t used_cards[4];
      pos += pbuf_unpack(&buf[pos], "sd", 4, used_cards);
      for (i=1;i<CARD_ID_LAST;i++)
      {
         int offs = i/8;
         int bit = i%8;
         if (used_cards[offs] & (1u << bit))
         { /* Remove card from draw deck and put in discard pile */
            card_t* p_card = cards_draw(&core.cards_head, i);
            REQUIRE(p_card != NULL);
            SLNK_ADD(&core.cards_discard_head, p_card);
            TRC_DBG(core, "loadgame: Card %d moved to discard pile", i);
         }
      }
   }
   REQUIRE(pos <= len);
   /* Load element bag */
   pos += pbuf_unpack(&buf[pos], "sd", 6, core.element_bag);
   REQUIRE(pos <= len);
   /* Load wanderlust tiles state */
   pos += pbuf_unpack(&buf[pos], "sdsdsd", 3, core.wanderlust_tiles, 3,
      core.wanderlust_tiles_left, 7, core.wanderlust_tile_bag);
   core_net_broadcast(NET_CMD_SERVER_WANDERLUST_UPDATE, NULL);
   REQUIRE(pos <= len);
   /* Load current round, state and action*/
   pos += pbuf_unpack(&buf[pos], "bbb", &core.current_round, &core.state,
      &core.current_action);
   core_log(NULL, "Game loaded!");
   res = TRUE;
done:
   if (buf)
   {
      free(buf);
   }
   return res;
}
#endif

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void core_add_player(player_t* p_player)
{
   p_player->color = PLAYER_COLOR_LAST; /* No color selected yet */
   p_player->ap = 6;
   //TRC_DBG(core, "Player %d got color %d", core.n_players, p_player->color);
   core.n_players++;
   SLNK_ADD(&core.players_head, p_player);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void core_rm_player(player_t* p_player)
{
   SLNK_REMOVE(&core.players_head, p_player);
   core.n_players--;
   free(p_player);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
player_t* core_find_player(int id)
{
   player_t* p_player = SLNK_NEXT(player_t, &core.players_head);
   while(p_player != NULL)
   {
      if (p_player->id == id)
      {
         break;
      }
      p_player = SLNK_NEXT(player_t, p_player);
   }
   return p_player;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
player_t* core_find_player_by_name(char* name)
{
   player_t* p_player = SLNK_NEXT(player_t, &core.players_head);
   while(p_player != NULL)
   {
      if (strncmp(p_player->name, name, MAX_NAME_LENGTH) == 0)
      {
         break;
      }
      p_player = SLNK_NEXT(player_t, p_player);
   }
   return p_player;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
player_t* core_find_player_by_color(int color)
{
   player_t* p_player = SLNK_NEXT(player_t, &core.players_head);
   while(p_player != NULL)
   {
      if (p_player->color == color)
      {
         break;
      }
      p_player = SLNK_NEXT(player_t, p_player);
   }
   return p_player;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void core_board_lots_mark(void)
{
   int i;
   switch (core.state)
   {
   case CORE_STATE_SETUP:
   { /* Mark free startup buildings. */
      for (i=0;i<16;i++)
      {
         block_t* p_blk = &core.board_blocks[startup_buildings[i].block];
         if (p_blk->n_buildings == 0)
         {
            p_blk->lots_marked = startup_buildings[i].mark;
         }
      }
      break;
   }
#if 0
   case CORE_STATE_ACTION_SPECIATION:
   { /* Mark valid tiles around selected element. */
      if (core.active_player->gene_pool > 0)
      {
         for (i=0;i<3;i++)
         {
            if ((core.speciation_tiles[i] < MAX_BOARD_TILES) &&
                (core.speciation_species[i] > 0))
            {
               tile_t* p_tile = &core.board_tiles[core.speciation_tiles[i]];
               p_tile->marked = TRUE;
            }
         }
      }
      break;
   }
#endif
   default:
      break;
   }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void core_board_lots_clear(void)
{
   int i;
   for (i=0;i<MAX_BOARD_BLOCKS;i++)
   {
      core.board_blocks[i].lots_marked = 0;
   }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void core_board_cards_mark(void)
{
   int i;
   switch (core.state)
   {
#if 0
   case CORE_STATE_ACTION_ABUNDANCE:
   { /* Mark all "empty" elements connected to at least one tile */
      for (i=0;i<MAX_BOARD_TILES;i++)
      {
         tile_t* p_tile = &core.board_tiles[i];
         if (p_tile->terrain > TERRAIN_NONE)
         {
            int j;
            for (j=0;j<6;j++)
            {
               element_t* p_element;
               p_element = core_find_element(p_tile->x + element_dir[j].x,
                  p_tile->y + element_dir[j].y);
               REQUIRE(p_element != NULL);
               if (p_element->element == ELEMENT_NONE)
               {
                  p_element->marked = TRUE;
                  TRC_DBG(core, "Marked element at %d,%d", p_element->x,
                     p_element->y);
               }
            }
         }
      }
      break;
   }
#endif
   default:
      break;
   }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void core_board_cards_clear(void)
{
   int i;
   for (i=0;i<MAX_BOARD_CARDS;i++)
   {
      core.board_cards_marked[i] = FALSE;
   }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
player_t* core_get_next_player(void)
{
   player_t* p_player = SLNK_NEXT(player_t, core.active_player);
   if (p_player == NULL)
   {
      p_player = SLNK_NEXT(player_t, &core.players_head);
   }
   return p_player;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void core_next_player(void)
{
   core.active_player = core_get_next_player();
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void core_dbg_dump_player_data(player_t* p_player)
{
   int i;
   REQUIRE(p_player != NULL);
   TRC_DBG(core, "Dumping player data for id %d (%s)", p_player->id,
      p_player->name);
   TRC_DBG(core, "color=%d", p_player->color);
   TRC_DBG(core, "ap=%d", p_player->ap);
   TRC_DBG(core, "wealth=%d", p_player->wealth);
   TRC_DBG(core, "prestige=%d", p_player->prestige);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void core_log(player_t* p_player, const char* p_fmt, ...)
{
   va_list ap;
   core.log_entry.p_player = p_player;
   va_start(ap, p_fmt);
   vsnprintf(core.log_entry.text, MAX_CORE_LOG_ENTRY-1, p_fmt, ap);
   va_end(ap);
   core_net_broadcast(NET_CMD_SERVER_LOG_ENTRY, NULL);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void core_net_send(int sock, int cmd, void* data)
{
   core.net_send(sock, cmd, data);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void core_net_broadcast(int cmd, void* data)
{
   core.net_broadcast(cmd, data);
}

/* LOCAL FUNCTIONS ***********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void core_prepare_players(void)
{
   player_t* p_player = SLNK_NEXT(player_t, &core.players_head);
   int n = 0;
   while(p_player != NULL)
   {
      int i;
      p_player->ap = 6;
      p_player->wealth = 30;
      //p_player->gene_pool_max = 35 + (6 - core.n_players)*5 - 1;
      p_player->prestige = 0;
      core_dbg_dump_player_data(p_player);
      core_net_broadcast(NET_CMD_SERVER_PLAYER_UPDATE, p_player);
      n++;
      p_player = SLNK_NEXT(player_t, p_player);
   }
}

#if 0
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static int core_compare_ascending(const void* a, const void* b)
{
  return ( *(uint8_t*)a - *(uint8_t*)b );
}
#endif

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static int core_compare_descending(const void* a, const void* b)
{
  return ( *(uint8_t*)b - *(uint8_t*)a );
}

#if 0
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static bool_t core_find_tiles_adjacent_with_species(tile_t* p_tile,
   player_t* p_player, bool_t mark)
{
   bool_t ret = FALSE;
   int i;
   for (i=0;i<6;i++)
   {
      tile_t* p_tile_tmp = core_find_tile(p_tile->x + tile_dir[i].x,
         p_tile->y + tile_dir[i].y);
      if (p_tile_tmp)
      {
         if (p_tile_tmp->species[p_player->animal] > 0)
         {
            if (mark)
            {
               p_tile_tmp->marked = TRUE;
            }
            else
            {
               ret = TRUE;
               break;
            }
         }
      }
   }
   return ret;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static int core_count_adjacent_tiles(tile_t* p_tile)
{
   int n = 0;
   int i;
   for (i=0;i<6;i++)
   {
      tile_t* p_tile_tmp = core_find_tile(p_tile->x + tile_dir[i].x,
         p_tile->y + tile_dir[i].y);
      if (p_tile_tmp)
      {
         if (p_tile_tmp->terrain > TERRAIN_NONE)
         {
            n++;
         }
      }
   }
   return n;
}
#endif

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static int core_calc_block_value(int block)
{
   int i;
   int cost = 0;
   for (i=0;i<6;i++)
   {
      if (core.prestige_markers[i].columns & (1u << (block%6)))
      {
         cost += i + 1;
      }
      if (core.prestige_markers[i].rows & (1u << (block/6)))
      {
         cost += i + 1;
      }
   }
   for (i=0;i<12;i++)
   {
      if (core.wealth_markers[i].columns & (1u << (block%6)))
      {
         cost += i + 1;
      }
      if (core.wealth_markers[i].rows & (1u << (block/6)))
      {
         cost += i + 1;
      }
   }
   return cost;
}


/* END OF FILE ***************************************************************/
