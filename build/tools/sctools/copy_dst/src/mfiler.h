#ifndef	_MFILER_H_
#define	_MFILER_H_


typedef struct _MY_ENTRY_LIST {
  struct _MY_ENTRY_LIST *prev;
  struct _MY_ENTRY_LIST *next;
  FSDirectoryEntryInfo content;
  char src_path[ 512 ];
} MY_ENTRY_LIST;


void MFILER_CurrentDir_Init(void);
char *MFILER_Get_CurrentDir(void);
void MFILER_Change_CurrentDir(const char *path);

int MFILER_ReadDir(MY_ENTRY_LIST **headp, const char *path_src);
void MFILER_DisplayDir(TEXT_CTRL *tc, MY_ENTRY_LIST **headp, int mode);
BOOL MFILER_ClearDir(MY_ENTRY_LIST **headp);

void MFILER_CursorY_Down( void );
void MFILER_CursorY_Up( void );

void MFILER_SetCursorY(int pos_y);
int MFILER_GetCursorY(void);

char *MFILER_GetCursorFullPath(MY_ENTRY_LIST **headp);
char *MFILER_GetCursorEntryPath(MY_ENTRY_LIST **headp);
BOOL MFILER_Is_Cursor_Dir(MY_ENTRY_LIST **headp);
BOOL MFILER_Is_Cursor_TextFile(MY_ENTRY_LIST **headp);

#endif /* _MFILER_H_ */
