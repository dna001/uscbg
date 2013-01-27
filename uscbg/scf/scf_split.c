/******************************************************************************
Copyright (c) 2003, All Rights Reserved.

$Author$
$Id$ $Revision$ $Date$
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file scf_split.c
\brief String Conversion Functions Implementation. */
/*---------------------------------------------------------------------------*/
#include "sys_def.h"
#include "sys_assert.h"
#include "scf.h"

/* CONTANTS / MACROS *********************************************************/

/* LOCAL DATATYPES ***********************************************************/

/* LOCAL FUNCTION PROTOTYPES *************************************************/

/* MODULE CONSTANTS / VARIABLES **********************************************/
SYS_DBC_FILE;

/* GLOBAL CONSTANTS / VARIABLES **********************************************/

/* GLOBAL FUNCTIONS **********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
size_t scf_split(char* p_str, char* sub[], size_t max_sub, char sep)
{
   size_t n_sub = 0;
   char* p_sub;

   REQUIRE(p_str != NULL);

   if (*p_str == sep)
   {
      while (*p_str == sep)
      {  /* Take care of leading separators */
         p_str++;
      }
   }
   p_sub = p_str;
   while (*p_sub != 0x00)
   {
      char ch = *p_str;
      if (ch == sep)
      {
         *p_str++ = 0x00;
      }
      if ((ch == sep) || (ch == 0x00))
      {
         if (*p_sub != 0x00)
         {
            sub[n_sub++] = p_sub;
         }
         p_sub = p_str;
      }
      else
      {
         p_str++;
      }
      if (n_sub == max_sub)
      {
         break;
      }
   }
   return n_sub;
}

/* LOCAL FUNCTIONS ***********************************************************/

/******************************** END OF FILE ********************************/

