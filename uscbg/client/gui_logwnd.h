/******************************************************************************
Copyright (c) 2013, All Rights Reserved.
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file gui_logwnd.h
\brief The Goa gui_logwnd interface. */
/*---------------------------------------------------------------------------*/
#ifndef GUI_LOGWND_H
#define GUI_LOGWND_H
/* INCLUDE FILES *************************************************************/

/* EXPORTED DEFINES **********************************************************/

/* EXPORTED DATA TYPES *******************************************************/

/* GLOBAL VARIABLES **********************************************************/

/* INTERFACE FUNCTIONS *******************************************************/

/*---------------------------------------------------------------------------*/
/*! \brief Initialize. */
/*---------------------------------------------------------------------------*/
void gui_logwnd_init(void);

/*---------------------------------------------------------------------------*/
/*! \brief Create colonies window. */
/*---------------------------------------------------------------------------*/
gui_wnd_t* gui_logwnd_create(
   int x,
   int y,
   int w,
   int h,
   gui_wgt_evt_cb_t* p_cb
);

#endif /* #ifndef GUI_LOGWND_H */
/* END OF FILE ***************************************************************/
