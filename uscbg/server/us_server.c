/******************************************************************************
Copyright (c) 2013, All Rights Reserved.
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file us_server.c
\brief Urban Sprawl Board Game Server. */
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
#include <unistd.h>
#include "trc.h"
#include "hsm.h"
#include "server_hsm.h"
#include "net.h"
#include "net_us.h"
#include "net_server.h"
#include "core.h"

/* CONSTANTS / MACROS ********************************************************/

/* LOCAL DATATYPES ***********************************************************/

/* LOCAL FUNCTION PROTOTYPES *************************************************/

/* MODULE CONSTANTS / VARIABLES **********************************************/
/*** Remove this comment if you want to use an ASSERT
SYS_ASSERT_FILE;
***/
SYS_DBC_FILE;  /*!< Defines the name of this source file once for all */
/* ver strings */
static const char b_rev[] = "@(#) us_server_0_0_1";
static const char b_date[] = __DATE__;
static const char b_time[] = __TIME__;

static char trc_buf[0x8000];

/* GLOBAL CONSTANTS / VARIABLES **********************************************/

/* GLOBAL FUNCTIONS **********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int main(int argc, char* argv[])
{
   TOUCH(argc);
   TOUCH(argv);

   /* Print program version, date and time */
   printf("%s %s %s\n", b_rev, b_date, b_time);

   TRC_INIT((char*)&trc_buf, 0x8000);
   TRC_MASK_FILTER(0xffffffff);
   TRC_MODE_SET(TRC_MODE_PRINT);

   /* Start server */
   net_init();
   net_server_init();
   net_server_start();

   /* Seed the random-number generator with current time so that
   * the numbers will be different every time we run.
   */
   srand((unsigned)time( NULL ));
   //printf ("First number: %d\n", rand() % 100);

   core_init(net_server_send_cmd, net_server_broadcast_cmd);

   hsm_init();
   srv_hsm_init();
   srv_hsm_start();

   while(1)
   { /* Wait for commands */
      char ch = getchar();
      if ((ch == 0) || (ch == EOF))
      {
#ifdef a_x86win
         usleep(50000);
#else
#if 0
         struct timespec t_req, t_rem;
         t_req.tv_sec = 0;
         t_req.tv_nsec = 50000000; /* 50ms */
         while (nanosleep(&t_req, &t_rem) < 0)
         {
            t_req = t_rem;
         }
#endif
         usleep(50000);
#endif
      }
      else
      {
         if (ch == 'e')
         {
            exit(0);
         }
         /* Todo: Add command handler */
      }
   }
   //gfw_main_loop();

   return 0;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void assert(const char* test, const char* file, int line)
{
   printf("ASSERT %s %s %d", test, file, line);
   exit(-1);
}

/* LOCAL FUNCTIONS ***********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/

/* END OF FILE ***************************************************************/
