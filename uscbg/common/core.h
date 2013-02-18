/******************************************************************************
Copyright (c) 2013, All Rights Reserved.
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file core.h
\brief The Core game logic interface for Urban Sprawl. */
/*---------------------------------------------------------------------------*/
#ifndef CORE_H
#define CORE_H
/* INCLUDE FILES *************************************************************/
#include "cards.h"

/* EXPORTED DEFINES **********************************************************/
#define MAX_NAME_LENGTH (40)
#define MAX_FILE_NAME_LENGTH (40)
#define MAX_PATH_LEN (255)
#define MAX_CORE_LOG_ENTRY (128)
#define MAX_BOARD_BLOCKS (36)
#define MAX_BOARD_LOTS (36*4)
#define MAX_BOARD_CARDS (13)

/* EXPORTED DATA TYPES *******************************************************/
typedef void core_net_send_fn_t(int sock, int cmd, void* data);
typedef void core_net_broadcast_fn_t(int cmd, void* data);

typedef struct
{
   int x;
   int y;
} coords_t;

typedef enum
{
   CORE_STATE_NONE = 0,
   CORE_STATE_SETUP,
   CORE_STATE_INVESTMENTS,
   CORE_STATE_ACTIONS,
   CORE_STATE_ACTION_SPEND_AP,
   CORE_STATE_ACTION_TAKE_CARD,
   CORE_STATE_ACTION_BUILD,
   CORE_STATE_ACTION_FAVOR,
   CORE_STATE_ACTION_END_OF_TURN,
   CORE_STATE_ACTION_END_OF_TURN_EVENT,
   CORE_STATE_RESET,
   CORE_STATE_GAME_END,
   CORE_STATE_LAST
} core_state_t;

/*typedef enum
{
   CORE_ACTION_INITIATIVE = 0,
   CORE_ACTION_ADAPTATION,
   CORE_ACTION_LAST
} core_action_t;*/

typedef enum
{
   ZONE_CIV = 0,
   ZONE_COM,
   ZONE_IND,
   ZONE_RES,
   ZONE_PARK,
   ZONE_LAST
} zone_t;

typedef enum
{
   VOCATION_FINANCE = 0,
   VOCATION_TRANSPORTATION,
   VOCATION_EDUCATION,
   VOCATION_ENERGY,
   VOCATION_TOURISM,
   VOCATION_FACTORY,
   VOCATION_PUBLIC_SERVICE,
   VOCATION_MEDIA,
   VOCATION_NONE,
   VOCATION_LAST = VOCATION_NONE
} vocation_t;

typedef enum
{
   POLITICIAN_MAYOR = 0,
   POLITICIAN_DISTRICT_ATTORNEY,
   POLITICIAN_TREASURER,
   POLITICIAN_POLICE_CHIEF,
   POLITICIAN_UNION_BOSS,
   POLITICIAN_CONTRACTOR, /* Not a politician, but here for convenience */
   POLITICIAN_LAST
} politician_t;

typedef enum
{
   PLAYER_COLOR_BLACK = 0,
   PLAYER_COLOR_GREEN,
   PLAYER_COLOR_PINK,
   PLAYER_COLOR_WHITE,
   PLAYER_COLOR_LAST
} player_color_t;

typedef struct
{
   zone_t zone;
   uint8_t size;
   uint8_t owner;
   uint8_t block_pos; /* Position of complete building in block (bits 0-3) */
} building_t;

typedef struct
{
   uint8_t id;
   building_t buildings[4];
   uint8_t n_buildings;
   uint32_t value;
   uint8_t lots_marked; /* Bits 0-3 */
} block_t;

typedef struct
{
   int x;
   int y;
   uint8_t rows;
   uint8_t columns;
} prestige_wealth_marker_t;

typedef struct
{
   slnk_t slnk;
   int id;
   char name[MAX_NAME_LENGTH];
   uint8_t color;
   slnk_t cards_head;
   slnk_t favor_head;
   uint8_t ap;
   uint8_t politicians;
   uint32_t vocations; /* Bits 0-23 */
   uint32_t wealth;
   uint32_t prestige;
   bool_t passed;
} player_t;

typedef struct
{
   player_t* p_player;
   char text[MAX_CORE_LOG_ENTRY];
} core_log_entry_t;

typedef struct
{
   bool_t is_server;
   slnk_t players_head;
   int n_players;
   slnk_t planning_deck_head;
   slnk_t planning_discard_head;
   slnk_t town_deck_head;
   slnk_t town_discard_head;
   slnk_t city_deck_head;
   slnk_t city_discard_head;
   slnk_t metropolis_deck_head;
   slnk_t metropolis_discard_head;
   card_t* board_planning_cards[5];
   card_t* board_contract_cards[8];
   bool_t board_cards_marked[MAX_BOARD_CARDS];
   uint8_t state;
   uint32_t board_vocations;
   block_t board_blocks[MAX_BOARD_BLOCKS];
   uint8_t board_election_track[5];
   prestige_wealth_marker_t prestige_markers[6];
   prestige_wealth_marker_t wealth_markers[12];
   player_t* active_player;
   uint8_t current_round;
   uint8_t available_colors;
   card_contract_t* current_contract_card;
   card_planning_t* current_planning_cards[5];
   bool_t last_round;
   int startup_buildings;
   core_net_send_fn_t* net_send;
   core_net_broadcast_fn_t* net_broadcast;
/* Temporary storage for net events etc */
   uint8_t board_pos_x;
   uint8_t board_pos_y;
   core_log_entry_t log_entry;
   uint8_t color_selection;
   uint8_t action_selection;
   uint8_t card_selection;
   uint8_t board_lot_selection;
} core_t;

/* GLOBAL VARIABLES **********************************************************/

/* INTERFACE FUNCTIONS *******************************************************/

/*---------------------------------------------------------------------------*/
/*! \brief Initialize. */
/*---------------------------------------------------------------------------*/
void core_init(
   core_net_send_fn_t* p_fn_send,
   core_net_broadcast_fn_t* p_fn_bc
   );

/*---------------------------------------------------------------------------*/
/*! \brief Get a pointer to the core_t struct. */
/*---------------------------------------------------------------------------*/
core_t* core_get(void);

/*---------------------------------------------------------------------------*/
/*! \brief Free resources. */
/*---------------------------------------------------------------------------*/
void core_free(void);

/*---------------------------------------------------------------------------*/
/*! \brief Initialize new game. */
/*---------------------------------------------------------------------------*/
bool_t core_newgame(
   bool_t load          /*!< New game/Load game */
   );

/*---------------------------------------------------------------------------*/
/*! \brief Select color for active player. */
/*---------------------------------------------------------------------------*/
void core_select_color(void);

/*---------------------------------------------------------------------------*/
/*! \brief Invest (discard planning card(s) for wealth). */
/*---------------------------------------------------------------------------*/
void core_invest(void);

/*---------------------------------------------------------------------------*/
/*! \brief Prepare next action, round or game end. */
/*---------------------------------------------------------------------------*/
core_state_t core_next_action(void);

/*---------------------------------------------------------------------------*/
/*! \brief Handle action done. */
/*---------------------------------------------------------------------------*/
void core_action_done(void);

/*---------------------------------------------------------------------------*/
/*! \brief Handle adaptation action. */
/*---------------------------------------------------------------------------*/
void core_action_take_card(void);

/*---------------------------------------------------------------------------*/
/*! \brief Handle regression action (active player). */
/*---------------------------------------------------------------------------*/
void core_action_build(void);

/*---------------------------------------------------------------------------*/
/*! \brief Prepare new round. */
/*---------------------------------------------------------------------------*/
void core_prepare_new_round(void);

/*---------------------------------------------------------------------------*/
/*! \brief Use selected card. */
/*---------------------------------------------------------------------------*/
void core_use_card(void);

/*---------------------------------------------------------------------------*/
/*! \brief Calculate final vp. */
/*---------------------------------------------------------------------------*/
void core_calculate_final_vp(void);

/*---------------------------------------------------------------------------*/
/*! \brief Find block based on coordinates. */
/*---------------------------------------------------------------------------*/
block_t* core_find_block(
   int x,               /*!< x coordinate */
   int y                /*!< y coordinate */
   );

/*---------------------------------------------------------------------------*/
/*! \brief Save game. */
/*---------------------------------------------------------------------------*/
bool_t core_savegame(
   char* name           /*!< Name of save game. */
   );

/*---------------------------------------------------------------------------*/
/*! \brief Load a saved game. */
/*---------------------------------------------------------------------------*/
bool_t core_loadgame(
   char* name           /*!< Name of saved game. */
   );

/*---------------------------------------------------------------------------*/
/*! \brief Add new player. */
/*---------------------------------------------------------------------------*/
void core_add_player(
   player_t* p_player   /*!< Player */
   );

/*---------------------------------------------------------------------------*/
/*! \brief Remove player. */
/*---------------------------------------------------------------------------*/
void core_rm_player(
   player_t* p_player   /*!< Player */
   );

/*---------------------------------------------------------------------------*/
/*! \brief Find player. */
/*---------------------------------------------------------------------------*/
player_t* core_find_player(
   int id               /*!< Player id (0 = this player) */
   );

/*---------------------------------------------------------------------------*/
/*! \brief Find player by name. */
/*---------------------------------------------------------------------------*/
player_t* core_find_player_by_name(
   char* name           /*!< Name */
   );

/*---------------------------------------------------------------------------*/
/*! \brief Find player by animal. */
/*---------------------------------------------------------------------------*/
player_t* core_find_player_by_color(
   int color            /*!< Color */
   );

/*---------------------------------------------------------------------------*/
/*! \brief Get next player. */
/*---------------------------------------------------------------------------*/
player_t* core_get_next_player(void);

/*---------------------------------------------------------------------------*/
/*! \brief Set next player as active player. */
/*---------------------------------------------------------------------------*/
void core_next_player(void);

/*---------------------------------------------------------------------------*/
/*! \brief Mark valid lots on game board. */
/*---------------------------------------------------------------------------*/
void core_board_lots_mark(void);

/*---------------------------------------------------------------------------*/
/*! \brief Clear marked lots on game board. */
/*---------------------------------------------------------------------------*/
void core_board_lots_clear(void);

/*---------------------------------------------------------------------------*/
/*! \brief Mark valid cards on game board. */
/*---------------------------------------------------------------------------*/
void core_board_cards_mark(void);

/*---------------------------------------------------------------------------*/
/*! \brief Clear marked cards. */
/*---------------------------------------------------------------------------*/
void core_board_cards_clear(void);

/*---------------------------------------------------------------------------*/
/*! \brief Convert vocation bit to vocation. */
/*---------------------------------------------------------------------------*/
vocation_t core_vocbit2voc(
   uint32_t vocation_bit
   );

/*---------------------------------------------------------------------------*/
/*! \brief Dump player data. */
/*---------------------------------------------------------------------------*/
void core_dbg_dump_player_data(
   player_t* p_player   /*!< Player */
   );

/*---------------------------------------------------------------------------*/
/*! \brief Add log entry. */
/*---------------------------------------------------------------------------*/
void core_log(
   player_t* p_player,  /*!< Player */
   const char* p_fmt,   /*!< Pointer to a format string */
   ...                  /*!< Variable argument list */
   );

/*---------------------------------------------------------------------------*/
/*! \brief Send net command. */
/*---------------------------------------------------------------------------*/
void core_net_send(
   int sock,
   int cmd,
   void* data
   );

/*---------------------------------------------------------------------------*/
/*! \brief Broadcast net command. */
/*---------------------------------------------------------------------------*/
void core_net_broadcast(
   int cmd,
   void* data
   );

#endif /* #ifndef CORE_H */
/* END OF FILE ***************************************************************/
