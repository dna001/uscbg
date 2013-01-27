/******************************************************************************
Copyright (c) 2003, All Rights Reserved.
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file
\brief Interface for String Conversion Functions.

The SCF unit implements string conversion functions. */
/*---------------------------------------------------------------------------*/
#ifndef SCF_H
#define SCF_H

/* EXPORTED DEFINES **********************************************************/

/* EXPORTED DATA TYPES *******************************************************/

/* GLOBAL VARIABLES **********************************************************/

/* INTERFACE FUNCTIONS *******************************************************/

/*---------------------------------------------------------------------------*/
/*! \brief Signed integer to ASCII string

Converts a signed binary value to an ASCII string.
\return Next free position in p_dst */
/*---------------------------------------------------------------------------*/
char* scf_sint_to_ascii(
   char *p_dst,         /*!< Storage location for output */
   int32_t val,         /*!< Value to be converted */
   int base             /*!< Conversion base. (2, 8, 10, 16, ...) */
   );
/*---------------------------------------------------------------------------*/
/*! \brief Unsigned integer to ASCII string

Converts a unsigned binary value to an ASCII string.
\return Next free position in p_dst */
/*---------------------------------------------------------------------------*/
char* scf_uint_to_ascii(
   char *p_dst,         /*!< Storage location for output */
   uint32_t val,        /*!< Value to be converted */
   int base             /*!< Conversion base. (2, 8, 10, 16, ...) */
   );
/*---------------------------------------------------------------------------*/
/*! \brief Binary array to ASCII string

Converts a binary array to an ASCII string suitable for display output.
Eg. Input = 0xA3 => Output = 0x41 and 0x33.
\return Numbers of bytes stored in destination buffer  */
/*---------------------------------------------------------------------------*/
int scf_bin_to_ascii(
   char *p_dst,         /*!< Destination string (ASCII) */
   const char *p_src,   /*!< Source array (Binary) */
   int n,               /*!< Bytes to convert from source */
   char sep             /*!< Separation character in output. '\0' none */
   );
/*---------------------------------------------------------------------------*/
/*! \brief ASCII string to binary integer value

Converts an ASCII string to a binary representation of a integer value. The
source pointer is updated and points tp the next character that terminated the
conversion.
\return Integer value  */
/*---------------------------------------------------------------------------*/
int32_t scf_ascii_to_bin(
   const char** pp_src, /*!< A pointer to a pointer to the ASCII string */
   int base             /*!< The base of the integer */
   );
/*---------------------------------------------------------------------------*/
/*! \brief Convert a ASCII string to an uint8_t array with values.

An ASCII string with one or more values is processed and each value found is
converted to its binary representation and put into consecutive positions in
the integer array. The function terminates when the end of the string is found
or if the maximum number of values specified for the array is reached.
\return Number of values converted  */
/*---------------------------------------------------------------------------*/
int scf_str_to_uint8(
   uint8_t* p_dst,      /*!< Destination integer array */
   int len,             /*!< Length of integer array */
   const char* p_src,   /*!< ASCII source string */
   int base             /*!< Integer base to convert to */
   );
/*---------------------------------------------------------------------------*/
/*! \brief Convert a ASCII string to an uint16_t array with values.

See description for str_to_uint8.
\return Number of values converted  */
/*---------------------------------------------------------------------------*/
int scf_str_to_uint16(
   uint16_t* p_dst,     /*!< Destination integer array */
   int len,             /*!< Length of integer array */
   const char* p_src,   /*!< ASCII source string */
   int base             /*!< Integer base to convert to */
   );
/*---------------------------------------------------------------------------*/
/*! \brief Convert a ASCII string to an int32_t array with values.

See description for str_to_uint8.
\returns Number of values converted  */
/*---------------------------------------------------------------------------*/
int scf_str_to_int32(
   int32_t* p_dst,      /*!< Destination integer array */
   int len,             /*!< Length of integer array */
   const char* p_src,   /*!< ASCII source string */
   int base             /*!< Integer base to convert to */
   );
/*---------------------------------------------------------------------------*/
/*! \brief Cleans the string from specified characters.

In addition to the cleanup characters followed by backspace are removed.
\return Length of cleaned string */
/*---------------------------------------------------------------------------*/
size_t scf_clean(
   char* p_str,         /*!< String to clean */
   const char* p_clean  /*!< Characters to remove */
   );
/*---------------------------------------------------------------------------*/
/*! \brief Split a string into substrings.

The input string is splitted into substrings were the separator is found.
Pointers to the found substrings will be set in the sub array until string is
terminated or max_sub substrings are found.
NOTE! The separator characters in the input string are replaced with NULL.
\return Number of substring pointers in the sub array */
/*---------------------------------------------------------------------------*/
size_t scf_split(
   char* p_str,            /*!< String to split */
   char* sub[],            /*!< Array of pointers to substrings */
   size_t max_sub,         /*!< Max allowed number of substrings */
   char sep                /*!< Separator */
   );
/*---------------------------------------------------------------------------*/
/*! \brief Converts and copies a tchar string into a char string.

This function converts and copies a tchar string into a charcter string.
The function terminates if NULL is found or len is reached. The result string
is always NULL terminated, also if len is reached. It is the responsibility of
the caller to ensure that the size of the result string is
sizeof(char)*(len + 1).
\return Number of characters copied */
/*---------------------------------------------------------------------------*/
int scf_t_to_c(
   char* p_dst,            /*!< The character destination string */
   const tchar_t* p_src,   /*!< The 16 bit tchar source string */
   int len                 /*!< Number of characters to copy */
   );
/*---------------------------------------------------------------------------*/
/*! \brief Converts and copies a char string into a tchar string.

This function converts and copies a character string into a tchar string.
The function terminates if NULL is found or len is reached. The result string
is always NULL terminated, also if len is reached. It is the responsibility of
the caller to ensure that the size of the result string is
sizeof(wchar_t)*(len + 1).
\return Number of characters copied */
/*---------------------------------------------------------------------------*/
int scf_c_to_t(
   tchar_t* p_dst,         /*!< The 16 bit tchar destination string */
   const char* p_src,      /*!< The character source string */
   int len                 /*!< Number of characters to copy */
   );

/*---------------------------------------------------------------------------*/
/*! \brief Converts string to lower case.
\return Nothing*/
/*---------------------------------------------------------------------------*/
void scf_case_to_lower(
   char* p_str,            /*!< String to convert */
   int len                 /*!< Number of characters to convert in string */
   );

/*---------------------------------------------------------------------------*/
/*! \brief Converts string to upper case.
\return Nothing */
/*---------------------------------------------------------------------------*/
void scf_case_to_upper(
   char* p_str,            /*!< String to convert */
   int len                 /*!< Number of characters to convert in string */
   );

#endif /* SCF_H */

/* END OF FILE ***************************************************************/
