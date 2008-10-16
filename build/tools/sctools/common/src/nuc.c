#include <twl.h>
#include <twl/sea.h>
#include <twl/na.h>
#include <TwlWiFi/nuc.h>

#include "netconnect.h"
#include "sitedefs.h"
#include "nuc.h"
#include "nuc_error_msg.h"

// Network Update�̃t�F�[�Y��\��
typedef enum{

    PHASE_INIT,                 // �������
    PHASE_READY,                // �J�n
    PHASE_CONNECTING_NETWORK,   // �l�b�g���[�N�ڑ���
    PHASE_TEST_READY,           // �ڑ��e�X�g�J�n
    PHASE_TEST_PROCESS,         // �ڑ��e�X�g��
    PHASE_TEST_GETTING_WII_ID,  // Wii ID�擾����
    PHASE_TEST_FINISHED,        // �ڑ��e�X�g����
    PHASE_TEST_CLEANUP,         // �ڑ��e�X�g�㏈��
    PHASE_NUP_BREAK,            // <�L�[���͑҂�>�A�b�v�f�[�g�O�m�F�҂�
    PHASE_NUP_READY,            // �l�b�g���[�N�A�b�v�f�[�g�J�n
    PHASE_NUP_CHECK,            // �l�b�g���[�N�A�b�v�f�[�g�X�V���擾��
    PHASE_NUP_DOWNLOAD,         // �l�b�g���[�N�A�b�v�f�[�g �_�E�����[�h�J�n
    PHASE_NUP_PROCESS,          // �l�b�g���[�N�A�b�v�f�[�g��
    PHASE_NUP_FINISHED,         // �l�b�g���[�N�A�b�v�f�[�g����
    PHASE_NUP_CLEANUP,          // �l�b�g���[�N�A�b�v�f�[�g�㏈��
    PHASE_NUP_SKIPPED,          // �l�b�g���[�N�A�b�v�f�[�g���X�L�b�v���ꂽ
    PHASE_CLEANING_UP,          // �㏈��
    PHASE_FINISHED,             // ����
    PHASE_ERROR_OCCURRED         // �G���[����
} NucPhaseState;


// �l�b�g���[�N�ڑ����
typedef enum{
    NET_CONNECT_NONE,
    NET_CONNECT_OK,
    NET_CONNECT_ERROR
} NetConnectState;

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
    SDK_ASSERT(((u32)ptr & (align - 1)) == 0);
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
static BOOL InitNupLib()
{
  BOOL ret;

  ret = NUC_Init(alloc, free);

  if (ret == FALSE) {
    OS_TPrintf("NUC_Init() failed, error code=%d\n", NUC_GetLastError());
  }
  return ret;
}

/*---------------------------------------------------------------------------*
  Name:         StartNupCheck
  Description:  �_�E�����[�h���X�g�ꗗ�̎擾���J�n���܂��B
 *---------------------------------------------------------------------------*/
static BOOL StartNupCheck(void)
{
    BOOL ret;
    TitleIdNum = sizeof(TitleIds) / sizeof(TitleIds[0]);

    ret = NUC_CheckAsync(TitleIds, &TitleIdNum);
    if (ret == FALSE)
    {
        OS_TPrintf("NUC_CheckAsync() failed, error code=%d\n", NUC_GetLastError());
    }

    return ret;
}


/*---------------------------------------------------------------------------*
  Name:         ProgressNupCheck
  Description:  �_�E�����[�h���X�g�ꗗ�̎擾�󋵂��m�F���܂�
 *---------------------------------------------------------------------------*/
static NucStatus ProgressNupCheck(void)
{
    u64 CurrentSize, TotalSize;
    NucStatus status;

    NUC_GetProgress(&CurrentSize, &TotalSize, &status);
    if (status == NUC_STATUS_ERROR)
    {
        OS_TPrintf("NUC_GetProgress() failed in checking, error code=%d\n", NUC_GetLastError());
    }
#if 0
    if (TestState.count++ % STRING_ANIM_CNT == 0)
    {
        u32 num = (TestState.count / STRING_ANIM_CNT) % 3;
        const char* msg[] = {
            "Now checking list.   ", 
            "Now checking list..  ",
            "Now checking list... "};
        
        PrintString(TEXT_X, TEXT_Y, COLOR_WHITE, msg[num]);
    }
#endif    
    return status;
}

/*---------------------------------------------------------------------------*
  Name:         StartNupDownload
  Description:  �_�E�����[�h���J�n���܂��B
 *---------------------------------------------------------------------------*/
static BOOL StartNupDownload(void)
{
  /* �������Ă΂ꂽ��FS���؂藣����Ă��܂��̂Ń��u�[�g���K�v */
    BOOL ret = NUC_DownloadAsync(TitleIds, TitleIdNum);

    if (ret == FALSE)
    {
        OS_TPrintf("NUP_DownloadAsync() failed, error code=%d\n", NUC_GetLastError());
    }

    return ret;
}

/*---------------------------------------------------------------------------*
  Name:         ProgressNupDownload
  Description:  �_�E�����[�h�̐i�s�󋵂�\�����܂��B
 *---------------------------------------------------------------------------*/
static NucStatus ProgressNupDownload(void)
{
    u64 CurrentSize, TotalSize;
    NucStatus status;

    NUC_GetProgress(&CurrentSize, &TotalSize, &status);
    if (status == NUC_STATUS_ERROR)
    {   // �G���[����
        OS_TPrintf("NUC_GetProgress() failed in download, error code=%d\n", NUC_GetLastError());
    }
    else { 
#if 0
      // �_�E�����[�h�󋵂�`��
      int dw = (int)((CurrentSize * BAR_W)/TotalSize);
      FillRect(BAR_X, BAR_Y, dw, BAR_H, GX_RGBA(31, 16, 16, 1));
#endif
    }

#if 0
    if (TestState.count++ % STRING_ANIM_CNT == 0)
    {
        u32 num = (TestState.count / STRING_ANIM_CNT) % 3;
        const char* msg[] = {
            "Now downloading.     ", 
            "Now downloading..    ",
            "Now downloading...   "};
        PrintString(TEXT_X, TEXT_Y, COLOR_WHITE, msg[num]);
    }
#endif    
    return status;
}

/*---------------------------------------------------------------------------*
  Name:         CleanNupLib
  Description:  NUC���C�u�������I�����܂��B
 *---------------------------------------------------------------------------*/
static BOOL CleanNupLib()
{
    BOOL ret = NUC_Cleanup(TitleIds, TitleIdNum);
    if (ret == FALSE)
    {
        OS_TPrintf("NUP_CleanUp() failed, error code=%d\n", NUC_GetLastError());
    }
    return ret;
}

/*---------------------------------------------------------------------------*
  Name:         ProgressNetConnect
  Description:  �l�b�g���[�N�ڑ���҂��܂��B
 *---------------------------------------------------------------------------*/
static void ProgressNetConnect(void)
{
#if 0
    if (TestState.count++ % STRING_ANIM_CNT == 0)
    {
        u32 num = (TestState.count / STRING_ANIM_CNT) % 3;
        const char* msg[] = {
            "Connecting network.  ", 
            "Connecting network.. ",
            "Connecting network..."};
        PrintString(TEXT_X, TEXT_Y, COLOR_WHITE, msg[num]);
    }
#endif
}



static void ShowErrorMsg(int error_code)
{
#if 0
    PrintString(TEXT_X, TEXT_Y,       COLOR_WHITE,      "Error Occurred        ");
#endif
    if (error_code > 0)
    {
#if 0
        PrintString(TEXT_X, TEXT_Y + 10,  COLOR_WHITE, "Error Code:%d", error_code);
        PrintString(TEXT_X, TEXT_Y + 12, COLOR_WHITE, "%s", GetPublicMsg(error_code));
        PrintString(TEXT_X, TEXT_Y + 15, COLOR_WHITE, "%s", GetPrivateMsg(error_code));
#endif
        OS_TPrintf( "Error Code:%d\n", error_code);
        OS_TPrintf( "%s\n", GetPublicMsg(error_code));
        OS_TPrintf( "%s\n", GetPrivateMsg(error_code));
    }
    else
    {
      OS_TPrintf( "%s\n", "Network Error occurred.\nTry again later.");
#if 0
        // �l�b�g���[�N�G���[���̕\�����b�Z�[�W(�b��)
        PrintString(TEXT_X, TEXT_Y + 10, COLOR_WHITE, "%s", "Network Error occurred.\nTry again later.");
#endif
    }
}

// �l�b�g���[�N�ڑ��֘A
#define	STACK_SIZE	(1024*4)
static OSThread NetThread;
static u64 NetStack[STACK_SIZE / sizeof(u64)];
#define	  THREAD1_PRIO 24
static volatile BOOL NetHTTPEnd = FALSE;


static void NetConnectProc(void *arg)
{
#pragma unused(arg)
  BOOL ret;
  NucStatus status;
  int error_code = 0;

    
    /* Start networking */
    NcStart(SITEDEFS_DEFAULTCLASS);
    
    NetConnect = NET_CONNECT_OK;
    //OS_TPrintf("%s %d\n",__FUNCTION__,__LINE__);
    // �I���̌Ăяo����҂�
//    OS_SleepThread(NULL);
    
  while(1) {
    OS_Sleep( 16 ); /* OS_WaitVBlankIntr�̑��� */

    switch ( TestState.state ) {
    case PHASE_CONNECTING_NETWORK:
      ProgressNetConnect();
      if (NetConnect == NET_CONNECT_OK)
	{
	  ChangeState(PHASE_NUP_BREAK);
	}
      else if (NetConnect == NET_CONNECT_ERROR)
	{   // �l�b�g���[�N�ڑ��G���[
	  error_code = -1;
	  ChangeState(PHASE_ERROR_OCCURRED);
	}
      break;
    case PHASE_NUP_BREAK: // A�{�^����NUP���C�u���������������܂��B
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
	}
      else if (status == NUC_STATUS_COMPLETED)
	{    // �X�V���X�g �擾�I��
	  if (TitleIdNum > 0 )
	    {   // �_�E�����[�h��
	      int i;
	      for (i = 0; i < TitleIdNum; i++)
		{
		  OS_TPrintf("DL list:%3d:0x%llx", i, TitleIds[i]);
		}
	      ChangeState(PHASE_NUP_DOWNLOAD);
	    }
	  else
	    {   // �X�V���ׂ����̂��Ȃ�
	      OS_TPrintf("No title to update\n");
	      ChangeState(PHASE_NUP_CLEANUP);
	    }
	}
      break;

    case PHASE_NUP_DOWNLOAD: // �_�E�����[�h���J�n���܂��B
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
	  ChangeState(PHASE_NUP_CLEANUP);
	  error_code = NUC_GetLastError();
	}
      else if (status == NUC_STATUS_COMPLETED)
	{   // �_�E�����[�h����
	  ChangeState(PHASE_NUP_CLEANUP);
	}
      break;

    case PHASE_NUP_CLEANUP:  // NUP���C�u�����̃N���[���A�b�v
      ret = CleanNupLib();
      if (ret == FALSE && error_code == 0)
	{   // ���ŃG���[���N�����Ă��Ȃ��ꍇ�̂ݏ㏑�����܂��B
	  error_code = NUC_GetLastError();
	}
      // �l�b�g�̐ؒf�ƌ�n��
      OS_WakeupThreadDirect(&NetThread);
      ChangeState(PHASE_CLEANING_UP);
      break;
            
    case PHASE_CLEANING_UP:  // �l�b�g�̐ؒf�I����҂��܂�
      if (NetConnect == NET_CONNECT_NONE)
	{   // �ؒf�I��
	  if (error_code == 0)
	    {    // ����I��
	      ChangeState(PHASE_FINISHED);
	    }
	  else
	    {
	      ChangeState(PHASE_ERROR_OCCURRED);
	    }
	}
      break;
            
    case PHASE_FINISHED:   // �l�b�g���[�N�A�b�v�f�[�g������I�����܂����B
      OS_TPrintf("Network Update Completed\n");
      if (TitleIdNum > 0)
	{
	  OS_TPrintf("%d file is updated\n", TitleIdNum);
	}
      else
	{
	  OS_TPrintf("Nothing is updated\n");
	}

      if (TitleIdNum > 0)
	{    // �ċN������
	  OS_TPrintf("�ċN�����܂�\n");
	}
      else
	{   // ���b�Z�[�W��\�����Ė߂�
	  OS_TPrintf("�A�b�v�f�[�g������̂��Ȃ������̂Ŗ߂�܂�\n");
	}
      goto end;

      break;
            
    case PHASE_ERROR_OCCURRED:  // �G���[���������Ă��܂��B

      ShowErrorMsg(error_code);
      goto end;
#if 0
      if (Trg == PAD_BUTTON_A)
	{
	  OS_TPrintf("�G���[�����������̂Ŗ߂�܂�\n");
	  goto end;
	}
#endif
      break;

    default:
OS_TPrintf("%s %d\n",__FUNCTION__,__LINE__);

      break;
    }
  }
  
  end:
    NcFinish();
    NetConnect = NET_CONNECT_NONE;

    OS_TPrintf("Network Connection ended\n");
}


/* ********************** */
void nuc_main(void)
{

  ChangeState(PHASE_INIT);
  // NcGlobalInit(); in netconnect.c

  /* �N���C�A���g�ؖ����̏����� */
  SEA_Init();
  // ���炩����WRAM�Ƀ��[�h���Ă����܂�
  if (!NA_LoadVersionDataArchive(WorkForNA, NA_VERSION_DATA_WORK_SIZE)) {
    OS_TPrintf("NA load error\n");
    goto end;
  }
  
  if (!NUC_LoadCert()) {
    // WRAM�Ƀ��[�h
    OS_TPrintf("Client cert load error\n");
    goto end;
  }
  
  (void)NA_UnloadVersionDataArchive();


  /* �ڑ��ݒ�X���b�h�̍쐬 */
  OS_CreateThread(&NetThread, NetConnectProc,
		  NULL, NetStack + STACK_SIZE / sizeof(u64),
		  STACK_SIZE, THREAD1_PRIO);
  OS_WakeupThreadDirect(&NetThread);
  ChangeState(PHASE_CONNECTING_NETWORK);

 end:
  return;
}
