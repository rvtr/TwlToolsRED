#include <string.h>
#include "entry.h"
//#include "nitro_romheader.h"
#include "twl_format_rom.h"


//void Entry::Initialize( void)
void Entry::Initialize( void)
{
    dirEntry = NULL;
    fileEntry = NULL;
    areaEntry = NULL;
}

void Entry::InitializeEntry( MyDirEntry* myDirEntry)
{
    myDirEntry->name = NULL;
    myDirEntry->full_path_name = NULL;
    myDirEntry->name_length = 0;
    myDirEntry->full_path_name_length = 0;
    myDirEntry->self_id = 0xFFFF;
    myDirEntry->parent_id = 0;
    myDirEntry->parent = NULL;
    myDirEntry->next = NULL;
}

void Entry::InitializeEntry( MyFileEntry* myFileEntry)
{
    myFileEntry->name = NULL;
    myFileEntry->full_path_name = NULL;
    myFileEntry->name_length = 0;
    myFileEntry->full_path_name_length = 0;
    myFileEntry->top = 0;
    myFileEntry->bottom = 0;
    myFileEntry->modified = false;
    myFileEntry->self_id = 0xFFFF;
    myFileEntry->parent_id = 0;
    myFileEntry->parent = NULL;
    myFileEntry->next = NULL;
}

void Entry::InitializeEntry( MyAreaEntry* myAreaEntry)
{
    myAreaEntry->name = NULL;
    myAreaEntry->name_length = 0;
    myAreaEntry->top = 0;
    myAreaEntry->bottom = 0;
    myAreaEntry->modified = false;
    myAreaEntry->next = NULL;
}

void Entry::CopyEntry( MyDirEntry* dest, MyDirEntry* src)
{
    memcpy( dest, src, sizeof(MyFileEntry));
}

void Entry::SetName( MyDirEntry* myDirEntry, char* fname, u16 len)
{
    myDirEntry->name = (char*)malloc( FILE_NAME_LENGTH);
    memset( myDirEntry->name, 0, len+1);
    memcpy( myDirEntry->name, fname, len);

    myDirEntry->name_length = len;
}

void Entry::SetName( MyFileEntry* myFileEntry, char* fname, u16 len)
{
    myFileEntry->name = (char*)malloc( FILE_NAME_LENGTH);
    memset( myFileEntry->name, 0, len+1);
    memcpy( myFileEntry->name, fname, len);
    
    myFileEntry->name_length = len;
}

void Entry::SetName( MyAreaEntry* myAreaEntry, char* fname, u16 len)
{
    myAreaEntry->name = (char*)malloc( FILE_NAME_LENGTH);
    memset( myAreaEntry->name, 0, len+1);
    memcpy( myAreaEntry->name, fname, len);
    
    myAreaEntry->name_length = len;
}

void Entry::addDirEntry( MyDirEntry* myDirEntry)
{
    MyDirEntry *currentEntry = dirEntry;
    if( !currentEntry)
    {
        dirEntry = myDirEntry;
        return;
    }
    while( currentEntry->next)
    {
        currentEntry = (MyDirEntry*)(currentEntry->next);
    }
    currentEntry->next = myDirEntry;
    myDirEntry->next = NULL;
}


void Entry::addFileEntry( MyFileEntry* myFileEntry)
{
    MyFileEntry *currentEntry = fileEntry;
    if( !currentEntry)
    {
        fileEntry = myFileEntry;
        return;
    }
    while( currentEntry->next)
    {
        currentEntry = (MyFileEntry*)(currentEntry->next);
    }
    currentEntry->next = myFileEntry;
    myFileEntry->next = NULL;
}

void Entry::addAreaEntry( MyAreaEntry* myAreaEntry)
{
    MyAreaEntry *currentEntry = areaEntry;
//    printf( "%s, 0x%lx, 0x%lx\n", myAreaEntry->name, myAreaEntry->top, myAreaEntry->bottom);
    if( !currentEntry)
    {
        areaEntry = myAreaEntry;
        return;
    }
    while( currentEntry->next)
    {
        currentEntry = (MyAreaEntry*)(currentEntry->next);
    }
    currentEntry->next = myAreaEntry;
    myAreaEntry->next = NULL;
}


MyDirEntry* Entry::FindDirEntry( u16 id)
{
    MyDirEntry *currentEntry = dirEntry;
    while( currentEntry)
    {
        if( currentEntry->self_id == id)
        {
            return currentEntry;
        }
//        printf( "%s, 0x%x, 0x%x\n", __FUNCTION__, currentEntry->self_id, id);
        currentEntry = (MyDirEntry*)(currentEntry->next);
    }
    return NULL;
}


MyFileEntry* Entry::FindFileEntry( u16 id)
{
    MyFileEntry *currentEntry = fileEntry;
    while( currentEntry)
    {
        if( currentEntry->self_id == id)
        {
            return currentEntry;
        }
//        printf( "%s, 0x%x, 0x%x\n", __FUNCTION__, currentEntry->self_id, id);
        currentEntry = (MyFileEntry*)(currentEntry->next);
    }
    return NULL;
}


MyDirEntry* Entry::FindDirEntry( char* my_full_path_name)
{
    MyDirEntry *currentEntry = dirEntry;
    while( currentEntry)
    {
        if( strcmp( currentEntry->full_path_name, my_full_path_name) == 0)
        {
            return currentEntry;
        }
        currentEntry = (MyDirEntry*)(currentEntry->next);
    }
    return NULL;
}


MyFileEntry* Entry::FindFileEntry( char* my_full_path_name)
{
    MyFileEntry *currentEntry = fileEntry;
    while( currentEntry)
    {
        if( strcmp( currentEntry->full_path_name, my_full_path_name) == 0)
        {
            return currentEntry;
        }
        currentEntry = (MyFileEntry*)(currentEntry->next);
    }
    return NULL;
}


/* アドレスの範囲に該当するファイルを表示する */
MyFileEntry* Entry::FindFileLocation( u32 start_adr, u32 end_adr)
{
    MyFileEntry* retEntry = NULL;
    MyFileEntry *currentEntry = fileEntry;
    while( currentEntry)
    {
        if( ((currentEntry->top <= start_adr)&&(currentEntry->bottom > start_adr)) ||
            ((currentEntry->top <= end_adr)&&(currentEntry->bottom > end_adr)))
        {
            retEntry = currentEntry;
            if( currentEntry->modified)
            {   // 改竄されているファイルの識別表示
//                printf( " %s(*),", currentEntry->full_path_name);
            }else{
//                printf( " %s,", currentEntry->full_path_name);
            }
        }
        currentEntry = (MyFileEntry*)(currentEntry->next);
    }
    return retEntry;
}


/* アドレスの範囲に該当するエリアを表示する */
MyAreaEntry* Entry::FindAreaLocation( u32 start_adr, u32 end_adr)
{
    MyAreaEntry *currentEntry = areaEntry;
    while( currentEntry)
    {
        if( ((currentEntry->top <= start_adr)&&(currentEntry->bottom > start_adr)) ||
            ((currentEntry->top <= end_adr)&&(currentEntry->bottom > end_adr)))
        {
            printf( " %s,", currentEntry->name);
        }
        currentEntry = (MyAreaEntry*)(currentEntry->next);
    }
    return NULL;
}


void Entry::AutoSetFullPath( void)
{
    MyDirEntry *currentDirEntry = dirEntry;
    while( currentDirEntry)
    {
        SetFullPath( currentDirEntry);
        currentDirEntry = (MyDirEntry*)(currentDirEntry->next);
    }

    MyFileEntry *currentFileEntry = fileEntry;
    while( currentFileEntry)
    {
        SetFullPath( currentFileEntry);
        currentFileEntry = (MyFileEntry*)(currentFileEntry->next);
    }
}

void Entry::SetFullPath( MyDirEntry *targetDirEntry)
{
    char tmp_name[256];
    u16  total_length;

    MyDirEntry *currentDirEntry = targetDirEntry;
    total_length = 0;
    tmp_name[255] = '\0';
    while( currentDirEntry)
    {
        total_length += (currentDirEntry->name_length);
        memcpy( &(tmp_name[255 - total_length]), currentDirEntry->name, currentDirEntry->name_length);
        if( currentDirEntry->self_id != 0xF000) // rootディレクトリでなければ
        {
            total_length++; // '/'のぶん
            tmp_name[255 - total_length] = '/';
        }
        currentDirEntry = (MyDirEntry*)(currentDirEntry->parent);
    }
    targetDirEntry->full_path_name = (char*)malloc( total_length + 1);
    targetDirEntry->full_path_name_length = total_length;
    memset( targetDirEntry->full_path_name, 0, total_length + 1);
    memcpy( targetDirEntry->full_path_name, &(tmp_name[255 - total_length]), total_length);
}

void Entry::SetFullPath( MyFileEntry *targetFileEntry)
{
    char tmp_name[256];
    u16  total_length;

    MyFileEntry *currentFileEntry = targetFileEntry;
    MyDirEntry *parentDirEntry;
    total_length = 0;
    tmp_name[255] = '\0';
    {
        total_length += (currentFileEntry->name_length);
        memcpy( &(tmp_name[255 - total_length]), currentFileEntry->name, currentFileEntry->name_length);
        total_length++; // '/'のぶん
        tmp_name[255 - total_length] = '/';

        parentDirEntry = (MyDirEntry*)(currentFileEntry->parent);
        if( parentDirEntry)
        {
            total_length += parentDirEntry->full_path_name_length;
            memcpy( &(tmp_name[255 - total_length]), parentDirEntry->full_path_name, parentDirEntry->full_path_name_length);
        }
    }
    targetFileEntry->full_path_name = (char*)malloc( total_length + 1);
    memset( targetFileEntry->full_path_name, 0, total_length + 1);
    memcpy( targetFileEntry->full_path_name, &(tmp_name[255 - total_length]), total_length);
}

/* parent リンクを繋げる */
void Entry::FollowParent( void)
{
    MyDirEntry *currentDirEntry = dirEntry;
    while( currentDirEntry)
    {
        currentDirEntry->parent = FindDirEntry( currentDirEntry->parent_id);
        currentDirEntry = (MyDirEntry*)(currentDirEntry->next);
    }

    MyFileEntry *currentFileEntry = fileEntry;
    while( currentFileEntry)
    {
        currentFileEntry->parent = FindDirEntry( currentFileEntry->parent_id);
//        printf( "%s, 0x%x, 0x%x\n", __FUNCTION__, currentFileEntry->parent_id, (u32)currentFileEntry->parent);
        currentFileEntry = (MyFileEntry*)(currentFileEntry->next);
    }
}


void Entry::PrintAllDirEntry( void)
{
    MyDirEntry *currentEntry = dirEntry;
    printf( "------- all directories are -------\n");
    while( currentEntry)
    {
//        printf( "directory : %s\n", currentEntry->name);
        printf( "fullpath : %s\n", currentEntry->full_path_name);
//        printf( "id : 0x%x\n", currentEntry->self_id);
//        printf( "parent id : 0x%x\n", currentEntry->parent_id);
        currentEntry = (MyDirEntry*)(currentEntry->next);
    }
}

void Entry::PrintAllFileEntry( void)
{
    MyFileEntry *currentEntry = fileEntry;
    printf( "------- all files are -------\n");
    while( currentEntry)
    {
//        printf( "file : %s\n", currentEntry->name);
        printf( "fullpath : %s\n", currentEntry->full_path_name);
//        printf( "id : 0x%x\n", currentEntry->self_id);
//        printf( "parent id : 0x%x\n", currentEntry->parent_id);
        currentEntry = (MyFileEntry*)(currentEntry->next);
    }
}

/*
void Entry::CheckAllFiles( Checker* checker, MyFileEntry *anotherFileEntry)
{
    MyFileEntry *currentEntry = fileEntry;
    MyFileEntry *hisEntry;
    while( currentEntry)
    {
        printf( "- %s", currentEntry->full_path_name);
        
        hisEntry = FindFileEntry( currentEntry->full_path_name);
        if( hisEntry)
        {
            printf( " --->（存在している）\n");
            checker.Diff( currentEntry->top, (currentEntry->bottom - currentEntry->top),
                          hisEntry->top, (hisEntry->bottom - hisEntry->top),
                          false, true);
        }
        else
        {
            printf( " --->（存在していない）\n");
        }
    }
}*/


/* フルパスをセットする（事前にFollowParent()を済ませておくことが必要） */
void Entry::Finalize()
{
}


