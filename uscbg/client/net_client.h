/******************************************************************************
Copyright (c) 2013, All Rights Reserved.
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file net_client.h
\brief The GOA net client interface. */
/*---------------------------------------------------------------------------*/
#ifndef NET_CLIENT_H
#define NET_CLIENT_H
/* INCLUDE FILES *************************************************************/

/* EXPORTED DEFINES **********************************************************/

/* EXPORTED DATA TYPES *******************************************************/

/* GLOBAL VARIABLES **********************************************************/

/* INTERFACE FUNCTIONS *******************************************************/

/*---------------------------------------------------------------------------*/
/*! \brief Initialize. */
/*---------------------------------------------------------------------------*/
void net_client_init(
   void
   );

/*---------------------------------------------------------------------------*/
/*! \brief Start. */
/*---------------------------------------------------------------------------*/
void net_client_start(
   char* name,
   char* ip,
   int port
   );

/*---------------------------------------------------------------------------*/
/*! \brief Send command to server. */
/*---------------------------------------------------------------------------*/
void net_client_send_cmd(
   net_us_cmd_t cmd,
   uint32_t data
);

#endif /* #ifndef NET_CLIENT_H */
/* END OF FILE ***************************************************************/
