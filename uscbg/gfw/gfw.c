/******************************************************************************
Copyright (c) 2013, All Rights Reserved.
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file gfw.c
\brief The gfw implementation. */
/*---------------------------------------------------------------------------*/
/* INCLUDE FILES *************************************************************/
#include "sys_def.h"
#include "sys_assert.h"
#include <malloc.h>
#include <stdio.h>
#include <time.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <GL/gl.h>
#include "slnk.h"
#include "gfw.h"

/* CONSTANTS / MACROS ********************************************************/

/* LOCAL DATATYPES ***********************************************************/
#if 0
typedef struct
{
   slnk_t slnk;
   gfw_evt_cb_fn_t* fn;
   void* p_data;
   int timeleft;
} gfw_evt_item_t;
#endif

typedef struct
{
   SDL_Surface* screen;
   gfw_cb_t* p_cb;
} gfw_t;

/* LOCAL FUNCTION PROTOTYPES *************************************************/
uint32_t gfw_tmr_cb(uint32_t interval, void *param);

/* MODULE CONSTANTS / VARIABLES **********************************************/
/*** Remove this comment if you want to use an ASSERT
SYS_ASSERT_FILE;
***/
SYS_DBC_FILE;  /*!< Defines the name of this source file once for all */

static gfw_t gfw;

/* GLOBAL CONSTANTS / VARIABLES **********************************************/

/* GLOBAL FUNCTIONS **********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void gfw_init(void)
{
   /* Seed the random-number generator with current time so that
   * the numbers will be different every time we run.
   */
   srand( (unsigned)time( NULL ) );

   /* initialize SDL video */
   if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0 )
   {
      printf("Unable to init SDL: %s\n", SDL_GetError());
      exit(1);
   }
   /* make sure SDL cleans up before exit */
   atexit(SDL_Quit);

   /* initialize SDL TTF */
   if(TTF_Init()==-1)
   {
      printf("Error: unable to initialize TTF_SDL, %s\n", TTF_GetError());
      exit(1);
   }
   atexit(TTF_Quit);

   SDL_EnableUNICODE(1);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void gfw_cb_attach(gfw_cb_t* p_cb)
{
   REQUIRE(p_cb != NULL);
   gfw.p_cb = p_cb;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
SDL_Surface* gfw_create_window(char* title, int w, int h, unsigned long flags)
{
   TOUCH(title);
   TOUCH(flags);
   SDL_GL_SetAttribute(SDL_GL_RED_SIZE,            8);
   SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,          8);
   SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,           8);
   SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,          8);
   SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,          24);
   SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE,         32);
/*   SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE,      8);
   SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE,    8);
   SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE,     8);
   SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE,    8);
   SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,  1);

   SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,  2);*/

#ifdef a_x86win
   SDL_WM_SetCaption(title, NULL);
#endif

   gfw.screen = SDL_SetVideoMode(w, h, 32,
      SDL_HWSURFACE | SDL_GL_DOUBLEBUFFER | SDL_OPENGL);
   if (!gfw.screen)
   {
      printf("Unable to set video mode: %s\n", SDL_GetError());
      return NULL;
   }
   glShadeModel(GL_SMOOTH);               /* Enable smooth shading */
   glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  /* Black background */
   glClearDepth(1.0f);                    /* Depth buffer setup */
   glEnable(GL_DEPTH_TEST);               /* Enables depth testing */
   glDepthFunc(GL_LEQUAL);                /* The type of depth testing to do */
   glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
   glEnable(GL_BLEND);

   glViewport(0, 0, w, h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(0, w, h, 0, 1, -1);
   glMatrixMode(GL_MODELVIEW);
   glEnable(GL_TEXTURE_2D);
   glLoadIdentity();
   return gfw.screen;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int gfw_main_loop(void)
{
   bool_t done = FALSE;
   /* program main loop */
   while (!done)
   {
      /* message processing loop */
      SDL_Event event;
      while (SDL_PollEvent(&event))
      {
         /* check for messages */
         switch (event.type)
         {
            /* exit if the window is closed */
            case SDL_QUIT:
               done = TRUE;
               break;

            /* check for keypresses */
            case SDL_KEYDOWN:
            //case SDL_KEYUP:
            {
               #if 0
               /* exit if ESCAPE is pressed */
               if (event.key.keysym.sym == SDLK_ESCAPE)
                  done = TRUE;
               else
               #endif
                  gfw.p_cb->on_key(&event);
               break;
            case SDL_MOUSEMOTION:
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                /*printf("Mouse moved by %d,%d to (%d,%d)\n",
                       event.motion.xrel, event.motion.yrel,
                       event.motion.x, event.motion.y);*/
                /*printf("Mouse button %d pressed at (%d,%d)\n",
                       event.button.button, event.button.x, event.button.y);*/
               gfw.p_cb->on_mouse(&event);
               break;
            case SDL_USEREVENT:
               gfw.p_cb->on_user(&event);
               break;
            }
         }
      }
      gfw.p_cb->on_update(SDL_GetTicks());

      /* DRAWING STARTS HERE */

      /* clear screen */
      //glx_drawrect(gfx_get()->scr, NULL, GLX_RGBA(0,0,0,0xff));
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glLoadIdentity();

      gfw.p_cb->on_draw();

      /* DRAWING ENDS HERE */

      /* finally, update the screen :) */
      //SDL_Flip(gfw.screen);
      SDL_GL_SwapBuffers();

      /* Poll misc here */
      gfw.p_cb->on_poll();

      SDL_Delay(30);
   }
   return 0;
}

#if 0
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void gfw_poll_events(long time)
{
   gfw_evt_item_t* p_item = SLNK_NEXT(gfw_evt_item_t, &evt_list_head);
   while(p_item != NULL)
   {
      gfw_evt_item_t* p_tmp = SLNK_NEXT(gfw_evt_item_t, p_item);
      if (p_item->timeleft <= 0)
      {
         SLNK_REMOVE(&evt_list_head, p_item);
         p_item->fn(p_item->p_data);
         free(p_item);
      }
      else
      {
         p_item->timeleft -= time;
      }
      p_item = p_tmp;
   }
}
#endif

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void gfw_tmr_evt(unsigned long time)
{
   SDL_AddTimer(time, gfw_tmr_cb, NULL);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void gfw_post_evt(uint8_t type, void* p_data)
{
   SDL_Event event;

   event.type = SDL_USEREVENT;
   event.user.code = type;
   event.user.data1 = p_data;
   event.user.data2 = 0;

   SDL_PushEvent(&event);
}

/* LOCAL FUNCTIONS ***********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
uint32_t gfw_tmr_cb(uint32_t interval, void *param)
{
   SDL_Event event;

   event.type = SDL_USEREVENT;
   event.user.code = GFW_EVT_TYPE_TIMER;
   event.user.data1 = NULL;
   event.user.data2 = NULL;

   SDL_PushEvent(&event);
   return 0;
}

/* END OF FILE ***************************************************************/
