/******************************************************************************
Copyright (c) 2013, All Rights Reserved.
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file gui_gbwnd.c
\brief The Urban Sprawl gameboard implementation. */
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
#include "GL/glu.h"
#include "scf.h"
#include "trc.h"
#include "gui.h"
#include "core.h"

/* CONSTANTS / MACROS ********************************************************/

/* LOCAL DATATYPES ***********************************************************/
typedef void gui_gbwnd_draw_fn_t(gui_widget_t* p_me, bool_t select);

typedef struct
{
   gui_widget_t base;
   glx_image_t* gb_img;
   glx_image_t* buildings_img[5*4];
   glx_image_t* vocations_img[8];
   glx_image_t* planning_cards_img[5];
   glx_image_t* contract_cards_img[8];
   glx_image_t* player_cubes_img[4];
   int offset_x;
   int offset_y;
   float zoom;
   bool_t rmb_down;
   uint32_t pos_x;
   uint32_t pos_y;
} gui_board_t;

/* LOCAL FUNCTION PROTOTYPES *************************************************/
static gui_wgt_create_fn_t gui_board_create;
static gui_wgt_free_fn_t gui_board_free;
static gui_wgt_set_cfg_fn_t gui_board_set_cfg;
static gui_wgt_get_cfg_fn_t gui_board_get_cfg;
static gui_wgt_draw_fn_t gui_board_draw;
static void gui_board_draw_blocks(gui_widget_t* p_me, bool_t select);
//static void gui_board_draw_elements(gui_widget_t* p_me, bool_t select);
//static void gui_board_draw_action_display(gui_widget_t* p_me, bool_t select);
//static void gui_board_draw_wanderlust_tiles(gui_widget_t* p_me, bool_t select);
static void gui_board_draw_cards(gui_widget_t* p_me, bool_t select);
//static void gui_board_draw_hexagon(float w, float h, float zoom);
static gui_wgt_mouse_fn_t gui_board_handle_mouse;
static int gui_board_handle_selection(gui_widget_t* p_me, int x, int y,
   gui_gbwnd_draw_fn_t* fn);

/* MODULE CONSTANTS / VARIABLES **********************************************/
SYS_ASSERT_FILE;
SYS_DBC_FILE;  /*!< Defines the name of this source file once for all */

TRC_DEF(gui_gbwnd);

static gui_widget_type_t widget_type = {
   .type = "board",
   .create = gui_board_create
};

static char* buildings_image_path[] = {
   "data/1-Civ.png",
   "data/2-civ.png",
   "data/3-civ.png",
   "data/4-civ.png",
   "data/1-Com.png",
   "data/2-com.png",
   "data/3-com.png",
   "data/4-com.png",
   "data/1-Ind.png",
   "data/2-ind.png",
   "data/3-ind.png",
   "data/4-ind.png",
   "data/1-Res.png",
   "data/2-res.png",
   "data/3-res.png",
   "data/4-res.png",
   "data/1-Park.png",
   "data/2-park.png",
   "data/3-park.png",
   "data/4-park.png"
};

static char* player_cubes_image_path[] = {
   "data/block_black.png",
   "data/block_green.png",
   "data/block_pink.png",
   "data/block_white.png"
};

static char* vocations_image_path[VOCATION_LAST] = {
   "data/Vocation-finance.png",
   "data/Vocation-transport.png",
   "data/Vocation-education.png",
   "data/Vocation-energy.png"
   "data/Vocation-tour.png"
   "data/Vocation-factory.png"
   "data/Vocation-public.png"
   "data/Vocation-media.png"
};

static const glx_rect_t initiative_boxes[6] =
{
   {1399,149,42,42},{1447,149,42,42},{1495,149,42,42},
   {1543,149,42,42},{1591,149,42,42},{1639,149,42,42}
};

static const glx_rect_t card_boxes[5] =
{
   {1743,114,146,201},{1743,321,146,201},{1743,526,146,201},
   {1743,732,146,201},{1743,939,146,201}
};

/* GLOBAL CONSTANTS / VARIABLES **********************************************/

/* GLOBAL FUNCTIONS **********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void gui_gbwnd_init(void)
{
   TRC_REG(gui_gbwnd, TRC_DEBUG);
   gui_widget_type_reg(&widget_type);
}

/* LOCAL FUNCTIONS ***********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
gui_wnd_t* gui_gbwnd_create(void)
{
   gui_wnd_t* p_wnd;
   gui_widget_t* p_wgt;
   p_wnd = gui_wnd_create(NULL, "gbwnd", 0, 0, glx_get()->w,
      glx_get()->h, 0);
   REQUIRE(p_wnd != NULL);
   p_wnd->bg_color = GLX_RGBA(0x00, 0x00, 0x00, 0xFF);
   //p_wnd->set_cfg(p_wnd, "bg_image", "data/wood_bg.png");
   /* Create board widget */
   p_wgt = gui_widget_create("board", "board", 0, 0,
      p_wnd->w, p_wnd->h);
   REQUIRE(p_wgt != NULL);
   p_wnd->add_widget(p_wnd, p_wgt);
   return p_wnd;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static gui_widget_t* gui_board_create(void)
{
   gui_board_t* p_board = (gui_board_t*)calloc(1, sizeof(gui_board_t));
   int i = 1;
   REQUIRE(p_board != NULL);
   p_board->base.set_cfg = gui_board_set_cfg;
   p_board->base.get_cfg = gui_board_get_cfg;
   p_board->base.on_draw = gui_board_draw;
   p_board->base.on_mouse = gui_board_handle_mouse;
   p_board->base.free = gui_board_free;
   p_board->gb_img = glx_load_image_v2("data/Map.jpg", FALSE);
   REQUIRE(p_board->gb_img != NULL);
   for (i=0;i<20;i++)
   {
      p_board->buildings_img[i] = glx_load_image(buildings_image_path[i]);
      REQUIRE(p_board->buildings_image_path[i] != NULL);
   }
   for (i=0;i<PLAYER_COLOR_LAST;i++)
   {
      p_board->player_cubes_img[i] = glx_load_image(player_cubes_image_path[i]);
      REQUIRE(p_board->player_cubes_img[i] != NULL);
   }
   /*p_board->tile_mark = glx_load_image("data/EarthTile_Mark.png");
   REQUIRE(p_board->tile_mark != NULL);
   p_board->element_ap_mark = glx_load_image("data/Element_Mark.png");
   REQUIRE(p_board->element_ap_mark != NULL);*/
   /*for (i=0;i<5;i++)
   {
      char path[64];
      sprintf(path, "data/marker_red_%d.png", i+1);
      p_board->auction_marker_img[0][i] = glx_load_image(path);
      REQUIRE(p_board->auction_marker_img[0][i] != NULL);
      sprintf(path, "data/marker_blue_%d.png", i+1);
      p_board->auction_marker_img[1][i] = glx_load_image(path);
      REQUIRE(p_board->auction_marker_img[1][i] != NULL);
      sprintf(path, "data/marker_green_%d.png", i+1);
      p_board->auction_marker_img[2][i] = glx_load_image(path);
      REQUIRE(p_board->auction_marker_img[2][i] != NULL);
      sprintf(path, "data/marker_yellow_%d.png", i+1);
      p_board->auction_marker_img[3][i] = glx_load_image(path);
      REQUIRE(p_board->auction_marker_img[3][i] != NULL);
   }*/
   p_board->zoom = 1.0;
   return &p_board->base;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void gui_board_free(gui_widget_t* p_me)
{
   gui_board_t* p_board = (gui_board_t*)p_me;
   int i;
   REQUIRE(p_board != NULL);
   glx_free_image(p_board->gb_img);
   for (i=0;i<5*4;i++)
   {
      if (p_board->buildings_img[i] != NULL)
      {
         glx_free_image(p_board->buildings_img[i]);
      }
   }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void gui_board_set_cfg(gui_widget_t* p_me, char* cfg, void* data)
{
   gui_board_t* p_board = (gui_board_t*)p_me;
   REQUIRE(p_board != NULL);
   if (strcmp(cfg, "update") == 0) {
      int i;
      /*for (i=0;i<5;i++)
      {
         card_t* p_card = core_get()->board_cards[i];
         if (p_board->cards_img[i] != NULL)
         {
            glx_free_image(p_board->cards_img[i]);
            p_board->cards_img[i] = NULL;
         }
         if (p_card != NULL)
         {
            char* path = cards_get_image_path(p_card);
            p_board->cards_img[i] = glx_load_image(path);
         }
      }*/
   }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void* gui_board_get_cfg(gui_widget_t* p_me, char* cfg)
{
   gui_board_t* p_board = (gui_board_t*)p_me;
   void* data = 0;
   REQUIRE(p_board != NULL);
   if (strcmp(cfg, "x") == 0) {
      data = (void*)p_board->pos_x;
   } else if (strcmp(cfg, "y") == 0) {
      data = (void*)p_board->pos_y;
   }
   return data;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void gui_board_draw(gui_widget_t* p_me)
{
   gui_board_t* p_board = (gui_board_t*)p_me;
   //glx_rect_t srcrect;
   glx_rect_t dstrect;
   glx_image_t* p_img = NULL;
   int i;
   glPushMatrix();
   glTranslatef(-p_board->offset_x, -p_board->offset_y, 0);
   glScalef(p_board->zoom, p_board->zoom, 1.0f);
   /* Draw game board */
   /*glx_rect_set(&srcrect, p_board->offset_x/p_board->zoom,
      p_board->offset_y/p_board->zoom, p_me->w/p_board->zoom,
      p_me->h/p_board->zoom);*/
   glx_rect_set(&dstrect, 0, 0, p_board->gb_img->w, p_board->gb_img->h);
   glx_drawimage(p_board->gb_img, NULL, &dstrect);
   /* Draw tiles, species and domination markers on "earth" */
   gui_board_draw_blocks(p_me, FALSE);
   //glx_rect_set(&dstrect, p_me->x + 40 + 80*i, p_me->y, 1, 480);
   //glx_drawrect(&dstrect, GLX_RGBA(0x80, 0x80, 0x80, 0xff));
   /* Draw cards */
   //gui_board_draw_cards(p_me, FALSE);
   glPopMatrix();
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void gui_board_draw_blocks(gui_widget_t* p_me, bool_t select)
{
   gui_board_t* p_board = (gui_board_t*)p_me;
   glx_rect_t dstrect;
   glx_image_t* p_img = NULL;
   int i;
   float x;
   float y;
   for (i=0;i<MAX_BOARD_BLOCKS;i++)
   {
      int j;
      block_t* p_blk = &core_get()->board_blocks[i];
      int w = 74;
      int h = 74;
      int offset_x = 93;
      int offset_y = 93;
      p_img = NULL;
      if (((p_blk->n_buildings == 0) && !p_blk->lots_marked) ||
          (!p_blk->lots_marked && select))
      {
         continue;
      }
      x = 430 + (i%6)*offset_x;
      y = 279 + (i/6)*offset_y;
      glPushMatrix();
      glTranslatef(x, y, 0);
      if ((p_blk->n_buildings > 0) && !select)
      {
         for (j=0;j<p_blk->n_buildings;j++)
         {
            building_t* p_bld = &p_blk->buildings[j];
            p_img = p_board->buildings_img[p_bld->zone*4 + (p_bld->size - 1)];
            glBindTexture(GL_TEXTURE_2D, p_img->texid);
            /* Draw building */
            if (p_bld->size == 1)
            {
               int lot;
               if (p_bld->block_pos & 0x1) lot = 0;
               if (p_bld->block_pos & 0x2) lot = 1;
               if (p_bld->block_pos & 0x4) lot = 2;
               if (p_bld->block_pos & 0x8) lot = 3;
               glx_rect_set(&dstrect, (lot%2)*w/2, (lot/2)*h/2, w/2, h/2);
               glx_drawimage(p_img, NULL, &dstrect);
            }
            if (p_bld->size == 3)
            {
               glx_rect_set(&dstrect, 0, 0, w, h);
               glx_drawimage(p_img, NULL, &dstrect);
            }
         }
      }
      for (j=0;j<4;j++)
      {
         if (p_blk->lots_marked & (1u << j))
         {
            if (select)
            {
               glLoadName(i*4 + j + 1);
            }
            glx_rect_set(&dstrect, (j%2)*w/2, (j/2)*h/2, w/2, h/2);
            glx_drawrect(&dstrect, GLX_RGBA(0x00, 0x80, 0x00, 0x80));
         }
      }
      glPopMatrix();
   }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void gui_board_draw_cards(gui_widget_t* p_me, bool_t select)
{
   gui_board_t* p_board = (gui_board_t*)p_me;
   glx_rect_t dstrect;
   glx_image_t* p_img = NULL;
   int i;
   for (i=0;i<5;i++)
   {
      //p_img = p_board->cards_img[i];
      dstrect = card_boxes[i];
      if (select && p_img)
      {
         glLoadName(i+1);
      }
      if (p_img)
      {
         glx_drawimage(p_img, NULL, &dstrect);
      }
      /*if (core_get()->board_cards_mark)
      {
         glx_drawrect(&dstrect, GLX_RGBA(0x00, 0x80, 0x0, 0x40));
      }*/
   }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void gui_board_draw_hexagon(float w, float h, float zoom)
{
   glBegin(GL_POLYGON);
      /* Hexagon tile */
      glTexCoord2f(0.25f,0.0f);
      glVertex3f(-0.25f*w, -0.5f*h, 0.0f);
      glTexCoord2f(0.0f,0.5f);
      glVertex3f(-0.5f*w, 0.0f, 0.0f);
      glTexCoord2f(0.25f,1.0f);
      glVertex3f(-0.25f*w, 0.5f*h, 0.0f);
      glTexCoord2f(0.75f,1.0f);
      glVertex3f(0.25f*w, 0.5f*h, 0.0f);
      glTexCoord2f(1.0f,0.5f);
      glVertex3f(0.5f*w, 0.0f, 0.0f);
      glTexCoord2f(0.75f,0.0f);
      glVertex3f(0.25f*w, -0.5f*h, 0.0f);
   glEnd();
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void gui_board_handle_mouse(gui_widget_t* p_me, gfw_evt_t* p_evt)
{
   gui_board_t* p_board = (gui_board_t*)p_me;
   bool_t update_offset = FALSE;
   REQUIRE(p_board != NULL);
   if (p_evt->type == SDL_MOUSEBUTTONDOWN)
   {
      TRC_DBG(gui_gbwnd, "button %d", p_evt->button.button);
   }
   if ((p_evt->type == SDL_MOUSEBUTTONDOWN) &&
       (p_evt->button.button == SDL_BUTTON_LEFT))
   { /* Check for any matching "button" areas */
      int x = p_evt->button.x - p_me->x;
      int y = p_evt->button.y - p_me->y;
      int id;
      TRC_DBG(gui_gbwnd, "mouse x,y (%d, %d)", x, y);
      id = gui_board_handle_selection(p_me, x, y, gui_board_draw_blocks);
      if (id > 0)
      {
         /*tile_t* p_tile = &core_get()->board_tiles[id-1]; */
         core_get()->board_lot_selection = id-1;
         TRC_DBG(gui_gbwnd, "lot %d, block %)", id-1, (id-1)/4);
         gui_send_event(p_me, "board_lot");
      }
#if 0
      if (id < 0)
      {
         id = gui_board_handle_selection(p_me, x, y,
            gui_board_draw_cards);
         if (id > 0)
         {
            card_t* p_card = core_get()->board_cards[id-1];
            core_get()->card_selection = id-1;
            core_get()->current_card = p_card;
            TRC_DBG(gui_gbwnd, "board card id %d", id-1);
            gui_send_event(p_me, "card");
         }
      }
#endif
   }
   if ((p_evt->type == SDL_MOUSEBUTTONDOWN) &&
       (p_evt->button.button == SDL_BUTTON_RIGHT))
   {
      p_board->rmb_down = TRUE;
   }
   if ((p_evt->type == SDL_MOUSEBUTTONUP) &&
       (p_evt->button.button == SDL_BUTTON_RIGHT))
   {
      p_board->rmb_down = FALSE;
   }
   if ((p_evt->type == SDL_MOUSEBUTTONDOWN) &&
       (p_evt->button.button == 4))
   { /* Mouse wheel down */
      p_board->zoom += 0.1f;
      if (p_board->zoom > 1.0f)
      {
         p_board->zoom = 1.0f;
      }
      update_offset = TRUE;
      TRC_DBG(gui_gbwnd, "zoom %f", p_board->zoom);
   }
   if ((p_evt->type == SDL_MOUSEBUTTONDOWN) &&
       (p_evt->button.button == 5))
   { /* Mouse wheel up */
      p_board->zoom -= 0.1f;
      if ((float)p_board->zoom*p_board->gb_img->h < (float)p_me->h)
      {
         TRC_DBG(gui_gbwnd, "gb height %f, window height %d",
            (float)p_board->zoom*p_board->gb_img->h, p_me->h);
         p_board->zoom += 0.1f;
      }
      update_offset = TRUE;
      TRC_DBG(gui_gbwnd, "zoom %f", p_board->zoom);
   }
   if (p_evt->type == SDL_MOUSEMOTION)
   {
      if (p_board->rmb_down)
      {
         p_board->offset_x -= p_evt->motion.xrel;
         p_board->offset_y -= p_evt->motion.yrel;
         update_offset = TRUE;
         //TRC_DBG(gui_gbwnd, "xrel %d, yrel %d", p_evt->motion.xrel,
         //   p_evt->motion.yrel);
      }
   }
   if (update_offset)
   {
      int max_ofs_x = p_board->gb_img->w*p_board->zoom - p_me->w;
      int max_ofs_y = p_board->gb_img->h*p_board->zoom - p_me->h;
      if (p_board->offset_x > max_ofs_x) {
         p_board->offset_x = max_ofs_x;
      }
      if (p_board->offset_y > max_ofs_y) {
         p_board->offset_y = max_ofs_y;
      }
      if (p_board->offset_x < 0) {
         p_board->offset_x = (max_ofs_x < 0)?max_ofs_x/2:0;
      }
      if (p_board->offset_y < 0) {
         p_board->offset_y = (max_ofs_y < 0)?max_ofs_y/2:0;;
      }
   }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static int gui_board_handle_selection(gui_widget_t* p_me, int x, int y,
   gui_gbwnd_draw_fn_t* fn)
{
   gui_board_t* p_board = (gui_board_t*)p_me;
   int id = -1;
   GLuint buffer[512]; /* Set up a selection buffer */
   GLint hits; /* The number of objects that we selected */
   /* The size of the viewport. [0] is <x>, [1] is <y>, [2] is <length>,
      [3] is <width> */
   GLint viewport[4];
   /* This sets the array <viewport> to the size and location of the screen
      relative to the window */
   glGetIntegerv(GL_VIEWPORT, viewport);
   /* Tell OpenGL to use our array for selection */
   glSelectBuffer(512, buffer);
   /* Puts OpenGL in selection mode. nothing will be drawn.
      Object id's and extents are stored in the buffer. */
   (void)glRenderMode(GL_SELECT);
   glInitNames(); /* Initializes the name stack */
   glPushName(0); /* Push 0 (at least one entry) onto the stack */
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity(); /* Resets the matrix */
   /* This creates a matrix that will zoom up to a small portion of the
      screen, where the mouse is. */
   gluPickMatrix((GLdouble)x, (GLdouble)(viewport[3]-y), 1.0f, 1.0f, viewport);
   /* Apply the perspective matrix */
   glOrtho(0, (GLfloat)(viewport[2]-viewport[0]),
      (GLfloat)(viewport[3]-viewport[1]), 0, -1.0f, 1.0f);
   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glTranslatef(-p_board->offset_x, -p_board->offset_y, 0);
   glScalef(p_board->zoom, p_board->zoom, 1.0f);
   fn(p_me, TRUE);
   glPopMatrix();
   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glMatrixMode(GL_MODELVIEW);
    /* Switch to render mode, find out how many objects were drawn
       where the mouse was. */
   hits = glRenderMode(GL_RENDER);
   if (hits > 0)
   {
      id = buffer[3]; /* Make our selection the first object */
      //int depth = buffer[1]; /* Store how far away it is */
   }
   return id;
}

/* END OF FILE ***************************************************************/
