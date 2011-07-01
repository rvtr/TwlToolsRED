#ifndef ENTRY_H_
#define ENTRY_H_


#include <stdio.h>
#include <stdlib.h>
#include "types.h"
//#include "nitro_romheader.h"
#include "twl_format_rom.h"
#include "banner.h"
//#include "checker.h"

typedef struct
{
    char* name;
    char* full_path_name;
    u8    name_length;
    u8    full_path_name_length;
    u16   self_id;
    u16   parent_id;
    void* parent;
    void* next;
} MyDirEntry;

typedef struct
{
    char* name;
    char* full_path_name;
    u8    name_length;
    u8    full_path_name_length;
    u32   top;
    u32   bottom;
    bool  modified;
    u16   self_id;
    u16   parent_id;
    void* parent;
    void* next;
} MyFileEntry;

typedef struct
{
    char* name;
    u8    name_length;
    u32   top;
    u32   bottom;
    bool  modified;
    void* next;
} MyAreaEntry;


class Entry
{
//  private:
  public:
    MyDirEntry*  dirEntry;
    MyFileEntry* fileEntry;
    MyAreaEntry* areaEntry;

  public:
    void Initialize( void);

    void InitializeEntry( MyDirEntry* myDirEntry);
    void InitializeEntry( MyFileEntry* myFileEntry);
    void InitializeEntry( MyAreaEntry* myAreaEntry);    

    void CopyEntry( MyDirEntry* dest, MyDirEntry* src);

    void SetName( MyDirEntry* myDirEntry, char* fname, u16 len);
    void SetName( MyFileEntry* myFileEntry, char* fname, u16 len);
    void SetName( MyAreaEntry* myAreaEntry, char* fname, u16 len);
    
    void addDirEntry( MyDirEntry* myDirEntry);
    void addFileEntry( MyFileEntry* myFileEntry);
    void addAreaEntry( MyAreaEntry* myAreaEntry);

    MyDirEntry* FindDirEntry( u16 id);
    MyFileEntry* FindFileEntry( u16 id);

    MyDirEntry* FindDirEntry( char* my_full_path_name);
    MyFileEntry* FindFileEntry( char* my_full_path_name);

    /* アドレスの範囲に該当するファイルを表示する */
    MyFileEntry* FindFileLocation( u32 start_adr, u32 end_adr);
    /* アドレスの範囲に該当するエリアを表示する */
    MyAreaEntry* FindAreaLocation( u32 start_adr, u32 end_adr);

    /* parent リンクを繋げる */
    void FollowParent( void);

    /* フルパスをセットする */
    void AutoSetFullPath( void);
    void SetFullPath( MyDirEntry *targetDirEntry);
    void SetFullPath( MyFileEntry *targetFileEntry);

    void PrintAllDirEntry( void);
    void PrintAllFileEntry( void);
    
//    void CheckAllFiles( Checker* checker, MyFileEntry *anotherFileEntry);
    
    void Finalize();
};


#endif
