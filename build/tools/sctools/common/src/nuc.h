#ifndef _NUC_H_
#define _NUC_H_

#ifdef	__cplusplus
extern "C" {
#endif

void nuc_main(void);

// Network Update Client API
#include <twl/types.h>

/*---------------------------------------------------------------------------*
    �萔
 *---------------------------------------------------------------------------*/
#define NUC_MAX_TITLE_UPDATE_COUNT 128

/*---------------------------------------------------------------------------*
    �^
 *---------------------------------------------------------------------------*/
typedef OSTitleId NUCTitleId;
typedef void* (*NUCAlloc)(u32 size, int align);
typedef void  (*NUCFree)(void* ptr);

typedef enum{
    NUC_STATUS_NOT_DONE, // �������s��
    NUC_STATUS_COMPLETED,// ��������
    NUC_STATUS_ERROR     // �����G���[
}   NucStatus;

typedef enum{
    NUC_ERROR_NO_SPACE,  // TWL�{�̕ۑ��������̋󂫗e�ʂ��s�����Ă��܂��B
    NUC_ERROR_CONNECT,   // �T�[�o�[�ɐڑ��ł��܂���B
    NUC_ERROR_INTERNET,  // �C���^�[�l�b�g�̃G���[�ɂ��ATWL�{�̂̍X�V���ł��܂���B
    NUC_ERROR_UPDATE     // �G���[���������܂������߁ATWL�{�̂̍X�V���ł��܂���B
}   NucError;

/*---------------------------------------------------------------------------*
    �֐�
 *---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
  Name:         NUC_LoadCert

  Description:  �N���C�A���g�ؖ�����WRAM�Ƀ��[�h���܂��B
                NA_LoadVersionDataArchive()�����炩���ߌĂ�ł��������B
                �I�����NA_UnloadVersionDataArchive()���Ă�ł�������

  Arguments:    �Ȃ�
     
  Returns:      ����������TRUE
 *---------------------------------------------------------------------------*/
BOOL NUC_LoadCert(void);

/*---------------------------------------------------------------------------*
  Name:         NUC_Init

  Description:  NUC ���C�u�����̏��������s���܂��B

  Arguments:    allocFunc:  �������m�ۊ֐��ւ̃|�C���^�B
                freeFunc:   ����������֐��ւ̃|�C���^�B

  Returns:      ����������TRUE
 *---------------------------------------------------------------------------*/
BOOL NUC_Init(NUCAlloc allocFunc, NUCFree freeFunc);

/*---------------------------------------------------------------------------*
  Name:         NUC_CheckAsync

  Description:  �A�b�v�f�[�g����titleID�̎擾

  Arguments:    titleIds:     �X�V�̂�����titleId���擾����o�b�t�@
                nTitleIds:    �X�V�̂�����titleId�̐����擾����|�C���^

  Returns:      ����������TRUE
 *---------------------------------------------------------------------------*/
BOOL NUC_CheckAsync(NUCTitleId *titleIds, u32 *nTitleIds);

/*---------------------------------------------------------------------------*
  Name:         NUC_DownloadAsync

  Description:  �Y��titleID�̍X�V�̃_�E�����[�h
                �e�p�����[�^��NULL�̏ꍇ�͑S�ă_�E�����[�h���܂��B
                �����ӁF���̊֐����Ă񂾌�ɂ�FS�֐����ĂԂ��Ƃ͂ł��Ȃ��Ȃ�܂��B
     
  Arguments:    titleIds:   �_�E�����[�h����titleID�̓������|�C���^
                nTitleIds:  �_�E�����[�h����titleID�̌�

  Returns:      ����������TRUE
 *---------------------------------------------------------------------------*/
BOOL NUC_DownloadAsync(NUCTitleId *titleIds, u32 nTitleIds);

/*---------------------------------------------------------------------------*
  Name:         NUC_GetProgress

  Description:  ���L�����̐i�s�����擾���܂��B
                �E�X�V���̎擾
                �E�X�V���ꂽ�^�C�g���̃_�E�����[�h

  Arguments:    pCurrentSize:   �����ϗʂ��i�[����o�b�t�@�ւ̃|�C���^�B
                pTotalSize:     �S�̗ʂ��i�[����o�b�t�@�ւ̃|�C���^�B
                pStatus:        ���݂̐i�s��

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
void NUC_GetProgress(u64* pCurrentSize, u64* pTotalSize, NucStatus* pStatus);

/*---------------------------------------------------------------------------*
  Name:         NUC_Cleanup

  Description:  NAM�ANHTTP�ANSSL�ANUP�AES�̏I�����������܂��B
                �I����Ɋe�V�X�e���A�v���̃Z�[�u�f�[�^���쐬�A�C�����܂��B

  Arguments:    titleIds:     �X�V�̂�����titleId�̔z��
                nTitleIds:    �X�V�̂�����titleId�̐�

  Returns:      ����������TRUE
 *---------------------------------------------------------------------------*/
BOOL NUC_Cleanup(const NUCTitleId *titleIds, u32 nTitleIds);

/*---------------------------------------------------------------------------*
  Name:         NUC_GetLastError

  Description:  �Ō�ɔ��������G���[���擾���܂��B
                �����ŕԂ��ꂽ�G���[�R�[�h�����[�U�ɒʒm���܂��B

  Arguments:    �Ȃ��B

  Returns:      �G���[�R�[�h
 *---------------------------------------------------------------------------*/
int NUC_GetLastError(void);

/*---------------------------------------------------------------------------*
  Name:         NUC_GetErrorType

  Description:  �G���[�R�[�h����A�G���[�^�C�v���擾���܂��B
                ���[�U�[�ɂ̓G���[�^�C�v�𔻕ʂ���
                �\�����郁�b�Z�[�W��ݒ肵�Ă��������B

  Arguments:    error_code:  �G���[�R�[�h

  Returns:      �G���[�̃^�C�v
 *---------------------------------------------------------------------------*/
NucError NUC_GetErrorType(int error_code);

/*---------------------------------------------------------------------------*
  Name:         NUC_IsResetNeeded

  Description:  �n�[�h�E�F�A���Z�b�g���K�v���ǂ�����Ԃ��܂��B

  Arguments:    �Ȃ��B

  Returns:      �G���[�R�[�h
 *---------------------------------------------------------------------------*/
BOOL NUC_IsResetNeeded(void);

#ifdef	__cplusplus
}          /* extern "C" */
#endif

#endif  /* _NUC_H_ */
