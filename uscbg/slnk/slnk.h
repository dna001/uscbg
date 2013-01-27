/******************************************************************************
Copyright (c) 2003, All Rights Reserved.
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file slnk.h
\brief Single Linked List Interface $Revision$. */
/*---------------------------------------------------------------------------*/
#ifndef SLNK_H
#define SLNK_H

/* INCLUDE FILES *************************************************************/

/* EXPORTED DEFINES **********************************************************/
#define SLNK_INIT(obj) slnk_init((slnk_t *)obj)
#define SLNK_ADD(pos, obj) slnk_add((slnk_t *)pos, (slnk_t *)obj)
#define SLNK_REMOVE(head, pos) slnk_remove(head, (slnk_t*) pos)
#define SLNK_REMOVE_NEXT(cast, pos) (cast*)slnk_remove_next((slnk_t*) pos)
#define SLNK_NEXT(cast, pos) (cast*)slnk_next((slnk_t*) pos)
#define SLNK_INSERT(pos, obj) slnk_insert((slnk_t*) pos, (slnk_t *)obj)

/* EXPORTED DATA TYPES *******************************************************/
typedef enum
{
   SLNK_LOAD,
   SLNK_STORE
} slnk_ar_t;      /*!< SLNK archive type */

/*---------------------------------------------------------------------------*/
/*! \brief Prototype for function called for object in for_each...

\return TRUE if loop shall be terminated */
/*---------------------------------------------------------------------------*/
typedef bool_t slnk_fn_t(
   slnk_t* p_obj,          /*!< The object in list to operate on */
   void* p_prm             /*!< Any user specified data */
   );
/* GLOBAL VARIABLES **********************************************************/

/* INTERFACE FUNCTIONS *******************************************************/

/*---------------------------------------------------------------------------*/
/*! \brief Initialize the list. */
/*---------------------------------------------------------------------------*/
void slnk_init(
   slnk_t* p_head          /*!< Head of the linked list */
   );
/*---------------------------------------------------------------------------*/
/*! \brief Insert object in single linked list

Inserts the object after position */
/*---------------------------------------------------------------------------*/
void slnk_insert(
   slnk_t* p_prv,          /*!< Position of insert */
   slnk_t* p_obj           /*!< Object to insert */
   );
/*---------------------------------------------------------------------------*/
/*! \brief Remove object from single linked list

Removes the specified object
\return The object removed */
/*---------------------------------------------------------------------------*/
slnk_t* slnk_remove(
   slnk_t* p_head,         /*!< The list head */
   slnk_t* p_obj           /*!< Position of remove */
   );
/*---------------------------------------------------------------------------*/
/*! \brief Remove object from single linked list

Removes the object after position
\return The object removed */
/*---------------------------------------------------------------------------*/
slnk_t* slnk_remove_next(
   slnk_t* p_obj           /*!< Position of remove */
   );
/*---------------------------------------------------------------------------*/
/*! \brief Get next object in list

\return The object after position. NULL if end of list is reached. */
/*---------------------------------------------------------------------------*/
slnk_t* slnk_next(
   slnk_t* p_prv           /*!< Current object */
   );
/*---------------------------------------------------------------------------*/
/*! \brief Add the object last to the list

This function iterates to the end of a list and adds the new object last in
the list.
\return The object added. */
/*---------------------------------------------------------------------------*/
slnk_t* slnk_add(
   slnk_t* p_head,         /*!< The list head */
   slnk_t* p_obj           /*!< The object to add */
   );
/*---------------------------------------------------------------------------*/
/*! \brief Do for each

Iterate through the linked list and call the defined callback for each
elemet in the list next from position.
\return FALSE if premature break of iterating loop */
/*---------------------------------------------------------------------------*/
bool_t slnk_for_each(
   slnk_t* p_prv,          /*!< Current object */
   slnk_fn_t* fn,          /*!< Function to call for each element in list */
   void* p_prm             /*!< Optional paramter to fn */
   );
/*---------------------------------------------------------------------------*/
/*! \brief Archive a SLNK list that is stored in continous memory

Note that this service can only be used in the special case were the linked
list items are stored continous buffer in memory. The pointers to the next
items are in case of storing replaced by offsets and in case of loading
offsets are replaced by pointers */
/*---------------------------------------------------------------------------*/
void slnk_serialize(
   slnk_ar_t ar,           /*!< Archive type as defined above */
   slnk_t* p_head          /*!< The list head */
   );

#endif /* #ifndef SLNK_H */
/* END OF FILE ***************************************************************/
