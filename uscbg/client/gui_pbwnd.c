/******************************************************************************
Copyright (c) 2013, All Rights Reserved.
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file gui_pbwnd.c
\brief The Urban Sprawl playerboard implementation. */
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
static gui_wnd_set_cfg_fn_t gui_pbwnd_set_cfg;

/* MODULE CONSTANTS / VARIABLES **********************************************/
SYS_ASSERT_FILE;
SYS_DBC_FILE;  /*!< Defines the name of this source file once for all */

static char* politicians_image_path[POLITICIAN_LAST] = {
   "data/Politician-Mayor.png",
   "data/Politician-District_Attorney.png",
   "data/Politician-Treasurer.png",
   "data/Politician-Police_Chief.png",
   "data/Politician-Union_Boss.png",
   "Politician-Contractor.png"
};

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

static const char* card_widget_names[] = {
   "card_1", "card_2", "card_3", "card_4",
   "card_5", "card_6", "card_7", "card_8"
};

static const char* politician_widget_names[] = {
   "politician_1", "politician_2", "politician_3", "politician_4",
   "politician_5"
};

static const char* vocation_widget_names[] = {
   "vocation_1", "vocation_2", "vocation_3", "vocation_4",
   "vocation_5", "vocation_6", "vocation_7", "vocation_8"
};

/* GLOBAL CONSTANTS / VARIABLES **********************************************/

/* GLOBAL FUNCTIONS **********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void gui_pbwnd_init(void)
{
}

/* LOCAL FUNCTIONS ***********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
gui_wnd_t* gui_pbwnd_create(int x, int y, int w, int h, gui_wgt_evt_cb_t* p_cb)
{
   gui_wnd_t* p_wnd;
   gui_widget_t* p_wgt;
   int xx;
   int yy;
   int i;

   p_wnd = gui_wnd_create(NULL, "pbwnd", x, y, w, h, 0);
   REQUIRE(p_wnd != NULL);
   p_wnd->set_cfg(p_wnd, "caption", "Player Board");
   p_wnd->set_cfg(p_wnd, "bg_image", "data/Map-H.png");
   p_wnd->set_cfg = gui_pbwnd_set_cfg;
   p_wnd->bg_color = GLX_RGBA(0x00, 0x00, 0x00, 0xFF);
   p_wnd->border = TRUE;
   p_wnd->border_color = GLX_RGBA(0x00, 0x00, 0x00, 0xFF);
   /* Create card widgets */
   xx = 160;
   yy = 9;
   for (i=0;i<6;i++)
   {
      p_wgt = gui_widget_create(card_widget_names[i], "image", xx, yy,
         131, 181);
      p_wgt->visible = FALSE;
      p_wgt->set_cfg(p_wgt, "cb_fn", p_cb);
      p_wnd->add_widget(p_wnd, p_wgt);
      xx += 150;
   }
   /* Create politician widgets */
   xx = 10;
   yy = 341;
   for (i=0;i<5;i++)
   {
      p_wgt = gui_widget_create(politician_widget_names[i], "image", xx, yy,
         129, 129);
      p_wgt->visible = FALSE;
      p_wgt->set_cfg(p_wgt, "image", politicians_image_path[i]);
      p_wgt->set_cfg(p_wgt, "cb_fn", p_cb);
      p_wnd->add_widget(p_wnd, p_wgt);
      xx += 131;
   }
   /* Create vocation widgets */
   xx = 676;
   yy = 406;
   for (i=0;i<8;i++)
   {
      p_wgt = gui_widget_create(vocation_widget_names[i], "image", xx, yy,
         44, 44);
      p_wgt->visible = FALSE;
      p_wgt->set_cfg(p_wgt, "image", vocations_image_path[i]);
      p_wgt->set_cfg(p_wgt, "cb_fn", p_cb);
      p_wnd->add_widget(p_wnd, p_wgt);
      xx += 45;
   }
   return p_wnd;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void gui_pbwnd_set_cfg(gui_wnd_t* p_me, char* cfg, void* data)
{
   REQUIRE(data != NULL);
   if (strcmp(cfg, "update") == 0) {
      player_t* p_player = (player_t*)data;
      gui_widget_t* p_wgt;
      card_t* p_card = SLNK_NEXT(card_t, &p_player->cards_head);
      int i;
      /* Add card(s) */
      for (i=0;i<6;i++)
      {
         char wgt_name[10];
         sprintf(wgt_name, "card_%d", i + 1);
         p_wgt = p_me->find_widget(p_me, wgt_name);
         REQUIRE(p_wgt != NULL);
         if (p_card != NULL)
         {
            p_wgt->set_cfg(p_wgt, "image", p_card->img_path);
            p_wgt->visible = TRUE;
            p_card = SLNK_NEXT(card_t, p_card);
         }
         else
         {
            //p_wgt->set_cfg(p_wgt, "image", NULL);
            p_wgt->visible = FALSE;
         }
      }
      /* Show/Hide politician(s) */
      for (i=0;i<5;i++)
      {
         char wgt_name[10];
         sprintf(wgt_name, "politician_%d", i + 1);
         p_wgt = p_me->find_widget(p_me, wgt_name);
         REQUIRE(p_wgt != NULL);
         if (p_player->politicians & (1u << i))
         {
            p_wgt->visible = TRUE;
         }
         else
         {
            p_wgt->visible = FALSE;
         }
      }
      /* Show/Hide vocation(s) */
      for (i=0;i<8;i++)
      {
         char wgt_name[10];
         sprintf(wgt_name, "vocation_%d", i + 1);
         p_wgt = p_me->find_widget(p_me, wgt_name);
         REQUIRE(p_wgt != NULL);
         p_wgt->visible = FALSE;
      }
      for (i=0;i<24;i++)
      {
         char wgt_name[10];
         if (p_player->vocations & (1u << i))
         {
            vocation_t vocation = core_vocbit2voc(i);
            sprintf(wgt_name, "vocation_%d", vocation + 1);
            p_wgt = p_me->find_widget(p_me, wgt_name);
            REQUIRE(p_wgt != NULL);
            p_wgt->visible = TRUE;
         }
      }
   }
}

/* END OF FILE ***************************************************************/
