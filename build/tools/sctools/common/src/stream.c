#include <twl.h>
#include "stream.h"

#define MIYA_MEM_FILE 1

#define MAKE_FOURCC(cc1, cc2, cc3, cc4) (u32)((cc1) | (cc2 << 8) | (cc3 << 16) | (cc4 << 24))

#define FOURCC_RIFF MAKE_FOURCC('R', 'I', 'F', 'F')
#define FOURCC_WAVE MAKE_FOURCC('W', 'A', 'V', 'E')
#define FOURCC_fmt  MAKE_FOURCC('f', 'm', 't', ' ')
#define FOURCC_data MAKE_FOURCC('d', 'a', 't', 'a')

#define L_CHANNEL 4
#define R_CHANNEL 5
#define ALARM_NUM 0
#define STREAM_THREAD_PRIO 12
#define THREAD_STACK_SIZE (1024 * 4)
#define STRM_BUF_PAGESIZE 1024*32
#define STRM_BUF_SIZE STRM_BUF_PAGESIZE*2


#ifdef MIYA_MEM_FILE

//#define NUM_OF_MY_FS_FILE 4
#define NUM_OF_MY_FS_FILE 3

typedef struct {
  FSFile  file;
  char path[256];
  u8 *data;
  u32 length;
  BOOL open_flag;
  u32 position;
} MyFSFile;

static MyFSFile myfsfile[NUM_OF_MY_FS_FILE];

#endif


// WAVフォーマットヘッダ
typedef struct WaveFormat
{
    u16     format;
    u16     channels;
    s32     sampleRate;
    u32     dataRate;
    u16     blockAlign;
    u16     bitPerSample;
}
WaveFormat;

// ストリームオブジェクト
typedef struct StreamInfo
{
#ifdef MIYA_MEM_FILE
  MyFSFile  file;
#else
    FSFile  file;
#endif
    WaveFormat format;
    u32     beginPos;
    s32     dataSize;
    u32     bufPage;
    BOOL    isPlay;
}
StreamInfo;

static BOOL ReadWaveFormat(StreamInfo * strm);
static void ReadStrmData(StreamInfo * strm);
static void SoundAlarmHandler(void *arg);
static void StrmThread(void *arg);
static void VBlankIntr(void);
static void PlayStream(StreamInfo * strm, const char *filename);
static void StopStream(StreamInfo * strm);

static u16 Cont;
static u16 Trg;
static u64 strmThreadStack[THREAD_STACK_SIZE / sizeof(u64)];
static OSThread strmThread;
static OSMessageQueue msgQ;
static OSMessage msgBuf[1];

static u8 strm_lbuf[STRM_BUF_SIZE] ATTRIBUTE_ALIGN(32);
static u8 strm_rbuf[STRM_BUF_SIZE] ATTRIBUTE_ALIGN(32);
static u8 strm_tmp[STRM_BUF_PAGESIZE * 2] ATTRIBUTE_ALIGN(32);

// ファイル名
//const char filename2[] = "kart_title.32.wav";
//const char filename1[] = "fanfare.32.wav";
const char filename0[] = "cursor.wav";
const char filename1[] = "ok.wav";
const char filename2[] = "ng.wav";
//const char filename3[] = "fanfare.32.wav";

static StreamInfo strm;

#ifdef MIYA_MEM_FILE
static BOOL MY_FS_OpenFile(MyFSFile *my_file, const char *name)
{

  if( STD_StrCmp(filename0, name) == 0 ) {
    STD_CopyMemory( (void *)my_file, (void *)&(myfsfile[0]) ,sizeof(MyFSFile) );
    my_file->open_flag = TRUE;
    return TRUE;
  }
  else if( STD_StrCmp(filename1, name) == 0 ) {
    STD_CopyMemory( (void *)my_file, (void *)&(myfsfile[1]) ,sizeof(MyFSFile) );
    my_file->open_flag = TRUE;
    return TRUE;
  }
  else if( STD_StrCmp(filename2, name) == 0 ) {
    STD_CopyMemory( (void *)my_file, (void *)&(myfsfile[2]) ,sizeof(MyFSFile) );
    my_file->open_flag = TRUE;
    return TRUE;
  }
#if 0
  else if( STD_StrCmp(filename3, name) == 0 ) {
    STD_CopyMemory( (void *)my_file, (void *)&(myfsfile[3]) ,sizeof(MyFSFile) );
    my_file->open_flag = TRUE;
    return TRUE;
  }
#endif
  my_file->open_flag = FALSE;
  return FALSE;
}

static BOOL MY_FS_IsFile(MyFSFile *my_file)
{
  return my_file->open_flag;
}


static BOOL MY_FS_CloseFile(MyFSFile *my_file)
{
  my_file->open_flag = FALSE;;
  return TRUE;
}

static u32 MY_FS_GetPosition(MyFSFile *my_file)
{
  return my_file->position;
}

//static BOOL MY_FS_SeekFile( MyFSFile *p_file, s32 offset, FSSeekMode origin)
static BOOL MY_FS_SeekFile( MyFSFile *p_file, s32 offset, FSSeekFileMode origin)
{
  s32 temp_offset;

  switch( origin ) {
  case FS_SEEK_SET:   /* seek from begin */
    temp_offset = offset;
    if( (0 <= (s32)temp_offset) && (temp_offset < (s32)p_file->length)) {
      p_file->position = (u32)temp_offset;
      return TRUE;
    }
    break;
  case FS_SEEK_CUR:   /* seek from current */
    temp_offset = offset + (s32)p_file->position;
    if( (0 <= (s32)temp_offset) && (temp_offset < (s32)p_file->length)) {
      p_file->position = (u32)temp_offset;
      return TRUE;
    }
    p_file->position = p_file->length-1;
    return TRUE;

    break;
  case FS_SEEK_END:    
    temp_offset = (s32)(p_file->length - 1);
    if( (0 <= (s32)temp_offset) && (temp_offset < (s32)p_file->length)) {
      p_file->position = (u32)temp_offset;
      return TRUE;
    }
    break;
  }
  return FALSE;
}


static BOOL MY_FS_SeekFileToBegin(MyFSFile *my_file)
{
  return MY_FS_SeekFile(my_file, 0, FS_SEEK_SET);
}

static s32 MY_FS_ReadFile( MyFSFile *p_file, void *dst, s32 len)
{
  s32 new_len;

  if( dst == NULL ) {
    return -1;
  }
  if( len < 0  ) {
    return -1;
  }
  if( p_file == NULL ) {
    return -1;
  }
  if( p_file->data == NULL ) {
    return -1;
  }
  
  if( p_file->open_flag ) {
    if( (p_file->position < 0 ) ) {
      return -1;
    }
    if( (p_file->position + len) < 0 ) {
      return -1;
    }
    
    if( (p_file->length <= (p_file->position + len)) ) {
      new_len = (s32)(p_file->position + len - p_file->length);
      new_len = len - new_len;
      STD_CopyMemory( (void *)dst, (void *)(p_file->data + p_file->position) ,(u32)new_len );
      p_file->position += new_len;
      return new_len;
    }

    STD_CopyMemory( (void *)dst, (void *)(p_file->data + p_file->position) ,(u32)len );
    p_file->position += len;
    return len;
  }
  else {
    return -1;
  }
  return -1;
}



#endif

void stream_main(void)
{
  int i;
  s32 readSize;

  strm.isPlay = FALSE;
  SND_LockChannel((1 << L_CHANNEL) | (1 << R_CHANNEL), 0);

#ifdef MIYA_MEM_FILE

  STD_StrCpy(myfsfile[0].path, filename0 );
  STD_StrCpy(myfsfile[1].path, filename1 );
  STD_StrCpy(myfsfile[2].path, filename2 );

  for( i = 0 ; i < NUM_OF_MY_FS_FILE ; i++ ) {
    FS_InitFile(&(myfsfile[i].file));

    if ( !FS_OpenFile(&(myfsfile[i].file), myfsfile[i].path) ) {
      // error;
    }
    myfsfile[i].open_flag = FALSE;
    myfsfile[i].position = 0;
    myfsfile[i].length = FS_GetFileLength(&(myfsfile[i].file));
    myfsfile[i].data = (u8 *)OS_Alloc(myfsfile[i].length);
    if( myfsfile[i].data == NULL ) {
      OS_TPrintf("Mem alloc error: %s %d\n", __FUNCTION__,__LINE__);
      return;
    }
    readSize = 0;

    //    OS_TPrintf("%s %d Readfile %s len=%d\n",__FUNCTION__,__LINE__,myfsfile[i].path, myfsfile[i].length);
    readSize = FS_ReadFile(&(myfsfile[i].file), myfsfile[i].data, (int)myfsfile[i].length );
    if( readSize != myfsfile[i].length ) {
      OS_TPrintf("Failed Read File: %s %s %d\n",myfsfile[i].path, __FUNCTION__,__LINE__);
      return;
    }
    (void)FS_CloseFile(&(myfsfile[i].file));
  }
  
#endif 

  /* ストリームスレッドの起動 */
  OS_CreateThread(&strmThread,
		  StrmThread,
		  NULL,
		  strmThreadStack + THREAD_STACK_SIZE / sizeof(u64),
		  THREAD_STACK_SIZE, STREAM_THREAD_PRIO);
  OS_WakeupThreadDirect(&strmThread);
}

void stream_play0(void)
{
  PlayStream(&strm, filename0);
}

void stream_play1(void)
{
  PlayStream(&strm, filename1);
}

void stream_play2(void)
{
  PlayStream(&strm, filename2);
}

#if 0
void stream_play3(void)
{
  PlayStream(&strm, filename3);
}
#endif

BOOL stream_play_is_end(void)
{
  if (strm.dataSize <= 0) {
    return TRUE;
  }
  return FALSE;
}

static void PlayStream(StreamInfo * strm, const char *filename)
{
    int     timerValue;
    u32     alarmPeriod;

    // 再生中であれば停止する
    // OS_TPrintf("%s %d\n",__FUNCTION__,__LINE__);


    if (strm->isPlay)
    {
        u32     tag;
        StopStream(strm);
        tag = SND_GetCurrentCommandTag();
        (void)SND_FlushCommand(SND_COMMAND_NOBLOCK | SND_COMMAND_IMMEDIATE);
        SND_WaitForCommandProc(tag);   // 停止を待つ
    }

#ifdef MIYA_MEM_FILE
    // ファイル走査
    if (MY_FS_IsFile(&strm->file)) {
        (void)MY_FS_CloseFile(&strm->file);
    }

    if ( ! MY_FS_OpenFile(&strm->file, filename) ) {
      OS_Panic("Error: failed to open file %s\n", filename);
    }
#else
    // ファイル走査
    if (FS_IsFile(&strm->file)) {
        (void)FS_CloseFile(&strm->file);
    }
    if ( ! FS_OpenFile(&strm->file, filename) ) {
        OS_Panic("Error: failed to open file %s\n", filename);
    }
#endif
    if (!ReadWaveFormat(strm))
    {
        OS_Panic("Error: failed to read wavefile\n");
    }

    strm->isPlay = TRUE;

    /* パラメータ設定 */
    timerValue = SND_TIMER_CLOCK / strm->format.sampleRate;
    alarmPeriod = timerValue * STRM_BUF_PAGESIZE / 32U;
    alarmPeriod /= (strm->format.bitPerSample == 16) ? sizeof(s16) : sizeof(s8);

    // 初期ストリームデータ読み込み
#ifdef MIYA_MEM_FILE
    (void)MY_FS_SeekFile(&strm->file, (s32)strm->beginPos, FS_SEEK_SET);
#else
    (void)FS_SeekFile(&strm->file, (s32)strm->beginPos, FS_SEEK_SET);
#endif
    strm->bufPage = 0;
    ReadStrmData(strm);
    ReadStrmData(strm);

    // チャンネルとアラームをセットアップ
    SND_SetupChannelPcm(L_CHANNEL,
                        (strm->format.bitPerSample ==
                         16) ? SND_WAVE_FORMAT_PCM16 : SND_WAVE_FORMAT_PCM8, strm_lbuf,
                        SND_CHANNEL_LOOP_REPEAT, 0, STRM_BUF_SIZE / sizeof(u32), 127,
                        SND_CHANNEL_DATASHIFT_NONE, timerValue, 0);
    SND_SetupChannelPcm(R_CHANNEL,
                        (strm->format.bitPerSample ==
                         16) ? SND_WAVE_FORMAT_PCM16 : SND_WAVE_FORMAT_PCM8,
                        (strm->format.channels == 1) ? strm_lbuf : strm_rbuf,
                        SND_CHANNEL_LOOP_REPEAT, 0, STRM_BUF_SIZE / sizeof(u32), 127,
                        SND_CHANNEL_DATASHIFT_NONE, timerValue, 127);
    SND_SetupAlarm(ALARM_NUM, alarmPeriod, alarmPeriod, SoundAlarmHandler, strm);
    SND_StartTimer((1 << L_CHANNEL) | (1 << R_CHANNEL), 0, 1 << ALARM_NUM, 0);
}

/*---------------------------------------------------------------------------*
  Name:         StopStream

  Description:  ストリーム再生を停止

  Arguments:    strm - ストリームオブジェクト

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void StopStream(StreamInfo * strm)
{
    SND_StopTimer((1 << L_CHANNEL) | (1 << R_CHANNEL), 0, 1 << ALARM_NUM, 0);
#ifdef MIYA_MEM_FILE
    if (MY_FS_IsFile(&strm->file)) {
        (void)MY_FS_CloseFile(&strm->file);
    }
#else
    if (FS_IsFile(&strm->file)) {
        (void)FS_CloseFile(&strm->file);
    }
#endif
    strm->isPlay = FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         StrmThread

  Description:  ストリームスレッド

  Arguments:    arg - ユーザーデータ（未使用）

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void StrmThread(void * /*arg */ )
{
    OSMessage message;

    OS_InitMessageQueue(&msgQ, msgBuf, 1);

    while (1)
    {
        (void)OS_ReceiveMessage(&msgQ, &message, OS_MESSAGE_BLOCK);
        (void)ReadStrmData((StreamInfo *) message);
    }
}

/*---------------------------------------------------------------------------*
  Name:         SoundAlarmHandler

  Description:  アラームコールバック関数

  Arguments:    arg - ストリームオブジェクト

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void SoundAlarmHandler(void *arg)
{
    (void)OS_SendMessage(&msgQ, (OSMessage)arg, OS_MESSAGE_NOBLOCK);
}

/*---------------------------------------------------------------------------*
  Name:         ReadStrmData

  Description:  ストリームデータ読み込み関数
                ファイルからバッファの１ページ分のストリームデータを読み込む

  Arguments:    strm - ストリームオブジェクト

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void ReadStrmData(StreamInfo * strm)
{
    int     i;
    s32     readSize;
    u8     *lbuf, *rbuf;

    // ストリームが終端に達している
    if (strm->dataSize <= 0)
    {
        StopStream(strm);
        return;
    }

    // バッファのページ設定
    if (strm->bufPage == 0)
    {
        lbuf = strm_lbuf;
        rbuf = strm_rbuf;
        strm->bufPage = 1;
    }
    else
    {
        lbuf = strm_lbuf + STRM_BUF_PAGESIZE;
        rbuf = strm_rbuf + STRM_BUF_PAGESIZE;
        strm->bufPage = 0;
    }

    // データ読み込み
    if (strm->format.channels == 1)
    {
        // モノラル
#ifdef MIYA_MEM_FILE
        readSize = MY_FS_ReadFile(&strm->file,
                               strm_tmp,
                               (strm->dataSize <
                                STRM_BUF_PAGESIZE) ? strm->dataSize : STRM_BUF_PAGESIZE);
#else
        readSize = FS_ReadFile(&strm->file,
                               strm_tmp,
                               (strm->dataSize <
                                STRM_BUF_PAGESIZE) ? strm->dataSize : STRM_BUF_PAGESIZE);
#endif
        if (readSize == -1)
            OS_Panic("read file end\n");

        if (strm->format.bitPerSample == 16)
        {
            // 16bitデータ
            for (i = 0; i < readSize / sizeof(s16); i++)
            {
                ((s16 *)lbuf)[i] = ((s16 *)strm_tmp)[i];
            }
            for (; i < STRM_BUF_PAGESIZE / sizeof(s16); i++)
            {
                ((s16 *)lbuf)[i] = 0;  // ストリームの終端に達した場合、残りを０で埋める
            }
        }
        else
        {
            // 8bitデータ
            for (i = 0; i < readSize / sizeof(s8); i++)
            {
                ((s8 *)lbuf)[i] = (s8)((s16)strm_tmp[i] - 128);
            }
            for (; i < STRM_BUF_PAGESIZE / sizeof(s8); i++)
            {
                ((s8 *)lbuf)[i] = 0;
            }
        }
    }
    else
    {
        // ステレオ
#ifdef MIYA_MEM_FILE
        readSize = MY_FS_ReadFile(&strm->file,
                               strm_tmp,
                               (strm->dataSize <
                                STRM_BUF_PAGESIZE * 2) ? strm->dataSize : STRM_BUF_PAGESIZE * 2);

#else
        readSize = FS_ReadFile(&strm->file,
                               strm_tmp,
                               (strm->dataSize <
                                STRM_BUF_PAGESIZE * 2) ? strm->dataSize : STRM_BUF_PAGESIZE * 2);
#endif
        if (readSize == -1)
            OS_Panic("read file end\n");

        if (strm->format.bitPerSample == 16)
        {
            // 16bitデータ
            for (i = 0; i < (readSize / 2) / sizeof(s16); i++)
            {
                ((s16 *)lbuf)[i] = ((s16 *)strm_tmp)[2 * i];
                ((s16 *)rbuf)[i] = ((s16 *)strm_tmp)[2 * i + 1];
            }
            for (; i < STRM_BUF_PAGESIZE / sizeof(s16); i++)
            {
                ((s16 *)lbuf)[i] = 0;
                ((s16 *)rbuf)[i] = 0;
            }
        }
        else
        {
            // 8bitデータ
            for (i = 0; i < (readSize / 2) / sizeof(s8); i++)
            {
                ((s8 *)lbuf)[i] = (s8)((s16)strm_tmp[2 * i] - 128);
                ((s8 *)rbuf)[i] = (s8)((s16)strm_tmp[2 * i + 1] - 128);
            }
            for (; i < STRM_BUF_PAGESIZE / sizeof(s8); i++)
            {
                ((s8 *)lbuf)[i] = 0;
                ((s8 *)rbuf)[i] = 0;
            }
        }
    }

    strm->dataSize -= readSize;

    return;
}


/*---------------------------------------------------------------------------*
  Name:         ReadWaveFormat

  Description:  WAVEフォーマットのデータのヘッダとデータ列の先頭位置、データサイズを取得

  Arguments:    strm - ストリームオブジェクト

  Returns:      読み取りに成功したらTRUE、失敗したらFALSE
 *---------------------------------------------------------------------------*/
static BOOL ReadWaveFormat(StreamInfo * strm)
{
    u32     tag;
    s32     size;
    BOOL    fFmt = FALSE, fData = FALSE;
#ifdef MIYA_MEM_FILE
    (void)MY_FS_SeekFileToBegin(&strm->file);
    (void)MY_FS_ReadFile(&strm->file, &tag, 4);
#else
    (void)FS_SeekFileToBegin(&strm->file);
    (void)FS_ReadFile(&strm->file, &tag, 4);
#endif
    if (tag != FOURCC_RIFF) {
OS_TPrintf("%s %d\n",__FUNCTION__,__LINE__);
        return FALSE;
    }

#ifdef MIYA_MEM_FILE
    (void)MY_FS_ReadFile(&strm->file, &size, 4);
    (void)MY_FS_ReadFile(&strm->file, &tag, 4);
#else
    (void)FS_ReadFile(&strm->file, &size, 4);
    (void)FS_ReadFile(&strm->file, &tag, 4);
#endif

    if (tag != FOURCC_WAVE) {
OS_TPrintf("%s %d\n",__FUNCTION__,__LINE__);
        return FALSE;
    }
    while (size > 0)
    {
        s32     chunkSize;
#ifdef MIYA_MEM_FILE
        if (MY_FS_ReadFile(&strm->file, &tag, 4) == -1)
        {
OS_TPrintf("%s %d\n",__FUNCTION__,__LINE__);
            return FALSE;
        }
        if (MY_FS_ReadFile(&strm->file, &chunkSize, 4) == -1)
        {
OS_TPrintf("%s %d\n",__FUNCTION__,__LINE__);
            return FALSE;
        }
#else
        if (FS_ReadFile(&strm->file, &tag, 4) == -1)
        {
            return FALSE;
        }
        if (FS_ReadFile(&strm->file, &chunkSize, 4) == -1)
        {
            return FALSE;
        }
#endif
        switch (tag)
        {
        case FOURCC_fmt:
#ifdef MIYA_MEM_FILE
            if (MY_FS_ReadFile(&strm->file, (u8 *)&strm->format, chunkSize) == -1)
            {
OS_TPrintf("%s %d\n",__FUNCTION__,__LINE__);
                return FALSE;
            }
#else
            if (FS_ReadFile(&strm->file, (u8 *)&strm->format, chunkSize) == -1)
            {
                return FALSE;
            }
#endif
            fFmt = TRUE;
            break;
        case FOURCC_data:
#ifdef MIYA_MEM_FILE
            strm->beginPos = MY_FS_GetPosition(&strm->file);
            strm->dataSize = chunkSize;
            (void)MY_FS_SeekFile(&strm->file, chunkSize, FS_SEEK_CUR);
#else
            strm->beginPos = FS_GetPosition(&strm->file);
            strm->dataSize = chunkSize;
            (void)FS_SeekFile(&strm->file, chunkSize, FS_SEEK_CUR);
#endif
            fData = TRUE;
            break;
        default:
#ifdef MIYA_MEM_FILE
            (void)MY_FS_SeekFile(&strm->file, chunkSize, FS_SEEK_CUR);
#else
            (void)FS_SeekFile(&strm->file, chunkSize, FS_SEEK_CUR);
#endif
            break;
        }
        if (fFmt && fData)
        {
            return TRUE;               // fmt と data を読み終えたら強制終了
        }

        size -= chunkSize;
    }

    if (size != 0) {
OS_TPrintf("%s %d\n",__FUNCTION__,__LINE__);
        return FALSE;
    }
    return TRUE;
}
