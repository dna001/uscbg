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
STATIC bool_t keep(char ch, const char* p_clean);

/* MODULE CONSTANTS / VARIABLES **********************************************/

/* GLOBAL CONSTANTS / VARIABLES **********************************************/

/* GLOBAL FUNCTIONS **********************************************************/
/*-----------------------------------------------------------------------------
The function removes backspace and termination sequence. The character
preceeding a backspace is also removed.
-----------------------------------------------------------------------------*/
size_t scf_clean(char* p_str, const char* p_clean)
{
   char* p_rd = p_str;
   char* p_wr = p_rd;

   while (*p_rd != 0x00)
   {
      if (*p_rd == '\b')
      {
         if (p_wr > p_str)
         {
            p_wr--;
         }
      }
      else if (keep(*p_rd, p_clean))
      {
         *p_wr++ = *p_rd;
      }
      p_rd++;
   }
   *p_wr = *p_rd; /* NULL termination */
   return strlen(p_str);
}

/* LOCAL FUNCTIONS ***********************************************************/
STATIC bool_t keep(char ch, const char* p_clean)
{
   bool_t res = TRUE;
   const char* p_ch;

   for (p_ch = p_clean; *p_ch != 0x00; p_ch++)
   {
      if (ch == *p_ch)
      {
         res = FALSE;
         break;
      }
   }
   return res;
}

/******************************** END OF FILE ********************************/

