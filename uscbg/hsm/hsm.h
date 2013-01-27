/******************************************************************************
Copyright (c) 2008, All Rights Reserved.
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file hsm.h
\brief The hsm (Hierarchical State Machine) interface.

This HSM implementation is based on an articel from
Embedded System Programming August 2000, Miro Samek, Paul Montgomery. */
/*---------------------------------------------------------------------------*/
#ifndef HSM_H
#define HSM_H
/* INCLUDE FILES *************************************************************/

/* EXPORTED DEFINES **********************************************************/

#define HSM_MSG_PROCESSED 0

/* EXPORTED DATA TYPES *******************************************************/
typedef int hsm_evt_t;

enum
{
   HSM_EVT_EXIT= -3,       /*!< State exit event */
   HSM_EVT_ENTRY,          /*!< State entry event */
   HSM_EVT_INIT,           /*!< State initialize event */
   HSM_EVT_NULL,
   HSM_EVT_USER            /*!< Start of user events */
};

/*---------------------------------------------------------------------------*/
/*! \brief Event message definition.

This message is intended to be sub classed by a concrete state mashine if
needed. */
/*---------------------------------------------------------------------------*/
typedef struct
{
   hsm_evt_t evt;          /*!< The event */
} hsm_msg_t;

typedef struct hsm hsm_t;  /*!< Forward hsm declaration */

/*---------------------------------------------------------------------------*/
/*! \brief Event handler prototype.
\return NULL if message is processed otherwise message to be catched */
/*---------------------------------------------------------------------------*/
typedef hsm_msg_t const* hsm_evt_hnd_t(
   hsm_t* p_hsm,           /*!< The state machine */
   hsm_msg_t const* p_msg  /*!< The message */
   );

typedef struct hsm_state
{
   struct hsm_state* p_super; /*!< Super state. Defines the nesting */
   hsm_evt_hnd_t* evt_hnd; /*!< The state event handler function */
   char const* p_name;     /*!< Name of the state */
} hsm_state_t;

struct hsm                 /*!< The state machine base class */
{
   char const* p_name;     /*!< Name of the state machine*/
   hsm_state_t* p_curr;    /*!< Current state */
   hsm_state_t* p_next;    /*!< Next state. (Non 0 if transition taken) */
   hsm_state_t top;        /*!< Top most state */
};

/* GLOBAL VARIABLES **********************************************************/

/* INTERFACE FUNCTIONS *******************************************************/

/*---------------------------------------------------------------------------*/
/*! \brief Initialize the state machine framework. */
/*---------------------------------------------------------------------------*/
void hsm_init(
   void
   );
/*---------------------------------------------------------------------------*/
/*! \brief HSM constructor.

State machine constructor. Must be constructed before the state hierarachy. */
/*---------------------------------------------------------------------------*/
void hsm_ctor(
   hsm_t* p_me,            /*!< this */
   char const* p_name,     /*!< State machine name */
   hsm_evt_hnd_t* hnd      /*!< State handler */
   );
#define HSM_CTOR(me, name, hnd)\
   hsm_ctor(REINTERPRET_CAST(hsm_t*, (me)), name, hnd)

/*---------------------------------------------------------------------------*/
/*! \brief HSM state constructor.

This function creates the state objects and sets up the the state hierarchy.
It must be called for each state during initialization. */
/*---------------------------------------------------------------------------*/
void hsm_state_ctor(
   hsm_state_t* p_me,      /*!< this */
   char const* p_name,     /*!< Name of the state */
   hsm_evt_hnd_t* hnd,     /*!< State handler */
   hsm_state_t* p_super    /*!< The super state (parent) */
   );
#define HSM_STATE_CTOR(me, name, hnd, super)\
   hsm_state_ctor(REINTERPRET_CAST(hsm_state_t*, (me)), (name),\
   REINTERPRET_CAST(hsm_evt_hnd_t*, (hnd)), (super))

/*---------------------------------------------------------------------------*/
/*! \brief HSM start.

Enters and start the top state. After the top state entry it will follow the
trace if init actions and for each init action the trace of entry actions will
be executed. */
/*---------------------------------------------------------------------------*/
void hsm_start(
   hsm_t* p_me             /*!< this */
   );
#define HSM_START(me)\
   hsm_start(REINTERPRET_CAST(hsm_t*, (me)))

/*---------------------------------------------------------------------------*/
/*! \brief HSM event.

This function generates an event to the state mashine. The event is offered to
the state hierarchy starting with the current state and down until message is
handled. */
/*---------------------------------------------------------------------------*/
void hsm_evt(
   hsm_t* p_me,            /*!< this */
   hsm_msg_t const* p_msg  /*!< The event message */
   );
#define HSM_EVT(me, msg)\
   hsm_evt(REINTERPRET_CAST(hsm_t*, (me)), REINTERPRET_CAST(hsm_msg_t*, (msg)))

/*---------------------------------------------------------------------------*/
/*! \brief HSM event get.  */
/*---------------------------------------------------------------------------*/
hsm_evt_t hsm_evt_get(
   hsm_msg_t const* p_msg  /*!< The event message */
   );
#define HSM_EVT_GET(msg)\
   hsm_evt_get(REINTERPRET_CAST(hsm_msg_t const*, (msg)))

/*---------------------------------------------------------------------------*/
/*! \brief HSM current state.
\return The current state */
/*---------------------------------------------------------------------------*/
hsm_state_t* hsm_state_curr(
   hsm_t* p_me             /*!< this */
   );
/*---------------------------------------------------------------------------*/
/*! \brief HSM initialize transition.

This function is intended to be called by any state from which an initial
transition is defined on the HSM_EVT_INIT signal.*/
/*---------------------------------------------------------------------------*/
void hsm_state_init(
   hsm_t* p_me,            /*!< this */
   hsm_state_t* p_trg      /*!< The target state to transit to */
   );
#define HSM_STATE_INIT(me, trg)\
   hsm_state_init(REINTERPRET_CAST(hsm_t*, (me)), trg)

/*---------------------------------------------------------------------------*/
/*! \brief HSM state transition.

This function will take the state mashine from the cureent state to the target
state and call all defined exit, entry and init functions on the path. */
/*---------------------------------------------------------------------------*/
void hsm_state_tran(
   hsm_t* p_me,            /*!< this */
   hsm_state_t* p_trg,     /*!< The target state to transit to */
   uint8_t* p_to_lca       /*!< Steps to the least common anchestor */
   );
#define HSM_STATE_TRAN(me, trg)\
{\
   static uint8_t to_lca = 0;\
   hsm_state_tran(REINTERPRET_CAST(hsm_t*, (me)), (trg), &to_lca);\
}

#endif /* #ifndef HSM_H */
/* END OF FILE ***************************************************************/
