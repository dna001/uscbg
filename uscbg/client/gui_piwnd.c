/******************************************************************************
Copyright (c) 2013, All Rights Reserved.
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file gui_piwnd.c
\brief The Urban Sprawl playerinfo implementation. */
/*---------------------------------------------------------------------------*/
/* INCLUDE FILES *************************************************************/
#include "sys_def.h"
#include "sys_assert.h"
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "slnk.h"
#include "dlnk.h"
#include "gfw.h"
#include "glx.h"
#include "scf.h"
#include "gui.h"
#include "core.h"

/* CONSTANTS / MACROS ********************************************************/

/* LOCAL DATATYPES ***********************************************************/

/* LOCAL FUNCTION PROTOTYPES *************************************************/
static gui_wnd_set_cfg_fn_t gui_piwnd_set_cfg;

/* MODULE CONSTANTS / VARIABLES **********************************************/
SYS_ASSERT_FILE;
SYS_DBC_FILE;  /*!< Defines the name of this source file once for all */

static char* vocations_image_path[VOCATION_LAST] = {
   "data/Vocation-finance.png",
   "data/Vocation-transport.png",
   "data/Vocation-education.png",
   "data/Vocation-energy.png",
   "data/Vocation-tour.png",
   "data/Vocation-factory.png",
   "data/Vocation-public.png",
   "data/Vocation-media.png"
};

/* GLOBAL CONSTANTS / VARIABLES **********************************************/

/* GLOBAL FUNCTIONS **********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void gui_piwnd_init(void)
{
}

/* LOCAL FUNCTIONS ***********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
gui_wnd_t* gui_piwnd_create(int x, int y, int w, int h, gui_wgt_evt_cb_t* p_cb)
{
   gui_wnd_t* p_wnd;
   gui_widget_t* p_wgt;
   static int color = 0;
   int xx;
   int yy;
   int i;
   p_wnd = gui_wnd_create(NULL, "piwnd", x, y, w, h, 0);
   REQUIRE(p_wnd != NULL);
   p_wnd->set_cfg = gui_piwnd_set_cfg;
   p_wnd->bg_color = GLX_RGBA(0x00, 0x00, 0x00, 0x40);
   p_wnd->border = TRUE;
   p_wnd->border_color = GLX_RGBA(0x00, 0x00, 0x00, 0xFF);
   /* Name */
   p_wgt = gui_widget_create("name_text", "text", 5, 5, 90, 20);
   p_wgt->set_cfg(p_wgt, "border", (void*)FALSE);
   p_wgt->set_cfg(p_wgt, "edit", (void*)FALSE);
   p_wgt->set_cfg(p_wgt, "text_color", (void*)GLX_RGBA(0xFF, 0xFF, 0xFF, 0xFF));
   p_wgt->set_cfg(p_wgt, "text", "Name");
   p_wnd->add_widget(p_wnd, p_wgt);
   /* Wealth */
   p_wgt = gui_widget_create("wealth_image", "image", 5, 30, 40, 20);
   p_wgt->set_cfg(p_wgt, "image", "data/Urb_Money20_FINAL.png");
   p_wnd->add_widget(p_wnd, p_wgt);
   p_wgt = gui_widget_create("wealth", "text", 50, 30, 20, 20);
   p_wgt->set_cfg(p_wgt, "border", (void*)FALSE);
   p_wgt->set_cfg(p_wgt, "edit", (void*)FALSE);
   p_wgt->set_cfg(p_wgt, "text_color", (void*)GLX_RGBA(0xFF, 0xFF, 0xFF, 0xFF));
   p_wgt->set_cfg(p_wgt, "text", "0");
   p_wnd->add_widget(p_wnd, p_wgt);
   /* Prestige points */
   p_wgt = gui_widget_create("prestige_text", "text", 5, 55, 50, 20);
   p_wgt->set_cfg(p_wgt, "border", (void*)FALSE);
   p_wgt->set_cfg(p_wgt, "edit", (void*)FALSE);
   p_wgt->set_cfg(p_wgt, "text_color", (void*)GLX_RGBA(0xFF, 0xFF, 0xFF, 0xFF));
   p_wgt->set_cfg(p_wgt, "text", "P: 0");
   p_wnd->add_widget(p_wnd, p_wgt);
#if 0
   /* Dominant display background image */
   p_wnd->p_bg = glx_load_image(dominant_display_image_path[animal]);
   /* Elements */
   xx = 100;
   yy = 6;
   for (i=0;i<4;i++)
   {
      p_wgt = gui_widget_create(element_list[i], "image", xx, yy, 28, 28);
      p_wgt->set_cfg(p_wgt, "cb_fn", p_cb);
      p_wnd->add_widget(p_wnd, p_wgt);
      xx += 31;
   }
   yy = 0;
   xx = 230;
   /* Action pawns */
   /*p_wgt = gui_widget_create("ap_text", "text", xx, yy, 20, 20);
   p_wgt->set_cfg(p_wgt, "border", (void*)FALSE);
   p_wgt->set_cfg(p_wgt, "edit", (void*)FALSE);
   p_wgt->set_cfg(p_wgt, "text_color", (void*)GLX_RGBA(0x00, 0x00, 0x00, 0xFF));
   p_wgt->set_cfg(p_wgt, "text", "AP");*/
   p_wgt = gui_widget_create("ap_image", "image", xx, yy, 20, 20);
   p_wgt->set_cfg(p_wgt, "image", ap_image_path[animal]);
   p_wnd->add_widget(p_wnd, p_wgt);
   yy += 20;
   p_wgt = gui_widget_create("ap", "text", xx, yy, 20, 20);
   p_wgt->set_cfg(p_wgt, "border", (void*)FALSE);
   p_wgt->set_cfg(p_wgt, "edit", (void*)FALSE);
   p_wgt->set_cfg(p_wgt, "text_color", (void*)GLX_RGBA(0x00, 0x00, 0x00, 0xFF));
   p_wgt->set_cfg(p_wgt, "text", "0/0");
   p_wnd->add_widget(p_wnd, p_wgt);
   yy = 2;
   xx += 22;
   /* Gene pool */
   /*p_wgt = gui_widget_create("gp_text", "text", xx, yy, 20, 20);
   p_wgt->set_cfg(p_wgt, "border", (void*)FALSE);
   p_wgt->set_cfg(p_wgt, "edit", (void*)FALSE);
   p_wgt->set_cfg(p_wgt, "text_color", (void*)GLX_RGBA(0x00, 0x00, 0x00, 0xFF));
   p_wgt->set_cfg(p_wgt, "text", "GP");*/
   p_wgt = gui_widget_create("gp_image", "image", xx, yy, 20, 20);
   p_wgt->set_cfg(p_wgt, "image", species_image_path[animal]);
   p_wnd->add_widget(p_wnd, p_wgt);
   yy = 20;
   p_wgt = gui_widget_create("gp", "text", xx, yy, 20, 20);
   p_wgt->set_cfg(p_wgt, "border", (void*)FALSE);
   p_wgt->set_cfg(p_wgt, "edit", (void*)FALSE);
   p_wgt->set_cfg(p_wgt, "text_color", (void*)GLX_RGBA(0x00, 0x00, 0x00, 0xFF));
   p_wgt->set_cfg(p_wgt, "text", "0");
   p_wnd->add_widget(p_wnd, p_wgt);
   yy = 0;
   xx += 22;
   /* Victory points */
   p_wgt = gui_widget_create("vp_text", "text", xx, yy, 20, 20);
   p_wgt->set_cfg(p_wgt, "border", (void*)FALSE);
   p_wgt->set_cfg(p_wgt, "edit", (void*)FALSE);
   p_wgt->set_cfg(p_wgt, "text_color", (void*)GLX_RGBA(0x00, 0x00, 0x00, 0xFF));
   p_wgt->set_cfg(p_wgt, "text", "VP");
   p_wnd->add_widget(p_wnd, p_wgt);
   yy = 20;
   p_wgt = gui_widget_create("vp", "text", xx, yy, 20, 20);
   p_wgt->set_cfg(p_wgt, "border", (void*)FALSE);
   p_wgt->set_cfg(p_wgt, "edit", (void*)FALSE);
   p_wgt->set_cfg(p_wgt, "text_color", (void*)GLX_RGBA(0x00, 0x00, 0x00, 0xFF));
   p_wgt->set_cfg(p_wgt, "text", "0");
   p_wnd->add_widget(p_wnd, p_wgt);
   animal++;
#endif
   return p_wnd;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void gui_piwnd_set_cfg(gui_wnd_t* p_me, char* cfg, void* data)
{
   REQUIRE(data != NULL);
   if (strcmp(cfg, "update") == 0) {
      char txt[40];
      glx_color_t color;
      int i;
      player_t* p_player = (player_t*)data;
      gui_widget_t* p_wgt;
      p_wgt = p_me->find_widget(p_me, "name_text");
      REQUIRE(p_wgt != NULL);
      switch (p_player->color)
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
      p_wgt->set_cfg(p_wgt, "text_color", (void*)color);
      p_wgt->set_cfg(p_wgt, "text", p_player->name);
      p_wgt = p_me->find_widget(p_me, "wealth");
      REQUIRE(p_wgt != NULL);
      sprintf(txt, "%d", p_player->wealth);
      p_wgt->set_cfg(p_wgt, "text", txt);
      p_wgt = p_me->find_widget(p_me, "prestige_text");
      REQUIRE(p_wgt != NULL);
      sprintf(txt, "P: %d", p_player->prestige);
      p_wgt->set_cfg(p_wgt, "text", txt);
      if (p_player == SLNK_NEXT(player_t, &core_get()->players_head))
      { /* Red border if this player */
         p_me->border_color = GLX_RGBA(0xFF, 0x00, 0x00, 0xFF);
         //p_me->border = TRUE;
      }
      else
      { /* Black border otherwise */
         p_me->border_color = GLX_RGBA(0x00, 0x00, 0x00, 0xFF);
         //p_me->border = FALSE;
      }
   }
}

/* END OF FILE ***************************************************************/
