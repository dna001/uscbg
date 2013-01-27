/******************************************************************************
Copyright (c) 2013, All Rights Reserved.
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file cfg.c
\brief The cfg implementation. */
/*---------------------------------------------------------------------------*/
/* INCLUDE FILES *************************************************************/
#include "sys_def.h"
#include "sys_assert.h"
#include <stdio.h>
#include <string.h>
#include "scf.h"
#include "cfg.h"

/* CONSTANTS / MACROS ********************************************************/

/* LOCAL DATATYPES ***********************************************************/

/* LOCAL FUNCTION PROTOTYPES *************************************************/

/* MODULE CONSTANTS / VARIABLES **********************************************/
/*** Remove this comment if you want to use an ASSERT
SYS_ASSERT_FILE;
***/
SYS_DBC_FILE;  /*!< Defines the name of this source file once for all */

/* GLOBAL CONSTANTS / VARIABLES **********************************************/

/* GLOBAL FUNCTIONS **********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void cfg_init(void)
{
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int cfg_get_key(char* fn, char* key, char* key_val, int key_sz)
{
   FILE* fp;
   int sz = 0;
   fp = fopen(fn, "rt");
   if (fp != NULL)
   {
      char buf[MAX_CFG_KEY_LEN];
      while (fgets(buf, MAX_CFG_KEY_LEN, fp) != NULL)
      {
         if (strstr(buf, key) == (char*)buf)
         {
            char* key_val_pos = strchr(buf, '=');
            scf_clean(buf, "\n\r");
            if (key_val_pos != NULL)
            {
               strncpy(key_val, key_val_pos+1, key_sz);
               sz = strlen(key_val_pos+1);
               break;
            }
         }
      }
      fclose(fp);
   }
   return sz;
}

/* LOCAL FUNCTIONS ***********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/

/* END OF FILE ***************************************************************/
