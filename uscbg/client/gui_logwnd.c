/******************************************************************************
Copyright (c) 2013, All Rights Reserved.
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file gui_logwnd.c
\brief The Goa logwnd implementation. */
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
static gui_wnd_set_cfg_fn_t gui_logwnd_set_cfg;

/* MODULE CONSTANTS / VARIABLES **********************************************/
SYS_ASSERT_FILE;
SYS_DBC_FILE;  /*!< Defines the name of this source file once for all */

/* GLOBAL CONSTANTS / VARIABLES **********************************************/

/* GLOBAL FUNCTIONS **********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void gui_logwnd_init(void)
{
}

/* LOCAL FUNCTIONS ***********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
gui_wnd_t* gui_logwnd_create(int x, int y, int w, int h, gui_wgt_evt_cb_t* p_cb)
{
   gui_wnd_t* p_wnd;
   gui_widget_t* p_wgt;
   int xx;
   int yy;
   p_wnd = gui_wnd_create(NULL, "logwnd", x, y, w, h, 0);
   p_wnd->set_cfg(p_wnd, "caption", "Game Log");
   p_wnd->set_cfg = gui_logwnd_set_cfg;
   p_wnd->border = TRUE;
   p_wnd->border_color = GLX_RGBA(0x80, 0x80, 0x80, 0xFF);
   p_wnd->bg_color = GLX_RGBA(0x40, 0x40, 0x40, 0x80);
   /* Create widgets */
   xx = 0;
   yy = 0;
   p_wgt = gui_widget_create("log", "gamelog", xx, yy, w,
      h - p_wnd->caption_offset_y);
   p_wgt->set_cfg(p_wgt, "cb_fn", p_cb);
   p_wnd->add_widget(p_wnd, p_wgt);
   return p_wnd;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void gui_logwnd_set_cfg(gui_wnd_t* p_me, char* cfg, void* data)
{
   if (strcmp(cfg, "add_entry") == 0)
   {
   }
}

/* END OF FILE ***************************************************************/
