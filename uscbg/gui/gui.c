/******************************************************************************
Copyright (c) 2013, All Rights Reserved.
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file gui.c
\brief The gui implementation. */
/*---------------------------------------------------------------------------*/
/* INCLUDE FILES *************************************************************/
#include "sys_def.h"
#include "sys_assert.h"
#include <string.h>
#include <malloc.h>
#include "slnk.h"
#include "dlnk.h"
#include "trc.h"
#include "gfw.h"
#include "glx.h"
#include "gui.h"
#include "gui_button.h"
#include "gui_image.h"
#include "gui_text.h"

/* CONSTANTS / MACROS ********************************************************/

/* LOCAL DATATYPES ***********************************************************/

/* LOCAL FUNCTION PROTOTYPES *************************************************/
static gui_wnd_set_cfg_fn_t gui_wnd_set_cfg;
static gui_wnd_get_cfg_fn_t gui_wnd_get_cfg;
static gui_wnd_add_widget_fn_t gui_wnd_add_widget;
static gui_wnd_rm_widget_fn_t gui_wnd_rm_widget;
static gui_wnd_find_widget_fn_t gui_wnd_find_widget;
static gui_wnd_update_fn_t gui_wnd_update;
static gui_wnd_draw_fn_t gui_wnd_draw;
static gui_wnd_mouse_fn_t gui_wnd_mouse;
//typedef void gui_wnd_lost_focus_t(gui_wnd_t* p_me);

static gui_widget_type_t* find_widget_type(const char* type);

/* MODULE CONSTANTS / VARIABLES **********************************************/
SYS_ASSERT_FILE;
SYS_DBC_FILE;  /*!< Defines the name of this source file once for all */

TRC_DEF(gui);

static gui_t gui;

/* GLOBAL CONSTANTS / VARIABLES **********************************************/

/* GLOBAL FUNCTIONS **********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void gui_init(void)
{
   TRC_REG(gui, TRC_ERROR | TRC_DEBUG);
   SLNK_INIT(&gui.widget_type_lst);
   DLNK_INIT(&gui.wnd_lst);
   gui.p_key_focus = NULL;
   gui.p_mouse_focus = NULL;
   /* Register standard components */
   gui_button_init();
   gui_text_init();
   gui_image_init();
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void gui_widget_type_reg(gui_widget_type_t* p_type)
{
   SLNK_INIT(p_type);
   SLNK_ADD(&gui.widget_type_lst, p_type);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
gui_wnd_t* gui_wnd_create(gui_wnd_t* p_wnd, const char* name, int x, int y,
   int w, int h, unsigned long flags)
{
   if (p_wnd == NULL)
   {
      p_wnd = (gui_wnd_t*)calloc(1, sizeof(gui_wnd_t));
   }
   REQUIRE(p_wnd);
   DLNK_INIT(p_wnd);
   DLNK_INIT(&p_wnd->widget_lst);
   p_wnd->name = name;
   p_wnd->x = x;
   p_wnd->y = y;
   p_wnd->w = w;
   p_wnd->h = h;
   p_wnd->visible = TRUE;
   p_wnd->border = FALSE;
   p_wnd->caption = NULL;
   p_wnd->p_font = glx_get()->default_font;
   p_wnd->text_color = GLX_RGBA(0xFF, 0xFF, 0xFF, 0xFF);
   p_wnd->bg_color = GLX_RGBA(0x00, 0x00, 0x00, 0x80);
   p_wnd->border_color = GLX_RGBA(0x80, 0x80, 0x80, 0xFF);
   p_wnd->set_cfg = gui_wnd_set_cfg;
   p_wnd->get_cfg = gui_wnd_get_cfg;
   p_wnd->add_widget = gui_wnd_add_widget;
   p_wnd->rm_widget = gui_wnd_rm_widget;
   p_wnd->find_widget = gui_wnd_find_widget;
   p_wnd->on_update = gui_wnd_update;
   p_wnd->on_draw = gui_wnd_draw;
   p_wnd->on_mouse = gui_wnd_mouse;
   return p_wnd;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
gui_widget_t* gui_widget_create(const char* name, const char* type,
                                int x, int y, int w, int h)
{
   gui_widget_type_t* p_type = find_widget_type(type);
   gui_widget_t* p_wgt;
   REQUIRE(p_type != NULL);
   p_wgt = p_type->create();
   DLNK_INIT(p_wgt);
   strncpy(p_wgt->name, name, MAX_WIDGET_NAME_LEN-1);
   p_wgt->type = type;
   p_wgt->x = x;
   p_wgt->y = y;
   p_wgt->w = w;
   p_wgt->h = h;
   p_wgt->enabled = TRUE;
   p_wgt->visible = TRUE;
   return p_wgt;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void gui_wnd_add(gui_wnd_t* p_wnd)
{
   REQUIRE(p_wnd != NULL);
   DLNK_INSERT(&gui.wnd_lst, p_wnd);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void gui_wnd_focus(gui_wnd_t* p_wnd)
{
   REQUIRE(p_wnd != NULL);
   /* Move window to last position in list (ie move to front). */
   (void)DLNK_REMOVE(gui_wnd_t, p_wnd);
   DLNK_INSERT(&gui.wnd_lst, p_wnd);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void gui_widget_focus(gui_widget_t* p_wgt)
{
   REQUIRE(p_wgt != NULL);
   gui.p_key_focus = p_wgt;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void gui_send_event(gui_widget_t* p_wgt, char* event)
{
   SDL_Event sdl_event;

   if (p_wgt->evt_cb != NULL)
   {
      sdl_event.type = SDL_USEREVENT;
      sdl_event.user.code = GFW_EVT_TYPE_GUI;
      sdl_event.user.data1 = (void*)p_wgt;
      sdl_event.user.data2 = (void*)event;
      SDL_PushEvent(&sdl_event);
   }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void gui_handle_event(gfw_evt_t* p_evt)
{ /* Only need to handle data1 and data2 at this stage */
   gui_widget_t* p_wgt = (gui_widget_t*)p_evt->user.data1;
   if (p_wgt->evt_cb != NULL)
   {
      TRC_DBG(gui, "Widget: %s, Event: %s", p_wgt->name,
         (char*)p_evt->user.data2);
      p_wgt->evt_cb(p_wgt, (char*)p_evt->user.data2);
   }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void gui_update(long time)
{
   gui_wnd_t* p_wnd = DLNK_NEXT(gui_wnd_t, &gui.wnd_lst);
   while (&p_wnd->dlnk != &gui.wnd_lst)
   {
      p_wnd->on_update(p_wnd, time);
      p_wnd = DLNK_NEXT(gui_wnd_t, p_wnd);
   }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void gui_draw(void)
{
   gui_wnd_t* p_wnd = DLNK_NEXT(gui_wnd_t, &gui.wnd_lst);
   while (&p_wnd->dlnk != &gui.wnd_lst)
   {
      if (p_wnd->visible)
      {
         p_wnd->on_draw(p_wnd);
      }
      p_wnd = DLNK_NEXT(gui_wnd_t, p_wnd);
   }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void gui_key_evt(gfw_evt_t* p_evt)
{
   if (gui.p_key_focus)
   {
      if (gui.p_key_focus->on_key)
      {
         gui.p_key_focus->on_key(gui.p_key_focus, p_evt);
      }
   }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void gui_mouse_evt(gfw_evt_t* p_evt)
{
   gui_wnd_t* p_wnd = DLNK_PREV(gui_wnd_t, &gui.wnd_lst);
   gui_widget_t* p_old_mouse_focus = gui.p_mouse_focus;
   while (&p_wnd->dlnk != &gui.wnd_lst)
   {
      if (p_wnd->visible && p_wnd->on_mouse)
      {
         gui_widget_t* p_wgt;
         p_wgt = p_wnd->on_mouse(p_wnd, p_evt);
         gui.p_mouse_focus = p_wgt;
         if (p_evt->type == SDL_MOUSEBUTTONDOWN)
         {
            gui.p_key_focus = p_wgt;
         }
         if (p_wgt != NULL)
         {
            break;
         }
      }
      p_wnd = DLNK_PREV(gui_wnd_t, p_wnd);
   }
   if ((gui.p_mouse_focus != p_old_mouse_focus) && (p_old_mouse_focus) &&
       (p_old_mouse_focus->on_lost_focus))
   {
      p_old_mouse_focus->on_lost_focus(p_old_mouse_focus);
   }
}

/* LOCAL FUNCTIONS ***********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void gui_wnd_set_cfg(gui_wnd_t* p_me, char* cfg, void* data)
{
   REQUIRE(p_me != NULL);
   if (strcmp(cfg, "border") == 0) {
      p_me->border = (bool_t)data;
   } else if (strcmp(cfg, "caption") == 0) {
      p_me->caption = (char*)data;
      if (p_me->p_caption) {
         glx_free_image(p_me->p_caption);
      }
      p_me->p_caption = glx_drawtext(p_me->p_font, p_me->caption,
         p_me->text_color, NULL);
      REQUIRE(p_me->p_caption != NULL);
      p_me->caption_offset_y = p_me->p_caption->h + 4;
   } else if (strcmp(cfg, "font") == 0) {
      p_me->p_font = (glx_font_t*)data;
   } else if (strcmp(cfg, "text_color") == 0) {
      p_me->text_color = (uint32_t)data;
   } else if (strcmp(cfg, "bg_color") == 0) {
      p_me->bg_color = (uint32_t)data;
   } else if (strcmp(cfg, "bg_image") == 0) {
      char* path = (char*)data;
      p_me->p_bg = glx_load_image(path);
      REQUIRE(p_me->p_bg != NULL);
   } else if (strcmp(cfg, "bg_tiled") == 0) {
      p_me->bg_tiled = (bool_t)data;
   }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void* gui_wnd_get_cfg(gui_wnd_t* p_me, char* cfg)
{
   return NULL;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void gui_wnd_add_widget(gui_wnd_t* p_me, gui_widget_t* p_wgt)
{
   p_wgt->p_owner = p_me;
   DLNK_INSERT(&p_me->widget_lst, p_wgt);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void gui_wnd_rm_widget(gui_wnd_t* p_me, gui_widget_t* p_wgt)
{
   if (gui.p_key_focus == p_wgt)
   {
      gui.p_key_focus = NULL;
   }
   if (gui.p_mouse_focus == p_wgt)
   {
      gui.p_mouse_focus = NULL;
   }
   (void)DLNK_REMOVE(gui_widget_t, p_wgt);
   if (p_wgt->free)
   {
      p_wgt->free(p_wgt);
   }
   free(p_wgt);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static gui_widget_t* gui_wnd_find_widget(gui_wnd_t* p_me, const char* name)
{
   gui_widget_t* p_wgt = DLNK_NEXT(gui_widget_t, &p_me->widget_lst);
   while(&p_wgt->dlnk != &p_me->widget_lst)
   {
      if (strcmp(p_wgt->name, name) == 0)
      {
         break;
      }
      p_wgt = DLNK_NEXT(gui_widget_t, p_wgt);
   }
   if (&p_wgt->dlnk == &p_me->widget_lst)
   {
      p_wgt = NULL;
   }
   return p_wgt;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void gui_wnd_update(gui_wnd_t* p_me, long time)
{
   gui_widget_t* p_wgt = DLNK_NEXT(gui_widget_t, &p_me->widget_lst);
   while(&p_wgt->dlnk != &p_me->widget_lst)
   {
      if ((p_wgt->visible) && (p_wgt->on_update))
      {
         p_wgt->on_update(p_wgt, time);
      }
      p_wgt = DLNK_NEXT(gui_widget_t, p_wgt);
   }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void gui_wnd_draw(gui_wnd_t* p_me)
{
   gui_widget_t* p_wgt = DLNK_NEXT(gui_widget_t, &p_me->widget_lst);
   glx_rect_t dstrect;
   int y_offs = 0;
   glx_rect_set(&dstrect, p_me->x, p_me->y + p_me->caption_offset_y,
      p_me->w, p_me->h - p_me->caption_offset_y);
   if (p_me->p_bg)
   { /* Draw background image (tiled) */
      glx_drawimage(p_me->p_bg, NULL, &dstrect);
   }
   else
   { /* Draw background color */
      glx_drawrect(&dstrect, p_me->bg_color);
   }
   if (p_me->border)
   {
      glx_rect_set(&dstrect, p_me->x, p_me->y, p_me->w, 1);
      glx_drawrect(&dstrect, p_me->border_color);
      glx_rect_set(&dstrect, p_me->x, p_me->y, 1, p_me->h);
      glx_drawrect(&dstrect, p_me->border_color);
      glx_rect_set(&dstrect, p_me->x, p_me->y+p_me->h-1, p_me->w, 1);
      glx_drawrect(&dstrect, p_me->border_color);
      glx_rect_set(&dstrect, p_me->x+p_me->w-1, p_me->y, 1, p_me->h);
      glx_drawrect(&dstrect, p_me->border_color);
   }
   if (p_me->p_caption)
   { /* Draw window caption border and text. Apply offset*/
      int x, y;
      y_offs = p_me->p_caption->h + 4;
      glx_rect_set(&dstrect, p_me->x, p_me->y, p_me->w, y_offs);
      glx_drawrect(&dstrect, p_me->border_color);
      x = p_me->w/2-p_me->p_caption->w/2;
      y = y_offs/2-p_me->p_caption->h/2;
      glx_rect_set(&dstrect, p_me->x+x, p_me->y+y, 0, 0);
      glx_drawimage(p_me->p_caption, NULL, &dstrect);
   }
   while(&p_wgt->dlnk != &p_me->widget_lst)
   {
      if ((p_wgt->visible) && (p_wgt->on_draw))
      {
         glPushMatrix ();
         glTranslatef(p_me->x, p_me->y + y_offs, 0);
         p_wgt->on_draw(p_wgt);
         glPopMatrix ();
      }
      p_wgt = DLNK_NEXT(gui_widget_t, p_wgt);
   }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static gui_widget_t* gui_wnd_mouse(gui_wnd_t* p_me, gfw_evt_t* p_evt)
{
   gui_widget_t* p_wgt_focus = NULL;
   int x = (p_evt->type == SDL_MOUSEMOTION)?p_evt->motion.x:p_evt->button.x;
   int y = (p_evt->type == SDL_MOUSEMOTION)?p_evt->motion.y:p_evt->button.y;
   if ((x >= p_me->x) && (x < (p_me->x+p_me->w)) &&
       (y >= p_me->y) && (y < (p_me->y+p_me->h)))
   {
      x -= p_me->x;
      y -= (p_me->y + p_me->caption_offset_y);
      gui_widget_t* p_wgt = DLNK_PREV(gui_widget_t, &p_me->widget_lst);
      while(&p_wgt->dlnk != &p_me->widget_lst)
      {
         if ((p_wgt->visible) && (p_wgt->enabled) &&
             (x >= p_wgt->x) && (x < (p_wgt->x+p_wgt->w)) &&
             (y >= p_wgt->y) && (y < (p_wgt->y+p_wgt->h)))
         {
            if (p_wgt->on_mouse)
            {
               p_wgt->on_mouse(p_wgt, p_evt);
            }
            p_wgt_focus = p_wgt;
            break;
         }
         p_wgt = DLNK_PREV(gui_widget_t, p_wgt);
      }
   }
   return p_wgt_focus;
}
//static void gui_wnd_lost_focus_t(gui_wnd_t* p_me);
//static void gui_wnd_event_cb_t(gui_widget_t* p_me, char* event);

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static gui_widget_type_t* find_widget_type(const char* type)
{
   gui_widget_type_t* p_type = SLNK_NEXT(gui_widget_type_t, &gui.widget_type_lst);
   while(p_type != NULL)
   {
      if (strcmp(type, p_type->type) == 0)
      {
         break;
      }
      p_type = SLNK_NEXT(gui_widget_type_t, p_type);
   }
   return p_type;
}

/* END OF FILE ***************************************************************/
