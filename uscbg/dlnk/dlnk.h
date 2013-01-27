/******************************************************************************
Copyright (c) 2003, All Rights Reserved.
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file dlnk.h
\brief The dlnk interface. */
/*---------------------------------------------------------------------------*/
#ifndef DLNK_H
#define DLNK_H
/* INCLUDE FILES *************************************************************/

/* EXPORTED DEFINES **********************************************************/
#define DLNK_INIT(obj) dlnk_init((dlnk_t *)obj)
#define DLNK_ADD(pos, obj) dlnk_add((dlnk_t*) pos, (dlnk_t *)obj)
#define DLNK_REMOVE(cast, pos) (cast*)dlnk_remove((dlnk_t*) pos)
#define DLNK_NEXT(cast, pos) (cast*)dlnk_next((const dlnk_t*) pos)
#define DLNK_PREV(cast, pos) (cast*)dlnk_prev((const dlnk_t*) pos)
#define DLNK_INSERT(pos, obj) dlnk_insert((dlnk_t *)pos, (dlnk_t *)obj)

/* EXPORTED DATA TYPES *******************************************************/

/*---------------------------------------------------------------------------*/
/*! \brief Prototype for function called for object in for_each...

\return TRUE if loop shall be terminated */
/*---------------------------------------------------------------------------*/
typedef bool_t dlnk_fn_t(
   dlnk_t* p_obj,          /*!< The object in list to operate on */
   void* p_prm             /*!< Any user specified data */
   );
/* GLOBAL VARIABLES **********************************************************/

/* INTERFACE FUNCTIONS *******************************************************/

/*---------------------------------------------------------------------------*/
/*! \brief Initialize dlnk. */
/*---------------------------------------------------------------------------*/
void dlnk_init(
   dlnk_t* p_lnk           /*!< Linked list to initiate */
   );
/*---------------------------------------------------------------------------*/
/*! \brief Add an object in the list

Inserts the object behind position
\return The next object */
/*---------------------------------------------------------------------------*/
dlnk_t* dlnk_add(
   dlnk_t* p_pos,          /*!< Position to insert after */
   dlnk_t* p_obj           /*!< Object to insert */
   );
/*---------------------------------------------------------------------------*/
/*! \brief Insert an element

Insert an object before position. (Inserts an elenemt last in list if p_pos is
the list header.)
\return The next object. */
/*---------------------------------------------------------------------------*/
dlnk_t* dlnk_insert(
   dlnk_t* p_pos,          /*< Position to insert before */
   dlnk_t* p_obj           /*< Object to insert */
   );
/*---------------------------------------------------------------------------*/
/*! \brief Removes the object from list

\return The next object */
/*---------------------------------------------------------------------------*/
dlnk_t* dlnk_remove(
   dlnk_t* p_obj           /*!< Object to insert */
   );
/*---------------------------------------------------------------------------*/
/*! \brief Get the next object

\return The next object after pos in list. */
/*---------------------------------------------------------------------------*/
dlnk_t* dlnk_next(
   const dlnk_t* p_pos     /*< Position to get next from */
   );
/*---------------------------------------------------------------------------*/
/*! \brief Get the previos object

\return The previos object to pos in list. */
/*---------------------------------------------------------------------------*/
dlnk_t* dlnk_prev(
   const dlnk_t* p_pos     /*< Position from wich prev object is retrived */
   );
/*---------------------------------------------------------------------------*/
/*! \brief Do for each

Iterate through the linked list and call the defined callback for each
element in the list with start from the head.
\return The link that returned TRUE otherwise NULL */
/*---------------------------------------------------------------------------*/
dlnk_t* dlnk_for_each(
   dlnk_t* p_head,         /*!< The head of linked list */
   dlnk_fn_t* fn,          /*!< Function to call for each element in list */
   void* p_prm             /*!< Optional paramter to fn */
   );

#endif /* #ifndef DLNK_H */
/* END OF FILE ***************************************************************/
