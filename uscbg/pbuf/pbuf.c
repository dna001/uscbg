/******************************************************************************
Copyright (c) 2007, All Rights Reserved.
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file pbuf.c
\brief The pbuf implementation. */
/*---------------------------------------------------------------------------*/
/* INCLUDE FILES *************************************************************/
#include "sys_def.h"
#include "sys_assert.h"
#include "pbuf.h"
#include <stdarg.h>

/* CONTANTS / MACROS *********************************************************/
#ifdef HWA_BIG_ENDIAN
#define BYTE0_SHIFT 24
#define BYTE1_SHIFT 16
#define BYTE2_SHIFT 8
#define BYTE3_SHIFT 0
#else
#define BYTE0_SHIFT 0
#define BYTE1_SHIFT 8
#define BYTE2_SHIFT 16
#define BYTE3_SHIFT 24
#endif

/* LOCAL DATATYPES ***********************************************************/

/* LOCAL FUNCTION PROTOTYPES *************************************************/

/* MODULE CONSTANTS / VARIABLES **********************************************/
SYS_DBC_FILE;

/* GLOBAL CONSTANTS / VARIABLES **********************************************/

/* GLOBAL FUNCTIONS **********************************************************/

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int pbuf_pack(uint8_t* p_dst, const char* p_fmt, ...)
{
   va_list arg;
   const char* p_f;
   uint8_t* p_d = p_dst;
   uint8_t* p_s;
   uint16_t h;
   uint32_t w;
   int sz = 0;

   va_start(arg, p_fmt);
   for (p_f = p_fmt; *p_f != 0; p_f++)
   {
      switch (*p_f)
      {
      case 'b':
      case 'B':
         *p_d++ = UINT8_CAST(va_arg(arg, int));
         break;
      case 'h':  /* 16 bit Little endian */
         h = UINT16_CAST(va_arg(arg, int));
         *p_d++ = UINT8_CAST(h >> BYTE0_SHIFT);
         *p_d++ = UINT8_CAST(h >> BYTE1_SHIFT);
         break;
      case 'H':  /* 16 bit Big endian */
         h = UINT16_CAST(va_arg(arg, int));
         *p_d++ = UINT8_CAST(h >> BYTE1_SHIFT);
         *p_d++ = UINT8_CAST(h >> BYTE0_SHIFT);
         break;
      case 'w':  /* 32 bit Little endian */
         w = UINT32_CAST(va_arg(arg, uint32_t));
         *p_d++ = UINT8_CAST(w >> BYTE0_SHIFT);
         *p_d++ = UINT8_CAST(w >> BYTE1_SHIFT);
         *p_d++ = UINT8_CAST(w >> BYTE2_SHIFT);
         *p_d++ = UINT8_CAST(w >> BYTE3_SHIFT);
         break;
      case 'W':  /* 32 bit Big endian */
         w = UINT32_CAST(va_arg(arg, uint32_t));
         *p_d++ = UINT8_CAST(w >> BYTE3_SHIFT);
         *p_d++ = UINT8_CAST(w >> BYTE2_SHIFT);
         *p_d++ = UINT8_CAST(w >> BYTE1_SHIFT);
         *p_d++ = UINT8_CAST(w >> BYTE0_SHIFT);
         break;
      case 's':  /* Stream data size */
      case 'S':
         sz = va_arg(arg, int);
         break;
      case 'd':  /* Stream data */
      case 'D':
         REQUIRE(sz != 0);
         p_s = REINTERPRET_CAST(uint8_t*, va_arg(arg, addr_t));
         while (sz != 0)
         {
            *p_d++ = *p_s++;
            sz--;
         }
         break;
      default:
         return -1;
      }
   }
   return p_d - p_dst;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int pbuf_unpack(uint8_t* p_src, const char* p_fmt, ...)
{
   va_list arg;
   const char* p_f;
   uint8_t* p_s = p_src;
   uint8_t* p_d;
   uint8_t* p_b;
   uint16_t* p_h;
   uint32_t* p_w;
   int sz = 0;

   va_start(arg, p_fmt);
   for (p_f = p_fmt; *p_f != 0; p_f++)
   {
      switch (*p_f)
      {
      case 'b':
      case 'B':
         p_b = va_arg(arg, uint8_t*);
         *p_b = *p_s++;
         break;
      case 'h':  /* 16 bit Little endian */
         p_h = va_arg(arg, uint16_t*);
         *p_h = UINT16_CAST(*p_s++ << BYTE0_SHIFT);
         *p_h |= UINT16_CAST(*p_s++ << BYTE1_SHIFT);
         break;
      case 'H':  /* 16 bit Big endian */
         p_h = va_arg(arg, uint16_t*);
         *p_h = UINT16_CAST(*p_s++ << BYTE1_SHIFT);
         *p_h |= UINT16_CAST(*p_s++ << BYTE0_SHIFT);
         break;
      case 'w':  /* 32 bit Little endian */
         p_w = va_arg(arg, uint32_t*);
         *p_w = (*p_s++ << BYTE0_SHIFT);
         *p_w |= (*p_s++ << BYTE1_SHIFT);
         *p_w |= (*p_s++ << BYTE2_SHIFT);
         *p_w |= (*p_s++ << BYTE3_SHIFT);
         break;
      case 'W':  /* 32 bit Big endian */
         p_w = va_arg(arg, uint32_t*);
         *p_w = (*p_s++ << BYTE3_SHIFT);
         *p_w |= (*p_s++ << BYTE2_SHIFT);
         *p_w |= (*p_s++ << BYTE1_SHIFT);
         *p_w |= (*p_s++ << BYTE0_SHIFT);
         break;
      case 's':  /* Stream data size */
      case 'S':
         sz = va_arg(arg, int);
         break;
      case 'd':  /* Stream data */
      case 'D':
         REQUIRE(sz != 0);
         p_d = REINTERPRET_CAST(uint8_t*, va_arg(arg, addr_t));
         while (sz != 0)
         {
            *p_d++ = *p_s++;
            sz--;
         }
         break;
      default:
         return -1;
      }
   }
   return p_s - p_src;
}

/* LOCAL FUNCTIONS ***********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/

/* END OF FILE ***************************************************************/
