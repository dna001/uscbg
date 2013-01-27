/******************************************************************************
Copyright (c) 2013, All Rights Reserved.
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file gui_image.c
\brief The gui_image implementation. */
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
   glx_image_t* p_img;
   void* data;
} gui_image_t;

/* LOCAL FUNCTION PROTOTYPES *************************************************/
static gui_wgt_create_fn_t gui_image_create;
static gui_wgt_free_fn_t gui_image_free;
static gui_wgt_set_cfg_fn_t gui_image_set_cfg;
static gui_wgt_get_cfg_fn_t gui_image_get_cfg;
static gui_wgt_draw_fn_t gui_image_draw;
static gui_wgt_mouse_fn_t gui_image_handle_mouse;
//static gui_wgt_lost_focus_fn_t gui_image_lost_focus;

/* MODULE CONSTANTS / VARIABLES **********************************************/
SYS_ASSERT_FILE;
SYS_DBC_FILE;  /*!< Defines the name of this source file once for all */

static gui_widget_type_t widget_type = {
   .type = "image",
   .create = gui_image_create
};

/* GLOBAL CONSTANTS / VARIABLES **********************************************/

/* GLOBAL FUNCTIONS **********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void gui_image_init(void)
{
   gui_widget_type_reg(&widget_type);
}

/* LOCAL FUNCTIONS ***********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static gui_widget_t* gui_image_create(void)
{
   gui_image_t* p_img = (gui_image_t*)calloc(1, sizeof(gui_image_t));
   REQUIRE(p_img != NULL);
   p_img->base.set_cfg = gui_image_set_cfg;
   p_img->base.get_cfg = gui_image_get_cfg;
   p_img->base.on_draw = gui_image_draw;
   p_img->base.on_mouse = gui_image_handle_mouse;
   //p_img->base.on_lost_focus = gui_image_lost_focus;
   p_img->base.free = gui_image_free;
   return &p_img->base;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void gui_image_free(gui_widget_t* p_me)
{
   gui_image_t* p_img = (gui_image_t*)p_me;
   REQUIRE(p_img != NULL);
   if (p_img->p_img)
   {
      glx_free_image(p_img->p_img);
   }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void gui_image_set_cfg(gui_widget_t* p_me, char* cfg, void* data)
{
   gui_image_t* p_img = (gui_image_t*)p_me;
   REQUIRE(p_img != NULL);
   if (strcmp(cfg, "image") == 0) {
      char* path = (char*)data;
      if (p_img->p_img)
      {
         glx_free_image(p_img->p_img);
      }
      p_img->p_img = glx_load_image(path);
      REQUIRE(p_img->p_img != NULL);
   } else if (strcmp(cfg, "data") == 0) {
      p_img->data = data;
   } else if (strcmp(cfg, "cb_fn") == 0) {
      p_me->evt_cb = (gui_wgt_evt_cb_t*)data;
   }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void* gui_image_get_cfg(gui_widget_t* p_me, char* cfg)
{
   gui_image_t* p_img = (gui_image_t*)p_me;
   void* data = 0;
   REQUIRE(p_img != NULL);
   if (strcmp(cfg, "data") == 0) {
      data = p_img->data;
   }
   return data;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void gui_image_draw(gui_widget_t* p_me)
{
   gui_image_t* p_img = (gui_image_t*)p_me;
   REQUIRE(p_img != NULL);
   /* Draw image */
   if (p_img->p_img)
   {
      glx_rect_t dstrect;
      glx_rect_set(&dstrect, p_me->x, p_me->y, p_me->w, p_me->h);
      glx_drawimage(p_img->p_img, NULL, &dstrect);
   }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void gui_image_handle_mouse(gui_widget_t* p_me, gfw_evt_t* p_evt)
{
   gui_image_t* p_img = (gui_image_t*)p_me;
   REQUIRE(p_img != NULL);
   if (p_me->evt_cb) {
      if (p_evt->type == SDL_MOUSEBUTTONDOWN) {
         gui_send_event(p_me, "clicked");
      }
   }
}

#if 0
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void gui_image_lost_focus(gui_widget_t* p_me)
{
   gui_image_t* p_img = (gui_image_t*)p_me;
   p_img->mouse_down = FALSE;
}
#endif

/* END OF FILE ***************************************************************/
