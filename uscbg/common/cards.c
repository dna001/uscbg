/******************************************************************************
Copyright (c) 2013, All Rights Reserved.
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file cards.c
\brief Expedition cards implementation. */
/*---------------------------------------------------------------------------*/
/* INCLUDE FILES *************************************************************/
#include "sys_def.h"
#include "sys_assert.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include "slnk.h"
#include "net_us.h"
#include "core.h"
#include "trc.h"

/* CONSTANTS / MACROS ********************************************************/
#define N_CARDS 26

/* LOCAL DATATYPES ***********************************************************/
typedef struct
{
   char* name;
   char* path;
   card_action_fn_t* use_fn;
   card_mark_fn_t* mark_fn;
   uint8_t done_allowed;
} card_res_t;

/* LOCAL FUNCTION PROTOTYPES *************************************************/
//static card_action_fn_t card_aquatic;
//static card_mark_fn_t card_aquatic_mark;

/* MODULE CONSTANTS / VARIABLES **********************************************/
SYS_ASSERT_FILE;
SYS_DBC_FILE;  /*!< Defines the name of this source file once for all */

TRC_EXT(core);

static const card_res_t cards_res[] = {
   {"None", NULL, NULL, NULL},
   //{"Aquatic", "data/Card_Aquatic.jpg", card_aquatic, card_aquatic_mark, 1},
};

/* GLOBAL CONSTANTS / VARIABLES **********************************************/
extern const uint8_t bonus_vp_tbl[];

/* GLOBAL FUNCTIONS **********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void cards_init(void)
{
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void cards_create_deck(slnk_t* p_deck)
{
   int i;
   for(i=0;i<CARD_ID_LAST-1;i++)
   {
      card_t* p_card = (card_t*)malloc(sizeof(card_t));
      REQUIRE(p_card != NULL);
      SLNK_INIT(p_card);
      p_card->id = i + 1;
      p_card->use = cards_res[i+1].use_fn;
      p_card->mark = cards_res[i+1].mark_fn;
      p_card->done_allowed = cards_res[i+1].done_allowed?TRUE:FALSE;
      SLNK_ADD(p_deck, p_card);
   }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void cards_free_deck(slnk_t* p_deck)
{
   card_t* p_card = SLNK_NEXT(card_t, p_deck);
   while(p_card != NULL)
   {
      card_t* p_tmp = p_card;
      p_card = SLNK_NEXT(card_t, p_card);
      SLNK_REMOVE(p_deck, p_tmp);
      free(p_tmp);
   }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void cards_shuffle_deck(slnk_t* p_deck)
{
   slnk_t deck_tmp;
   int n_cards = 0;
   card_t* p_card = SLNK_NEXT(card_t, p_deck);
   SLNK_INIT(&deck_tmp);
   /* Count cards in deck */
   while(p_card != NULL)
   {
      n_cards++;
      p_card = SLNK_NEXT(card_t, p_card);
   }
   if (n_cards > 1)
   {
      int i, r, n;
      int number = n_cards;
      for (i=--number; i>0; i--)
      {
         n = 0;
         r = (int)(((double)rand()/(double)RAND_MAX)*i);
         p_card = SLNK_NEXT(card_t, p_deck);
         while(p_card != NULL)
         {
            if (n == r)
               break;
            n++;
            p_card = SLNK_NEXT(card_t, p_card);
         }
         REQUIRE(n == r);
         SLNK_REMOVE(p_deck, p_card);
         SLNK_ADD(&deck_tmp, p_card);
      }
      *p_deck = deck_tmp;
      /* Put Ice Age card last */
      p_card = cards_draw(p_deck, CARD_ID_ICE_AGE);
      REQUIRE(p_card != NULL);
      SLNK_REMOVE(p_deck, p_card);
      SLNK_ADD(p_deck, p_card);
   }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void cards_merge_decks(slnk_t* p_dst_deck, slnk_t* p_src_deck)
{
   card_t* p_card = SLNK_NEXT(card_t, p_src_deck);
   while(p_card != NULL)
   {
      card_t* p_tmp = p_card;
      p_card = SLNK_NEXT(card_t, p_card);
      SLNK_REMOVE(p_src_deck, p_tmp);
      SLNK_ADD(p_dst_deck, p_tmp);
   }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
card_t* cards_draw(slnk_t* p_deck, int id)
{
   card_t* p_card = SLNK_NEXT(card_t, p_deck);
   while(p_card != NULL)
   {
      if ((id == -1) || (p_card->id == id))
      {
         SLNK_REMOVE(p_deck, p_card);
         break;
      }
      p_card = SLNK_NEXT(card_t, p_card);
   }
   return p_card;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
card_t* cards_find(slnk_t* p_deck, int id)
{
   card_t* p_card = SLNK_NEXT(card_t, p_deck);
   while(p_card != NULL)
   {
      if (p_card->id == id)
      {
         break;
      }
      p_card = SLNK_NEXT(card_t, p_card);
   }
   return p_card;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
card_action_t cards_use(card_t* p_card, card_evt_t evt)
{
   card_action_t action = CARD_ACTION_DONE;
   REQUIRE(p_card != NULL);
   if (p_card->use)
   {
      action = p_card->use(evt);
   }
   else
   {
      TRC_ERR(core, "Card %s not implemented", cards_get_name(p_card));
   }
   if (action == CARD_ACTION_DONE)
   { /* Remove card from board and put in discard deck */
      /*int i;
      for (i=0;i<5;i++)
      {
         card_t* p_card_board = core_get()->board_cards[i];
         if (p_card_board == p_card)
         {
            core_get()->board_cards[i] = NULL;
            SLNK_ADD(&core_get()->cards_discard_head, p_card);
            TRC_DBG(core, "card %s put in discard pile",
               cards_get_name(p_card));
            core_net_broadcast(NET_CMD_SERVER_BOARD_CARDS_UPDATE, NULL);
            break;
         }
      }*/
   }
   return action;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void cards_mark(card_t* p_card, card_mark_t mark)
{
   REQUIRE(p_card != NULL);
   if (p_card->mark)
   {
      p_card->mark(mark);
   }
   else
   {
      TRC_ERR(core, "Card %s does not have mark function",
         cards_get_name(p_card));
   }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
char* cards_get_name(card_t* p_card)
{
   return cards_res[p_card->id].name;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
char* cards_get_image_path(card_t* p_card)
{
   return cards_res[p_card->id].path;
}

/* LOCAL FUNCTIONS ***********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
#if 0
static card_action_t card_aquatic(card_evt_t evt)
{ /* Place an element of your choice onto any sea tile or wetland tile.
     Place up to 4 species from your gene pool onto that tile. */
   card_action_t action = CARD_ACTION_NEXT;
   player_t* p_player = core_get()->active_player;
   core_t* p_core = core_get();
   static int species_used = 0;

   switch (evt)
   {
   case CARD_EVT_START:
      species_used = 0;
      p_core->available_elements = core_get_available_bag_elements();
      core_net_send(p_player->id, NET_CMD_SERVER_SELECT_ELEMENT, NULL);
      break;
   case CARD_EVT_ELEMENT:
      core_net_send(p_player->id, NET_CMD_SERVER_SELECT_BOARD_ELEMENT, NULL);
      break;
   case CARD_EVT_BOARD_ELEMENT:
   {
      uint8_t board_element = p_core->board_element_selection;
      element_t* p_element = &p_core->board_elements[board_element];
      TRC_DBG(core, "%s put element %d at board element position (%d,%d)",
         p_player->name, p_core->element_selection,
         p_element->x, p_element->y);
      p_element->element = p_core->element_selection;
      core_net_broadcast(NET_CMD_SERVER_ELEMENT_UPDATE, p_element);
      core_net_send(p_player->id, NET_CMD_SERVER_SELECT_BOARD_TILE, NULL);
      break;
   }
   case CARD_EVT_BOARD_TILE:
   {
      tile_t* p_tile = &p_core->board_tiles[p_core->board_tile_selection];
      species_used++;
      p_player->gene_pool--;
      p_tile->species[p_player->animal]++;
      p_tile->used = 1;
      core_net_broadcast(NET_CMD_SERVER_TILE_UPDATE, p_tile);
      core_net_broadcast(NET_CMD_SERVER_PLAYER_UPDATE, p_player);
      core_log(p_player, "added 1 species to tile (%d,%d)", p_tile->x,
         p_tile->y);
      TRC_DBG(core, "%s added 1 species to tile (%d,%d)",
         p_player->name, p_tile->x, p_tile->y);
      if ((species_used < 4) && (p_player->gene_pool > 0))
      {
         core_net_send(p_player->id, NET_CMD_SERVER_SELECT_BOARD_TILE, NULL);
      }
      else
      {
         action = CARD_ACTION_DONE;
      }
      break;
   }
   case CARD_EVT_DONE:
      action = CARD_ACTION_DONE;
      break;
   default:
      break;
   }
   if (action == CARD_ACTION_DONE)
   { /* Clear used tile */
      tile_t* p_tile = &p_core->board_tiles[p_core->board_tile_selection];
      p_tile->used = 0;
      core_net_broadcast(NET_CMD_SERVER_TILE_UPDATE, p_tile);
   }
   return action;
}
#endif

/* END OF FILE ***************************************************************/
