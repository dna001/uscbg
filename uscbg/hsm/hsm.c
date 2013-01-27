/******************************************************************************
Copyright (c) 2006, All Rights Reserved.
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file hsm.c
\brief The hsm implementation.

The hierarchical state machine framework allows you to implement most of the
UML state machine mechanisms in plain C.*/
/*---------------------------------------------------------------------------*/
/* INCLUDE FILES *************************************************************/
#include "sys_def.h"
#include "sys_assert.h"
#include "trc.h"
#include "hsm.h"

/* CONTANTS / MACROS *********************************************************/
#define HSM_TRC_EVT
#define MAX_STATE_NESTING 8

/*---------------------------------------------------------------------------*/
/*! \brief HSM state event. */
/*---------------------------------------------------------------------------*/
#define hsm_state_evt(me, hsm, msg)\
   (*(me)->evt_hnd)((hsm), (msg))

/* LOCAL DATATYPES ***********************************************************/

/* LOCAL FUNCTION PROTOTYPES *************************************************/
STATIC void hsm_exit(hsm_t* p_me, uint8_t to_lca);
STATIC uint8_t hsm_to_lca(hsm_t* p_me, hsm_state_t* p_trg);

/* MODULE CONSTANTS / VARIABLES **********************************************/
SYS_DBC_FILE;  /*!< Defines the name of this source file once for all */
TRC_DEF(hsm);

static hsm_msg_t const hsm_msg_init = {HSM_EVT_INIT};
static hsm_msg_t const hsm_msg_entry = {HSM_EVT_ENTRY};
static hsm_msg_t const hsm_msg_exit = {HSM_EVT_EXIT};

/* GLOBAL CONSTANTS / VARIABLES **********************************************/

/* GLOBAL FUNCTIONS **********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void hsm_init(void)
{
#ifdef HSM_TRC_EVT
   TRC_REG(hsm, TRC_ERROR | TRC_EVT);
#else
   TRC_REG(hsm, TRC_ERROR);
#endif
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void hsm_ctor(hsm_t* p_me, char const* p_name, hsm_evt_hnd_t* hnd)
{
   hsm_state_ctor(&p_me->top, "top", hnd, NULL);
   p_me->p_name = p_name;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void hsm_state_ctor(hsm_state_t* p_me, char const* p_name, hsm_evt_hnd_t* hnd,
   hsm_state_t* p_super)
{
   p_me->p_name = p_name;
   p_me->evt_hnd = hnd;
   p_me->p_super = p_super;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void hsm_start(hsm_t* p_me)
{
   hsm_state_t* entry_path[MAX_STATE_NESTING];
   hsm_state_t** pp_trace;
   hsm_state_t* p_s;

   p_me->p_curr = &p_me->top;
   p_me->p_next = NULL;
   TRC(hsm, TRC_EVT, "%s entry", p_me->p_name);
   hsm_state_evt(p_me->p_curr, p_me, &hsm_msg_entry);
   while (hsm_state_evt(p_me->p_curr, p_me, &hsm_msg_init), p_me->p_next)
   {  /* For each init action execute the trace of entry actions. */
      TRC(hsm, TRC_EVT, "%s init", p_me->p_curr->p_name);
      pp_trace = entry_path;
      *pp_trace = 0;
      for (p_s = p_me->p_next; p_s != p_me->p_curr; p_s = p_s->p_super)
      {
         *(++pp_trace) = p_s;
      }
      while ((p_s = *pp_trace--) != NULL)
      {
         TRC(hsm, TRC_EVT, "%s entry", p_s->p_name);
         hsm_state_evt(p_s, p_me, &hsm_msg_entry);
      }
      p_me->p_curr = p_me->p_next;
      p_me->p_next = NULL;
   }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void hsm_evt(hsm_t* p_me, hsm_msg_t const* p_msg)
{
   hsm_state_t* entry_path[MAX_STATE_NESTING];
   hsm_state_t** pp_trace;
   hsm_state_t* p_s;

   for (p_s = p_me->p_curr; p_s != NULL; p_s = p_s->p_super)
   {  /* Execute events down to the super state if not handled */
      TRC(hsm, TRC_EVT, "%s msg %d", p_s->p_name, *p_msg);
      if ((p_msg = hsm_state_evt(p_s, p_me, p_msg)) == NULL)
      {  /* Message handled. Execute possible events caused by state change */
         if (p_me->p_next != NULL)
         {
            pp_trace = entry_path;
            *pp_trace = 0;
            for (p_s = p_me->p_next; p_s != p_me->p_curr; p_s = p_s->p_super)
            {  /* Record the trace of entry actions to follow */
               *(++pp_trace) = p_s;
            }
            while ((p_s = *pp_trace--) != NULL)
            {  /* Execute the trace of entry actions */
               TRC(hsm, TRC_EVT, "%s entry", p_s->p_name);
               hsm_state_evt(p_s, p_me, &hsm_msg_entry);
            }
            p_me->p_curr = p_me->p_next;
            p_me->p_next = NULL;
            while (hsm_state_evt(p_me->p_curr, p_me, &hsm_msg_init),
               p_me->p_next)
            {  /* Execute remaining init actions from the current state */
               TRC(hsm, TRC_EVT, "%s init", p_me->p_curr->p_name);
               pp_trace = entry_path;
               *pp_trace = 0;
               for (p_s = p_me->p_next->p_super; p_me->p_curr;
                  p_s = p_s->p_super)
               {  /* Record the trace of entry actions to get to target */
                  *(++pp_trace) = p_s;
               }
               while ((p_s = *pp_trace--) != NULL)
               {  /* Execute the trace of entry actions */
                  TRC(hsm, TRC_EVT, "%s entry", p_s->p_name);
                  hsm_state_evt(p_s, p_me, &hsm_msg_entry);
               }
               p_me->p_curr = p_me->p_next;
               p_me->p_next = NULL;
            }
         }
         break;   /* Event is processed */
      }
   }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
hsm_evt_t hsm_evt_get(hsm_msg_t const* p_msg)
{
   return p_msg->evt;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
hsm_state_t* hsm_state_curr(hsm_t* p_me)
{
   return p_me->p_curr;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void hsm_state_init(hsm_t* p_me, hsm_state_t* p_trg)
{
   REQUIRE(p_me->p_next == NULL);
   p_me->p_next = p_trg;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void hsm_state_tran(hsm_t* p_me, hsm_state_t* p_trg, uint8_t* p_to_lca)
{
   REQUIRE(p_me->p_next == NULL);
   if (*p_to_lca == 0)
   {  /* Find out levels down to LCA */
      *p_to_lca = hsm_to_lca(p_me, p_trg);
   }
   hsm_exit(p_me, *p_to_lca);
   p_me->p_next = p_trg;
}

/* LOCAL FUNCTIONS ***********************************************************/
/*-----------------------------------------------------------------------------
Execute the exit actions for a state and it's super states down to LCA
-----------------------------------------------------------------------------*/
STATIC void hsm_exit(hsm_t* p_me, uint8_t to_lca)
{
   hsm_state_t* p_s = p_me->p_curr;

   while (to_lca-- > 0)
   {
      TRC(hsm, TRC_EVT, "%s exit", p_s->p_name);
      hsm_state_evt(p_s, p_me, &hsm_msg_exit);
      p_s = p_s->p_super;
   }
   p_me->p_curr = p_s;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
STATIC uint8_t hsm_to_lca(hsm_t* p_me, hsm_state_t* p_trg)
{
   hsm_state_t* p_t;
   hsm_state_t* p_s = p_me->p_curr;
   uint8_t to_lca = 1;

   for (p_s = p_me->p_curr->p_super; p_s != NULL; p_s = p_s->p_super)
   {
      for (p_t = p_trg; p_t != NULL; p_t = p_t->p_super)
      {
         if (p_s == p_t)
         {
            return to_lca;
         }
      }
      to_lca++;
   }
   return 0;
}

/* END OF FILE ***************************************************************/
