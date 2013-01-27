/******************************************************************************
Copyright (c) 2013, All Rights Reserved.
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file gui_text.c
\brief The gui_text implementation. */
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
#define MAX_TEXT_LEN (256)

/* LOCAL DATATYPES ***********************************************************/
typedef struct
{
   gui_widget_t base;
   char text[MAX_TEXT_LEN];
   glx_font_t* p_font;
   uint32_t text_color;
   glx_image_t* p_text_img;
   bool_t center;
   bool_t edit;
   bool_t border;
   uint32_t border_color;
   uint32_t bg_color;
} gui_text_t;

/* LOCAL FUNCTION PROTOTYPES *************************************************/
static gui_wgt_create_fn_t gui_text_create;
static gui_wgt_free_fn_t gui_text_free;
static gui_wgt_set_cfg_fn_t gui_text_set_cfg;
static gui_wgt_get_cfg_fn_t gui_text_get_cfg;
static gui_wgt_draw_fn_t gui_text_draw;
static gui_wgt_key_fn_t gui_text_handle_key;

/* MODULE CONSTANTS / VARIABLES **********************************************/
SYS_ASSERT_FILE;
SYS_DBC_FILE;  /*!< Defines the name of this source file once for all */

static gui_widget_type_t widget_type = {
   .type = "text",
   .create = gui_text_create
};

/* GLOBAL CONSTANTS / VARIABLES **********************************************/

/* GLOBAL FUNCTIONS **********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void gui_text_init(void)
{
   gui_widget_type_reg(&widget_type);
}

/* LOCAL FUNCTIONS ***********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static gui_widget_t* gui_text_create(void)
{
   gui_text_t* p_text = (gui_text_t*)calloc(1, sizeof(gui_text_t));
   REQUIRE(p_text != NULL);
   p_text->base.set_cfg = gui_text_set_cfg;
   p_text->base.get_cfg = gui_text_get_cfg;
   p_text->base.on_draw = gui_text_draw;
   p_text->base.on_key = gui_text_handle_key;
   p_text->base.free = gui_text_free;
   p_text->text_color = GLX_RGBA(0xff,0xff,0xff,0xff);
   p_text->p_font = glx_get()->default_font;
   p_text->center = TRUE;
   p_text->border_color = GLX_RGBA(0x80, 0x80, 0x80, 0xff);
   p_text->bg_color = GLX_RGBA(0x40, 0x40, 0x40, 0xff);
   return &p_text->base;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void gui_text_free(gui_widget_t* p_me)
{
   gui_text_t* p_text = (gui_text_t*)p_me;
   REQUIRE(p_text != NULL);
   if (p_text->p_text_img)
   {
      glx_free_image(p_text->p_text_img);
   }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void gui_text_set_cfg(gui_widget_t* p_me, char* cfg, void* data)
{
   gui_text_t* p_text = (gui_text_t*)p_me;
   REQUIRE(p_text != NULL);
   if (strcmp(cfg, "text") == 0) {
      memset(p_text->text, 0, MAX_TEXT_LEN);
      strncpy(p_text->text, (char*)data, MAX_TEXT_LEN-1);
      if (p_text->p_text_img)
      {
         glx_free_image(p_text->p_text_img);
      }
      p_text->p_text_img = glx_drawtext(p_text->p_font, p_text->text,
         p_text->text_color, NULL);
   } else if (strcmp(cfg, "center") == 0) {
      p_text->center = (bool_t)data;
   } else if (strcmp(cfg, "edit") == 0) {
      p_text->edit = (bool_t)data;
   } else if (strcmp(cfg, "border") == 0) {
      p_text->border = (bool_t)data;
   } else if (strcmp(cfg, "border_color") == 0) {
      p_text->border_color = (uint32_t)data;
   } else if (strcmp(cfg, "bg_color") == 0) {
      p_text->bg_color = (uint32_t)data;
   } else if (strcmp(cfg, "text_color") == 0) {
      p_text->text_color = (uint32_t)data;
   } else if (strcmp(cfg, "font") == 0) {
      p_text->p_font = (glx_font_t*)data;
   } else {
      ASSERT(FALSE);
   }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void* gui_text_get_cfg(gui_widget_t* p_me, char* cfg)
{
   gui_text_t* p_text = (gui_text_t*)p_me;
   void* data = NULL;
   REQUIRE(p_text != NULL);
   if (strcmp(cfg, "text") == 0) {
      data = p_text->text;
   } else if (strcmp(cfg, "center") == 0) {
      data = (void*)p_text->center;
   } else if (strcmp(cfg, "edit") == 0) {
      data = (void*)p_text->edit;
   } else if (strcmp(cfg, "border") == 0) {
      data = (void*)p_text->border;
   }
   return data;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void gui_text_draw(gui_widget_t* p_me)
{
   gui_text_t* p_text = (gui_text_t*)p_me;
   REQUIRE(p_text != NULL);
   if (p_text->border)
   {
      glx_rect_t rect;
      glx_rect_set(&rect, p_me->x, p_me->y, p_me->w, 1);
      glx_drawrect(&rect, p_text->border_color);
      glx_rect_set(&rect, p_me->x, p_me->y, 1, p_me->h);
      glx_drawrect(&rect, p_text->border_color);
      glx_rect_set(&rect, p_me->x, p_me->y+p_me->h-1, p_me->w, 1);
      glx_drawrect(&rect, p_text->border_color);
      glx_rect_set(&rect, p_me->x+p_me->w-1, p_me->y, 1, p_me->h);
      glx_drawrect(&rect, p_text->border_color);
      glx_rect_set(&rect, p_me->x+1, p_me->y+1, p_me->w-2, p_me->h-2);
      glx_drawrect(&rect, p_text->bg_color);
   }
   if (p_text->p_text_img)
   {
      glx_rect_t dstrect;
      int x, y;
      x = (p_text->center)?p_me->w/2-p_text->p_text_img->w/2:0;
      y = (p_text->center)?p_me->h/2-p_text->p_text_img->h/2:0;
      glx_rect_set(&dstrect, p_me->x+x, p_me->y+y, 0, 0);
      glx_drawimage(p_text->p_text_img, NULL, &dstrect);
   }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void gui_text_handle_key(gui_widget_t* p_me, gfw_evt_t* p_evt)
{
   gui_text_t* p_text = (gui_text_t*)p_me;
   REQUIRE(p_text != NULL);
   if (p_text->edit)
   {
      if (p_evt->key.keysym.sym == SDLK_BACKSPACE) /* Backspace */
      {
         int len = strlen(p_text->text);
         if (len > 0)
         {
            p_text->text[len-1] = 0;
         }
      }
      else
      {
         char ch = p_evt->key.keysym.unicode;
         int len = strlen(p_text->text);
         if (len < MAX_TEXT_LEN-1)
         {
            p_text->text[len] = ch;
         }
      }
      if (p_text->p_text_img)
      {
         glx_free_image(p_text->p_text_img);
         p_text->p_text_img = NULL;
      }
      if (strlen(p_text->text) > 0)
      {
         p_text->p_text_img = glx_drawtext(p_text->p_font, p_text->text,
            p_text->text_color, NULL);
      }
   }
}

/* END OF FILE ***************************************************************/
