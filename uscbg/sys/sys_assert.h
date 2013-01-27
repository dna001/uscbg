/******************************************************************************
Copyright (c) 2003, All Rights Reserved.

$Author$
$Id$ $Revision$ $Date$
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file sys_assert.h
\brief Assert macros. */
/*---------------------------------------------------------------------------*/

#ifndef SYS_ASSERT_H
#define SYS_ASSERT_H

/* EXPORTED DEFINES **********************************************************/
/*---------------------------------------------------------------------------*/
/*! \brief ASSERT support macros 

The assert function will be available somewere in all normal builds.
For special builds it might be necessary to make your own implemenatation.
The "design by contract" MACROS exists for all DEBUG build but are removed if
compiled for RELEASE. ASSERT will always be checked. */
/*---------------------------------------------------------------------------*/
void assert(const char* test, const char* file, int line);

#define SYS_ASSERT_FILE static const char assert_file[] = __FILE__

#define ASSERT(test)\
   ((test)? (void)0 : assert(#test, assert_file, __LINE__))
#ifdef _DEBUG
#define SYS_DBC_FILE static const char dbc_file[] = __FILE__
#define ASSERT_DBG(test)\
   ((test)? (void)0 : assert(#test, dbc_file, __LINE__))
#define REQUIRE(test) ASSERT_DBG(test)     /*!< To test precondition */
#define ENSURE(test) ASSERT_DBG(test)      /*!< To test postcondition */
#define INVARIANT(test) ASSERT_DBG(test)   /*!< To test invariant */
#define ALLEGE(test) ASSERT_DBG(test)      /*!< Assert with side effect */
#else /* If RELEASE build */
#define SYS_DBC_FILE typedef char dbc_file
#define REQUIRE(test)
#define ENSURE(test)
#define INVARIANT(test)
#define ALLEGE(test) ((void)(test))
#endif /* _DEBUG */

/* EXPORTED DATA TYPES *******************************************************/

/* GLOBAL VARIABLES **********************************************************/

/* INTERFACE FUNCTIONS *******************************************************/

#endif /* #ifndef SYS_ASSERT_H */

/* END OF FILE ***************************************************************/
