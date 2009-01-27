/*---------------------------------------------------------------------------*
  Project:  TwlSDK - tests - appjumpTest - Nand-2
  File:     main.c

  Copyright 2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-03#$
  $Rev: 8251 $
  $Author: nishimoto_takashi $
 *---------------------------------------------------------------------------*/
#include <twl/nam.h>
#include <twl.h>
#include <twl/os/common/format_rom.h>

#include "application_jump_private.h"
#include "common.h"
#include "screen.h"

#define DMA_NO_FS		1
#define CHECK_APP_NUM	10
#define REGION_NUM		4

/*---------------------------------------------------------------------------*
    �ϐ� ��`
 *---------------------------------------------------------------------------*/
static u8 gRegion;

static u8 gDisableDebugFlg[CHECK_APP_NUM];
static BOOL gError[CHECK_APP_NUM];

const static u32 gamecode[CHECK_APP_NUM][REGION_NUM] = {
// 	 J ���{		 U ��		 P ���B		 E �I�[�X�g
    {0x484e414A, 0x484e4155, 0x484e4150, 0x484e4145}, // �����`��
    {0x484e424A, 0x484e4255, 0x484e4250, 0x484e4245}, // �{�̐ݒ�
    {0x484e4441, 0x484e4441, 0x484e4441, 0x484e4441}, // �_�E��			all region
    {0x484e4541, 0x484e4541, 0x484e4541, 0x484e4541}, // �s�N�g�`���b�g	all region
    {0x484e464A, 0x484e4655, 0x484e4650, 0x484e4645}, // �V���b�v
    {0x484e494A, 0x484e4955, 0x484e4950, 0x484e4945}, // �J����
    {0x484e4a4A, 0x484e4a55, 0x484e4a50, 0x484e4a45}, // �]�[��
    {0x484e4b4A, 0x484e4b55, 0x484e4b50, 0x484e4b45}, // �T�E���h
    {0x344e4641, 0x344e4641, 0x344e4641, 0x344e4641}, // NandFiler		all region
    {0x34544E41, 0x34544E41, 0x34544E41, 0x34544E41}  // TwlNMenu		all region
};

static NAMTitleId titleID[CHECK_APP_NUM] = {
	0x00030017484e4100, // �����`��
	0x00030015484e4200, // �{�̐ݒ�
	0x00030005484e4441, // DS�_�E�����[�h�v���C all region
	0x00030005484e4541, // �s�N�g�`���b�g		all region
	0x00030015484e4600, // �V���b�v
	0x00030005484e4900, // �J����
	0x00030005484e4a00, // �]�[��
	0x00030005484e4b00, // �T�E���h
	0x00030015344e4641, // NandFiler			all region
	0x0003001534544E41, // TwlNMenu				all region
};

const static u32 regioncode[REGION_NUM] = {
	0x4A, // ���{
	0x45, // �A�����J
    0x50, // ���B
    0x55, // �I�[�X�g�����A
};

char *gAppName[CHECK_APP_NUM] = {
	"Launcher",
    "Setting",
    "DS Download",
    "PictChat",
    "Shop",
    "Camera",
    "NintendoZone",
    "DS Sound",
    "NandFiler",
    "TwlNMenu"
};

typedef struct DataStruct
{
	NAMTitleId	id;
	BOOL		normaljmp_flag;
} DataStruct;

/*---------------------------------------------------------------------------*
   Prototype
 *---------------------------------------------------------------------------*/
static void DrawScene(void);
static void* AllocForNAM(u32 size);
static void FreeForNAM(void* ptr);
static void CheckDisableDebugFlg(void);
/*---------------------------------------------------------------------------*/


void TwlMain(void)
{
	InitCommon();
    InitScreen();

    GX_DispOn();
    GXS_DispOn();

	FS_Init(DMA_NO_FS);
    NAM_Init(AllocForNAM, FreeForNAM);

    ClearScreen();
    
	// �{�̃��[�W�����𒲂ׂ�
    // OSTwlRegion�񋓌^
    // OS_TWL_REGION_JAPAN 		0 
	// OS_TWL_REGION_AMERICA	1 
	// OS_TWL_REGION_EUROPE 	2 
	// OS_TWL_REGION_AUSTRALIA 	3

	gRegion = OS_GetRegion();

    OS_TPrintf("Region : %d\n", gRegion);

    CheckDisableDebugFlg();

    while(TRUE)
    {
		// ��ʕ`��
		DrawScene();
        
        // �u�u�����N�҂�
        OS_WaitVBlankIntr();
        
        // ��ʃN���A
        ClearScreen();
    }

	// �u�u�����N�҂� �Ō�ɉ�ʂ��X�V���Ă���I��
    OS_WaitVBlankIntr();
    OS_Terminate();
}


static void CheckDisableDebugFlg(void)
{
	char file_path[FS_ENTRY_LONGNAME_MAX];
    u8 buf[0x1000];
    u32 i;
    
    for(i=0; i<CHECK_APP_NUM; i++){
        // all region�̃A�v���͂��̂܂܂�OK
        if(i != 2 && i != 3 && i != 8 && i != 9){
        	titleID[i] |= regioncode[gRegion];
        }

    	// NAMTitleId -> NAMTitleId -> u64
    	if(NAM_GetTitleBootContentPath( file_path, titleID[i] ) == NAM_OK){
			FSFile f;
            BOOL bSuccess;
            s32 readSize;
            
			OS_TPrintf(" ok ");
			OS_TPrintf(" %s ", file_path);

            FS_InitFile(&f);

            // �t�@�C���I�[�v��
            bSuccess = FS_OpenFileEx(&f, file_path, FS_FILEMODE_R);

            if(!bSuccess){
                OS_TPrintf(" File Open Error...\n");
                gError[i] = TRUE;
				continue;
            }
            // �t�@�C�����[�h
            readSize = FS_ReadFile(&f, buf, sizeof(buf));

			// Disable Debug Flg �̃`�F�b�N
            if(((ROM_Header *)buf)->s.disable_debug){
				gDisableDebugFlg[i] = '1';
            }
            else{
				gDisableDebugFlg[i] = '0';
            }
			OS_TPrintf("DisableFlg( %c )", gDisableDebugFlg[i]);
            
            // �t�@�C���N���[�Y
            bSuccess = FS_CloseFile(&f);

            OS_TPrintf(" File Read/Close Success\n");
            gError[i] = FALSE;
    	}
    	else{
			OS_TPrintf(" ng \n");
            gError[i] = TRUE;
            gDisableDebugFlg[i] = '-';
    	}
    }
}


static void DrawScene(void)
{
	s32 i;
    u8 color;
    BOOL success = TRUE;
    
    for(i=0; i<CHECK_APP_NUM; i++){
        if(!gError[i]){
            if(gDisableDebugFlg[i] == '0'){
				success = FALSE;
            }
        }
    }

    // color : 0xf8 = ��   0xf1 = ��   0xff = ��
    color = success ? (u8)0xf8 : (u8)0xf1;
    
	PutMainScreen( 0, 3, color, " - SysMenu Flg Checker - ");

    PutMainScreen( 0, 6, color, " Application    Flg   State");
    PutMainScreen( 0, 7, color, " --------------------------");
	for(i=0; i<CHECK_APP_NUM; i++){
		PutMainScreen(  1, 8+i, color, gAppName[i]);
        PutMainScreen( 16, 8+i, color, "(%c)",gDisableDebugFlg[i]);
        if(gDisableDebugFlg[i] == '0'){
			PutMainScreen(22, 8+i, 0xf1, "NG");
        }
    }
    PutMainScreen( 0,18, color, " --------------------------");
}


/*---------------------------------------------------------------------------*
  Name:         VBlankIntr

  Description:  �u�u�����N�����݃n���h���B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void VBlankIntr(void)
{
    // �e�L�X�g�\�����X�V
    UpdateScreen();

    // IRQ �`�F�b�N�t���O47���Z�b�g
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}

static void* AllocForNAM(u32 size)
{
	void* ptr;
	ptr = OS_AllocFromMain(size);
	
	if (ptr == NULL)
	{
		OS_Panic("alloc failed.");
	}
	
	return ptr;
	
}

static void FreeForNAM(void* ptr)
{
	OS_FreeToMain(ptr);
}


/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
