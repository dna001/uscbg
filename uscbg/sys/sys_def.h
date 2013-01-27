/******************************************************************************
Copyright (c) 2003, All Rights Reserved.

$Author$
$Id$ $Revision$ $Date$
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file sys_def.h
\brief  This module contains type definitions common to the complete system. */
/*---------------------------------------------------------------------------*/
#ifndef SYS_DEF_H
#define SYS_DEF_H

/*---------------------------------------------------------------------------*/
/*! Some of the c-lib implementations seem to have a very poor implementation
in some development environments. The following defines allows alternative
implementations to be called.
If the C_LIB_OVERRIDE only is defined the code embraced by C_LIB_OVERRIDE
will be added to the build. After this it is up to the linker to replace the
original c-lib function with the alternative implementation.
If the C_LIB_PREPROCESSOR_OVERRIDE is defined all function names defined in the
hwa_def.h override table will be renamed. This allows the standard c-lib 
functions to be called by for example 3rd party libraries. */
/*---------------------------------------------------------------------------*/
/***/
//#define C_LIB_OVERRIDE
/***/
/***/
//#define C_LIB_PREPROCESSOR_OVERRIDE
/***/

#include <stddef.h>  /* NULL, size_t, ... */
//#include "hwa_def.h" /* Include hardware architecure dependent stuff */
#include <stdint.h>

/* EXPORTED DEFINES **********************************************************/
#ifndef TOUCH
#define TOUCH(x)
#endif

/*---------------------------------------------------------------------------*/
/*! If TEST_LOCAL is defined all static declarations are removed to expose the
module internal functions to the test engine. It is expected that TEST_LOCAL
is defined as a make parameter "-D". */
/*---------------------------------------------------------------------------*/
#if defined(TEST_LOCAL)
#define STATIC
#else
#define STATIC static
#endif

/*---------------------------------------------------------------------------*/
/*! Make sure that the bool_t literals are defined */
/*---------------------------------------------------------------------------*/
#ifndef FALSE
#define FALSE 0
#define TRUE  1
#endif

/*---------------------------------------------------------------------------*/
/*! With NORMAL_CAST all cast MACROS are expanded to the appropriate cast.
Undef NORMAL_CAST to get compiler warnings for code inspection. */
/*---------------------------------------------------------------------------*/
#define NORMAL_CAST

#if defined (NORMAL_CAST)
#define CHAR_CAST(value) (char)(value)
#define INT8_CAST(value) (int8_t)(value)
#define UINT8_CAST(value) (uint8_t)(value)
#define INT16_CAST(value) (int16_t)(value)
#define UINT16_CAST(value) (uint16_t)(value)
#define INT32_CAST(value) (int32_t)(value)
#define UINT32_CAST(value) (uint32_t)(value)
#define INT_CAST(value) (int)(value)
#define UINT_CAST(value) (uint_t)(value)
#define BOOL_CAST(value) (bool_t)(value)
#define ADDR_CAST(value) (addr_t)(value)
#else
#define CHAR_CAST(value)
#define INT8_CAST(value)
#define UINT8_CAST(value)
#define INT16_CAST(value)
#define UINT16_CAST(value)
#define INT32_CAST(value)
#define UINT32_CAST(value)
#define INT_CAST(value)
#define UINT_CAST(value)
#define BOOL_CAST(value)
#define ADDR_CAST(value)
#endif

/*---------------------------------------------------------------------------*/
/*! Usage of the following cast macros is recommended to increase readability
as well as make it possible to efficient search of casts. */
/*---------------------------------------------------------------------------*/
/*! This cast macro shall be used for conversion between different structure
representations of the same data. E.g. Package and package header. */
#define STATIC_CAST(cast, value) (cast)(value)

/*! This cast macro shall be used to remove the const and volatile type
qualifiers. */
#define CONST_CAST(cast, value) (cast)(value)   /*!< Remove const */

/*! This cast macro allows any pointer to be converted into any other pointer
type. Choose one of the above if applicable. */
#define REINTERPRET_CAST(cast, value) (cast)(value)

/*---------------------------------------------------------------------------*/
/*! Set the corresponding bit. */
/*---------------------------------------------------------------------------*/
#define BIT(n) (0x1U << (n))

/*---------------------------------------------------------------------------*/
/*! Swap endianism. */
/*---------------------------------------------------------------------------*/
#define SWAP_END_16(v) (uint16_t)(((v)>>8) | ((v)<<8))
#define SWAP_END_32(v)\
   (((v)>>24) | (((v)&0x00ff0000)>>8) | (((v)&0x0000ff00)<<8) | ((v)<<24))

/*---------------------------------------------------------------------------*/
/*! SYS definition of the min and max macro. */
/*---------------------------------------------------------------------------*/
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#define MAX(a, b)  (((a) < (b)) ? (b) : (a))
#define ABS(a)     (((a) < 0) ? -(a) : (a))

/*---------------------------------------------------------------------------*/
/*! SYS definition of ALIGN. */
/*---------------------------------------------------------------------------*/
#define ALIGN(val, alignment) ((((val)+alignment-1)/alignment)*alignment)

/*---------------------------------------------------------------------------*/
/*! Bit manipulation macros. */
/*---------------------------------------------------------------------------*/
#define U16_LO(val) (uint8_t)((val) & 0x00FF)
#define U16_HI(val) (uint8_t)(((val) & 0xFF00) >> 8)
#define U32_LO(val) (uint16_t)((val) & 0x0000FFFF)
#define U32_HI(val) (uint16_t)(((val) & 0xFFFF0000) >> 16)

/*---------------------------------------------------------------------------*/
/*! Misc macros. */
/*---------------------------------------------------------------------------*/
#define U8_TO_16(h, l) \
   (uint16_t)(((h)<<8) + ((l)<<0))
#define U8_TO_32(hh, hl, lh, ll) \
   (uint32_t)(((hh)<<24) + ((hl)<<16) + ((lh)<<8) + ((ll)<<0))
#define U16_TO_32(h, l) \
   (uint32_t)(((h)<<16) + ((l)<<0))

/* EXPORTED DATA TYPES *******************************************************/
#ifndef tchar_t
#define tchar_t char
#endif

#ifndef addr_t
typedef int addr_t;
#endif

/*---------------------------------------------------------------------------*/
/*! Types common to all hardware architectures. */
/*---------------------------------------------------------------------------*/
typedef int bool_t;
typedef unsigned int uint;

/*--------------------------------------------------------------------------*/
/*! \brief Type definition for a single linked list node. */
/*--------------------------------------------------------------------------*/
typedef struct slnk
{
    struct slnk* p_next;   /*!< Pointer to the next element in the list */
} slnk_t;

/*--------------------------------------------------------------------------*/
/*! \brief Type definition for a double linked list node. */
/*--------------------------------------------------------------------------*/
typedef struct dlnk
{
   struct dlnk* p_prev;    /*!< Pointer to the previous element in the list */
   struct dlnk* p_next;    /*!< Pointer to the next element in the list */
} dlnk_t;

/*--------------------------------------------------------------------------*/
/*! \brief Type definition for a ranked linked list node. */
/*--------------------------------------------------------------------------*/
typedef struct rlnk
{
   dlnk_t dlnk;            /*!< Double linked list node */
   uint32_t delta;         /*!< Difference (delta) inbetween nodes */
} rlnk_t;

#endif /*#ifndef SYS_DEF_H*/

/* END OF FILE ***************************************************************/
