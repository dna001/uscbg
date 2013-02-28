/******************************************************************************
Copyright (c) 2013, All Rights Reserved.
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file gui_cardwnd.c
\brief The Urban Sprawl (active) card window implementation. */
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
static gui_wnd_set_cfg_fn_t gui_cardwnd_set_cfg;

/* MODULE CONSTANTS / VARIABLES **********************************************/
SYS_ASSERT_FILE;
SYS_DBC_FILE;  /*!< Defines the name of this source file once for all */

/* GLOBAL CONSTANTS / VARIABLES **********************************************/

/* GLOBAL FUNCTIONS **********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void gui_cardwnd_init(void)
{
}

/* LOCAL FUNCTIONS ***********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
gui_wnd_t* gui_cardwnd_create(int x, int y, int w, int h, gui_wgt_evt_cb_t* p_cb)
{
   gui_wnd_t* p_wnd;
   gui_widget_t* p_wgt;
   //static int color = 0;

   p_wnd = gui_wnd_create(NULL, "cardwnd", x, y, w, h, 0);
   REQUIRE(p_wnd != NULL);
   p_wnd->set_cfg = gui_cardwnd_set_cfg;
   p_wnd->bg_color = GLX_RGBA(0x40, 0x40, 0x40, 0xFF);
   p_wnd->border = TRUE;
   p_wnd->border_color = GLX_RGBA(0xC0, 0xC0, 0xC0, 0xFF);
   /* Card */
   p_wgt = gui_widget_create("card_image", "image", 5, 5, w - 10, h - 10);
   p_wnd->add_widget(p_wnd, p_wgt);
   return p_wnd;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void gui_cardwnd_set_cfg(gui_wnd_t* p_me, char* cfg, void* data)
{
   REQUIRE(data != NULL);
   if (strcmp(cfg, "update") == 0) {
      char txt[40];
      glx_color_t color;
      player_t* p_player = (player_t*)data;
      gui_widget_t* p_wgt;
      card_t* p_card = core_get()->current_contract_card;
      REQUIRE(p_card != NULL);
      p_wgt = p_me->find_widget(p_me, "card_image");
      REQUIRE(p_wgt != NULL);
      p_wgt->set_cfg(p_wgt, "image", p_card->img_path);
   }
}

/* END OF FILE ***************************************************************/
