/******************************************************************************
Copyright (c) 2013, All Rights Reserved.
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file trc.h
\brief The trc (Trace and Event Logging) interface.

The idea behind the trc component is that all clients that need to provide
runtime information to the developer or tester can be registered and output
information, errors or any other information that could be helpful for
debuging and testing. This output is routed to different entities depending
on target platform, compiler switches or other settings.

The trc component exposes a unified interface for all trace clients via the
command parser (DCP) that makes it possible to change the mask (active mask)
that allows this output or not.

Trace output is allowed in any client also in release builds. If the trc macros
are used a compiler switch can easily disable the trace and remove this extra
code.

For debug purpose there is a shortcut to get out trace information without
registration. This macro must only be used for temporary debug and should not
be left in any release code. The main reason for this is that it must be
possible to select the information needed in runtime to minimize the amount of
information. A second reason is that the trc component automatically prepends
the output string with the component name if the component is properly
registered.*/
/*---------------------------------------------------------------------------*/
#ifndef TRC_H
#define TRC_H
/* INCLUDE FILES *************************************************************/
#include "sys_ctrl.h"

//#if (CMD_CTRL & SYS_CMD_TRC)
#define TRC_TRACE_EN
//#endif

/* EXPORTED DATA TYPES *******************************************************/
typedef struct
{
   const char* p_name;
   uint32_t mask;
} trc_reg_t;

typedef struct
{
   dlnk_t dlnk;
   trc_reg_t* p_client;
} trc_lnk_t;

typedef enum
{
   TRC_MODE_BUF,            /*!< Trace will be buffered */
   TRC_MODE_PRINT,          /*!< Trace will sent using trc_print */
   TRC_MODE_LAST
} trc_mode_t;

/*---------------------------------------------------------------------------*/
/*! \brief Print callout function

Attached to reroute trace printouts. */
/*--------------------------------------------------------------------------*/
typedef void trc_print_co_t(
   const char* p_trc,      /*!< String pointer */
   size_t len              /*!< Length of string */
);

/* EXPORTED DEFINES **********************************************************/
/*---------------------------------------------------------------------------*/
/*! \brief Trace mask definitions

Definition of trace levels for the trc component.
The usage of error levels for the trace should follow the following guide-
lines. FATAL is when something goes completrcy wrong but the reporting unit
relies on the caller to handle this error. MAJOR is when an error occurs but
the problem can be fixed in the unit itself. MINOR can be used to indicate
an error that can be ignered.
In other words FATAL, MAJOR and MINOR shall be used to trace errors that are
considered as errornous behaviour that might be repaired by the unit itself,
by the caller or or by another entity in the system. If the error can't be
recovered avoid the trace functions, use ASSERT instead. */
/*--------------------------------------------------------------------------*/
#define TRC_NONE        0x00000000 /*!< No trace */
#define TRC_ERROR       0x00000001 /*!< Error. */
#define TRC_DEBUG       0x00000002 /*!< Output debug prints. */
#define TRC_INFO        0x00000004 /*!< Normal trace info level 1 */
#define TRC_DATA        0x00000008 /*!< Possible to use for dumping data */
#define TRC_EVT         0x00000010 /*!< Event trace */

/* Defines for prefix options. All enabled by default. */
#define TRC_PREFIX_TIME 0x1
#define TRC_PREFIX_COMP 0x2
#define TRC_PREFIX_TYPE 0x4

/*---------------------------------------------------------------------------*/
/*! \brief Enable macros for trace and event logging

Put TRC_EXT(comp_name); in a include file that is included by all c files in
the component that use trc macros.
Put TRC_DEF(comp_name); in the global variable section of the c file
that registers the component as a trc client.  This will create a variable
trc_comp_name_node that is global.
Put TRC_REG(comp_name, def_mask); in some initialisation code that registers
the component as a trc client. This macro must be used only once to register
the client if not TRC_DE_REG(comp_name); is used in between to deregister.
The TRC macro will output a trace on all platforms.

IMPORTANT! Please use the following features only for temporary debug tracing!
The TRC_D macro will output a trace on all platform without any registration.
It is also always possible to output a trace without any prevoius
registration with the NULL pointer and call the trc_trace function with the
TRC_DBG_PRINT mask. The active mask can of course always be changed also for
this ID if the trc commands are enabled. */
/*---------------------------------------------------------------------------*/
#ifdef TRC_TRACE_EN
/*! TRC_I outputs trace information. */
#define TRC(comp, msk, fmt, ARGS...)\
   (void) trc_trace(&trc_## comp ##_node, msk, fmt, ##ARGS)
#define TRC_ERR(comp, fmt, ARGS...)\
   TRC(comp, TRC_ERROR, fmt, ##ARGS)
#define TRC_DBG(comp, fmt, ARGS...)\
   TRC(comp, TRC_DEBUG, fmt, ##ARGS)
#define TRC_D(msk, fmt, ARGS...)\
   (void) trc_trace(&trc_dbg_node, msk, fmt, ##ARGS)
/*! TRC_DEF defines a global node varible for the client. */
#define TRC_DEF(comp)\
   static const char trc_name[] = #comp;\
   static trc_reg_t trc_reg_id = {trc_name, 0};\
   trc_lnk_t trc_## comp ##_node = {{0, 0}, 0}
/*! TRC_MASK_GET gets the current trace mask for the client. */
#define TRC_MASK_GET()\
   trc_reg_id.mask
/*! TRC_MASK_SET changes the trace mask for the client. */
#define TRC_MASK_SET(new_mask)\
   trc_reg_id.mask = new_mask
/*! TRC_EXT declares the global node varible in other modules of a comp. */
#define TRC_EXT(comp)\
    extern trc_lnk_t trc_## comp ##_node
/*! TRC_INIT initiates the trc component. */
#define TRC_INIT(p_buf, buf_sz)\
   trc_init(p_buf, buf_sz)
/*! TRC_START starts the trc component in case of an underlying OS. */
#define TRC_START()\
   trc_start()
/*! TRC_MASK_FILTER filters the client mask on registration . */
#define TRC_MASK_FILTER(filter)\
   trc_mask_filter(filter)
/*! PRC_PREF_FILTER filters the prefix info printout. */
#define TRC_PREF_FILTER(filter)\
   trc_pref_filter(filter)
/*! TRC_REG registers a client to the trace server. */
#define TRC_REG(comp, msk)\
   trc_reg_id.mask = msk;\
   trc_reg(&trc_reg_id, &trc_## comp ##_node)
/*! TRC_DE_REG deregisters a client from the trc component. */
#define TRC_DEREG(comp)\
   trc_dereg(trc_## comp ##_node)
#define TRC_MODE_SET(mode)\
   trc_mode_set(mode)
#define TRC_MODE_GET()\
   trc_mode_get()
#define TRC_VIEW_BUF(id, p_nm, part, dst)\
   trc_view_buf(id, p_nm, part, dst)
#define TRC_GET_BUF_SZ(p_sz_queued, p_sz_free)\
   trc_get_buf_sz(p_sz_queued, p_sz_free)
#define TRC_CLEAR_BUF()\
   trc_clear_buf()
#define TRC_PRINT_ATTACH(fn)\
   trc_print_co_attach(fn)
#else /* Empty macros */
#define TRC(comp, msk, fmt, ARG...)
#define TRC_D(msk, fmt, ARGS...)
#define TRC_DT(comp, msk, size, data)
#define TRC_DEF(comp)\
   trc_lnk_t trc_## comp ##_node
#define TRC_MASK_GET()
#define TRC_MASK_SET(new_mask)
#define TRC_EXT(comp)\
    extern trc_lnk_t trc_## comp ##_node
#define TRC_INIT(p_buf, buf_sz)
#define TRC_START()
#define TRC_MASK_FILTER(filter)
#define TRC_PREF_FILTER(filter)
#define TRC_REG(comp, msk)
#define TRC_DEREG(comp)
#define TRC_MODE_SET(mode)
#define TRC_MODE_GET()
#define TRC_VIEW_BUF(id, p_nm, part, dst)
#define TRC_GET_BUF_SZ(p_sz_queued, p_sz_free)
#define TRC_CLEAR_BUF()
#define TRC_PRINT_ATTACH(fn)
#endif

/* GLOBAL VARIABLES **********************************************************/

/* INTERFACE FUNCTIONS *******************************************************/
/*---------------------------------------------------------------------------*/
/*! \brief Initialise the trc component

This function initialises the trc component. It must be called only once.
The trc component must be initialized before it is called within another
context. Attempts to violate this rule are catched with ASSERT in debug
builds. Define a trace buffer used by trc */
/*---------------------------------------------------------------------------*/
void trc_init(
   char* p_buf,          /*!< Trace buffer pointer */
   uint32_t buf_sz       /*!< Trace buffer size in bytes */
   );
/*---------------------------------------------------------------------------*/
/*! \brief Start the trc component

This function will start a low priority thread that is responsible of the
formatting and output in case of an underlying OS. */
/*---------------------------------------------------------------------------*/
void trc_start(
   void
   );
/*---------------------------------------------------------------------------*/
/*! \brief Set the trc register mask filter

This function sets a mask filter that limits the allowed output. It is only
used during registration. Client trace masks can later be changed by the client
itself or through the command interface.
\return The original mask */
/*---------------------------------------------------------------------------*/
uint32_t trc_mask_filter(
   uint32_t filter         /*!< Filter bits */
   );
/*---------------------------------------------------------------------------*/
/*! \brief Set the trc prefix info filter

This function sets a filter that controls the prefixed information of trace
printouts.
\return The original mask */
/*---------------------------------------------------------------------------*/
uint32_t tel_pref_filter(
   uint32_t filter         /*!< Filter bits */
   );
/*---------------------------------------------------------------------------*/
/*! \brief Attach print callout function

Attached a print callout function to reroute printouts to a user defined
function.
\return  Original print function */
/*---------------------------------------------------------------------------*/
trc_print_co_t* trc_print_co_attach(
   trc_print_co_t* p_co    /*!< Function to attach */
   );
/*---------------------------------------------------------------------------*/
/*! \brief Registers a client */
/*---------------------------------------------------------------------------*/
void trc_reg(
   trc_reg_t* p_reg,       /*!< Client register information */
   trc_lnk_t* p_obj        /*!< Node for this register information. */
   );
/*---------------------------------------------------------------------------*/
/*! \brief Deregistration of a client

This function removes a client from the trc component register. */
/*---------------------------------------------------------------------------*/
void trc_dereg(
   trc_lnk_t* p_obj        /*!< Node for this register information. */
   );
/*---------------------------------------------------------------------------*/
/*! \brief Get a reference to the client list
\return Reference to client list. */
/*---------------------------------------------------------------------------*/
dlnk_t* trc_client_list(
   void
   );
/*---------------------------------------------------------------------------*/
/*! \brief Send a trace buffer

Sends the trace information to the trc componet. This call is never suspended.
If the trace buffer is full the sent information is lost.
\return Number of characters sent */
/*---------------------------------------------------------------------------*/
int trc_trace(
   trc_lnk_t* p_obj,       /*!< Node for this register information. */
   uint32_t mask,          /*!< Trace mask */
   const char* p_fmt,      /*!< Pointer to a format string */
   ...                     /*!< Variable argument list */
   );
/*---------------------------------------------------------------------------*/
/*! \brief Set the mode of operation */
/*---------------------------------------------------------------------------*/
void trc_mode_set(
   trc_mode_t mode         /*!< Mode */
   );
/*---------------------------------------------------------------------------*/
/*! \brief Get the mode of operation */
/*---------------------------------------------------------------------------*/
trc_mode_t trc_mode_get(
   void
   );
/*---------------------------------------------------------------------------*/
/*! \brief View trace buffer */
/*---------------------------------------------------------------------------*/
void trc_view_buf(
   void
   );
/*---------------------------------------------------------------------------*/
/*! \brief Get buffer size */
/*---------------------------------------------------------------------------*/
void trc_get_buf_sz(
   uint32_t* p_no_used,      /*!< Numner of bytes used */
   uint32_t* p_no_free       /*!< Number of bytes free */
   );
/*---------------------------------------------------------------------------*/
/*! \brief Clear buffer */
/*---------------------------------------------------------------------------*/
void trc_clear_buf(
   void
   );
/*---------------------------------------------------------------------------*/
/*! \brief Set trace mask for matching clients */
/*---------------------------------------------------------------------------*/
void trc_do_set(
   int id,                /*!< Id */
   dlnk_t* p_trc_list,    /*!< Client list */
   char* p_nm,            /*!< Client name */
   uint32_t mask          /*!< Trace mask */
   );
#endif /* #ifndef TRC_H */
/* END OF FILE ***************************************************************/
