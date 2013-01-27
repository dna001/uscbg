/******************************************************************************
Copyright (c) 2013, All Rights Reserved.
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file gui.h
\brief The gui interface. */
/*---------------------------------------------------------------------------*/
#ifndef GUI_H
#define GUI_H
/* INCLUDE FILES *************************************************************/
#include "gfw.h"
#include "glx.h"

/* EXPORTED DEFINES **********************************************************/
#define GUI_WND_DO_NOT_CREATE_SURFACE (1u)
#define GUI_EVENT_SELECT "select"
#define GUI_EVENT_CHANGE "change"
#define GUI_EVENT_USER "user"

#define MAX_WIDGET_NAME_LEN 80

/* EXPORTED DATA TYPES *******************************************************/
typedef struct gui_wnd gui_wnd_t;
typedef struct gui_widget gui_widget_t;

typedef void gui_wnd_set_cfg_fn_t(gui_wnd_t* p_me, char* cfg, void* data);
typedef void* gui_wnd_get_cfg_fn_t(gui_wnd_t* p_me, char* cfg);
typedef void gui_wnd_add_widget_fn_t(gui_wnd_t* p_me, gui_widget_t* p_wgt);
typedef void gui_wnd_rm_widget_fn_t(gui_wnd_t* p_me, gui_widget_t* p_wgt);
typedef gui_widget_t* gui_wnd_find_widget_fn_t(gui_wnd_t* p_me, const char* name);

typedef void gui_wnd_update_fn_t(gui_wnd_t* p_me, long time);
typedef void gui_wnd_draw_fn_t(gui_wnd_t* p_me);
typedef void gui_wnd_key_fn_t(gui_wnd_t* p_me, gfw_evt_t* p_evt);
typedef gui_widget_t* gui_wnd_mouse_fn_t(gui_wnd_t* p_me, gfw_evt_t* p_evt);
//typedef void gui_wnd_lost_focus_t(gui_wnd_t* p_me);
//typedef void gui_wnd_event_cb_t(gui_widget_t* p_me, char* event);

typedef gui_widget_t* gui_wgt_create_fn_t(void);
typedef void gui_wgt_set_cfg_fn_t(gui_widget_t* p_me, char* cfg, void* data);
typedef void* gui_wgt_get_cfg_fn_t(gui_widget_t* p_me, char* cfg);
typedef void gui_wgt_update_fn_t(gui_widget_t* p_me, long time);
typedef void gui_wgt_draw_fn_t(gui_widget_t* p_me);
typedef void gui_wgt_key_fn_t(gui_widget_t* p_me, gfw_evt_t* p_evt);
typedef void gui_wgt_mouse_fn_t(gui_widget_t* p_me, gfw_evt_t* p_evt);
typedef void gui_wgt_lost_focus_fn_t(gui_widget_t* p_me);
typedef void gui_wgt_free_fn_t(gui_widget_t* p_me);
typedef void gui_wgt_evt_cb_t(gui_widget_t* p_me, char* event);

typedef struct
{
   slnk_t slnk;
   const char* type;
   gui_wgt_create_fn_t* create;
} gui_widget_type_t;

struct gui_widget
{
   dlnk_t dlnk;
   char name[MAX_WIDGET_NAME_LEN];
   const char* type;
   int x;
   int y;
   int h;
   int w;
   bool_t enabled;
   bool_t visible;
   bool_t refresh;
   gui_wnd_t* p_owner;
   gui_wgt_set_cfg_fn_t* set_cfg;
   gui_wgt_get_cfg_fn_t* get_cfg;
   gui_wgt_update_fn_t* on_update;
   gui_wgt_draw_fn_t* on_draw;
   gui_wgt_key_fn_t* on_key;
   gui_wgt_mouse_fn_t* on_mouse;
   gui_wgt_lost_focus_fn_t* on_lost_focus;
   gui_wgt_free_fn_t* free;
   gui_wgt_evt_cb_t* evt_cb;
};

struct gui_wnd
{
   dlnk_t dlnk;
   dlnk_t widget_lst;
   const char* name;
   int x;
   int y;
   int w;
   int h;
   bool_t visible;
   bool_t border;
   bool_t close_btn;
   char* caption;
   glx_image_t* p_caption;
   int caption_offset_y;
   glx_font_t* p_font;
   uint32_t text_color;
   uint32_t bg_color;
   uint32_t border_color;
   bool_t bg_tiled;
   glx_image_t* p_bg;
   /* GUI functions */
   gui_wnd_set_cfg_fn_t* set_cfg;
   gui_wnd_get_cfg_fn_t* get_cfg;
   gui_wnd_add_widget_fn_t* add_widget;
   gui_wnd_rm_widget_fn_t* rm_widget;
   gui_wnd_find_widget_fn_t* find_widget;
   gui_wnd_update_fn_t* on_update;
   gui_wnd_draw_fn_t* on_draw;
   gui_wnd_key_fn_t* on_key;
   gui_wnd_mouse_fn_t* on_mouse;
   //gui_wnd_lost_focus_t* on_lost_focus;
   //gui_set_focus_t* set_focus;
   //gui_clear_focus_t* clear_focus;
   //gui_get_focus_t* get_focus;
   //gui_show_t* show;
   //gui_hide_t* hide;
   //gui_get_visible_t* get_visible;
};

typedef struct
{
   slnk_t widget_type_lst;
   dlnk_t wnd_lst;
   gui_widget_t* p_key_focus;
   gui_widget_t* p_mouse_focus;
} gui_t;

/* GLOBAL VARIABLES **********************************************************/

/* INTERFACE FUNCTIONS *******************************************************/

/*---------------------------------------------------------------------------*/
/*! \brief Initialize gui. */
/*---------------------------------------------------------------------------*/
void gui_init(void);

/*---------------------------------------------------------------------------*/
/*! \brief Register GUI widget type. */
/*---------------------------------------------------------------------------*/
void gui_widget_type_reg(
   gui_widget_type_t* p_type /*!< Pointer to widget type struct */
   );

/*---------------------------------------------------------------------------*/
/*! \brief Create a GUI window. */
/*---------------------------------------------------------------------------*/
gui_wnd_t* gui_wnd_create(
   gui_wnd_t* p_wnd,    /*!< Pre allocated gui window struct or NULL */
   const char* name,    /*!< Window name */
   int x,               /*!< Top left x position */
   int y,               /*!< Top left y position */
   int w,               /*!< Width */
   int h,               /*!< Height */
   unsigned long flags  /*!< Configuration flags */
   );

/*---------------------------------------------------------------------------*/
/*! \brief Create GUI widget. */
/*---------------------------------------------------------------------------*/
gui_widget_t* gui_widget_create(
   const char* name,    /*!< Widget name */
   const char* type,    /*!< Widget type */
   int x,               /*!< Top left x position */
   int y,               /*!< Top left y position */
   int w,               /*!< Width */
   int h                /*!< Height */
   );

/*---------------------------------------------------------------------------*/
/*! \brief Add window to GUI. */
/*---------------------------------------------------------------------------*/
void gui_wnd_add(
   gui_wnd_t* p_wnd         /*!< GUI window to add */
   );

/*---------------------------------------------------------------------------*/
/*! \brief Move window to front. */
/*---------------------------------------------------------------------------*/
void gui_wnd_focus(
   gui_wnd_t* p_wnd         /*!< GUI window */
   );

/*---------------------------------------------------------------------------*/
/*! \brief Send a GUI event. */
/*---------------------------------------------------------------------------*/
void gui_send_event(
   gui_widget_t* p_wgt,     /*!< GUI widget */
   char* event              /*!< GUI event */
   );

/*---------------------------------------------------------------------------*/
/*! \brief Handle GUI event. */
/*---------------------------------------------------------------------------*/
void gui_handle_event(
   gfw_evt_t* p_evt          /*!< GFW event */
   );

/*---------------------------------------------------------------------------*/
/*! \brief Update GUI. */
/*---------------------------------------------------------------------------*/
void gui_update(long time);

/*---------------------------------------------------------------------------*/
/*! \brief Draw GUI. */
/*---------------------------------------------------------------------------*/
void gui_draw(void);

/*---------------------------------------------------------------------------*/
/*! \brief GUI char event. */
/*---------------------------------------------------------------------------*/
void gui_key_evt(gfw_evt_t* p_evt);

/*---------------------------------------------------------------------------*/
/*! \brief GUI mouse event. */
/*---------------------------------------------------------------------------*/
void gui_mouse_evt(gfw_evt_t* p_evt);

#endif /* #ifndef GUI_H */
/* END OF FILE ***************************************************************/
