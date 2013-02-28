/******************************************************************************
Copyright (c) 2013, All Rights Reserved.
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file gui_log.c
\brief The gui_log implementation. */
/*---------------------------------------------------------------------------*/
/* INCLUDE FILES *************************************************************/
#include "sys_def.h"
#include "sys_assert.h"
#include <malloc.h>
#include <string.h>
#include "dlnk.h"
#include "glx.h"
#include "gui.h"
#include "core.h"

/* CONSTANTS / MACROS ********************************************************/
#define MAX_TEXT_LEN (256)

/* LOCAL DATATYPES ***********************************************************/
typedef struct
{
   dlnk_t dlnk;
   glx_image_t* p_name;
   glx_image_t* p_text;
} gui_log_entry_t;

typedef struct
{
   gui_widget_t base;
   glx_font_t* p_font;
   uint32_t log_color;
   bool_t border;
   uint32_t border_color;
   uint32_t bg_color;
   dlnk_t log_head;
   gui_log_entry_t* p_pos;
   int max_log_items;
   int n_items;
} gui_log_t;

/* LOCAL FUNCTION PROTOTYPES *************************************************/
static gui_wgt_create_fn_t gui_log_create;
static gui_wgt_free_fn_t gui_log_free;
static gui_wgt_set_cfg_fn_t gui_log_set_cfg;
static gui_wgt_get_cfg_fn_t gui_log_get_cfg;
static gui_wgt_draw_fn_t gui_log_draw;
static gui_wgt_key_fn_t gui_log_handle_key;

/* MODULE CONSTANTS / VARIABLES **********************************************/
SYS_ASSERT_FILE;
SYS_DBC_FILE;  /*!< Defines the name of this source file once for all */

static gui_widget_type_t widget_type = {
   .type = "gamelog",
   .create = gui_log_create
};

/* GLOBAL CONSTANTS / VARIABLES **********************************************/

/* GLOBAL FUNCTIONS **********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void gui_log_init(void)
{
   gui_widget_type_reg(&widget_type);
}

/* LOCAL FUNCTIONS ***********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static gui_widget_t* gui_log_create(void)
{
   gui_log_t* p_log = (gui_log_t*)calloc(1, sizeof(gui_log_t));
   REQUIRE(p_log != NULL);
   p_log->base.set_cfg = gui_log_set_cfg;
   p_log->base.get_cfg = gui_log_get_cfg;
   p_log->base.on_draw = gui_log_draw;
   p_log->base.on_key = gui_log_handle_key;
   p_log->base.free = gui_log_free;
   p_log->log_color = GLX_RGBA(0xff,0xff,0xff,0xff);
   p_log->p_font = glx_load_font("fonts/cour.ttf", 12);
   p_log->border_color = GLX_RGBA(0x80, 0x80, 0x80, 0xff);
   p_log->bg_color = GLX_RGBA(0x40, 0x40, 0x40, 0xff);
   DLNK_INIT(&p_log->log_head);
   p_log->p_pos = (gui_log_entry_t*)&p_log->log_head;
   p_log->max_log_items = 20;
   return &p_log->base;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void gui_log_free(gui_widget_t* p_me)
{
   gui_log_t* p_log = (gui_log_t*)p_me;
   gui_log_entry_t* p_tmp;
   REQUIRE(p_log != NULL);
   p_tmp = DLNK_NEXT(gui_log_entry_t, &p_log->log_head);
   while (&p_tmp->dlnk != &p_log->log_head)
   {
      (void)DLNK_REMOVE(gui_log_entry_t, p_tmp);
      glx_free_image(p_tmp->p_name);
      glx_free_image(p_tmp->p_text);
      free(p_tmp);
      p_tmp = DLNK_NEXT(gui_log_entry_t, &p_log->log_head);
   }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void gui_log_set_cfg(gui_widget_t* p_me, char* cfg, void* data)
{
   gui_log_t* p_log = (gui_log_t*)p_me;
   REQUIRE(p_log != NULL);
   if (strcmp(cfg, "add_log_entry") == 0) {
      core_log_entry_t* p_clog = (core_log_entry_t*)data;
      gui_log_entry_t* p_glog;
      uint32_t color = GLX_RGBA(0x00, 0x00, 0x00, 0xFF);
      if (p_log->n_items >= p_log->max_log_items) {
         /* Reuse first item. Remove from list and free textures. */
         gui_log_entry_t* p_tmp = DLNK_NEXT(gui_log_entry_t, &p_log->log_head);
         (void)DLNK_REMOVE(gui_log_entry_t, p_tmp);
         glx_free_image(p_tmp->p_name);
         glx_free_image(p_tmp->p_text);
         DLNK_INIT(p_tmp);
         p_glog = p_tmp;
      }
      else {
         p_glog = (gui_log_entry_t*)calloc(1, sizeof(gui_log_entry_t));
         REQUIRE(p_glog != NULL);
         DLNK_INIT(p_glog);
      }
      if (p_clog->p_player) {
         switch (p_clog->p_player->color)
         {
         case PLAYER_COLOR_BLACK:
            color = GLX_RGBA(0x00, 0x00, 0x00, 0xFF);
            break;
         case PLAYER_COLOR_GREEN:
            color = GLX_RGBA(0x00, 0xFF, 0x00, 0xFF);
            break;
         case PLAYER_COLOR_PINK:
            color = GLX_RGBA(0xFF, 0x80, 0x80, 0xFF);
            break;
         case PLAYER_COLOR_WHITE:
            color = GLX_RGBA(0xFF, 0xFF, 0xFF, 0xFF);
            break;
         default:
            color = GLX_RGBA(0x80, 0x80, 0x80, 0xFF);
            break;
         }
         p_glog->p_name = glx_drawtext(p_log->p_font, p_clog->p_player->name,
            color, NULL);
      } else {
         /* Server message */
         color = GLX_RGBA(0xC0, 0xC0, 0xC0, 0xFF);
         p_glog->p_name = glx_drawtext(p_log->p_font, "Server",
            color, NULL);
      }
      p_glog->p_text = glx_drawtext(p_log->p_font, p_clog->text,
         p_log->log_color, NULL);
      DLNK_INSERT(&p_log->log_head, p_glog);
      /* Set current pos to last log entry */
      p_log->p_pos = p_glog;
   } else if (strcmp(cfg, "border") == 0) {
      p_log->border = (bool_t)data;
   } else if (strcmp(cfg, "border_color") == 0) {
      p_log->border_color = (uint32_t)data;
   } else if (strcmp(cfg, "bg_color") == 0) {
      p_log->bg_color = (uint32_t)data;
   } else if (strcmp(cfg, "log_color") == 0) {
      p_log->log_color = (uint32_t)data;
   }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void* gui_log_get_cfg(gui_widget_t* p_me, char* cfg)
{
   TOUCH(p_me);
   TOUCH(cfg);
   return NULL;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void gui_log_draw(gui_widget_t* p_me)
{
   gui_log_t* p_log = (gui_log_t*)p_me;
   gui_log_entry_t* p_glog;
   glx_rect_t rect;
   int x;
   int y;
   REQUIRE(p_log != NULL);
   p_glog = p_log->p_pos;
   if (p_log->border)
   {
      glx_rect_set(&rect, p_me->x, p_me->y, p_me->w, 1);
      glx_drawrect(&rect, p_log->border_color);
      glx_rect_set(&rect, p_me->x, p_me->y, 1, p_me->h);
      glx_drawrect(&rect, p_log->border_color);
      glx_rect_set(&rect, p_me->x, p_me->y+p_me->h-1, p_me->w, 1);
      glx_drawrect(&rect, p_log->border_color);
      glx_rect_set(&rect, p_me->x+p_me->w-1, p_me->y, 1, p_me->h);
      glx_drawrect(&rect, p_log->border_color);
   }
   glx_rect_set(&rect, p_me->x+1, p_me->y+1, p_me->w-2, p_me->h-2);
   glx_drawrect(&rect, p_log->bg_color);
   /* Draw log down up */
   y = p_me->h - 2;
   while (&p_glog->dlnk != &p_log->log_head)
   {
      x = 5;
      y -= (p_glog->p_name->h);
      if (y <= 0) {
         break;
      }
      glx_rect_set(&rect, p_me->x + x, p_me->y + y, 0, 0);
      glx_drawimage(p_glog->p_name, NULL, &rect);
      x += (p_glog->p_name->w + 5);
      glx_rect_set(&rect, p_me->x + x, p_me->y + y, 0, 0);
      glx_drawimage(p_glog->p_text, NULL, &rect);
      p_glog = DLNK_PREV(gui_log_entry_t, p_glog);
   }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void gui_log_handle_key(gui_widget_t* p_me, gfw_evt_t* p_evt)
{
   gui_log_t* p_log = (gui_log_t*)p_me;
   gui_log_entry_t* p_glog;
   REQUIRE(p_log != NULL);
   if (p_evt->key.keysym.sym == SDLK_UP) {
      p_glog = DLNK_PREV(gui_log_entry_t, p_log->p_pos);
      if (&p_glog->dlnk != &p_log->log_head) {
         p_log->p_pos = p_glog;
      }
   } else if (p_evt->key.keysym.sym == SDLK_DOWN) {
      p_glog = DLNK_NEXT(gui_log_entry_t, p_log->p_pos);
      if (&p_glog->dlnk != &p_log->log_head) {
         p_log->p_pos = p_glog;
      }
   }
}

/* END OF FILE ***************************************************************/
