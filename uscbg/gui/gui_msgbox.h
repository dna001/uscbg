/******************************************************************************
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file gui_msgbox.h
\brief The gui_msgbox interface. */
/*---------------------------------------------------------------------------*/
#ifndef GUI_MSGBOX_H
#define GUI_MSGBOX_H
/* INCLUDE FILES *************************************************************/

/* EXPORTED DEFINES **********************************************************/

/* EXPORTED DATA TYPES *******************************************************/
#define MSGBOX_BTN_OK (1u)
#define MSGBOX_BTN_CANCEL (1u<<1)

/* GLOBAL VARIABLES **********************************************************/

/* INTERFACE FUNCTIONS *******************************************************/

/*---------------------------------------------------------------------------*/
/*! \brief Initialize msgbox. */
/*---------------------------------------------------------------------------*/
void gui_msgbox_init(void);

/*---------------------------------------------------------------------------*/
/*! \brief New msgbox. */
/*---------------------------------------------------------------------------*/
gui_wnd_t* gui_msgbox_create(
   char* title,
   char* text,
   int flags
);

#endif /* #ifndef GUI_MSGBOX_H */
/* END OF FILE ***************************************************************/
