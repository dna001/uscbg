/******************************************************************************
Copyright (c) 2013, All Rights Reserved.
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file net.c
\brief The net implementation. */
/*---------------------------------------------------------------------------*/
/* INCLUDE FILES *************************************************************/
#include "sys_def.h"
#include "sys_assert.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#ifdef WIN32
#undef UNICODE
#define _WIN32_WINNT 0x501
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#ifndef __USE_POSIX
   #define __USE_POSIX
#endif
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#endif
#include "slnk.h"
#include "scf.h"
#include "trc.h"
#include "net.h"

/* CONSTANTS / MACROS ********************************************************/

/* LOCAL DATATYPES ***********************************************************/
typedef struct
{
   slnk_t slnk;
   char name[MAX_CLIENT_NAME_LEN];
   int sock;
} net_client_t;

typedef struct
{
   slnk_t slnk;
   int evt;
   int sock;
   void* data;
   int len;
} net_evt_t;

typedef struct
{
   bool_t started;
   pthread_t thread_id;
   pthread_mutex_t queue_mutex;
   slnk_t queue_head;
   slnk_t client_head;
   net_cfg_t net_cfg;
} net_t;

/* LOCAL FUNCTION PROTOTYPES *************************************************/
static void *server_thread(void *arg);
static void *client_thread(void *arg);
static int recv_complete_packet(int sock);
static void add_to_queue(int sock, int evt, void* data, int len);

/* MODULE CONSTANTS / VARIABLES **********************************************/
/*** Remove this comment if you want to use an ASSERT
SYS_ASSERT_FILE;
***/
SYS_DBC_FILE;  /*!< Defines the name of this source file once for all */

TRC_DEF(net);

static net_t net;

/* GLOBAL CONSTANTS / VARIABLES **********************************************/

/* GLOBAL FUNCTIONS **********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void net_init(void)
{
#ifdef WIN32
   /*Initialize Winsock */
   WSADATA wsaData;
   int error = WSAStartup(MAKEWORD(1,1), &wsaData);
   if (error != 0) {
      printf("WSAStartup failed: %d\n", error);
   }
#endif
   net.started = FALSE;
   pthread_mutex_init(&net.queue_mutex, NULL);
   SLNK_INIT(&net.queue_head);
   TRC_REG(net, TRC_ERROR /*| TRC_DEBUG */);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void net_start(net_cfg_t* p_cfg)
{
   REQUIRE(p_cfg != NULL);
   net.net_cfg = *p_cfg;
   if (net.net_cfg.is_server) {
      /* Create server thread */
      pthread_create(&net.thread_id, NULL, server_thread, NULL);
      net.started = TRUE;
   } else {
      /* Create client thread */
      pthread_create(&net.thread_id, NULL, client_thread, NULL);
      net.started = TRUE;
   }
}

void net_stop(void)
{
#ifdef WIN32
   WSACleanup();
#endif
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void net_poll(void)
{
   net_evt_t* p_evt;
   while((p_evt = SLNK_NEXT(net_evt_t, &net.queue_head)) != NULL) {
      pthread_mutex_lock(&net.queue_mutex);
      SLNK_REMOVE(&net.queue_head, p_evt);
      pthread_mutex_unlock(&net.queue_mutex);
      net.net_cfg.evt_fn(p_evt->evt, p_evt->sock, p_evt->data, p_evt->len);
      if (p_evt->len != 0) {
         free(p_evt->data);
      }
      free(p_evt);
   }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int net_write_packet(int sock, void* data, int len)
{
   char packet[MAX_PACKET_SZ+2];
   int ntot = 0;
   int nleft = len+2;
   REQUIRE(len <= MAX_PACKET_SZ);
   packet[0] = (len >> 8) &0xff; /* 2 bytes packet size info */
   packet[1] = len & 0xff;
   memcpy(packet+2, data, len);
   TRC_DBG(net, "Sending %d bytes", nleft);
   /* Make sure complete packet is sent */
   while(nleft > 0)
   {
      int nbytes = send(sock, packet + ntot, nleft, 0);
      if (nbytes <= 0) {
         TRC_ERR(net, "Error: send\n");
         //add_to_queue(sock, NET_EVT_DISCONNECTED, packet, ntot);
         close(sock);
         ntot = -1;
         break;
      } else {
         nleft -= nbytes;
         ntot += nbytes;
      }
   }
   return ntot;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int net_read_packet(int sock, void* data, int max_len)
{
   return 0;
}

/* LOCAL FUNCTIONS ***********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void *server_thread(void *arg)
{
   fd_set master;    /* Master file descriptor list */
   fd_set read_fds;  /* Temp file descriptor list for select() */
   int fdmax;        /* Maximum file descriptor number */
   int listener;     /* Listener socket */
   int sock;         /* Socket for new connections */
   struct addrinfo hints, *servinfo, *p;
   struct sockaddr_storage remoteaddr; /* Client address information */
   socklen_t addrlen;
   //char s[INET6_ADDRSTRLEN];
   char port[6];
   int yes = 1;
   int rv;
   int i;

   TRC_DBG(net, "Starting server thread\n");

   /* Setup */
   FD_ZERO(&master); /* Clear the master and temp sets */
   FD_ZERO(&read_fds);

   memset(&hints, 0, sizeof(hints));
   hints.ai_family = AF_INET; /* IPv4 */
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_flags = AI_PASSIVE; /* Use my IP */
   scf_uint_to_ascii(port, net.net_cfg.port, 10);

   if ((rv = getaddrinfo(NULL, port, &hints, &servinfo)) != 0) {
      TRC_ERR(net, "Error: getaddrinfo: %s\n", gai_strerror(rv));
      goto server_error;
   }
   /* Loop through all the results and bind to the first we can */
   for(p = servinfo; p != NULL; p = p->ai_next)
   {
      if ((listener = socket(p->ai_family, p->ai_socktype,
           p->ai_protocol)) == -1) {
         TRC_ERR(net, "Error: server: socket\n");
         continue;
      }
      /* Lose the pesky "address already in use" error message */
      setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(int));

      if (bind(listener, p->ai_addr, p->ai_addrlen) == -1) {
         close(listener);
         TRC_ERR(net, "Error: server: bind\n");
         continue;
      }
      break;
   }
   if (p == NULL) {
      TRC_ERR(net, "Error: server: failed to bind\n");
      goto server_error;
   }
   freeaddrinfo(servinfo);

   if (listen(listener, 1) == -1) {
      TRC_ERR(net, "Error: server: listen\n");
      close(listener);
      goto server_error;
   }
   /* Add the listener to the master set */
   FD_SET(listener, &master);
   /* Keep track of the biggest file descriptor */
   fdmax = listener; /* So far, it's this one */

   TRC_DBG(net, "Server: waiting for connection(s)...\n");

   while(1)  /* main select() loop */
   {
      read_fds = master;
      if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
         TRC_ERR(net, "Error: select\n");
         break;
      }
      /* Run through the existing connections looking for data to read */
      for(i = 0; i <= fdmax; i++) {
         if (FD_ISSET(i, &read_fds)) {
            if (i == listener) {
               /* Handle new connections */
               addrlen = sizeof(remoteaddr);
               sock = accept(listener, (struct sockaddr *)&remoteaddr,
                              &addrlen);
               if (sock == -1) {
                   TRC_ERR(net, "Error: accept\n");
               } else {
                  FD_SET(sock, &master); /* Add to master set */
                  if (sock > fdmax) {    /* Keep track of the max */
                     fdmax = sock;
                  }
#if 0
                  TRC_DBG(net, "selectserver: new connection from %s on "
                         "socket %d\n",
                         inet_ntop(remoteaddr.ss_family,
                            get_in_addr((struct sockaddr*)&remoteaddr),
                            remoteIP, INET6_ADDRSTRLEN),
                            sock);
#endif
                  /* Add new connecton event to queue */
                  add_to_queue(sock, NET_EVT_NEW_CONNECTION, NULL, 0);
               }
            } else {
               /* Handle data from a client */
               int ret = recv_complete_packet(i);
               if (ret <= 0) {
                  /* Got error or connection closed by client */
                  if (ret == 0) {
                     /* Connection closed */
                     TRC_DBG(net, "selectserver: socket %d hung up\n", i);
                     /* Add disconnect event to queue */
                     add_to_queue(i, NET_EVT_DISCONNECTED, NULL, 0);
                  } else if (ret == -1) {
                     TRC_ERR(net, "Error: recv\n");
                     add_to_queue(i, NET_EVT_DISCONNECTED, NULL, 0);
                  }
                  close(i);
                  FD_CLR(i, &master); /* Remove from master set */
               }
            } /* END Handle data from client */
         } /* END Got new incoming connection */
      } /* END Looping through file descriptors */
   }
   close(listener);
server_error:
   pthread_exit(NULL);
   return NULL;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void *client_thread(void *arg)
{
   int sock;
   struct addrinfo hints, *servinfo, *p;
   int rv;
   //char s[INET6_ADDRSTRLEN];
   char port[6];

   memset(&hints, 0, sizeof(hints));
   hints.ai_family = AF_INET;
   hints.ai_socktype = SOCK_STREAM;
   scf_uint_to_ascii(port, net.net_cfg.port, 10);

   if ((rv = getaddrinfo(net.net_cfg.addr, port, &hints, &servinfo)) != 0) {
      TRC_ERR(net, "Error: getaddrinfo: %s\n", gai_strerror(rv));
      goto client_error;
   }
   /* Loop through all the results and connect to the first we can */
   for(p = servinfo; p != NULL; p = p->ai_next) {
      if ((sock = socket(p->ai_family, p->ai_socktype,
               p->ai_protocol)) == -1) {
         TRC_ERR(net, "Error: client: socket\n");
         continue;
      }
      if (connect(sock, p->ai_addr, p->ai_addrlen) == -1) {
         close(sock);
         TRC_ERR(net, "Error: client: connect\n");
         continue;
      }
      break;
   }

   if (p == NULL) {
      TRC_ERR(net, "Error: client: failed to connect\n");
      goto client_error;
   }

#if 0
   inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
             s, sizeof s);
   TRC_DBG(net, "client: connecting to %s\n", s);
#endif
   freeaddrinfo(servinfo);

   /* Add new connecton event to queue */
   add_to_queue(sock, NET_EVT_NEW_CONNECTION, NULL, 0);

   while(1)
   {
      int ret = recv_complete_packet(sock);
      if (ret == -1) {
         TRC_ERR(net, "Error: recv\n");
         break;
      } else if (ret == 0) {
         /* Add disconnect event to queue */
         add_to_queue(sock, NET_EVT_DISCONNECTED, NULL, 0);
      }
   }

   close(sock);
client_error:
   pthread_exit(NULL);
   return NULL;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static int recv_complete_packet(int sock)
{
   char buf[MAX_PACKET_SZ];
   char* p_buf;
   int nbytes;
   int rc;
   char packet[MAX_PACKET_SZ];
   int ntot = 0;
   int nleft = MAX_PACKET_SZ+1;

   while(nleft > 0)
   {
      rc = nbytes = recv(sock, buf, MAX_PACKET_SZ, 0);
      TRC_DBG(net, "Received %d bytes", nbytes);
      if (nbytes <= 0) {
         break;
      } else {
         /* Check if more than one packet */
         p_buf = buf;
         while (nbytes > 0)
         {
            if (nleft == MAX_PACKET_SZ+1)
            {
               REQUIRE(nbytes >= 2); /* 2 packet bytes size info */
               nleft = (p_buf[0] << 8) | p_buf[1];
               REQUIRE(nleft <= MAX_PACKET_SZ);
               nbytes -= 2;
               p_buf += 2;
               TRC_DBG(net, "New packet: length %d", nleft);
            }
            if (nleft > nbytes)
            {
               nleft -= nbytes;
               memcpy(packet + ntot, p_buf, nbytes);
               ntot += nbytes;
               nbytes = 0;
            }
            else if (nleft == nbytes)
            {
               memcpy(packet + ntot, p_buf, nbytes);
               ntot += nbytes;
               add_to_queue(sock, NET_EVT_RX, packet, ntot);
               TRC_DBG(net, "Packet length %d added to queue", ntot);
               nleft = 0;
               nbytes = 0;
            }
            else
            {  /* More than one packet in buffer. Send packet and start a new.*/
               memcpy(packet + ntot, p_buf, nleft);
               ntot += nleft;
               add_to_queue(sock, NET_EVT_RX, packet, ntot);
               TRC_DBG(net, "Packet length %d added to queue", ntot);
               ntot = 0;
               nbytes -= nleft;
               p_buf += nleft;
               nleft = MAX_PACKET_SZ+1;
            }
         }
      }
   }
   return rc;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void add_to_queue(int sock, int evt, void* data, int len)
{
   net_evt_t* p_evt;
   if (net.net_cfg.poll)
   {
      p_evt = (net_evt_t*)malloc(sizeof(net_evt_t));
      REQUIRE(p_evt != NULL);
      SLNK_INIT(p_evt);
      p_evt->evt = evt;
      p_evt->sock = sock;
      if (len > 0) {
         p_evt->data = malloc(len);
         REQUIRE(p_evt->data != NULL);
         memcpy(p_evt->data, data, len);
      }
      p_evt->len = len;
      pthread_mutex_lock(&net.queue_mutex);
      SLNK_ADD(&net.queue_head, p_evt);
      pthread_mutex_unlock(&net.queue_mutex);
   }
   else
   {
      net.net_cfg.evt_fn(evt, sock, data, len);
   }
}

/* END OF FILE ***************************************************************/
