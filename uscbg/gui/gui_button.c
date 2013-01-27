/******************************************************************************
Copyright (c) 2013, All Rights Reserved.
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file gui_button.c
\brief The gui_button implementation. */
/*---------------------------------------------------------------------------*/
/* INCLUDE FILES *************************************************************/
#include "sys_def.h"
#include "sys_assert.h"
#include <malloc.h>
#include <string.h>
#include "slnk.h"
#include "glx.h"
#include "gui.h"

/* CONSTANTS / MACROS ********************************************************/

/* LOCAL DATATYPES ***********************************************************/
typedef struct
{
   gui_widget_t base;
   char* text;
   bool_t mouse_down;
   glx_font_t* p_font;
   uint32_t text_color;
   glx_image_t* p_text_img;
   void* data;
} gui_button_t;

/* LOCAL FUNCTION PROTOTYPES *************************************************/
static gui_wgt_create_fn_t gui_button_create;
static gui_wgt_free_fn_t gui_button_free;
static gui_wgt_set_cfg_fn_t gui_button_set_cfg;
static gui_wgt_get_cfg_fn_t gui_button_get_cfg;
static gui_wgt_draw_fn_t gui_button_draw;
static gui_wgt_mouse_fn_t gui_button_handle_mouse;
static gui_wgt_lost_focus_fn_t gui_button_lost_focus;

/* MODULE CONSTANTS / VARIABLES **********************************************/
SYS_ASSERT_FILE;
SYS_DBC_FILE;  /*!< Defines the name of this source file once for all */

static gui_widget_type_t widget_type = {
   .type = "button",
   .create = gui_button_create
};

/* GLOBAL CONSTANTS / VARIABLES **********************************************/

/* GLOBAL FUNCTIONS **********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void gui_button_init(void)
{
   gui_widget_type_reg(&widget_type);
}

/* LOCAL FUNCTIONS ***********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static gui_widget_t* gui_button_create(void)
{
   gui_button_t* p_btn = (gui_button_t*)calloc(1, sizeof(gui_button_t));
   REQUIRE(p_btn != NULL);
   p_btn->base.set_cfg = gui_button_set_cfg;
   p_btn->base.get_cfg = gui_button_get_cfg;
   p_btn->base.on_draw = gui_button_draw;
   p_btn->base.on_mouse = gui_button_handle_mouse;
   p_btn->base.on_lost_focus = gui_button_lost_focus;
   p_btn->base.free = gui_button_free;
   p_btn->text_color = GLX_RGBA(0,0,0,0xff);
   p_btn->p_font = glx_get()->default_font;
   return &p_btn->base;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void gui_button_free(gui_widget_t* p_me)
{
   gui_button_t* p_btn = (gui_button_t*)p_me;
   REQUIRE(p_btn != NULL);
   if (p_btn->p_text_img)
   {
      glx_free_image(p_btn->p_text_img);
   }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void gui_button_set_cfg(gui_widget_t* p_me, char* cfg, void* data)
{
   gui_button_t* p_btn = (gui_button_t*)p_me;
   REQUIRE(p_btn != NULL);
   if (strcmp(cfg, "text") == 0) {
      p_btn->text = (char*)data;
      if (p_btn->p_text_img)
      {
         glx_free_image(p_btn->p_text_img);
      }
      p_btn->p_text_img = glx_drawtext(p_btn->p_font, p_btn->text,
         p_btn->text_color, NULL);
      p_me->refresh = TRUE;
   } else if (strcmp(cfg, "cb_fn") == 0) {
      p_me->evt_cb = (gui_wgt_evt_cb_t*)data;
   } else if (strcmp(cfg, "font") == 0) {
      p_btn->p_font = (glx_font_t*)data;
   } else if (strcmp(cfg, "text_color") == 0) {
      p_btn->text_color = (uint32_t)data;
   } else if (strcmp(cfg, "data") == 0) {
      p_btn->data = data;
   }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void* gui_button_get_cfg(gui_widget_t* p_me, char* cfg)
{
   gui_button_t* p_btn = (gui_button_t*)p_me;
   void* data = 0;
   REQUIRE(p_btn != NULL);
   if (strcmp(cfg, "data") == 0) {
      data = p_btn->data;
   }
   return data;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void gui_button_draw(gui_widget_t* p_me)
{
   gui_button_t* p_btn = (gui_button_t*)p_me;
   REQUIRE(p_btn != NULL);
   /* Draw button */
   if (p_btn->mouse_down)
   {
      glx_rect_t rect;
      glx_rect_set(&rect, p_me->x, p_me->y, p_me->w, 1);
      glx_drawrect(&rect, GLX_RGBA(0x80, 0x80, 0x80, 0xff));
      glx_rect_set(&rect, p_me->x, p_me->y, 1, p_me->h);
      glx_drawrect(&rect, GLX_RGBA(0x80, 0x80, 0x80, 0xff));
      glx_rect_set(&rect, p_me->x, p_me->y+p_me->h-1, p_me->w, 1);
      glx_drawrect(&rect, GLX_RGBA(0xff, 0xff, 0xff, 0xff));
      glx_rect_set(&rect, p_me->x+p_me->w-1, p_me->y, 1, p_me->h);
      glx_drawrect(&rect, GLX_RGBA(0xff, 0xff, 0xff, 0xff));
      glx_rect_set(&rect, p_me->x+1, p_me->y+1, p_me->w-2, p_me->h-2);
      glx_drawrect(&rect, GLX_RGBA(0xC0, 0xC0, 0xC0, 0xff));
   }
   else
   {
      glx_rect_t rect;
      glx_rect_set(&rect, p_me->x, p_me->y, p_me->w, 1);
      glx_drawrect(&rect, GLX_RGBA(0xff, 0xff, 0xff, 0xff));
      glx_rect_set(&rect, p_me->x, p_me->y, 1, p_me->h);
      glx_drawrect(&rect, GLX_RGBA(0xff, 0xff, 0xff, 0xff));
      glx_rect_set(&rect, p_me->x, p_me->y+p_me->h-1, p_me->w, 1);
      glx_drawrect(&rect, GLX_RGBA(0x80, 0x80, 0x80, 0xff));
      glx_rect_set(&rect, p_me->x+p_me->w-1, p_me->y, 1, p_me->h);
      glx_drawrect(&rect, GLX_RGBA(0x80, 0x80, 0x80, 0xff));
      glx_rect_set(&rect, p_me->x+1, p_me->y+1, p_me->w-2, p_me->h-2);
      glx_drawrect(&rect, GLX_RGBA(0xC0, 0xC0, 0xC0, 0xff));
   }
   if (p_btn->p_text_img)
   {
      glx_rect_t dstrect;
      int x, y;
      x = p_me->w/2-p_btn->p_text_img->w/2;
      y = p_me->h/2-p_btn->p_text_img->h/2;
      if (p_btn->mouse_down) {
         glx_rect_set(&dstrect, p_me->x+x+1, p_me->y+y+1, 0, 0);
         glx_drawimage(p_btn->p_text_img, NULL, &dstrect);
      } else {
         glx_rect_set(&dstrect, p_me->x+x, p_me->y+y, 0, 0);
         glx_drawimage(p_btn->p_text_img, NULL, &dstrect);
      }
   }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void gui_button_handle_mouse(gui_widget_t* p_me, gfw_evt_t* p_evt)
{
   gui_button_t* p_btn = (gui_button_t*)p_me;
   REQUIRE(p_btn != NULL);
   if (p_evt->type == SDL_MOUSEBUTTONDOWN) {
      p_btn->mouse_down = TRUE;
   } else if (p_evt->type == SDL_MOUSEBUTTONUP) {
      p_btn->mouse_down = FALSE;
      gui_send_event(p_me, "clicked");
   }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void gui_button_lost_focus(gui_widget_t* p_me)
{
   gui_button_t* p_btn = (gui_button_t*)p_me;
   p_btn->mouse_down = FALSE;
}

/* END OF FILE ***************************************************************/
