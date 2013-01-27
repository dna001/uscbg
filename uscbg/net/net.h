/******************************************************************************
Copyright (c) 2013, All Rights Reserved.
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file net.h
\brief The net interface. */
/*---------------------------------------------------------------------------*/
#ifndef NET_H
#define NET_H
/* INCLUDE FILES *************************************************************/

/* EXPORTED DEFINES **********************************************************/
#define MAX_PACKET_SZ (1024)
#define MAX_CLIENT_NAME_LEN (32)

/* EXPORTED DATA TYPES *******************************************************/
enum
{
   NET_EVT_NONE = 0,
   NET_EVT_NEW_CONNECTION,
   NET_EVT_DISCONNECTED,
   NET_EVT_RX,
   NET_EVT_LAST
};

typedef void net_evt_cb_fn_t(int evt, int sock, void* data, int len);

typedef struct
{
   bool_t is_server;             /*!< Server or client */
   bool_t is_blocking;           /*!< Blocking or unblocking sockets */
   int port;                     /*!< Network port */
   char addr[16];                /*!< Connection address (client only) */
   int max_connections;          /*!< Max connections (server only) */
   net_evt_cb_fn_t* evt_fn;      /*!< Net event callback function */
   bool_t poll;                  /*!< Polling or not */
} net_cfg_t;

/* GLOBAL VARIABLES **********************************************************/

/* INTERFACE FUNCTIONS *******************************************************/

/*---------------------------------------------------------------------------*/
/*! \brief Initialize net. */
/*---------------------------------------------------------------------------*/
void net_init(void);

/*---------------------------------------------------------------------------*/
/*! \brief Start net. */
/*---------------------------------------------------------------------------*/
void net_start(
   net_cfg_t* p_cfg     /*!< Configuration */
);

/*---------------------------------------------------------------------------*/
/*! \brief Poll for network events. */
/*---------------------------------------------------------------------------*/
void net_poll(void);

/*---------------------------------------------------------------------------*/
/*! \brief Send packet.
\return Number of bytes written or -1 if error */
/*---------------------------------------------------------------------------*/
int net_write_packet(
   int sock,            /*!< Network socket */
   void* data,          /*!< Pointer to write buffer */
   int len              /*!< Length of packet */
);

/*---------------------------------------------------------------------------*/
/*! \brief Read packet.
\return Length of packet */
/*---------------------------------------------------------------------------*/
int net_read_packet(
   int sock,            /*!< Network socket */
   void* data,          /*!< Pointer to read buffer */
   int max_len          /*!< Max length of packet */
);

#endif /* #ifndef NET_H */
/* END OF FILE ***************************************************************/
