/******************************************************************************
Copyright (c) 2013, All Rights Reserved.
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*! \file trc.c
\brief The trc implementation. */
/*---------------------------------------------------------------------------*/
/* INCLUDE FILES *************************************************************/
#include "sys_def.h"
#include "sys_assert.h"
#include "scf.h"
#include "dlnk.h"
#include "trc.h"
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

/* CONTANTS / MACROS *********************************************************/
#define TRC_MAX_STR_LEN 256

/* LOCAL DATATYPES ***********************************************************/
typedef struct
{
   int id;              /*!< Channel or caller id */
   const char* p_name;  /*!< Name of command to find */
   size_t sz;           /*!< Size of command (To allow not NULL terminated) */
   uint32_t mask;       /*!< Trace mask */
} search_prm_t;         /*!< Search paramter structure */

/* LOCAL FUNCTION PROTOTYPES *************************************************/
STATIC size_t prefix_info(char* p_dst, uint32_t mask, const char* name,
   uint32_t time);
STATIC dlnk_fn_t trc_set_client;
STATIC trc_print_co_t trc_print_def;
STATIC int bit_num(uint32_t mask);

/* MODULE CONSTANTS / VARIABLES **********************************************/
SYS_DBC_FILE;  /*!< Defines the name of this source file once for all */
trc_lnk_t trc_lnk = {{NULL, NULL}, NULL};
uint32_t trc_mask = 0xffffffff;
static trc_print_co_t* trc_print_co = trc_print_def;
static trc_mode_t modei = TRC_MODE_PRINT;
static char* p_buf_start;
static char* p_buf_end;
static char* p_buf_head;
static const char* type_nm[] =
{
   "", "error", "dbg", "info", "data", "evt"
};

static uint32_t trc_prn_mask;

/* GLOBAL CONSTANTS / VARIABLES **********************************************/

/* GLOBAL FUNCTIONS **********************************************************/
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void trc_init(char* p_buf, uint32_t buf_sz)
{
   trc_print_co = trc_print_def;
   dlnk_init(&trc_lnk.dlnk);
   REQUIRE(p_buf);
   REQUIRE(buf_sz > TRC_MAX_STR_LEN);
   p_buf_start = p_buf;
   p_buf_head = p_buf;
   p_buf_end = p_buf_start + buf_sz - 1;
   trc_prn_mask = TRC_PREFIX_COMP | TRC_PREFIX_TYPE;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void trc_start(void)
{
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
trc_print_co_t* trc_print_co_attach(trc_print_co_t* p_co)
{
   trc_print_co_t* p_co_org = trc_print_co;

   trc_print_co = p_co;
   return p_co_org;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
uint32_t trc_mask_filter(uint32_t filter)
{
   uint32_t org_mask = trc_mask;

   {
      trc_mask = filter;
   }
   return org_mask;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void trc_reg(trc_reg_t* p_reg, trc_lnk_t* p_obj)
{
   REQUIRE(p_obj->p_client == NULL);
   dlnk_init(&p_obj->dlnk);
   dlnk_insert(&trc_lnk.dlnk, &p_obj->dlnk);
   p_obj->p_client = p_reg;
   p_reg->mask &= trc_mask;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void trc_dereg(trc_lnk_t* p_obj)
{
   dlnk_remove(&p_obj->dlnk);
}

/*-----------------------------------------------------------------------------
Get a reference to the TRC client list
-----------------------------------------------------------------------------*/
dlnk_t* trc_client_list(void)
{
   return &trc_lnk.dlnk;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int trc_trace(trc_lnk_t* p_obj, uint32_t mask, const char* p_fmt, ...)
{
   size_t n = 0;
   char trc_str[TRC_MAX_STR_LEN+1];

   if (p_obj->p_client->mask & mask)
   {
      va_list ap;
      n = prefix_info(trc_str, mask, p_obj->p_client->p_name, 0/*hwc_tmr_get()*/);
      va_start(ap, p_fmt);
      n += vsnprintf(&trc_str[n], TRC_MAX_STR_LEN-n, p_fmt, ap);
      va_end(ap);

      trc_str[n++] = '\n';
      trc_str[n] = 0;
      switch (modei)
      {
      case TRC_MODE_BUF:
         if ((p_buf_head + n) < p_buf_end)
         {
            strcpy(p_buf_head, trc_str);
            p_buf_head += n;
         }
         break;
      case TRC_MODE_PRINT:
         trc_print_co(trc_str, n);
         break;
      default:
         break;
      }
   }
   return n;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void trc_mode_set(trc_mode_t mode)
{
   modei = mode;
}

/*----------------------------------------------------------------------------
----------------------------------------------------------------------------*/
trc_mode_t trc_mode_get(void)
{
   return modei;
}

#if 0
/*----------------------------------------------------------------------------
----------------------------------------------------------------------------*/
void trc_view_buf(void)
{
   trc_buf_t* p_buf;
   trc_buf_t* p_buf_end;
   char txt[TRC_MAX_STR];
   size_t n;

   switch (part)
   {
   case TRC_VIEW_PART_QUEUED:
      p_buf = p_buf_tail;
      p_buf_end = p_buf_head;
      break;
   case TRC_VIEW_PART_ALL:
      p_buf = p_buf_first;
      p_buf_end = p_buf_head;
      break;
   default:
      break;
   }
   while (p_buf != p_buf_end)
   {
      n = unpack_trc(txt, STATIC_CAST(trc_head_t*, p_buf++));
      INVARIANT(n < TRC_MAX_STR);
      switch (dst)
      {
      case TRC_VIEW_DST_TRCP:
         trc_print_co(txt, n);
         break;
      case TRC_VIEW_DST_CMDP:
         dcp_printf(id, p_nm, txt);
         break;
      default:
         break;
      }
   }
}
#endif

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
void trc_get_buf_sz(uint32_t* p_no_used, uint32_t* p_no_free)
{
  uint32_t u;
  uint32_t f;
  u = (uint32_t)(p_buf_head - p_buf_start);
  f = (uint32_t)(p_buf_end -  p_buf_head);
  *p_no_used = u;
  *p_no_free = f;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
void trc_clear_buf(void)
{
   p_buf_head = p_buf_start;
   p_buf_head[0] = 0;
}

/*-----------------------------------------------------------------------------
The function iterates through the list of registered trc clients and calls the
set function for each client that begins with the defined characters.
-----------------------------------------------------------------------------*/
void trc_do_set(int id, dlnk_t* p_trc_list, char* p_nm, uint32_t mask)
{
   search_prm_t search_prm;

   search_prm.id = id;
   search_prm.p_name = p_nm;
   search_prm.sz = strlen(p_nm);
   search_prm.mask = mask;
   dlnk_for_each(p_trc_list, trc_set_client, &search_prm);
}

/* LOCAL FUNCTIONS ***********************************************************/
/*-----------------------------------------------------------------------------
Look for trc clients that matches the pattern in table in this node.
-----------------------------------------------------------------------------*/
STATIC bool_t trc_set_client(dlnk_t* p_obj, void* p_prm)
{
   trc_lnk_t* p_trc = REINTERPRET_CAST(trc_lnk_t*, p_obj);
   search_prm_t* p_search_prm = (search_prm_t*)p_prm;

   if(!strncmp(p_trc->p_client->p_name, p_search_prm->p_name, p_search_prm->sz))
   {
      p_trc->p_client->mask = p_search_prm->mask;
   }
   return FALSE;
}

/*----------------------------------------------------------------------------
Write prefix information in the buffer.
----------------------------------------------------------------------------*/
STATIC size_t prefix_info(char* p_dst, uint32_t mask, const char* name,
   uint32_t time)
{
   size_t size = 0;
   char* p_first = p_dst;
   int i = 32 - bit_num(mask);

   if (trc_prn_mask | TRC_PREFIX_TIME)
   {
      *p_dst++ = '[';
      p_dst = scf_uint_to_ascii(p_dst, time, 10);
      *p_dst++ = ']';
   }
   if (trc_prn_mask | TRC_PREFIX_TYPE)
   {
      int len;
      *p_dst++ = '[';
      len = strlen(type_nm[i]);
      strcpy(p_dst, type_nm[i]);
      p_dst += len;
      *p_dst++ = ']';
   }
   if (trc_prn_mask | TRC_PREFIX_COMP)
   {
      int len;
      *p_dst++ = '[';
      len = strlen(name);
      strcpy(p_dst, name);
      p_dst += len;
      *p_dst++ = ']';
   }
   *p_dst++ = ':';
   *p_dst++ = ' ';
   *p_dst = '\0';
   size = p_dst - p_first;
   return size;
}

/*----------------------------------------------------------------------------
----------------------------------------------------------------------------*/
STATIC void trc_print_def(const char* p_str, size_t len)
{
   TOUCH(len);
   printf("%s", p_str);
}

/*-----------------------------------------------------------------------------
Returns (31 - highest bit number). If 0 returns 32.
-----------------------------------------------------------------------------*/
STATIC int bit_num(uint32_t mask)
{
   int num = 0;
   uint32_t bit = 0x80000000;

   while (((mask & bit) == 0) && (bit != 0))
   {
      bit = bit >> 1;
      num++;
   }
   return num;
}

/* END OF FILE ***************************************************************/
