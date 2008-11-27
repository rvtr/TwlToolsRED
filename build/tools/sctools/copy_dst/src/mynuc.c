#include <twl.h>
#include <twl/sea.h>
#include <twl/na.h>
// #include <TwlWiFi/nuc.h>

#include "netconnect.h"
#include "sitedefs.h"
#include "nuc.h"
#include "nuc_error_msg.h"
#include "mynuc.h"

#include        "text.h"
#include        "mprintf.h"
#include        "logprintf.h"
#include        "my_fs_util.h"

#define STRING_ANIM_CNT 20


static BOOL log_active = FALSE;
static FSFile *log_fd;
static FSFile log_fd_real;


static volatile NetConnectState NetConnect = NET_CONNECT_NONE;

// state�Ǘ�
static struct
{
    NucPhaseState state;
    u32 count;
} TestState;

static inline void ChangeState(NucPhaseState state)
{
    TestState.state = state;
    TestState.count = 0;
}

// titleID �擾�p
static NUCTitleId TitleIds[NUC_MAX_TITLE_UPDATE_COUNT];
static u32 TitleIdNum;

static u8 WorkForNA[NA_VERSION_DATA_WORK_SIZE];

static BOOL AllocFailTest = FALSE;

static void *alloc(u32 size, int align)
{
    u32 *ptr = NULL, *realPtr = NULL;
    u32 realSize;
    OSIntrMode  old;

    old = OS_DisableInterrupts();

    /* realSize is size plus alignment and header */
    if (align < 4)
    {
        align = 4;
    }
    realSize = size + align + 4;

    realPtr = (u32 *) OS_Alloc(realSize);
    ptr = (u32 *)((((u32) realPtr) + 4 + align - 1) & ~(align - 1));

    *((u32 *)(((u32)ptr) - 4)) = (u32) realPtr;
    (void)OS_RestoreInterrupts( old );

  end:
    //    SDK_ASSERT(((u32)ptr & (align - 1)) == 0);
    if( ((u32)ptr & (align - 1)) != 0 ) {
      return NULL;
    }
    return (void *) ptr;
}

static void free(void *p)
{
    u32 realPtr = *((u32 *)(((u32)p) - 4));
    OS_Free((void *) realPtr);
}

/*---------------------------------------------------------------------------*
  Name:         InitNupLib
  Description:  NUC���C�u�������J�n���܂��B
 *---------------------------------------------------------------------------*/

BOOL InitNupLib()
{
  BOOL ret;

  char *error_msg[] = {
    "NUC_ERROR_NO_SPACE",
    "NUC_ERROR_CONNECT",
    "NUC_ERROR_INTERNET",
    "NUC_ERROR_UPDATE"
  };

  ret = NUC_Init(alloc, free);

  if (ret == FALSE) {
    // NUC_Init() failed, error code=34416
    miya_log_fprintf(log_fd, "NUC_Init() failed, error code=%d\n", NUC_GetLastError());
    miya_log_fprintf(log_fd, " error type:%s\n", error_msg[ NUC_GetErrorType(NUC_GetLastError())] );
    mprintf("NUC_Init() failed, error code=%d\n", NUC_GetLastError());
    mprintf(" error type:%s\n", error_msg[ NUC_GetErrorType(NUC_GetLastError())] );
  }
  return ret;
}

/*---------------------------------------------------------------------------*
  Name:         StartNupCheck
  Description:  �_�E�����[�h���X�g�ꗗ�̎擾���J�n���܂��B
 *---------------------------------------------------------------------------*/
BOOL StartNupCheck(void)
{
    BOOL ret;
    TitleIdNum = sizeof(TitleIds) / sizeof(TitleIds[0]);

    ret = NUC_CheckAsync(TitleIds, &TitleIdNum);
    if (ret == FALSE)
    {
        miya_log_fprintf(log_fd, "NUC_CheckAsync() failed, error code=%d\n", NUC_GetLastError());
        mprintf("NUC_CheckAsync() failed, error code=%d\n", NUC_GetLastError());
    }

    return ret;
}


/*---------------------------------------------------------------------------*
  Name:         ProgressNupCheck
  Description:  �_�E�����[�h���X�g�ꗗ�̎擾�󋵂��m�F���܂�
 *---------------------------------------------------------------------------*/
NucStatus ProgressNupCheck(void)
{
    u64 CurrentSize, TotalSize;
    NucStatus status;

    NUC_GetProgress(&CurrentSize, &TotalSize, &status);
    if (status == NUC_STATUS_ERROR)
    {
      // NUC_GetProgress() failed in checking, error code=34303
        miya_log_fprintf(log_fd, "NUC_GetProgress() failed in checking, error code=%d\n", NUC_GetLastError());
        mprintf("NUC_GetProgress() failed in checking\n");
    }
    if (TestState.count++ % STRING_ANIM_CNT == 0)
    {
        u32 num = (TestState.count / STRING_ANIM_CNT) % 11;
        const char* msg[] = {
            "Now checking list.           ", 
            "Now checking list..          ",
            "Now checking list...         ",
	    "Now checking list....        ",
            "Now checking list.....       ",
            "Now checking list......      ",
            "Now checking list.......     ",
            "Now checking list........    ",
            "Now checking list.........   ",
            "Now checking list..........  ",
            "Now checking list........... ",
            "Now checking list............"};
        
        mprintf("%s\r", msg[num]);
    }

    return status;
}

/*---------------------------------------------------------------------------*
  Name:         StartNupDownload
  Description:  �_�E�����[�h���J�n���܂��B
 *---------------------------------------------------------------------------*/
BOOL StartNupDownload(void)
{
  /* �������Ă΂ꂽ��FS���؂藣����Ă��܂��̂Ń��u�[�g���K�v */
    BOOL ret = NUC_DownloadAsync(TitleIds, TitleIdNum);

    if (ret == FALSE)
    {
        miya_log_fprintf(log_fd, "NUP_DownloadAsync() failed, error code=%d\n", NUC_GetLastError());
        mprintf("NUP_DownloadAsync() failed, error code=%d\n", NUC_GetLastError());
    }

    return ret;
}

/*---------------------------------------------------------------------------*
  Name:         ProgressNupDownload
  Description:  �_�E�����[�h�̐i�s�󋵂�\�����܂��B
 *---------------------------------------------------------------------------*/
NucStatus ProgressNupDownload(void)
{
    u64 CurrentSize, TotalSize;
    NucStatus status;
    int error_code;
    NUC_GetProgress(&CurrentSize, &TotalSize, &status);
    if (status == NUC_STATUS_ERROR)
    {   // �G���[����
      // NUC_GetProgress() failed in checking, error code=34303
      error_code = NUC_GetLastError();
      miya_log_fprintf(log_fd, "NUC_GetProgress failed in download, error code=%d\n", NUC_GetLastError());
      mprintf("\nNUC_GetProgress failed in download\n");
      ShowErrorMsg(error_code);
    }

    if (TestState.count++ % STRING_ANIM_CNT == 0)
    {
        u32 num = (TestState.count / STRING_ANIM_CNT) % 12;
        const char* msg[] = {
            "Now downloading.           ", 
            "Now downloading..          ",
            "Now downloading...         ",
            "Now downloading....        ",
            "Now downloading.....       ",
            "Now downloading......      ",
            "Now downloading.......     ",
            "Now downloading........    ",
            "Now downloading.........   ",
            "Now downloading..........  ",
            "Now downloading........... ",
            "Now downloading............"};
        mprintf("%s\r", msg[num]);
    }

    return status;
}

/*---------------------------------------------------------------------------*
  Name:         CleanNupLib
  Description:  NUC���C�u�������I�����܂��B
 *---------------------------------------------------------------------------*/
BOOL CleanNupLib(void)
{
    BOOL ret = NUC_Cleanup(TitleIds, TitleIdNum);
    if (ret == FALSE)
    {
        miya_log_fprintf(log_fd, "NUP_CleanUp() failed, error code=%d\n", NUC_GetLastError());
        mprintf( "NUP_CleanUp() failed, error code=%d\n", NUC_GetLastError());
    }
    return ret;
}

/*---------------------------------------------------------------------------*
  Name:         ProgressNetConnect
  Description:  �l�b�g���[�N�ڑ���҂��܂��B
 *---------------------------------------------------------------------------*/
void ProgressNetConnect(void)
{
    if (TestState.count++ % STRING_ANIM_CNT == 0)
    {
        u32 num = (TestState.count / STRING_ANIM_CNT) % 12;
        const char* msg[] = {
            "Connecting network.           ",
            "Connecting network..          ",
            "Connecting network...         ",
            "Connecting network....        ",
            "Connecting network.....       ",
            "Connecting network......      ",
            "Connecting network.......     ",
            "Connecting network........    ",
            "Connecting network.........   ",
            "Connecting network..........  ",
            "Connecting network........... ",
            "Connecting network............"};
        mprintf("%s\r", msg[num]);
    }
}



void ShowErrorMsg(int error_code)
{
  if (error_code > 0) {
    mprintf("--Error Code:%d\n", error_code);
    mprintf(" %s\n", GetPublicMsg(error_code));
    mprintf(" %s\n", GetPrivateMsg(error_code));

    miya_log_fprintf(log_fd, "--Error Code:%d\n", error_code);
    miya_log_fprintf(log_fd, " %s\n", GetPublicMsg(error_code));
    miya_log_fprintf(log_fd, " %s\n", GetPrivateMsg(error_code));
  }
  else
    {
      miya_log_fprintf(log_fd,  "%s\n", "Network Error occurred.\nTry again later.");
      // �l�b�g���[�N�G���[���̕\�����b�Z�[�W(�b��)
      mprintf( "%s", "Network Error occurred.\nTry again later.\n");
    }
}

BOOL my_numc_proc(void)
{
  BOOL ret;
  NucStatus status;
  int error_code = 0;

  ChangeState(PHASE_NUP_BREAK);
    
  while(1) {
    OS_Sleep( 16 ); /* OS_WaitVBlankIntr�̑��� */
    
    switch ( TestState.state ) {
    case PHASE_NUP_BREAK:
      mprintf("NUP Initialize\n");
      miya_log_fprintf(log_fd, "NUP Initialize\n");
      ret = InitNupLib();
      if (ret == FALSE)
	{   // �G���[����
	  ChangeState(PHASE_ERROR_OCCURRED);
	  error_code = NUC_GetLastError();
	}
      else
	{
	  ChangeState(PHASE_NUP_READY);
	}
      break;

    case PHASE_NUP_READY: // �X�V���̎擾���J�n���܂��B
      mprintf("NUP Check\n");
      miya_log_fprintf(log_fd, "NUP Check\n");
      ret = StartNupCheck();
      if (ret == FALSE)
	{   // �G���[����
	  error_code = NUC_GetLastError();
	  ChangeState(PHASE_NUP_CLEANUP);
	}
      else
	{
	  ChangeState(PHASE_NUP_CHECK);
	}
      break;

    case PHASE_NUP_CHECK: // �X�V���̎擾�󋵂�\�����܂��B
      status = ProgressNupCheck();
      if (status == NUC_STATUS_ERROR)
	{   // �G���[����
	  ChangeState(PHASE_NUP_CLEANUP);
	  error_code = NUC_GetLastError();
	  ShowErrorMsg(error_code);
	}
      else if (status == NUC_STATUS_COMPLETED)
	{    // �X�V���X�g �擾�I��
	  mprintf("\n");
	  miya_log_fprintf(log_fd, "\n");
	  if (TitleIdNum > 0 )
	    {   // �_�E�����[�h��
	      int i;
	      for (i = 0; i < TitleIdNum; i++) {	
		miya_log_fprintf(log_fd, "DL list:%3d:0x%llx\n", i, TitleIds[i]);
		mprintf("DL list:%3d:0x%llx\n", i, TitleIds[i]);
	      }
	      ChangeState(PHASE_NUP_DOWNLOAD);
	    }
	  else
	    {   // �X�V���ׂ����̂��Ȃ�
	      mprintf("No title to update\n");
	      miya_log_fprintf(log_fd, "No title to update\n");
	      ChangeState(PHASE_NUP_CLEANUP);
	    }
	}
      break;

    case PHASE_NUP_DOWNLOAD: // �_�E�����[�h���J�n���܂��B
      mprintf("NUP Download\n");
      miya_log_fprintf(log_fd, "NUP Download\n");
      ret = StartNupDownload();
      if (ret == FALSE)
	{   // �G���[����
	  ChangeState(PHASE_NUP_CLEANUP);
	  error_code = NUC_GetLastError();
	}
      else
	{
	  ChangeState(PHASE_NUP_PROCESS);
	}
      break;

    case PHASE_NUP_PROCESS: // �_�E�����[�h�̐i�s�󋵂�\�����܂��B
      status = ProgressNupDownload();
      if (status == NUC_STATUS_ERROR)
	{   // �G���[����
	  mprintf("\n");
	  miya_log_fprintf(log_fd, "\n");
	  ChangeState(PHASE_NUP_CLEANUP);
	  error_code = NUC_GetLastError();
	}
      else if (status == NUC_STATUS_COMPLETED)
	{   // �_�E�����[�h����
	  mprintf("\n");
	  miya_log_fprintf(log_fd, "\n");
	  ChangeState(PHASE_NUP_CLEANUP);
	}
      break;

    case PHASE_NUP_CLEANUP:  // NUP���C�u�����̃N���[���A�b�v
      mprintf("NUP Cleanup\n");
      miya_log_fprintf(log_fd, "NUP Cleanup\n");
      ret = CleanNupLib();
      if (ret == FALSE && error_code == 0)
	{   // ���ŃG���[���N�����Ă��Ȃ��ꍇ�̂ݏ㏑�����܂��B
	  error_code = NUC_GetLastError();
	}
      // �l�b�g�̐ؒf�ƌ�n��
      ChangeState(PHASE_FINISHED);
      break;
            
    case PHASE_FINISHED:   // �l�b�g���[�N�A�b�v�f�[�g������I�����܂����B
      if( error_code == 0 ) {
	if (TitleIdNum > 0) {
	  miya_log_fprintf(log_fd, "%d file is updated\n", TitleIdNum);
	  mprintf("%d file is updated\n", TitleIdNum);
	}
	else {
	  miya_log_fprintf(log_fd, "Nothing is updated\n");
	  mprintf("Nothing is updated\n");
	}
      }
      goto end;
      break;

    case PHASE_ERROR_OCCURRED:  // �G���[���������Ă��܂��B
      ShowErrorMsg(error_code);
      goto end;
      break;

    default:
      break;
    }
  }
  end:
  if( error_code != 0 ) {
    return FALSE;
  }
  else {
    return TRUE;
  }
}


FSFile *my_nuc_log_start(char *log_file_name )
{
  log_fd = &log_fd_real;
  log_active = Log_File_Open( log_fd, log_file_name );
  if( !log_active ) {
    log_fd = NULL;
  }
  miya_log_fprintf(log_fd, "%s START\n", __FUNCTION__);
  return log_fd;
}

void my_nuc_log_end(void)
{
  miya_log_fprintf(log_fd, "%s END\n\n", __FUNCTION__);
  if( log_active ) {
    Log_File_Close(log_fd);
  }
}
