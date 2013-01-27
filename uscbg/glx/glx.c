/******************************************************************************
Copyright (c) 2013, All Rights Reserved.
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file glx.c
\brief The glx implementation. */
/*---------------------------------------------------------------------------*/
/* INCLUDE FILES *************************************************************/
#include "sys_def.h"
#include "sys_assert.h"
#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include "slnk.h"
#include "trc.h"
#include "glx.h"

/* CONSTANTS / MACROS ********************************************************/
#define RGBA_R(x) (x >> 24)
#define RGBA_G(x) ((x >> 16) & 0xFF)
#define RGBA_B(x) ((x >> 8) & 0xFF)
#define RGBA_A(x) (x & 0xFF)

#define GLX_RES_TYPE_TEXTURE 0x01
#define GLX_RES_TYPE_FONT    0x02

/* LOCAL DATATYPES ***********************************************************/
typedef struct
{
   slnk_t slnk;
   uint8_t type;
   char* path;
   int tex_id;
   int w;
   int h;
   void* p_data;
   int ref_count;
} glx_res_t;

/* LOCAL FUNCTION PROTOTYPES *************************************************/
//static glx_font_t* find_font(int id);
//STATIC rm_add_fn_t rm_texture_add;
//STATIC rm_free_fn_t rm_texture_free;
//STATIC rm_add_fn_t rm_font_add;
//STATIC rm_free_fn_t rm_font_free;
static void glx_res_add(uint8_t type, char* path, void* p_data);
static void glx_res_rm(uint8_t type, void* p_data);
static glx_res_t* glx_res_find(uint8_t type, char* path);

/* MODULE CONSTANTS / VARIABLES **********************************************/
/*** Remove this comment if you want to use an ASSERT
SYS_ASSERT_FILE;
***/
SYS_DBC_FILE;  /*!< Defines the name of this source file once for all */

TRC_DEF(glx);

static slnk_t glx_res_head;
//static slnk_t font_lst;

static glx_t glx;

/* GLOBAL CONSTANTS / VARIABLES **********************************************/

/* GLOBAL FUNCTIONS **********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void glx_init(glx_surface_t* p_scr)
{
   glx.w = p_scr->w;
   glx.h = p_scr->h;
   glx.scr = p_scr;
   SLNK_INIT(&glx_res_head);
   TRC_REG(glx, /* TRC_DEBUG | */ TRC_ERROR);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
glx_t* glx_get(void)
{
   return &glx;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
glx_image_t* glx_load_image(char* fn)
{
   return glx_load_image_v2(fn, TRUE);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
glx_image_t* glx_load_image_v2(char* fn, bool_t tiled)
{
   SDL_Surface* p_surface;
   glx_image_t* p_img;
   glx_res_t* p_res = glx_res_find(GLX_RES_TYPE_TEXTURE, fn);
   p_img = (glx_image_t*)malloc(sizeof(glx_image_t));
   REQUIRE(p_img != NULL);
   memset(p_img, 0, sizeof(glx_image_t));
   p_img->tx1 = 0.0f;
   p_img->ty1 = 0.0f;
   p_img->tx2 = 1.0f;
   p_img->ty2 = 1.0f;
   if (p_res != NULL)
   {
      p_res->ref_count++;
      p_img->texid = p_res->tex_id;
      p_img->w = p_res->w;
      p_img->h = p_res->h;
      TRC_DBG(glx, "Ref count increased for tex id %d (%s)", p_res->tex_id,
         p_res->path);
   }
   else
   {
      p_surface = IMG_Load(fn);
      if (!p_surface)
      {
         TRC_ERR(glx, "Error: '%s' could not be opened: %s", fn, IMG_GetError());
         goto error;
      }
      if (p_surface->format->BytesPerPixel < 3)
      {
         TRC_ERR(glx, "Error: '%s' is not a 24 bpp or 32bpp with alpha image", fn);
         SDL_FreeSurface(p_surface);
         goto error;
      }
      /*Generate an OpenGL 2D texture from the SDL_Surface*.*/
      glPixelStorei(GL_UNPACK_ALIGNMENT,4);
      glGenTextures(1, &p_img->texid);
      TRC_DBG(glx, "New texture id %d", p_img->texid);
      glBindTexture(GL_TEXTURE_2D, p_img->texid);

      if (!tiled)
      {
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
      }

      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      if (p_surface->format->Amask)
      {
         glTexImage2D(GL_TEXTURE_2D, 0, 4, p_surface->w, p_surface->h, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, p_surface->pixels);
      }
      else
      {
         glTexImage2D(GL_TEXTURE_2D, 0, 3, p_surface->w, p_surface->h, 0,
            GL_RGB, GL_UNSIGNED_BYTE, p_surface->pixels);
      }
      p_img->w = p_surface->w;
      p_img->h = p_surface->h;
      glx_res_add(GLX_RES_TYPE_TEXTURE, fn, p_img);
      SDL_FreeSurface(p_surface);
   }
   return p_img;
error:
   free(p_img);
   return NULL;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void glx_free_image(glx_image_t* p_img)
{
   glx_res_rm(GLX_RES_TYPE_TEXTURE, p_img);
   free(p_img);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
glx_font_t* glx_load_font(const char* fn, int size)
{
   glx_res_t* p_res;
   TTF_Font* p_font;
   char path[80];
   snprintf(path, 79, "%s_%d", fn, size);
   p_res = glx_res_find(GLX_RES_TYPE_FONT, path);
   if (p_res != NULL)
   {
      p_res->ref_count++;
      p_font = (TTF_Font*)p_res->p_data;
      TRC_DBG(glx, "Ref count increased for font %s", p_res->path);
   }
   else
   {
      p_font = TTF_OpenFont(fn, size);
      if (p_font == NULL)
      {
         TRC_ERR(glx, "Unable to load font: %s", TTF_GetError());
      }
      else
      {
         glx_res_add(GLX_RES_TYPE_FONT, path, p_font);
      }
   }
   if (glx.default_font == NULL)
   {
      glx.default_font = p_font;
   }
   return (glx_font_t*)p_font;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
glx_image_t* glx_drawtext(glx_font_t* p_font, char* text, glx_color_t color,
   glx_rect_t* p_clip_rect)
{
   SDL_Surface* p_text;
   SDL_Color fontcolor = {RGBA_R(color),RGBA_G(color),
                             RGBA_B(color),RGBA_A(color)};
   /*SDL_Color fontcolor_bg = {RGBA_R(0),RGBA_G(0),
                             RGBA_B(0),RGBA_A(0xFF)};*/
   glx_image_t* p_img = (glx_image_t*)calloc(1, sizeof(glx_image_t));
   REQUIRE(p_img != NULL);
   /*p_text = TTF_RenderText_Shaded(p_font, text, fontcolor, fontcolor_bg);*/
   p_text = TTF_RenderText_Blended(p_font, text, fontcolor);
   /*Generate an OpenGL 2D texture from the SDL_Surface*.*/
   glGenTextures(1, &p_img->texid);
   TRC_DBG(glx, "glx_drawtext: New texture id %d", p_img->texid);
   glBindTexture(GL_TEXTURE_2D, p_img->texid);

   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, p_text->w, p_text->h, 0, GL_BGRA,
      GL_UNSIGNED_BYTE, p_text->pixels);

   p_img->w = p_text->w;
   p_img->h = p_text->h;
   p_img->tx1 = 0.0f;
   p_img->ty1 = 0.0f;
   p_img->tx2 = 1.0f;
   p_img->ty2 = 1.0f;

   /*Clean up.*/
   //glDeleteTextures(1, &p_img->texid);
   SDL_FreeSurface(p_text);

   return (glx_image_t*)p_img;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void glx_drawrect(glx_rect_t* p_rect, glx_color_t color)
{
   glColor4ub(RGBA_R(color), RGBA_G(color), RGBA_B(color), RGBA_A(color));
   glDisable(GL_TEXTURE_2D);
   glBegin(GL_QUADS);
      glVertex2i(p_rect->x, p_rect->y);
      glVertex2i(p_rect->x, p_rect->y+p_rect->h);
      glVertex2i(p_rect->x+p_rect->w, p_rect->y+p_rect->h);
      glVertex2i(p_rect->x+p_rect->w, p_rect->y);
   glEnd();
   glEnable(GL_TEXTURE_2D);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void glx_drawimage(glx_image_t* p_img, glx_rect_t* p_srcrect,
   glx_rect_t* p_dstrect)
{
   int w;
   int h;
   glColor4f(1.0f,1.0f,1.0f,1.0f);
   glBindTexture(GL_TEXTURE_2D, p_img->texid);
   if (p_srcrect)
   {
      p_img->tx1 = (float)p_srcrect->x/p_img->w;
      p_img->ty1 = (float)p_srcrect->y/p_img->h;
      p_img->tx2 = (float)(p_srcrect->x+p_srcrect->w)/p_img->w;
      p_img->ty2 = (float)(p_srcrect->y+p_srcrect->h)/p_img->h;
   }
   else if (p_img->tiled)
   {
      p_img->tx1 = 0;
      p_img->ty1 = 0;
      p_img->tx2 = (float)(p_dstrect->w)/p_img->w;
      p_img->ty2 = (float)(p_dstrect->h)/p_img->h;
   }
   w = (p_dstrect->w == 0)?p_img->w:p_dstrect->w;
   h = (p_dstrect->h == 0)?p_img->h:p_dstrect->h;
   glBegin(GL_QUADS);
      glTexCoord2f(p_img->tx1, p_img->ty1);
      glVertex2i(p_dstrect->x, p_dstrect->y);
      glTexCoord2f(p_img->tx1, p_img->ty2);
      glVertex2i(p_dstrect->x, p_dstrect->y+h);
      glTexCoord2f(p_img->tx2, p_img->ty2);
      glVertex2i(p_dstrect->x+w, p_dstrect->y+h);
      glTexCoord2f(p_img->tx2, p_img->ty1);
      glVertex2i(p_dstrect->x+w, p_dstrect->y);
   glEnd();
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void glx_rect_set(glx_rect_t* p_rect, int x, int y, int w, int h)
{
   REQUIRE(p_rect != NULL);
   p_rect->x = x;
   p_rect->y = y;
   p_rect->w = w;
   p_rect->h = h;
}

/* LOCAL FUNCTIONS ***********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void glx_res_add(uint8_t type, char* path, void* p_data)
{
   glx_res_t* p_res = glx_res_find(type, path);
   if (p_res != NULL)
   {
      p_res->ref_count++;
   }
   else
   {
      p_res = (glx_res_t*)malloc(sizeof(glx_res_t));
      REQUIRE(p_res != NULL);
      memset(p_res, 0, sizeof(glx_res_t));
      p_res->path = (char*)malloc(strlen(path) + 1);
      REQUIRE(p_res->path != NULL);
      strcpy(p_res->path, path);
      if (type == GLX_RES_TYPE_TEXTURE)
      {
         glx_image_t* p_img = (glx_image_t*)p_data;
         p_res->tex_id = p_img->texid;
         p_res->w = p_img->w;
         p_res->h = p_img->h;
      }
      else if (type == GLX_RES_TYPE_FONT)
      {
         p_res->p_data = p_data;
      }
      p_res->ref_count = 1;
      SLNK_ADD(&glx_res_head, p_res);
   }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void glx_res_rm(uint8_t type, void* p_data)
{
   glx_res_t* p_res = SLNK_NEXT(glx_res_t, &glx_res_head);
   while (p_res != NULL)
   {
      if (type == GLX_RES_TYPE_TEXTURE)
      {
         glx_image_t* p_img = (glx_image_t*)p_data;
         if (p_res->tex_id == p_img->texid)
         {
            break;
         }
      }
      else if (type == GLX_RES_TYPE_FONT)
      {
         if (p_res->p_data == p_data)
         {
            break;
         }
      }
      p_res = SLNK_NEXT(glx_res_t, p_res);
   }
   if (p_res != NULL)
   {
      if (p_res->ref_count == 1)
      {
         if (type == GLX_RES_TYPE_TEXTURE)
         {
            TRC_DBG(glx, "Removing texture id %d (%s)", p_res->tex_id,
               p_res->path);
            glDeleteTextures(1, (GLuint*)&p_res->tex_id);
         }
         else if (type == GLX_RES_TYPE_FONT)
         {
            TRC_DBG(glx, "Removing font %s", p_res->path);
            /* Todo */
         }
         SLNK_REMOVE(&glx_res_head, p_res);
         free(p_res->path);
         free(p_res);
      }
      else
      {
         TRC_DBG(glx, "Ref count decreased for %s", p_res->path);
         p_res->ref_count--;
      }
   }
   else
   { /* Added by glx_drawtext */
      glx_image_t* p_img = (glx_image_t*)p_data;
      TRC_DBG(glx, "Removing texture id %d (not in resource list)",
         p_img->texid);
      glDeleteTextures(1, &p_img->texid);
   }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static glx_res_t* glx_res_find(uint8_t type, char* path)
{
   glx_res_t* p_res = SLNK_NEXT(glx_res_t, &glx_res_head);
   TOUCH(type);
   while (p_res != NULL)
   {
      if (strcmp(p_res->path, path) == 0)
      {
         break;
      }
      p_res = SLNK_NEXT(glx_res_t, p_res);
   }
   return p_res;
}

/* END OF FILE ***************************************************************/
