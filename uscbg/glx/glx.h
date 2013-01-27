/******************************************************************************
Copyright (c) 2013, All Rights Reserved.
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file glx.h
\brief The Graphic Library Extended interface. */
/*---------------------------------------------------------------------------*/
#ifndef GLX_H
#define GLX_H
/* INCLUDE FILES *************************************************************/
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>
#include <GL/gl.h>
#include <GL/glext.h>

/* EXPORTED DEFINES **********************************************************/
#define GLX_RGBA(r,g,b,a) (uint32_t)(r<<24|(g<<16)|b<<8|a)

/* EXPORTED DATA TYPES *******************************************************/
typedef SDL_Surface glx_surface_t;
typedef TTF_Font glx_font_t;
typedef SDL_Rect glx_rect_t;
typedef uint32_t glx_color_t;

typedef struct
{
   GLuint texid;         /* Image texture id used to select a texture */
   GLuint w;             /* Real image width */
   GLuint h;             /* Real image height */
   GLuint tex_w;         /* Texture width (2^?) */
   GLuint tex_h;         /* Texture height (2^?) */
   GLuint bpp;           /* Image color depth in bits per pixel */
   GLuint type;          /* Image type (GL_RGB, GL_RGBA) */
   GLfloat tx1;          /* Texture coordinates */
   GLfloat ty1;
   GLfloat tx2;
   GLfloat ty2;
   bool_t tiled;
} glx_image_t;

typedef struct
{
   int w;
   int h;
   glx_surface_t* scr;
   glx_font_t* default_font;
} glx_t;

/* GLOBAL VARIABLES **********************************************************/

/* INTERFACE FUNCTIONS *******************************************************/

/*---------------------------------------------------------------------------*/
/*! \brief Initialize glx. */
/*---------------------------------------------------------------------------*/
void glx_init(
   glx_surface_t* p_scr
   );

/*---------------------------------------------------------------------------*/
/*! \brief Get pointer to glx struct. */
/*---------------------------------------------------------------------------*/
glx_t* glx_get();

/*---------------------------------------------------------------------------*/
/*! \brief Load image.
\return pointer to glx_image_t */
/*---------------------------------------------------------------------------*/
glx_image_t* glx_load_image(
   char* fn          /*!< Image file name */
   );

/*---------------------------------------------------------------------------*/
/*! \brief Load image version 2 (tiled option).
\return pointer to glx_image_t */
/*---------------------------------------------------------------------------*/
glx_image_t* glx_load_image_v2(
   char* fn,         /*!< Image file name */
   bool_t tiled      /*!< Tiled or not */
   );

/*---------------------------------------------------------------------------*/
/*! \brief Free image. */
/*---------------------------------------------------------------------------*/
void glx_free_image(
   glx_image_t* p_img  /*!< Pointer to glx image */
);

/*---------------------------------------------------------------------------*/
/*! \brief Load a font.
\return pointer to glx_font_t */
/*---------------------------------------------------------------------------*/
glx_font_t* glx_load_font(
   const char* fn,   /*!< Font file name */
   int size          /*!< Font size */
   );

/*---------------------------------------------------------------------------*/
/*! \brief Draw a text string with selected font and colour (create texture).
\return pointer to glx_image_t */
/*---------------------------------------------------------------------------*/
glx_image_t* glx_drawtext(
   glx_font_t* p_font,     /*!< Pointer to font */
   char* text,             /*!< Text to draw */
   glx_color_t color,      /*!< Text color */
   glx_rect_t* p_clip_rect /*!< Clip rectangle (for line feed calculation) */
);

/*---------------------------------------------------------------------------*/
/*! \brief Draw a filled rectangle with selected colour on target surface. */
/*---------------------------------------------------------------------------*/
void glx_drawrect(
   glx_rect_t* p_rect,    /*!< Position */
   glx_color_t color      /*!< Fill color */
);

/*---------------------------------------------------------------------------*/
/*! \brief Draw image. */
/*---------------------------------------------------------------------------*/
void glx_drawimage(
   glx_image_t* p_img,    /*!< Image to draw */
   glx_rect_t* p_srcrect, /*!< Source rectangle (NULL = complete image) */
   glx_rect_t* p_dstrect  /*!< Destination rectangle (w=0/h=0:Use image w/h */
);

/*---------------------------------------------------------------------------*/
/*! \brief Set parameters in glx_rect_t struct. */
/*---------------------------------------------------------------------------*/
void glx_rect_set(
   glx_rect_t* p_rect,    /*!< Pointer to rectangle struct */
   int x,                 /*!< x */
   int y,                 /*!< y */
   int w,                 /*!< width */
   int h                  /*!< height */
);

#endif /* #ifndef GLX_H */
/* END OF FILE ***************************************************************/
