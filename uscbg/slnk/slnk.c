/******************************************************************************
Copyright (c) 2003, All Rights Reserved.
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file slnk.c
\brief Single Linked List Interface. */
/*---------------------------------------------------------------------------*/
#include "sys_def.h"
#include "sys_assert.h"
#include "slnk.h"

/* MACROS ********************************************************************/

/* LOCAL DATATYPES ***********************************************************/

/* LOCAL FUNCTION PROTOTYPES *************************************************/

/* MODULE CONSTANTS / VARIABLES **********************************************/
SYS_DBC_FILE;

/* GLOBAL CONSTANTS / VARIABLES **********************************************/

/* GLOBAL FUNCTIONS **********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void slnk_init(slnk_t* p_head)
{
   REQUIRE(p_head);
   p_head->p_next = NULL;
}

/*----------------------------------------------------------------------------
----------------------------------------------------------------------------*/
void slnk_insert(slnk_t* p_prv, slnk_t* p_obj)
{
   REQUIRE(p_prv && p_obj);
   p_obj->p_next = p_prv->p_next;
   p_prv->p_next = p_obj;
}

/*----------------------------------------------------------------------------
----------------------------------------------------------------------------*/
slnk_t* slnk_remove(slnk_t* p_head, slnk_t* p_obj)
{
   slnk_t* p_prev = p_head;

   REQUIRE(p_head);
   REQUIRE(p_obj);
   while ((p_prev->p_next != p_obj) && (p_prev->p_next != NULL))
   {
      p_prev = slnk_next(p_prev);
   }
   if(p_prev->p_next)
   {
      p_prev->p_next = p_obj->p_next;
   }
   return p_obj;
}

/*----------------------------------------------------------------------------
----------------------------------------------------------------------------*/
slnk_t* slnk_remove_next(slnk_t* p_obj)
{
   slnk_t* pp;

   REQUIRE(p_obj);
   pp = p_obj->p_next;
   if(pp)
   {
      p_obj->p_next = pp->p_next;
   }
   return pp;
}

/*----------------------------------------------------------------------------
----------------------------------------------------------------------------*/
slnk_t* slnk_next(slnk_t* p_prv)
{
   REQUIRE(p_prv);
   return p_prv->p_next;
}

/*----------------------------------------------------------------------------
----------------------------------------------------------------------------*/
slnk_t* slnk_add(slnk_t* p_head, slnk_t* p_obj)
{
   slnk_t* p_nxt = p_head;

   REQUIRE(p_head && p_obj);
   while (p_nxt)
   {
      p_nxt = slnk_next(p_head);
      if (p_nxt != NULL)
      {
         p_head = p_nxt;
      }
   }
   p_obj->p_next = p_head->p_next;
   p_head->p_next = p_obj;
   return p_obj;
}

/*----------------------------------------------------------------------------
----------------------------------------------------------------------------*/
bool_t slnk_for_each(slnk_t* p_prv, slnk_fn_t* fn, void* p_prm)
{
   slnk_t* p_pos;
   bool_t res = TRUE;

   REQUIRE(p_prv && fn);

   p_pos = slnk_next(p_prv);
   while (p_pos)
   {
      if (fn(p_pos, p_prm))
      {
         res = FALSE;
         break;
      }
      p_pos = slnk_next(p_pos);
   }
   return res;
}

/* LOCAL FUNCTIONS ***********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/

/* END OF FILE ***************************************************************/
