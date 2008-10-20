#include <twl.h>
#include        "text.h"
#include        "mprintf.h"
#include        "gfx.h"
#include        "my_fs_util.h"
#include        "mfiler.h"

#define DISPLAY_Y_MAX 16
static int cursor_pos_y = 0;
static int display_offset_y = 0;
static int list_count_max = 0;


static MY_ENTRY_LIST *current_dir_list_head = NULL;

static BOOL mfilter_add_list( MY_ENTRY_LIST **headp, FSDirectoryEntryInfo *entry_src, const char *src_path )
{
  MY_ENTRY_LIST *list_temp;
  MY_ENTRY_LIST *list_prev_temp;

  if( *headp == NULL ) {
    *headp = (MY_ENTRY_LIST *)OS_Alloc( sizeof(MY_ENTRY_LIST) );
    (*headp)->prev = NULL;
    (*headp)->next = NULL;
    STD_CopyMemory( (void *)&((*headp)->content), (void *)entry_src ,sizeof(FSDirectoryEntryInfo) );
    STD_StrCpy((*headp)->src_path, src_path);
  }
  else {
    for( list_temp = *headp ; list_temp->next != NULL ; list_temp = list_temp->next ) {
      ;
    }
    list_temp->next = (MY_ENTRY_LIST *)OS_Alloc( sizeof(MY_ENTRY_LIST) );
    list_prev_temp = list_temp;
    list_temp = list_temp->next;
    list_temp->prev = list_prev_temp;
    list_temp->next = NULL;
    STD_CopyMemory( (void *)&(list_temp->content), (void *)entry_src ,sizeof(FSDirectoryEntryInfo) );
    STD_StrCpy(list_temp->src_path, src_path);
  }
  return TRUE;
}

BOOL MFILER_Is_Cursor_Dir(MY_ENTRY_LIST **headp)
{
  MY_ENTRY_LIST *list_temp;
  int cursor_pos = 0;
  if( headp == NULL ) {
    return FALSE;
  }
  else {
    for( list_temp = *headp ; list_temp != NULL ; list_temp = list_temp->next ) {
      if( cursor_pos == cursor_pos_y ) {
	if( (list_temp->content.attributes & FS_ATTRIBUTE_IS_DIRECTORY) != 0 ) {
	  return TRUE;
	}
	else {
	  return FALSE;
	}
      }
      cursor_pos++;
    }
  }
  return FALSE;
}


char *MFILER_GetCursorFullPath(MY_ENTRY_LIST **headp)
{
  MY_ENTRY_LIST *list_temp;
  int cursor_pos = 0;
  if( headp == NULL ) {
    return NULL;
  }
  else {
    for( list_temp = *headp ; list_temp != NULL ; list_temp = list_temp->next ) {
      if( cursor_pos ==  cursor_pos_y ) {
	return list_temp->src_path;
      }
      cursor_pos++;
    }
  }
  return NULL;
}

char *MFILER_GetCursorEntryPath(MY_ENTRY_LIST **headp)
{
  MY_ENTRY_LIST *list_temp;
  int cursor_pos = 0;
  if( headp == NULL ) {
    return NULL;
  }
  else {
    for( list_temp = *headp ; list_temp != NULL ; list_temp = list_temp->next ) {
      if( cursor_pos ==  cursor_pos_y ) {
	return list_temp->content.longname;
      }
      cursor_pos++;
    }
  }
  return NULL;
}


void MFILER_CurrentDir_Init(void)
{
  current_dir_list_head = (MY_ENTRY_LIST *)OS_Alloc( sizeof(MY_ENTRY_LIST) );
  current_dir_list_head->prev = NULL;
  current_dir_list_head->next = NULL;
  //  STD_CopyMemory( (void *)&((*headp)->content), (void *)entry_src ,sizeof(FSDirectoryEntryInfo) );
  STD_StrCpy(current_dir_list_head->src_path, "sdmc:");  
}

char *MFILER_Get_CurrentDir(void)
{
  MY_ENTRY_LIST *list_temp;
  list_temp = current_dir_list_head;
  while( list_temp != NULL ) {
    if( list_temp->next == NULL ) {
      return list_temp->src_path;
    }
    list_temp = list_temp->next;
  }
  return NULL;
}

int MFILER_ReadDir(MY_ENTRY_LIST **headp, const char *path_src)
{
  FSFile f_src;
  FSDirectoryEntryInfo entry_src;
  BOOL bSuccess;
  char *path_src_dir = NULL;
  char *path_src_full = NULL;

  int ret_value = 0;

  FS_InitFile(&f_src);
  bSuccess = FS_OpenDirectory(&f_src, path_src, FS_PERMIT_R);
  if(!bSuccess) {
#if 0
    mprintf("Failed Open SRC Directory\n");
    mprintf(" %s\n", my_fs_util_get_fs_result_word( FS_GetArchiveResultCode(path_src) ));
#endif
    ret_value = -1;
    goto end_process;
  }

  path_src_dir = (char *)OS_Alloc( FILE_PATH_LEN );
  if( path_src_dir == NULL ) {
#if 0
    mprintf("Error: alloc error src_dir\n");
#endif
    ret_value = -1;
    goto end_process;
  }
  path_src_full = (char *)OS_Alloc( FILE_PATH_LEN );
  if( path_src_full == NULL ) {
#if 0
    mprintf("Error: alloc error src_full\n");
#endif
    ret_value = -1;
    goto end_process;
  }
  STD_MemSet((void *)path_src_dir, 0, FILE_PATH_LEN);
  STD_MemSet((void *)path_src_full, 0, FILE_PATH_LEN);
  STD_StrCpy(path_src_dir, path_src);
  STD_StrCat(path_src_dir, "/");


  while( FS_ReadDirectory(&f_src, &entry_src) ) {
#if 1
    if( STD_StrCmp(entry_src.longname, ".") == 0 ) {

    }
    else if( STD_StrCmp(entry_src.longname, "..") == 0 ) {
    }
    else if( entry_src.attributes & FS_ATTRIBUTE_DOS_VOLUME ) {
    }
    else {
      STD_StrCpy( path_src_full , path_src_dir );
      STD_StrCat( path_src_full , entry_src.longname );
      mfilter_add_list( headp, &entry_src, path_src_full );
    }
#else
    STD_StrCpy( path_src_full , path_src_dir );
    STD_StrCat( path_src_full , entry_src.longname );
    mfilter_add_list( headp, &entry_src, path_src_full );
#endif
  }

  bSuccess = FS_CloseDirectory(&f_src);
  if(!bSuccess) {
#if 0
    mprintf("Failed Close SRC Directory\n");
    mprintf(" %s\n", my_fs_util_get_fs_result_word( FS_GetArchiveResultCode(path_src) ));
#endif
    ret_value = -1;
    //    goto end_process;
  }

 end_process:
  if( path_src_dir != NULL ) {
    OS_Free(path_src_dir);
  }
  if( path_src_full != NULL ) {
    OS_Free(path_src_full);
  }

  return ret_value;
}

void MFILER_DisplayDir(TEXT_CTRL *tc, MY_ENTRY_LIST **headp, int mode )
{
  MY_ENTRY_LIST *list_temp;
  //  FSPathInfo path_info;
  u32 attributes;
  int list_count = 0;
  int backslash_count = 0;

  mfprintf(tc, "page %d entry no. %d/%d\n", display_offset_y, cursor_pos_y , list_count_max);
  m_set_palette(tc, 0x4);	/* yellow  */
  mfprintf(tc,"-------------------------------\n");
  m_set_palette(tc, 0xF);	/* white */

  if( cursor_pos_y > list_count_max ) {
    cursor_pos_y = 0;
  }

  if( headp == NULL ) {
  }
  else {
    for( list_temp = *headp ; list_temp != NULL ; list_temp = list_temp->next ) {
      if( (display_offset_y <= list_count) && (list_count < (display_offset_y + DISPLAY_Y_MAX) )) {
	if( list_count ==  cursor_pos_y ) {
	  m_set_palette(tc, 0x4);	/* yellow  */
	  m_putchar( tc, '*');
	  m_set_palette(tc, 0xF);	/* white */
	}
	else {
	  m_putchar( tc, ' ');
	}
	switch( mode ) {
	case 0:
	  attributes = list_temp->content.attributes;
	  ( attributes & FS_ATTRIBUTE_DOS_DIRECTORY )? m_putchar( tc, 'd') : m_putchar( tc, '-');
	  ( attributes & FS_ATTRIBUTE_DOS_VOLUME )?    m_putchar( tc, 'v') : m_putchar( tc, '-');
	  ( attributes & FS_ATTRIBUTE_DOS_SYSTEM )?    m_putchar( tc, 's') : m_putchar( tc, '-');
	  ( attributes & FS_ATTRIBUTE_DOS_ARCHIVE )?   m_putchar( tc, 'a') : m_putchar( tc, '-');
	  ( attributes & FS_ATTRIBUTE_DOS_HIDDEN )?    m_putchar( tc, 'h') : m_putchar( tc, '-');
	  ( attributes & FS_ATTRIBUTE_DOS_READONLY )?  m_putchar( tc, 'R') : m_putchar( tc, '-');
	  m_putchar( tc, ' ');
	  mfprintf(tc, "%s\n", list_temp->content.longname );

	  break;
	case 1:
	  mfprintf(tc, "size %7d ", list_temp->content.filesize);
	  mfprintf(tc, "%s\n", list_temp->content.longname );
	  break;
	case 2:
	  mfprintf(tc, "%s\n", list_temp->src_path );
	if( list_count ==  cursor_pos_y ) {
	    text_display_newline_on( &tc[1] );
	    mfprintf(&tc[1], "\f\n%s\n", list_temp->src_path);
	    text_display_newline_off( &tc[1] );
	  }
	  break;
	case 3:
	  mfprintf(tc, "%4d/%02d/%02d %02d:%02d:%02d ", list_temp->content.mtime.year, 
		   list_temp->content.mtime.month, list_temp->content.mtime.day,
		   list_temp->content.mtime.hour, list_temp->content.mtime.minute,
		   list_temp->content.mtime.second );

	  mfprintf(tc, "%s\n", list_temp->content.longname );
	  break;
	case 4:
	  mfprintf(tc, "%4d/%02d/%02d %02d:%02d:%02d ", list_temp->content.ctime.year, 
		   list_temp->content.ctime.month, list_temp->content.ctime.day,
		   list_temp->content.ctime.hour, list_temp->content.ctime.minute,
		   list_temp->content.ctime.second );

	  mfprintf(tc, "%s\n", list_temp->content.longname );
	  break;
	case 5:
	  mfprintf(tc, "%4d/%02d/%02d %02d:%02d:%02d ", list_temp->content.atime.year, 
		   list_temp->content.atime.month, list_temp->content.atime.day,
		   list_temp->content.atime.hour, list_temp->content.atime.minute,
		   list_temp->content.atime.second );
	  mfprintf(tc, "%s\n", list_temp->content.longname );
	  break;
	}
	backslash_count++;
      }
      list_count++;
    }
  }
  list_count_max = list_count - 1;

  while( backslash_count++ < DISPLAY_Y_MAX ) {
    m_putchar( tc, '\n');
  }
}

BOOL MFILER_ClearDir(MY_ENTRY_LIST **headp)
{
  MY_ENTRY_LIST *list_temp1 = *headp;
  MY_ENTRY_LIST *list_temp2;

  *headp = NULL;

  while( list_temp1 ) {
    list_temp2 = list_temp1->next;
    OS_Free( list_temp1 );
    list_temp1 = list_temp2;
  }
  return TRUE;
}

void MFILER_SetCursorY(int pos_y)
{
  cursor_pos_y = pos_y;
}

int MFILER_GetCursorY(void)
{
  return cursor_pos_y;
}


void MFILER_CursorY_Down( void )
{

  cursor_pos_y++;
  if( cursor_pos_y > list_count_max ) {
    //    cursor_pos_y = list_count_max;
    cursor_pos_y = 0;
  }

  if( (cursor_pos_y % DISPLAY_Y_MAX) == 0 ) {
    display_offset_y += DISPLAY_Y_MAX;
    if( display_offset_y > list_count_max ) {
      display_offset_y = 0;
    }
  }

}

void MFILER_CursorY_Up( void )
{

  cursor_pos_y--;
  if( cursor_pos_y < 0 ) {
    //    cursor_pos_y = 0;
    display_offset_y = (list_count_max / DISPLAY_Y_MAX) * DISPLAY_Y_MAX ;
    cursor_pos_y = list_count_max;
  }

  if( (cursor_pos_y % DISPLAY_Y_MAX) == (DISPLAY_Y_MAX-1) ) {
    display_offset_y -= DISPLAY_Y_MAX;
    if( display_offset_y < 0 ) {
      display_offset_y = (list_count_max / DISPLAY_Y_MAX) * DISPLAY_Y_MAX ;
    }
  }
}
