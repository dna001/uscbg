/******************************************************************************
Copyright (c) 2013, All Rights Reserved.
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file cards.h
\brief The cards interface. */
/*---------------------------------------------------------------------------*/
#ifndef CARDS_H
#define CARDS_H
/* INCLUDE FILES *************************************************************/

/* EXPORTED DEFINES **********************************************************/

/* EXPORTED DATA TYPES *******************************************************/
typedef enum
{
   CARD_ID_NONE = 0,
   CARD_ID_AQUATIC,
   CARD_ID_BIODIVERSITY,
   CARD_ID_BIOMASS,
   CARD_ID_BLIGHT,
   CARD_ID_CATASTROPHE,
   CARD_ID_COLDSNAP,
   CARD_ID_DISEASE,
   CARD_ID_ECODIVERSITY,
   CARD_ID_EVOLUTION,
   CARD_ID_FECUNDITY,
   CARD_ID_FERTILE,
   CARD_ID_HABITAT,
   CARD_ID_HIBERNATION,
   CARD_ID_ICE_AGE,
   CARD_ID_ICE_SHEET,
   CARD_ID_IMMIGRANTS,
   CARD_ID_INSTINCT,
   CARD_ID_INTELLIGENCE,
   CARD_ID_MASSEXODUS,
   CARD_ID_METAMORPHOSIS,
   CARD_ID_NICHE_BIOMES,
   CARD_ID_NOCTURNAL,
   CARD_ID_OMNIVORE,
   CARD_ID_PARASITISM,
   CARD_ID_PREDATOR,
   CARD_ID_SYMBIOTIC,
   CARD_ID_LAST
} card_id_t;

typedef enum
{
   CARD_ACTION_DONE = 0,
   CARD_ACTION_NEXT,
   CARD_ACTION_SELECT_ANIMAL,
   CARD_ACTION_SELECT_ELEMENT_BOX,
   CARD_ACTION_SELECT_BOARD_TILE,
   CARD_ACTION_SELECT_BOARD_ELEMENT,
   CARD_ACTION_SELECT_ELEMENT,
   CARD_ACTION_LAST
} card_action_t;

typedef enum
{
   CARD_EVT_START = 0,
   CARD_EVT_PLACE_ACTION_PAWN,
   CARD_EVT_ANIMAL,
   CARD_EVT_ELEMENT_BOX,
   CARD_EVT_BOARD_TILE,
   CARD_EVT_BOARD_ELEMENT,
   CARD_EVT_ELEMENT,
   CARD_EVT_CARD_CHOICE,
   CARD_EVT_DONE,
   CARD_EVT_LAST
} card_evt_t;

typedef enum
{
   CARD_MARK_BOARD_TILES = 0,
   CARD_MARK_BOARD_ELEMENTS,
   CARD_MARK_LAST
} card_mark_t;

typedef card_action_t card_action_fn_t(card_evt_t evt);
typedef void card_mark_fn_t(card_mark_t mark);

typedef struct
{
   slnk_t slnk;
   card_id_t id;     /*!< ID of card */
   card_action_fn_t* use;
   card_mark_fn_t* mark;
   bool_t done_allowed;
} card_t;

/* GLOBAL VARIABLES **********************************************************/

/* INTERFACE FUNCTIONS *******************************************************/

/*---------------------------------------------------------------------------*/
/*! \brief Initialize. */
/*---------------------------------------------------------------------------*/
void cards_init(void);

/*---------------------------------------------------------------------------*/
/*! \brief Create new card deck. */
/*---------------------------------------------------------------------------*/
void cards_create_deck(
   slnk_t* p_deck        /*!< List head of deck */
   );

/*---------------------------------------------------------------------------*/
/*! \brief Free card deck. */
/*---------------------------------------------------------------------------*/
void cards_free_deck(
   slnk_t* p_deck        /*!< List head of deck */
   );

/*---------------------------------------------------------------------------*/
/*! \brief Shuffle cards in deck. */
/*---------------------------------------------------------------------------*/
void cards_shuffle_deck(
   slnk_t* p_deck        /*!< List head of deck */
   );

/*---------------------------------------------------------------------------*/
/*! \brief Merge cards in decks. */
/*---------------------------------------------------------------------------*/
void cards_merge_decks(
   slnk_t* p_dst_deck,   /*!< List head of destination deck */
   slnk_t* p_src_deck    /*!< List head of source deck */
   );

/*---------------------------------------------------------------------------*/
/*! \brief Draw any (-1) or specified card from deck. */
/*---------------------------------------------------------------------------*/
card_t* cards_draw(
   slnk_t* p_deck,       /*!< List head of deck */
   int id                /*!< Id of card or -1 for any */
   );

/*---------------------------------------------------------------------------*/
/*! \brief Find card in deck by id. */
/*---------------------------------------------------------------------------*/
card_t* cards_find(
   slnk_t* p_deck,       /*!< List head of deck */
   int id                /*!< Id of card */
   );

/*---------------------------------------------------------------------------*/
/*! \brief Use card. */
/*---------------------------------------------------------------------------*/
card_action_t cards_use(
   card_t* p_card,       /*!< Card */
   card_evt_t evt        /*!< Event */
   );

/*---------------------------------------------------------------------------*/
/*! \brief Count cards in deck. */
/*---------------------------------------------------------------------------*/
int cards_count(
   slnk_t* p_deck        /*!< List head of deck */
   );

/*---------------------------------------------------------------------------*/
/*! \brief Handle mark for different card actions. */
/*---------------------------------------------------------------------------*/
void cards_mark(
   card_t* p_card,       /*!< Card */
   card_mark_t mark      /*!< What to mark */
   );

/*---------------------------------------------------------------------------*/
/*! \brief Get card name. */
/*---------------------------------------------------------------------------*/
char* cards_get_name(
   card_t* p_card        /*!< Card */
   );

/*---------------------------------------------------------------------------*/
/*! \brief Get card image path. */
/*---------------------------------------------------------------------------*/
char* cards_get_image_path(
   card_t* p_card        /*!< Card */
   );

#endif /* #ifndef CARDS_H */
/* END OF FILE ***************************************************************/
