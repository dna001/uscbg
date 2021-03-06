/******************************************************************************
Copyright (c) 2013, All Rights Reserved.
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file net_server.h
\brief The Dominant Species net server interface. */
/*---------------------------------------------------------------------------*/
#ifndef NET_SERVER_H
#define NET_SERVER_H
/* INCLUDE FILES *************************************************************/

/* EXPORTED DEFINES **********************************************************/

/* EXPORTED DATA TYPES *******************************************************/

/* GLOBAL VARIABLES **********************************************************/

/* INTERFACE FUNCTIONS *******************************************************/

/*---------------------------------------------------------------------------*/
/*! \brief Initialize. */
/*---------------------------------------------------------------------------*/
void net_server_init(
   void
   );

/*---------------------------------------------------------------------------*/
/*! \brief Start. */
/*---------------------------------------------------------------------------*/
void net_server_start(
   void
   );

/*---------------------------------------------------------------------------*/
/*! \brief Send command to client. */
/*---------------------------------------------------------------------------*/
void net_server_send_cmd(
   int sock,
   int cmd,
   void* data
   );

/*---------------------------------------------------------------------------*/
/*! \brief Send command to all clients. */
/*---------------------------------------------------------------------------*/
void net_server_broadcast_cmd(
   int cmd,
   void* data
   );

#endif /* #ifndef NET_SERVER_H */
/* END OF FILE ***************************************************************/
