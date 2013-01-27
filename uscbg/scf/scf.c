/******************************************************************************
Copyright (c) 2003, All Rights Reserved.
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file
\brief String Conversion Functions Implementation. */
/*---------------------------------------------------------------------------*/
#include "sys_def.h"
#include "scf.h"
#include <stdarg.h>
#include <string.h>

/* CONTANTS / MACROS *********************************************************/

/* LOCAL DATATYPES ***********************************************************/

/* LOCAL FUNCTION PROTOTYPES *************************************************/

/* MODULE CONSTANTS / VARIABLES **********************************************/
//static const char digit_lc [] = "0123456789abcdef";
static const char digit_uc [] = "0123456789ABCDEF";

/* GLOBAL CONSTANTS / VARIABLES **********************************************/

/* GLOBAL FUNCTIONS **********************************************************/
/*-----------------------------------------------------------------------------
Converts a signed binary value to an ASCII string.
-----------------------------------------------------------------------------*/
char* scf_sint_to_ascii(char *p_dst, int32_t val, int base)
{
    if (val < 0)
    {
        *p_dst++ = '-';
        p_dst = scf_uint_to_ascii(p_dst, (uint32_t)-val, base);
    }
    else
    {
        p_dst = scf_uint_to_ascii(p_dst, (uint32_t)val, base);
    }
    return p_dst;
}

/*-----------------------------------------------------------------------------
Converts a unsigned binary value to an ASCII string.
-----------------------------------------------------------------------------*/
char* scf_uint_to_ascii(char *p_dst, uint32_t val, int base)
{
    int n = 0;
    uint8_t rem[32];

    if (val != 0)
    {
        while(val)
        {
            rem[n++] = (uint8_t)(val % base);
            val /= base;
        }
        do
        {
            n--;
            *p_dst++ = (char)(rem[n]>=10 ? rem[n]-10+'a' : rem[n]+'0');
        } while (n > 0);
    }
    else
    {
        *p_dst++ = '0';
    }
    *p_dst = 0x00;
    return p_dst;
}

/*-----------------------------------------------------------------------------
Converts a string of bytes to hex ASCII code.
Eg. Input = 0xA3 => Output = 0x41 and 0x33.
-----------------------------------------------------------------------------*/
int scf_bin_to_ascii(char *p_dst, const char *p_src, int n, char sep)
{
   int k;
   int i = 0;

   for(k=n ; k > 0; k--)
   {
      *p_dst++ = digit_uc[(*p_src >> 4) & 0x0F];
      i++;
      *p_dst++ = digit_uc[(*p_src & 0x0F)];
      i++;
      p_src++;
      if ((sep != '\0') && (k > 1))
      {
         *p_dst++ = sep;
         i++;
      }
   }
   *p_dst = 0x00;
   return i;
}

/*-----------------------------------------------------------------------------
Converts a ASCII code string to a binary representation of an integer.
-----------------------------------------------------------------------------*/
int32_t scf_ascii_to_bin(const char** pp_src, int base)
{
   uint8_t c = 0;
   int32_t res = 0;
   int sign = 1;

   if (**pp_src == '-')
   {
      sign = -1;
      *pp_src += 1;
   }
   while (c < base)
   {
      res = (int32_t)(res * base + c);
      c = (uint8_t)(**pp_src - '0');
      *pp_src += 1;
      if (c >= 10)
      {                   /* A...F or a...f */
          c -= 7;
          if (c >= 16)
          {               /* a...f */
              c -= 32;
              if ((c == 1) || (c == 33))
              {   /* Restart if x or X is found */
                  c = 0;
                  res = 0;
              }
              else if (c <= 9)
              {
                c = 0xff; /* Break for separator :->@ */
              }
          }
          else if (c <= 9)
          {
              c = 0xff; /* Break for separator :->@ */
          }
      }
   }
   *pp_src -= 1;
   return res * sign;
}

/*-----------------------------------------------------------------------------
Converts a character input string to an array of int8.
-----------------------------------------------------------------------------*/
int scf_str_to_uint8(uint8_t* p_dst, int len, const char* p_src, int base)
{
   int n = 0;

   do
   {
      *p_dst++ = (uint8_t)scf_ascii_to_bin(&p_src, base);
      n++;
   } while ((n < len) && (*p_src++ != 0x00));
   return n;
}

/*-----------------------------------------------------------------------------
Converts a character input string to an array of int16.
-----------------------------------------------------------------------------*/
int scf_str_to_uint16(uint16_t* p_dst, int len, const char* p_src, int base)
{
   int n = 0;

   do
   {
      *p_dst++ = (uint16_t)scf_ascii_to_bin(&p_src, base);
      n++;
   } while ((n < len) && (*p_src++ != 0x00));
   return n;
}

/*-----------------------------------------------------------------------------
Converts a character input string to an array of int32.
-----------------------------------------------------------------------------*/
int scf_str_to_int32(int32_t* p_dst, int len, const char* p_src, int base)
{
   int n = 0;

   do
   {
      *p_dst++ = scf_ascii_to_bin(&p_src, base);
      n++;
   } while ((n < len) && (*p_src++ != 0x00));
   return n;
}

/* L O C A L    F U N C T I O N S ********************************************/

/******************************** END OF FILE ********************************/

