/******************************************************************************
Copyright (c) 2013, All Rights Reserved.
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file us.c
\brief Urban Sprawl Board Game Client. */
/*---------------------------------------------------------------------------*/
/* INCLUDE FILES *************************************************************/
#include "sys_def.h"
#include "sys_assert.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include "gfw.h"
#include "glx.h"
#include "gui.h"
#include "gui_gbwnd.h"
#include "gui_log.h"
#include "net.h"
#include "net_us.h"
#include "net_client.h"
#include "trc.h"
#include "hsm.h"
#include "main_hsm.h"
#include "cfg.h"
#include "core.h"

/* CONSTANTS / MACROS ********************************************************/
//#define SCREEN_WIDTH 1200
//#define SCREEN_HEIGHT 700
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 800

/* LOCAL DATATYPES ***********************************************************/

/* LOCAL FUNCTION PROTOTYPES *************************************************/
static gfw_user_evt_fn_t user_evt_handler;
static gfw_user_poll_fn_t user_poll;
static trc_print_co_t trc_print;

/* MODULE CONSTANTS / VARIABLES **********************************************/
/*** Remove this comment if you want to use an ASSERT
SYS_ASSERT_FILE;
***/
SYS_DBC_FILE;  /*!< Defines the name of this source file once for all */

TRC_DEF(us);

/* ver strings */
static const char b_rev[] = "@(#) us_0_0_1";
static const char b_date[] = __DATE__;
static const char b_time[] = __TIME__;

static gfw_cb_t gfw_cb = {
   .on_update = gui_update,
   .on_draw = gui_draw,
   .on_key = gui_key_evt,
   .on_mouse = gui_mouse_evt,
   .on_user = user_evt_handler,
   .on_poll = user_poll
};

static char trc_buf[0x8000];

/* GLOBAL CONSTANTS / VARIABLES **********************************************/

/* GLOBAL FUNCTIONS **********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int main(int argc, char* argv[])
{
   glx_surface_t* screen;
   glx_font_t* tmpfont;
   int screen_width = SCREEN_WIDTH;
   int screen_height = SCREEN_HEIGHT;
   char key_val[20];
   TOUCH(argc);
   TOUCH(argv);

   /* Print program version, date and time */
   printf("%s %s %s\n", b_rev, b_date, b_time);

   TRC_INIT((char*)&trc_buf, 0x8000);
   trc_print_co_attach(trc_print);

   TRC_REG(us, TRC_ERROR | TRC_DEBUG);

   /* Initialize game framework (ie SDL init) */
   gfw_init();
   gfw_cb_attach(&gfw_cb);

   /* Read screen resolution from config file */
   if (cfg_get_key("us.ini", "screen_width", key_val, 20) > 0)
   {
      screen_width = atoi(key_val);
      TRC_DBG(us, "Screen width %d read from config file", screen_width);
   }
   if (cfg_get_key("us.ini", "screen_height", key_val, 20) > 0)
   {
      screen_height = atoi(key_val);
      TRC_DBG(us, "Screen height %d read from config file", screen_height);
   }

   /* Create a new window */
   screen = gfw_create_window("Urban Sprawl",
      screen_width, screen_height, 0);
   glx_init(screen);

   /* Load default font */
   tmpfont = glx_load_font("fonts/times.ttf", 16);
   if (tmpfont == NULL)
   {
      return 1;
   }

   gui_init();
   /* Add widget types */
   gui_gbwnd_init();
   gui_log_init();

   net_init();
   net_client_init();
   core_init(NULL, NULL);

   hsm_init();
   main_hsm_init();
   main_hsm_start();

   gfw_main_loop();

   return 0;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void assert(const char* test, const char* file, int line)
{
   printf("ASSERT %s %s %d", test, file, line);
   TRC_ERR(us, "ASSERT %s %s %d", test, file, line);
   exit(-1);
}

/* LOCAL FUNCTIONS ***********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void trc_print(const char* p_trc, size_t len)
{
   FILE* fp;
   fp = fopen("log.txt", "a");
   if (fp != NULL)
   {
      fwrite(p_trc, 1, len, fp);
      fclose(fp);
   }
   printf("%s", p_trc);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void user_evt_handler(gfw_evt_t* p_evt)
{
   if (p_evt->user.code == GFW_EVT_TYPE_GUI)
   {
      gui_handle_event(p_evt);
   }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void user_poll(void)
{
   net_poll();
}

/* END OF FILE ***************************************************************/
