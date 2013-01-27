/******************************************************************************
Copyright (c) 2007, All Rights Reserved.
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file pbuf.h
\brief The package buffer interface.

This component is intended to be used to pack and unpack messages that are
sent and/or received with a specific protocol. The pbuf functions simplifies
the task to pack and unpack different sizes and solve big and little endian
formats. The pack and unpack functions are controlled with a format string
that defines how to pack and unpack the data.
\arg \c 'b': Byte
\arg \c 'B': Byte
\arg \c 'h': Halfword in little endian (16 bits)
\arg \c 'H': Halfword in big endian (16 bits)
\arg \c 'w': Word in little endian (32 bits)
\arg \c 'W': Word in big endian (32 bits)
\arg \c 's': Size of following stream data
\arg \c 'S': Size of following stream data
\arg \c 'd': Stream data reference
\arg \c 'D': Stream data reference

Fore example of how to use it look in t_pbuf. */
/*---------------------------------------------------------------------------*/
#ifndef PBUF_H
#define PBUF_H
/* INCLUDE FILES *************************************************************/

/* EXPORTED DEFINES **********************************************************/

/* EXPORTED DATA TYPES *******************************************************/

/* GLOBAL VARIABLES **********************************************************/

/* INTERFACE FUNCTIONS *******************************************************/

/*---------------------------------------------------------------------------*/
/*! \brief Pack a buffer according to the specified format.
\return The number of bytes packed */
/*---------------------------------------------------------------------------*/
int pbuf_pack(
   uint8_t* p_dst,         /*!< The buffer were data is packed */
   const char* p_fmt,      /*!< The package format */
   ...                     /*!< The data unpacked */
   );
/*---------------------------------------------------------------------------*/
/*! \brief Unpack data from a buffer according to the specified format.
\return The number of bytes unpacked */
/*---------------------------------------------------------------------------*/
int pbuf_unpack(
   uint8_t* p_src,         /*!< The buffer were data is packed */
   const char* p_fmt,      /*!< The package format */
   ...                     /*!< The data unpacked */
   );

#endif /* #ifndef PBUF_H */
/* END OF FILE ***************************************************************/
