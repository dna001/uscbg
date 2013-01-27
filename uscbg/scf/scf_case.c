/******************************************************************************
Copyright Sony Ericsson Mobile Communications 2006

The copyright to the computer program(s) herein is the property of Sony 
Ericsson Mobile Communications. The program(s) may be used and/or copied only
with the written permission from Sony Ericsson Mobile Communications or in
accordance with the terms and conditions stipulated in the agreement/contract
under wich the program(s) have been supplied.
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file
\brief String Conversion Functions Implementation. */
/*---------------------------------------------------------------------------*/
#include "sys_def.h"
#include "scf.h"

/* CONTANTS / MACROS *********************************************************/
#define CAP_DELTA 0x20

/* LOCAL DATATYPES ***********************************************************/

/* LOCAL FUNCTION PROTOTYPES *************************************************/

/* MODULE CONSTANTS / VARIABLES **********************************************/

/* GLOBAL CONSTANTS / VARIABLES **********************************************/

/* GLOBAL FUNCTIONS **********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void scf_case_to_upper(char* p_str, int len)
{
   int n;
   for (n = 0; n < len; n++)
   {      
      if (*p_str >= 'a' && *p_str <= 'z')
      {
         *p_str -= CAP_DELTA;
      }
      p_str++;
   }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void scf_case_to_lower(char* p_str, int len)
{
   int n;
   for (n = 0; n < len; n++)
   {      
      if (*p_str >= 'A' && *p_str <= 'Z')
      {
         *p_str += CAP_DELTA;
      }
      p_str++;
   }
}

/* LOCAL FUNCTIONS ***********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/

/* END OF FILE ***************************************************************/
