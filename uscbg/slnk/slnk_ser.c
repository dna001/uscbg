/******************************************************************************
Copyright (c) 2003, All Rights Reserved.

$Author$
$Id$ $Revision$ $Date$
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file slnk_ser.c
\brief Single Linked List Serialization. */
/*---------------------------------------------------------------------------*/
#include "sys_def.h"
#include "sys_assert.h"
#include "slnk.h"

/* MACROS ********************************************************************/

/* LOCAL DATATYPES ***********************************************************/

/* LOCAL FUNCTION PROTOTYPES *************************************************/

/* MODULE CONSTANTS / VARIABLES **********************************************/

/* GLOBAL CONSTANTS / VARIABLES **********************************************/

/* GLOBAL FUNCTIONS **********************************************************/
/*----------------------------------------------------------------------------
----------------------------------------------------------------------------*/
void slnk_serialize(slnk_ar_t ar, slnk_t* p_head)
{
   slnk_t* p_nxt = slnk_next(p_head);
   slnk_t* p_cur = p_nxt;

   switch (ar)
   {
   case SLNK_STORE:
      while (p_cur)
      {
         uint32_t* p_offs = (uint32_t*)&p_cur->p_next;
         p_nxt = slnk_next(p_cur);
         if(p_nxt != NULL)
         {
            *p_offs = p_nxt - p_cur;
         }
         p_cur = p_nxt;
      }      
      break;
   case SLNK_LOAD:
      while (p_nxt)
      {
         uint32_t base = STATIC_CAST(uint32_t, p_cur);
         uint32_t offs = STATIC_CAST(uint32_t, p_nxt->p_next)*4;
         base = offs>0 ? base + offs : offs;
         p_cur->p_next = STATIC_CAST(slnk_t*, base);
         p_nxt = slnk_next(p_cur);
         p_cur = p_nxt;
      }      
      break;
   default:
      break;
   }
}


/* LOCAL FUNCTIONS ***********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/

/* END OF FILE ***************************************************************/
