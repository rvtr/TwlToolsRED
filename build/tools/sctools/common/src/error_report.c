#include <twl.h>
#include        "text.h"
#include        "mprintf.h"
#include        "logprintf.h"

#include "error_report.h"


typedef struct _ERROR_REPORT {
  struct _ERROR_REPORT *next;
  char *report_str;
} ERROR_REPORT;

static ERROR_REPORT *er_head;
static BOOL er_active = FALSE;

void Error_Report_Init(void)
{
  er_active = TRUE;
  er_head = NULL;
}

void Error_Report_End(void)
{
  ERROR_REPORT *er_temp;
  ERROR_REPORT *er_next;

  if( er_active == FALSE ) {
    return;
  }

  er_temp = er_head;
  while( er_temp != NULL ) {
    if( er_temp->report_str ) {
      OS_Free( (void *)(er_temp->report_str) );
    }
    er_next = er_temp->next;
    OS_Free( er_temp );
    er_temp = er_next;
  }
  er_head = NULL;
  er_active = FALSE;
}

BOOL Error_Report_Display(TEXT_CTRL *tc)
{
  ERROR_REPORT *er_temp;
  if( er_active == FALSE ) {
    return FALSE;
  }
  if( er_head == NULL ) {
    return FALSE;
  }

  er_temp = er_head;
  while( er_temp != NULL ) {
    if( er_temp->report_str ) {
      mfprintf(tc, "%s", er_temp->report_str );
    }
    er_temp = er_temp->next;
  }
  return TRUE;
}


BOOL Error_Report_Add(char *str)
{
  ERROR_REPORT *er_temp;
  ERROR_REPORT **er_next;
  char *str_buf;
  u32 str_len;
  if( er_active == FALSE ) {
    return FALSE;
  }
  if( str == NULL ) {
    return FALSE;
  }
  er_temp = (ERROR_REPORT *)OS_Alloc( sizeof(ERROR_REPORT) );
  if( er_temp == NULL ) {
    return FALSE;
  } 
  str_len = (u32)STD_StrLen(str);
  str_buf = (char *)OS_Alloc( str_len + 1 );
  if( str_buf == NULL ) {
    return FALSE;
  } 
  STD_StrCpy(str_buf , str);
  er_temp->report_str = str_buf;
  str_buf += str_len;
  *str_buf = '\0';
  er_temp->next = NULL;

  er_next = &er_head;

  while( 1 ) {
    if( *er_next == NULL ) {
      *er_next = er_temp;
      return TRUE;
    }
    er_next = &((*er_next)->next);
  }
  return FALSE;
}

BOOL Error_Report_Printf(const char *fmt, ...)
{
#define STR_BUF_LEN 200
  char str_buf[STR_BUF_LEN];
  int     ret;
  va_list vlist;

  if( er_active == FALSE ) {
    return FALSE;
  }

  va_start(vlist, fmt);
  ret = STD_TVSNPrintf(str_buf, STR_BUF_LEN, fmt, vlist);
  va_end(vlist);
  OS_TPrintf( "%s len = %d\n",__FUNCTION__,ret);
  //  return ret;
  return Error_Report_Add(str_buf);
}
