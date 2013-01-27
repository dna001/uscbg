/******************************************************************************
Copyright (c) 2013, All Rights Reserved.
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file gfw.h
\brief The Game Frame Work interface. */
/*---------------------------------------------------------------------------*/
#ifndef GFW_H
#define GFW_H
/* INCLUDE FILES *************************************************************/
#include <SDL/SDL.h>

/* EXPORTED DEFINES **********************************************************/
#define GFW_EVT_TYPE_TIMER   1
#define GFW_EVT_TYPE_GUI     2
#define GFW_EVT_TYPE_NET     3
#define GFW_EVT_TYPE_USER    10

/* EXPORTED DATA TYPES *******************************************************/
typedef SDL_Event gfw_evt_t;
typedef void gfw_update_fn_t(long time);
typedef void gfw_draw_fn_t(void);
typedef void gfw_key_evt_fn_t(gfw_evt_t* p_evt);
typedef void gfw_mouse_evt_fn_t(gfw_evt_t* p_evt);
typedef void gfw_user_evt_fn_t(gfw_evt_t* p_evt);
typedef void gfw_user_poll_fn_t(void);

typedef void gfw_evt_cb_fn_t(int type, void* p_data);

/* GLOBAL VARIABLES **********************************************************/

/* INTERFACE FUNCTIONS *******************************************************/
typedef struct
{
   gfw_update_fn_t* on_update;
   gfw_draw_fn_t* on_draw;
   gfw_key_evt_fn_t* on_key;
   gfw_mouse_evt_fn_t* on_mouse;
   gfw_user_evt_fn_t* on_user;
   gfw_user_poll_fn_t* on_poll;
} gfw_cb_t;

/*---------------------------------------------------------------------------*/
/*! \brief Initialize gfw. */
/*---------------------------------------------------------------------------*/
void gfw_init(void);

/*---------------------------------------------------------------------------*/
/*! \brief Creates a windows or drawing area of specified size. */
/*---------------------------------------------------------------------------*/
SDL_Surface* gfw_create_window(
   char* title,          /*!< Title of window (if applicable) */
   int w,                /*!< Width of the window */
   int h,                /*!< Height of the window */
   unsigned long flags   /*!< Various configuration flags */
   );

/*---------------------------------------------------------------------------*/
/*! \brief Attach callback functions */
/*---------------------------------------------------------------------------*/
void gfw_cb_attach(
   gfw_cb_t* p_cb    /*!< Functions to attach */
   );

/*---------------------------------------------------------------------------*/
/*! \brief Main Loop */
/*---------------------------------------------------------------------------*/
int gfw_main_loop(void);

/*---------------------------------------------------------------------------*/
/*! \brief Register callback function for event type */
/*---------------------------------------------------------------------------*/
void gfw_evt_attach_cb(
   int type,              /*!< Event type */
   gfw_evt_cb_fn_t* p_fn  /*!< Callback function */
   );

/*---------------------------------------------------------------------------*/
/*! \brief Post timer event */
/*---------------------------------------------------------------------------*/
void gfw_tmr_evt(
   unsigned long time     /*!< Time in ms before posting timer event */
   );

#if 0
/*---------------------------------------------------------------------------*/
/*! \brief Optional callback function used by application */
/*---------------------------------------------------------------------------*/
void gfw_evt(
   gfw_evt_t evt,        /*!< Type of event */
   unsigned long p1,     /*!< Parameter 1 */
   unsigned long p2      /*!< Parameter 2 */
   );
#endif

/*---------------------------------------------------------------------------*/
/*! \brief Post event function */
/*---------------------------------------------------------------------------*/
void gfw_post_evt(
   uint8_t type,             /*!< Event type */
   void* p_data              /*!< Data */
   );

#endif /* #ifndef GFW_H */
/* END OF FILE ***************************************************************/
