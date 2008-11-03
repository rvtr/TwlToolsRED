#ifndef	_MY_NUC_H_
#define	_MY_NUC_H_

#ifdef __cplusplus
extern "C" {
#endif

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


BOOL InitNupLib(void);
BOOL StartNupCheck(void);
NucStatus ProgressNupCheck(void);
BOOL StartNupDownload(void);
NucStatus ProgressNupDownload(void);
BOOL CleanNupLib(void);
void ProgressNetConnect(void);
void ShowErrorMsg(int error_code);
BOOL my_numc_proc(void);
FSFile *my_nuc_log_start(char *log_file_name );
void my_nuc_log_end(void);


#ifdef __cplusplus
}
#endif

#endif /* _MY_NUC_H_ */
