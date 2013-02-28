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
//#define N_CARDS 26
#define Z_ALL 0x0f
#define Z_CIV (1u << ZONE_CIV)
#define Z_COM (1u << ZONE_COM)
#define Z_IND (1u << ZONE_IND)
#define Z_RES (1u << ZONE_RES)

/* LOCAL DATATYPES ***********************************************************/
typedef struct
{
   char* img_path;
   uint8_t zones;
   uint8_t n_permits;
   uint8_t payout; /* Wealth payout */
   bool_t election;
   uint8_t n;
} card_build_permit_res_t;

typedef struct
{
   char* name;
   char* img_path;
   uint8_t zone;
   uint8_t size;
   uint8_t payout; /* Prestige payout */
   uint8_t vocation;
   uint8_t vocation_value;
   void* script_fn;
} card_contract_res_t;

typedef struct
{
   char* img_path;
   card_action_fn_t* use_fn;
   card_mark_fn_t* mark_fn;
   uint8_t done_allowed;
} card_event_res_t;

/* LOCAL FUNCTION PROTOTYPES *************************************************/
static void cards_create_planning_deck(slnk_t* p_deck);
static void cards_create_contract_deck(slnk_t* p_deck, card_deck_t deck);
//static card_action_fn_t card_aquatic;
//static card_mark_fn_t card_aquatic_mark;

/* MODULE CONSTANTS / VARIABLES **********************************************/
SYS_ASSERT_FILE;
SYS_DBC_FILE;  /*!< Defines the name of this source file once for all */

TRC_EXT(core);

static const card_build_permit_res_t cards_build_permit_res[] = {
   {"data/Urb_card_Plan01_FINAL.png", Z_ALL, 1, 7, TRUE, 3},
   {"data/Urb_card_Plan04_FINAL.png", Z_CIV | Z_COM | Z_RES, 1, 8, TRUE, 3},
   {"data/Urb_card_Plan07_FINAL.png", Z_CIV | Z_RES, 1, 11, TRUE, 1},
   {"data/Urb_card_Plan08_FINAL.png", Z_RES, 1, 11, TRUE, 2},
   {"data/Urb_card_Plan10_FINAL.png", Z_RES, 1, 12, TRUE, 3},
   {"data/Urb_card_Plan13_FINAL.png", Z_ALL, 2, 4, FALSE, 3},
   {"data/Urb_card_Plan16_FINAL.png", Z_CIV | Z_COM | Z_RES, 2, 5, FALSE, 3},
   {"data/Urb_card_Plan19_FINAL.png", Z_CIV | Z_COM, 2, 9, FALSE, 1},
   {"data/Urb_card_Plan20_FINAL.png", Z_COM, 2, 9, FALSE, 2},
   {"data/Urb_card_Plan22_FINAL.png", Z_COM, 2, 10, FALSE, 3},
   {"data/Urb_card_Plan25_FINAL.png", Z_ALL, 3, 2, TRUE, 2},
   {"data/Urb_card_Plan27_FINAL.png", Z_CIV | Z_COM | Z_IND, 3, 3, TRUE, 3},
   {"data/Urb_card_Plan30_FINAL.png", Z_COM | Z_IND, 3, 6, TRUE, 2},
   {"data/Urb_card_Plan32_FINAL.png", Z_ALL, 4, 1, FALSE, 2},
   {"data/Urb_card_Plan34_FINAL.png", Z_CIV | Z_COM | Z_IND, 4, 1, FALSE, 1},
   {"data/Urb_card_Plan35_FINAL.png", Z_CIV | Z_COM | Z_IND, 4, 2, FALSE, 1},
   {"data/Urb_card_Plan36_FINAL.png", Z_CIV, 4, 6, TRUE, 1},
   {NULL, 0, 0, 0, FALSE, 0}
};

static const card_contract_res_t cards_town_contracts_res[] = {
   {"Visitor's Center", "data/Urb_card_Town01_FINAL.png", ZONE_CIV, 1, 0,
    VOCATION_PUBLIC_SERVICE, 1, NULL},
   {"Clinic", "data/Urb_card_Town02_FINAL.png", ZONE_CIV, 1, 1,
    VOCATION_PUBLIC_SERVICE, 1, NULL},
   {"Grade School", "data/Urb_card_Town03_FINAL.png", ZONE_CIV, 2, 2,
    VOCATION_EDUCATION, 2, NULL},
   {"High School", "data/Urb_card_Town04_FINAL.png", ZONE_CIV, 2, 3,
    VOCATION_EDUCATION, 3, NULL},
   {"Court House", "data/Urb_card_Town05_FINAL.png", ZONE_CIV, 2, 0,
    VOCATION_NONE, 0, NULL},
   {"Post Office", "data/Urb_card_Town06_FINAL.png", ZONE_CIV, 1, 0,
    VOCATION_PUBLIC_SERVICE, 1, NULL},
   {"Police Station", "data/Urb_card_Town07_FINAL.png", ZONE_CIV, 1, 0,
    VOCATION_PUBLIC_SERVICE, 1, NULL},
   {"Fire Station", "data/Urb_card_Town08_FINAL.png", ZONE_CIV, 1, 0,
    VOCATION_PUBLIC_SERVICE, 1, NULL},
   {"Truck Stop", "data/Urb_card_Town09_FINAL.png", ZONE_COM, 2, 0,
    VOCATION_NONE, 0, NULL},
   {"Bakery", "data/Urb_card_Town10_FINAL.png", ZONE_COM, 2, 1,
    VOCATION_NONE, 0, NULL},
   {"Barber Shop", "data/Urb_card_Town11_FINAL.png", ZONE_COM, 1, 2,
    VOCATION_NONE, 0, NULL},
   {"Restaurant", "data/Urb_card_Town12_FINAL.png", ZONE_COM, 2, 3,
    VOCATION_NONE, 0, NULL},
   {"Gas Station", "data/Urb_card_Town13_FINAL.png", ZONE_COM, 1, 0,
    VOCATION_ENERGY, 2, NULL},
   {"Grocery Store", "data/Urb_card_Town14_FINAL.png", ZONE_COM, 2, 0,
    VOCATION_NONE, 0, NULL},
   {"Hardware Store", "data/Urb_card_Town15_FINAL.png", ZONE_COM, 2, 0,
    VOCATION_NONE, 0, NULL},
   {"Motel", "data/Urb_card_Town16_FINAL.png", ZONE_COM, 2, 0,
    VOCATION_TOURISM, 1, NULL},
   {"News Paper", "data/Urb_card_Town17_FINAL.png", ZONE_COM, 1, 0,
    VOCATION_MEDIA, 0, NULL},
   {"Pawn Shop", "data/Urb_card_Town18_FINAL.png", ZONE_COM, 1, 0,
    VOCATION_FINANCE, 2, NULL},
   {"Bank", "data/Urb_card_Town19_FINAL.png", ZONE_COM, 2, 0,
    VOCATION_FINANCE, 4, NULL},
   {"Textile Mill", "data/Urb_card_Town20_FINAL.png", ZONE_IND, 2, 1,
    VOCATION_FACTORY, 2, NULL},
   {"Gravel Pit", "data/Urb_card_Town21_FINAL.png", ZONE_IND, 3, 2,
    VOCATION_FACTORY, 0, NULL},
   {"Dam", "data/Urb_card_Town22_FINAL.png", ZONE_IND, 1, 3,
    VOCATION_ENERGY, 1, NULL},
   {"Printer", "data/Urb_card_Town23_FINAL.png", ZONE_IND, 1, 0,
    VOCATION_FACTORY, 2, NULL},
   {"Kindergarten", "data/Urb_card_Town24_FINAL.png", ZONE_RES, 1, 0,
    VOCATION_EDUCATION, 1, NULL},
   {"Apartments", "data/Urb_card_Town25_FINAL.png", ZONE_RES, 2, 1,
    VOCATION_NONE, 0, NULL},
   {"Mobile Home Park", "data/Urb_card_Town26_FINAL.png", ZONE_RES, 3, 2,
    VOCATION_NONE, 0, NULL},
   {"Ranch", "data/Urb_card_Town27_FINAL.png", ZONE_RES, 4, 3,
    VOCATION_NONE, 0, NULL},
   {"Church", "data/Urb_card_Town28_FINAL.png", ZONE_RES, 1, 1,
    VOCATION_NONE, 0, NULL},
   {"Villa", "data/Urb_card_Town29_FINAL.png", ZONE_RES, 1, 2,
    VOCATION_NONE, 0, NULL},
   {"Duplex", "data/Urb_card_Town30_FINAL.png", ZONE_RES, 1, 3,
    VOCATION_NONE, 0, NULL},
   {"House", "data/Urb_card_Town31_FINAL.png", ZONE_RES, 1, 6,
    VOCATION_NONE, 0, NULL},
   {"House", "data/Urb_card_Town32_FINAL.png", ZONE_RES, 1, 6,
    VOCATION_NONE, 0, NULL},
   {"House", "data/Urb_card_Town33_FINAL.png", ZONE_RES, 1, 6,
    VOCATION_NONE, 0, NULL},
   {"House", "data/Urb_card_Town34_FINAL.png", ZONE_RES, 1, 6,
    VOCATION_NONE, 0, NULL},
   {"House", "data/Urb_card_Town35_FINAL.png", ZONE_RES, 1, 6,
    VOCATION_NONE, 0, NULL},
   {"Mobile Home", "data/Urb_card_Town36_FINAL.png", ZONE_RES, 1, 0,
    VOCATION_NONE, 0, NULL},
   {"Airport", "data/Urb_card_Town37_FINAL.png", 0, 0, 0,
    VOCATION_NONE, 0, NULL},
   {NULL, NULL, 0, 0, 0, 0, 0, NULL}
};

/* GLOBAL CONSTANTS / VARIABLES **********************************************/
extern const uint8_t bonus_vp_tbl[];

/* GLOBAL FUNCTIONS **********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void cards_init(void)
{
}

void cards_create_deck(slnk_t* p_deck, card_deck_t deck)
{
   if (deck == CARD_DECK_PLANNING)
   {
      cards_create_planning_deck(p_deck);
   }
   else
   {
      cards_create_contract_deck(p_deck, deck);
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
      p_card = cards_draw(p_deck, -1);
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
#if 0
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
#endif
   return action;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void cards_mark(card_t* p_card, card_mark_t mark)
{
   REQUIRE(p_card != NULL);
#if 0
   if (p_card->mark)
   {
      p_card->mark(mark);
   }
   else
   {
      TRC_ERR(core, "Card %s does not have mark function",
         cards_get_name(p_card));
   }
#endif
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
char* cards_get_image_path(card_t* p_card)
{
   //return cards_res[p_card->id].path;
   return NULL;
}

/* LOCAL FUNCTIONS ***********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void cards_create_planning_deck(slnk_t* p_deck)
{
   const card_build_permit_res_t* p_res = &cards_build_permit_res[0];
   card_planning_t* p_card;
   int id = 1;
   int i;

   while (p_res->img_path != NULL)
   {
      for (i=0;i<p_res->n;i++)
      {
         p_card = (card_planning_t*)malloc(sizeof(card_planning_t));
         REQUIRE(p_card != NULL);
         SLNK_INIT(p_card);
         p_card->card.id = id;
         p_card->card.deck = CARD_DECK_PLANNING;
         p_card->card.img_path = p_res->img_path;
         p_card->type = CARD_TYPE_BUILD_PERMIT;
         p_card->zones = p_res->zones;
         p_card->n_permits = p_res->n_permits;
         p_card->payout = p_res->payout;
         p_card->election = p_res->election;
         SLNK_ADD(p_deck, p_card);
         id++;
      }
      p_res++;
   }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void cards_create_contract_deck(slnk_t* p_deck, card_deck_t deck)
{
   const card_contract_res_t* p_res;
   card_contract_t* p_card;
   int id = 1;

   switch (deck)
   {
   case CARD_DECK_TOWN:
      p_res = cards_town_contracts_res;
      break;
   case CARD_DECK_CITY:
      //p_res = cards_city_contracts_res;
      break;
   case CARD_DECK_METROPOLIS:
      //p_res = cards_metropolis_contracts_res;
      break;
   default:
      ASSERT(TRUE);
      break;
   }
   while (p_res->img_path != NULL)
   {
      p_card = (card_contract_t*)malloc(sizeof(card_contract_t));
      REQUIRE(p_card != NULL);
      SLNK_INIT(p_card);
      p_card->card.id = id;
      p_card->card.deck = deck;
      p_card->card.img_path = p_res->img_path;
      p_card->type = (p_res->size > 0)?CARD_TYPE_CONTRACT:CARD_TYPE_EVENT;
      p_card->name = p_res->name;
      p_card->zone = p_res->zone;
      p_card->size = p_res->size;
      p_card->payout = p_res->payout;
      p_card->vocation = p_res->vocation;
      p_card->vocation_value = p_res->vocation_value;
      SLNK_ADD(p_deck, p_card);
      id++;
      p_res++;
   }
}

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
