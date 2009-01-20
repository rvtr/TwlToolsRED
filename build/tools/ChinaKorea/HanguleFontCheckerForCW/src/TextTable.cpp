#include "TextTable.h"

//����I���^�C�g��
const Texts g_TextTypes = 
{
	L"Hangeul",	
	L"Japanese",
	L"English" 
};

//�e�L�X�g�e�[�u��
// ��P�v�f�F�n���O���I�����\��
// ��Q�v�f�F���{��I�����\��
// ��R�v�f�F�p��I�����\��
//
// unidode�\���̂��߁AL""�ň�̕������\�����܂��B
// �V�t�gJIS�R�[�h�ŋL�q�\�ȏꍇ�͂��̂܂܏������Ƃ��\�ł��i'a''��'�Ȃ�)
// DS�ŗL�t�H���g��n���O���Ȃǂ̓���ȕ����ɂ��Ă�\x����͂��܂�S����16�i���ŕ����R�[�h���L�q���܂��B
// �����R�[�h�ɂ��Ă͕��������t�@�C��ds_ipl_korean.xlor���Q�Ƃ��������B

const Texts g_TextTable[] = {
	{ 	
		//"---------------------------------
		//---------------------------------
		//---------------------------------"
		L"--------------------------------\n--------------------------------\n--------------------------------",
		L"--------------------------------\n--------------------------------\n--------------------------------",
		L"--------------------------------\n--------------------------------\n--------------------------------",
	},
	{ 	
		//PictoChat
		L"PictoChat",
		L"PictoChat",
		L"PictoChat",
	},
	{ 	
		//�����������Ă��܂���
		L"\xC544\xBB34\xAC83\xB3C4\x0020\xAF42\xD600\x000A\xC788\xC9C0\x0020\xC54A\xC2B5\xB2C8\xB2E4",
		L"no font",
 		L"There is no DS Card\ninserted"
	},
	{ 	
		//"����������������������������
		//��
		//��"
		L"\xC544\xC544\xC544\xC544\xC544\xC544\xC544\xC544\xC544\xC544\xC544\xC544\xC544\xC544\x000A\xC544\x000A\xC544",
		L"����������������������������\n��\n��",
		L"����������������������������\n��\n��",
	},
	{ 	
		//GBA�J�[�g���b�W
		L"\x0047\x0042\x0041\x0020\xCE74\xD2B8\xB9AC\xC9C0",
		L"GBA�J�[�g���b�W",
		L"Start GBA game."
	},
	{ 	
		//�����������Ă��܂���
		L"\xC544\xBB34\xAC83\xB3C4\x0020\xAF42\xD600\x000A\xC788\xC9C0\x0020\xC54A\xC2B5\xB2C8\xB2E4",
		L"no font",
		L"There is no Game Pak\ninserted."
	},
	{ 	
		//DS�I�v�V�����J�[�g���b�W
		L"\x0044\x0053\x0020\xC635\xC158\x0020\xCE74\xD2B8\xB9AC\xC9C0",
		L"DS�I�v�V�����J�[�g���b�W",
		L"There is a DS Option\nPak inserted."
	},
	{ 	
		//"------------------------
		//------------------------
		//------------------------"
		L"------------------------\n------------------------\n------------------------\n",
		L"------------------------\n------------------------\n------------------------\n",
		L"------------------------\n------------------------\n------------------------\n",
	},
	{ 	
		//�{�̐ݒ�
		L"\xBCF8\xCCB4\x0020\xC124\xC815",
		L"no font",
		L"Settings",
	},
	{ 	
		//DS�_�E�����[�h�v���C
		L"\x0044\x0053\x0020\xB2E4\xC6B4\xB85C\xB4DC\x0020\xD50C\xB808\xC774",
		L"DS�_�E�����[�h�v���C",
		L"DS Download Play"
	},
	{ 	
		//�B ������x
		L"\xE002\x0020\x0020\xD55C\x0020\xBC88\x0020\xB354",
		L"no font",
		L"\xE002\x0020Try again"
	},
	{ 	
		//�@ �Ƃ���
		L"\xE000\x0020\xB2EB\xAE30",
		L"\xE000\x0020�Ƃ���",
		L"\xE000\x0020Close"
	},
	{ 	
		//�@ �͂�
		L"\xE000\x0020\xC608",
		L"\xE000\x0020�͂�",
		L"\xE000\x0020Yes"
	},
	{ 	
		//�A ������
		L"\xE001\x0020\xC544\xB2C8\xC624",
		L"\xE001\x0020������",
		L"\xE001\x0020No"
	},
	{ 	
		//�B �A���[�����Z�b�g
		L"\xE002\x0020\x0020\xC54C\xB78C\xC744\x0020\xC124\xC815",
		L"\xE002\x0020�A���[�����Z�b�g",
		L"\xE002\x0020Turn Alarm On"
	},
	{ 	
		//�@ ����
		L"\xE000\x0020\xACB0\xC815",
		L"no font",
		L"\xE000\x0020Confirm",
	},
	{ 	
		//�@ ����
		L"\xE000\x0020\xC785\xB825",
		L"no font",
		L"\xE000\x0020Input"
	},
	{ 	
		//�@ �Q��
		L"\xE000\x0020\xCC38\xAC00",
		L"no font",
		L"\xE000\x0020Join"
	},
	{ 	
		//�@ OK
		L"\xE000\x0020\x004F\x004B",
		L"\xE000\x0020\x004F\x004B",
		L"\xE000\x0020\x004F\x004B",
	},
	{ 	
		//�@ �I��
		L"\xE000\x0020\xC120\xD0DD",
		L"no font",
		L"\xE000\x0020\x0053\x0065\x006C\x0065\x0063\x0074"
	},
	{ 	
		//�|�|�|�|�|�|�|�|
		L"�|�|�|�|�|�|�|�|",
		L"�|�|�|�|�|�|�|�|",
		L"�|�|�|�|�|�|�|�|",
	},
	{ 	
		//�@ �͂�
		L"\xE000\x0020\xC608",
		L"\xE000\x0020�͂�",
		L"\xE000\x0020Yes"
	},
	{ 	
		//�A ���ǂ�
		L"\xE001\x0020\xB3CC\xC544\xAC00\xAE30",
		L"\xE001\x0020���ǂ�",
		L"\xE001\x0020Go back"
	},
	{ 	
		//�A �L�����Z��
		L"\xE001\x0020\xCDE8\xC18C",
		L"\xE001\x0020�L�����Z��",
		L"\xE001\x0020Cancel"
	},
	{ 	
		//�A ����
		L"\xE001\x0020\xC0AD\xC81C",
		L"no font",
		L"\xE001\x0020Erase"
	},
	{ 	
		//�A ������
		L"\xE001\x0020\xC544\xB2C8\xC624",
		L"\xE001\x0020������",
		L"\xE001\x0020No"
	},
	{ 	
		//�A �I��
		L"\xE001\x0020\xC885\xB8CC",
		L"no font",
		L"\xE001\x0020Quit"
	},
	{ 	
		//�|�|�|�|�|�|�|�|�|
		L"�|�|�|�|�|�|�|�|�|",
		L"�|�|�|�|�|�|�|�|�|",
		L"�|�|�|�|�|�|�|�|�|",
	},
	{ 	
		
		//������
		L"\xE001\x0020\xC544\xB2C8\xC624",
		L"\xE001\x0020������",
		L"\xE001\x0020No"
	},
	{ 	
		
		//�͂�
		L"\xE000\x0020\xC608",
		L"\xE000\x0020�͂�",
		L"\xE000\x0020Yes"
	},
	{ 	
		
		//�`���b�g��-��A
		L"\xCC44\xD305\xBC29\x0020\x0041",
		L"�`���b�g��-��A",
		L"Chat Room A"
	},
	{ 	
		
		//�`���b�g��-��B
		L"\xCC44\xD305\xBC29\x0020\x0042",
		L"�`���b�g��-��B",
		L"Chat Room B"
	},
	{ 	
		
		//�`���b�g��-��C
		L"\xCC44\xD305\xBC29\x0020\x0043",
		L"�`���b�g��-��C",
		L"Chat Room C"
	},
	{ 	
		
		//�`���b�g��-��D
		L"\xCC44\xD305\xBC29\x0020\x0044",
		L"�`���b�g��-��D",
		L"Chat Room D"
	},
	{ 	
		
		//�Q�����������[����I�����ĉ������B
		L"\xCC38\xAC00\xD558\xACE0\x0020\xC2F6\xC740\x0020\xCC44\xD305\xBC29\xC744\x0020\xC120\xD0DD\xD574\x0020\xC8FC\xC138\xC694\x002E",
		L"no font",
		L"Choose a Chat Room to join."
	},
	{ 	
		
		//"�a�������߂łƂ��I
		//�����́�����̒a�����ł��B"
		L"\xC0DD\xC77C\x0020\x0020\xCD95\xD558\xD569\xB2C8\xB2E4\x0021\x0020\x0020\xC624\xB298\xC740\x000A\x2229\x0020\xB2D8\xC758\x0020\x0020\xC0DD\xC77C\xC785\xB2C8\xB2E4\x002E",
		L"no font",
		L"Happy Birthday!\nToday is ��'s Birthday!"
	},
	{ 	
		//"������
		//���[�����ֈړ����܂����B"
		L"\x2229\x0020\xB2D8\xC774\x000A\xCC44\xD305\xBC29\x22BF\xB85C\x0020\x0020\xC774\xB3D9\xD588\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"�� has moved to\nChat Room ��."
	},
	{ 	
		//�Q�� �Y��  �� ����
		L"\xCC38\xAC00\x0020\xE019\x22BF\x0020\x0020\x2229\x0020\xB2D8",
		L"no font",
		L"Now entering ��: ��"
	},
	{ 	
		//�ގ� �Z��  �� ����
		L"\xD1F4\xC2E4\x0020\xE01A\x22BF\x0020\x0020\x2229\x0020\xB2D8",
		L"no font",
		L"Now leaving ��: ��"
	},
	{ 	
		
		//�p�q�r�s�H�r�t�u�s
		L"�p�q�r�s�H�r�t�u�s",
		L"�p�q�r�s�H�r�t�u�s",
		L"�p�q�r�s�H�r�t�u�s",
	},
	{ 	
		//"�ʐM�G���[���������܂����B
		//���[���I���ւ��ǂ�܂��B"
		L"\xD1B5\xC2E0\x0020\xC5D0\xB7EC\xAC00\x0020\xBC1C\xC0DD\xD588\xC2B5\xB2C8\xB2E4\x002E\x000A\xCC44\xD305\xBC29\x0020\xC120\xD0DD\xC73C\xB85C\x0020\xB418\xB3CC\xC544\xAC11\xB2C8\xB2E4\x002E",
		L"no font",
		L"Communication error.\nReturn to room selection."
	},
	{ 	
		//"����I�[�o�[�ł��B
		//���[�����ɓ���܂���ł����B"
		L"\xC815\xC6D0\x0020\xCD08\xACFC\xC785\xB2C8\xB2E4\x002E\x000A\xCC44\xD305\xBC29\x0020\x22BF\x0020\xC5D0\x0020\xB4E4\xC5B4\xAC00\xC9C0\x0020\xBABB\x0020\xD588\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"Chat Room �� is full. You\ncannot enter Chat Room ��."
	},
	{ 	
		//"�ʐM�G���[���������܂����B
		//��x�d����؂��ĉ������B"
		L"\xD1B5\xC2E0\x0020\xC5D0\xB7EC\xAC00\x0020\xBC1C\xC0DD\xD588\xC2B5\xB2C8\xB2E4\x002E\x000A\xC804\xC6D0\xC744\x0020\xAEBC\x0020\xC8FC\xC138\xC694\x002E",
		L"no\xfont",
		L"Communication error.\nPlease turn the power off."
	},
	{ 	
		
		//�_�E�����[�h���Ă��܂��B
		L"\xB2E4\xC6B4\xB85C\xB4DC\xD558\xACE0\x0020\xC788\xC2B5\xB2C8\xB2E4\x002E",
		L"�_�E�����[�h���Ă��܂��B",
		L"Downloading...",
	},
	{ 	
		//�_�E�����[�h���L�����Z�����܂����B
		L"\xB2E4\xC6B4\xB85C\xB4DC\xB97C\x0020\xCDE8\xC18C\xD588\xC2B5\xB2C8\xB2E4\x002E",
		L"�_�E�����[�h���L�����Z�����܂����B",
		L"Download has been canceled."
	},
	{ 	
		
		//"����I�[�o�[�ł��B
		//�_�E�����[�h�ł��܂���ł����B"
		L"\xC815\xC6D0\x0020\xCD08\xACFC\xC785\xB2C8\xB2E4\x002E\x000A\xB2E4\xC6B4\xB85C\xB4DC\xC5D0\x0020\xC2E4\xD328\xD588\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"Maximum number of participants has\nbeen reached. Unable to download."
	},
	{ 	
		
		//�_�E�����[�h���������܂����B
		L"\xB2E4\xC6B4\xB85C\xB4DC\xAC00\x0020\xC644\xB8CC\xB418\xC5C8\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"The software has been\nsuccessfully downloaded."
	},
	{ 	
		
		//���̃\�t�g���_�E�����[�h���܂����H
		L"\xC774\x0020\xC18C\xD504\xD2B8\xB97C\x0020\xB2E4\xC6B4\xB85C\xB4DC\xD558\xACA0\xC2B5\xB2C8\xAE4C\x003F",
		L"���̃\�t�g���_�E�����[�h���܂����H",
		L"Would you like to download\nthis software?"
	},
	{ 	
		//"�ʐM�G���[���������܂����B
		//�_�E�����[�h�ł��܂���ł����B"
		L"\xD1B5\xC2E0\x0020\xC5D0\xB7EC\xAC00\x0020\xBC1C\xC0DD\xD588\xC2B5\xB2C8\xB2E4\x002E\x000A\xB2E4\xC6B4\xB85C\xB4DC\xC5D0\x0020\xC2E4\xD328\xD588\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"Communication error.\nDownload not completed."
	},
	{ 	
		//"�_�E�����[�h�ł���
		//�\�t�g��T���Ă��܂��B"
		L"\xB2E4\xC6B4\xB85C\xB4DC\x0020\xAC00\xB2A5\xD55C\x000A\xC18C\xD504\xD2B8\xB97C\x0020\xCC3E\xACE0\x0020\xC788\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"Looking for software available\nfor download..."
	},
	{ 	
		
		//�u�A���[���v���������܂����B
		L"\x300C\xC54C\xB78C\x300D\xC744\x0020\xD574\xC81C\xD588\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"The alarm has been turned off."
	},
	{ 	
		
		//�u�A���[���v���������܂����H
		L"\x300C\xC54C\xB78C\x300D\xC744\x0020\xD574\xC81C\xD558\xACA0\xC2B5\xB2C8\xAE4C\x003F",
		L"no font",
		L"Would you like to turn the\nalarm off?"
	},
	{ 	
		
		//"����:�����Ɂu�A���[���v���Z�b�g���܂��B
		//��낵���ł����H"
		L"\x25CF\x25CF\x003A\x25CF\x25CF\xC73C\xB85C\x0020\xC54C\xB78C\xC744\x0020\xC124\xC815\xD558\xACA0\xC2B5\xB2C8\xAE4C\xFF1F",
		L"no font",
		L"The alarm has been set for ����:����.\nWould you like to turn the alarm on?"
	},
	{ 	
		//�U���@�\���g���܂����H
		L"\xC9C4\xB3D9\x0020\xAE30\xB2A5\xC744\x0020\xC0AC\xC6A9\xD558\xACA0\xC2B5\xB2C8\xAE4C\x003F",
		L"no font",
		L"Would you like the Rumble Featureto accompany the alarm?"
	},
	{ 	
		//"�u�A���[���v���Z�b�g����Ă��܂��B
		//��������ɂ́A��ʂ��^�b�`���邩
		//�D�E�ȊO�̃{�^���������ĉ������B
		//�i�K�v�ɉ����āA�{�����[���𒲐����ĉ������B�j"
		L"\x300C\xC54C\xB78C\x300D\xC774\x0020\xC124\xC815\xB418\xC5B4\x0020\xC788\xC2B5\xB2C8\xB2E4\x002E\x000A\xD574\xC81C\xD558\xB824\xBA74\x0020\xD654\xBA74\xC744\x0020\xD130\xCE58\xD558\xAC70\xB098\x000A\xE004\xE005\x0020\xC774\xC678\xC758\x0020\xBC84\xD2BC\xC744\x0020\xB20C\xB7EC\x0020\xC8FC\xC138\xC694\x002E\x000A\xFF08\xD544\xC694\xC5D0\x0020\xB530\xB77C\x0020\xBCFC\xB968\xC744\x0020\xC870\xC815\xD574\x0020\xC8FC\xC138\xC694\xFF09",
		L"no font",
		L"The alarm is currently on. Touch the Touch\n"
		L"Screen or press any button to turn it off.\n"
		L"�D�E will not turn the alarm off.\n"
		L"(Adjust volume if necessary.)"
	},
	{ 	
		
		//"�Z�b�g���������ɂȂ�܂����B
		//��������ɂ́A��ʂ��^�b�`���邩
		//�D�E�ȊO�̃{�^���������ĉ������B"
		L"\xC124\xC815\xD55C\x0020\xC2DC\xAC01\xC774\x0020\xB418\xC5C8\xC2B5\xB2C8\xB2E4\x002E\x000A\xD574\xC81C\xD558\xB824\xBA74\x0020\xD654\xBA74\xC744\x0020\xD130\xCE58\xD558\xAC70\xB098\x000A\xE004\xE005\x0020\xC774\xC678\xC758\x0020\xBC84\xD2BC\xC744\x0020\xB20C\xB7EC\x0020\xC8FC\xC138\xC694\x002E",
		L"no font",
L"The alarm has been turned on.\n"
L"To turn the alarm off, touch the Touch\n"
L"Screen or press any button.\n"
L"�D�E will not turn the alarm off."
	},
	{ 	
		
		//"�Z�b�g�����������灜��:���������܂����B
		//��������ɂ́A��ʂ��^�b�`���邩
		//�D�E�ȊO�̃{�^���������ĉ������B"
		L"\xC124\xC815\xD55C\x0020\xC2DC\xAC01\xC73C\xB85C\xBD80\xD130\x0020\x25CF\x25CF\x003A\x25CF\x25CF\x0020\xC9C0\xB0AC\xC2B5\xB2C8\xB2E4\x002E\x000A\xD574\xC81C\xD558\xB824\xBA74\x0020\xD654\xBA74\xC744\x0020\xD130\xCE58\xD558\xAC70\xB098\x000A\xE004\xE005\x0020\xC774\xC678\xC758\x0020\xBC84\xD2BC\xC744\x0020\xB20C\xB7EC\x0020\xC8FC\xC138\xC694\x002E",
		L"no font",
		L"It is ����:���� past the set alarm time.\n"
		L"To turn the alarm off, touch the Touch\n"
		L"Screen or press any button.\n"
		L"�D�E will not turn the alarm off.\n"
	},
	{ 	
		
		//"-------------------------------------
		//-------------------------------------
		//-------------------------------------"
		L"-------------------------------------\n"
		L"-------------------------------------\n"
		L"-------------------------------------",
		
		L"-------------------------------------\n"
		L"-------------------------------------\n"
		L"-------------------------------------",
		
		L"-------------------------------------\n"
		L"-------------------------------------\n"
		L"-------------------------------------"
	},
	{ 	
		
		//���ݎ���
		L"\xD604\xC7AC\x0020\xC2DC\xAC01",
		L"no font",
		L"Current Time"
	},
	{ 	
		//0:00:00
		L"0:00:00",
		L"0:00:00",
		L"0:00:00",
	},
	{ 	
		
		//�c�莞��
		L"\xB0A8\xC740\x0020\xC2DC\xAC04",
		L"no font",
		L"Time Remaining",
	},
	{ 	
		
		//�o�ߎ���
		L"\xACBD\xACFC\x0020\xC2DC\xAC04",
		L"no font",
		L"Elapsed Time",
	},
	{ 	
		
		//-----------------
		L"-----------------",
		L"-----------------",
		L"-----------------",
	},
	{ 	
		
		//�ݒ莞��
		L"\xC124\xC815\x0020\xC2DC\xAC01",
		L"no font",
		L"Alarm Setting",
	},
	{ 	
		
		//DS�_�E�����[�h�v���C
		L"\x0044\x0053\x0020\xB2E4\xC6B4\xB85C\xB4DC\x0020\xD50C\xB808\xC774",
		L"DS�_�E�����[�h�v���C",
		L"DS Download Play",
	},
	{ 	
		
		//�I�v�V����
		L"\xC635\xC158",
		L"�I�v�V����",
		L"Options",
	},
	{ 	
		
		//�N�����[�h
		L"\xC2E4\xD589\x0020\xBAA8\xB4DC",
		L"no font",
		L"Start-up",
	},
	{ 	
		
		//GBA�\��
		L"\x0047\x0042\x0041\x0020\xD45C\xC2DC",
		L"no font",
		L"GBA Mode"
	},
	{ 	
		
		//����
		L"\xC5B8\xC5B4",
		L"no font",
		L"Language",
	},
	{ 	
		
		//���[�U�[
		L"\xC720\xC800",
		L"���[�U�[",
		L"User",
	},
	{ 	
		
		//���[�U�[�l�[��
		L"\xC720\xC800\x0020\xB124\xC784",
		L"���[�U�[�l�[��",
		L"User Name",
	},
	{ 	
		
		//���[�U�[�J���[
		L"\xC720\xC800\x0020\xCEEC\xB7EC",
		L"���[�U�[�J���[",
		L"Color",
	},
	{ 	
		
		//�a����
		L"\xC0DD\xC77C",
		L"no font",
		L"Birthday",
	},
	{ 	
		
		//�R�����g
		L"\xCF54\xBA58\xD2B8",
		L"�R�����g",
		L"Message",
	},
	{ 	
		
		//�^�b�`�X�N���[��
		L"\xD130\xCE58\xC2A4\xD06C\xB9B0",
		L"�^�b�`�X�N���[��",
		L"Touch Screen",
	},
	{ 	
		
		//�{�̐ݒ�
		L"\xBCF8\xCCB4\x0020\xC124\xC815",
		L"no font",
		L"Settings",
	},
	{ 	
		
		//���t������
		L"\xB0A0\xC9DC\xFF06\xC2DC\xAC01",
		L"no font",
		L"Clock",
	},
	{ 	
		
		//���t
		L"\xB0A0\xC9DC",
		L"no font",
		L"Date",
	},
	{ 	
		
		//����
		L"\xC2DC\xAC01",
		L"no font",
		L"Time",
	},
	{ 	
		
		//�A���[��
		L"\xC54C\xB78C",
		L"�A���[��",
		L"Alarm",
	},
	{ 	
		
		//�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|
		L"�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|",
		L"�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|",
		L"�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|",
	},
	{ 	
		
		//��
		L"\xC2DC",
		L"no font",
		L"Hour",
	},
	{ 	
		
		//��
		L"\xBD84",
		L"no font",
		L"Minute",
	},
	{ 	
		
		//��
		L"\xC77C",
		L"no font",
		L"Day",
	},
	{ 	
		
		//��
		L"\xC6D4",
		L"no font",
		L"Month",
	},
	{ 	
		
		//��
		L"\xC77C",
		L"no font",
		L"Day",
	},
	{ 	
		
		//��
		L"\xC6D4",
		L"no font",
		L"Month",
	},
	{ 	
		
		//�N
		L"\xB144",
		L"no font",
		L"Year",
	},
	{ 	
		
		//��
		L"\xC2DC",
		L"no font",
		L"Hour",
	},
	{ 	
		
		//��
		L"\xBD84",
		L"no font",
		L"Minute",
	},
	{ 	
		
		//"�{�̂̐ݒ�����܂��B
		//�ݒ肵�����p�l����I�����ĉ������B"
		L"\xBCF8\xCCB4\x0020\xC124\xC815\xC744\x0020\xD569\xB2C8\xB2E4\x002E\x000A\xC124\xC815\xD558\xACE0\x0020\xC2F6\xC740\x0020\xD328\xB110\xC744\x0020\xC120\xD0DD\xD574\x0020\xC8FC\xC138\xC694\x002E",
		L"no font",
		L"Change system settings here. Select\n"
		L"the setting you'd like to change."
	},
	{ 	
		
		//"---------------------------------
		//---------------------------------"
		L"---------------------------------\n---------------------------------",
		L"---------------------------------\n---------------------------------",
		L"---------------------------------\n---------------------------------",
	},
	{ 	
		
		//�_�E�����[�h����\�t�g��I�����܂��B
		L"\xB2E4\xC6B4\xB85C\xB4DC\xD560\x0020\xC18C\xD504\xD2B8\xB97C\x0020\xC120\xD0DD\xD569\xB2C8\xB2E4\x002E",
		L"no font",
		L"Download software via DS Download Play."
	},
	{ 	
		
		//�I�v�V������ݒ肵�܂��B
		L"\xC635\xC158\xC744\x0020\xC124\xC815\xD569\xB2C8\xB2E4\x002E",
		L"no font",
		L"Change other settings."
	},
	{ 	
		
		//"GBA�\�t�g���v���C�����ʂ�
		//�I�����ĉ������B"
		L"\x0047\x0042\x0041\x0020\xC18C\xD504\xD2B8\xB97C\x0020\xD50C\xB808\xC774\xD560\x0020\xD654\xBA74\xC744\x000A\xC120\xD0DD\xD574\x0020\xC8FC\xC138\xC694\x002E",
		L"no font",
		L"Select the screen you would like to use\nwhen starting GBA Mode."
	},
	{ 	
		
		//"�d������ꂽ����
		//�N�����@��I�����ĉ������B"
		L"\xC804\xC6D0\xC744\x0020\xB123\xC5C8\xC744\x0020\xB54C\xC758\x000A\xC2E4\xD589\x0020\xBC29\xBC95\xC744\x0020\xC120\xD0DD\xD574\x0020\xC8FC\xC138\xC694\x002E",
		L"no font",
		L"Set how you would like your system to\n"
		L"start up when you turn the power on."
	},
	{ 	
		
		//�g�p���錾���I�����ĉ������B
		L"\xC0AC\xC6A9\xD560\x0020\xC5B8\xC5B4\xB97C\x0020\xC120\xD0DD\xD574\x0020\xC8FC\xC138\xC694\x002E",
		L"no font",
		L"Select the language to use."
	},
	{ 	
		
		//���t�E�����E�A���[����ݒ肵�܂��B
		L"\xB0A0\xC9DC\x30FB\xC2DC\xAC01\x30FB\xC54C\xB78C\xC744\x0020\xC124\xC815\xD569\xB2C8\xB2E4\x002E",
		L"no font",
		L"Change date, time, and alarm settings."
	},
	{ 	
		
		//�����̓��t�ɍ��킹�ĉ������B
		L"\xC624\xB298\x0020\xB0A0\xC9DC\xB85C\x0020\xB9DE\xCD94\xC5B4\x0020\xC8FC\xC138\xC694\x002E",
		L"no font",
		L"Enter today's date."
	},
	{ 	
		
		//���݂̎����ɍ��킹�ĉ������B
		L"\xD604\xC7AC\x0020\xC2DC\xAC01\xC73C\xB85C\x0020\xB9DE\xCD94\xC5B4\x0020\xC8FC\xC138\xC694\x002E",
		L"no font",
		L"Enter the current time."
	},
	{ 	
		
		//"�A���[���A�C�R���i�G�j���^�b�`�����
		//�ݒ肵�������ɃA���[������܂��B"
		L"\xC54C\xB78C\x0020\xC544\xC774\xCF58\xFF08\xE007\xFF09\xC744\x0020\xD130\xCE58\xD558\xBA74\x000A\xC124\xC815\xD55C\x0020\xC2DC\xAC01\xC5D0\x0020\xC54C\xB78C\xC774\x0020\xC6B8\xB9BD\xB2C8\xB2E4\x002E",
		L"no font",
		L"Tap the alarm icon (�G) to have the\nalarm sound at the designated time."
	},
	{ 	
		
		//���[�U�[����o�^���܂��B
		L"\xC720\xC800\x0020\xC815\xBCF4\xB97C\x0020\xB4F1\xB85D\xD569\xB2C8\xB2E4\x002E",
		L"no font",
		L"Enter user information."
	},
	{ 	
		
		//�j�b�N�l�[���Ȃǂ���͂��ĉ������B
		L"\xB2C9\xB124\xC784\x0020\xB4F1\xC744\x0020\xC785\xB825\xD574\x0020\xC8FC\xC138\xC694\x002E",
		L"no font",
		L"Enter your nickname."
	},
	{ 	
		
		//�R�����g����͂��ĉ������B
		L"\xCF54\xBA58\xD2B8\xB97C\x0020\xC785\xB825\xD574\x0020\xC8FC\xC138\xC694\x002E",
		L"no font",
		L"Enter a personal message."
	},
	{ 	
		
		//�D���ȐF��I�����ĉ������B
		L"\xC88B\xC544\xD558\xB294\x0020\xC0C9\xC744\x0020\xC120\xD0DD\xD574\x0020\xC8FC\xC138\xC694\x002E",
		L"no font",
		L"Select your favorite color."
	},
	{ 	
		
		//�a��������͂��ĉ������B
		L"\xC0DD\xC77C\xC744\x0020\xC785\xB825\xD574\x0020\xC8FC\xC138\xC694\x002E",
		L"no font",
		L"Enter your birthday."
	},
	{ 	
		//"�^�b�`�ʒu�̂����␳���܂��B
		//�^�b�`�y�����T���^�b�`���ĉ������B"
		L"\xD130\xCE58\x0020\xC704\xCE58\xC758\x0020\xC624\xCC28\xB97C\x0020\xC218\xC815\xD569\xB2C8\xB2E4\x002E\x000A\xD130\xCE58\xD39C\xC73C\xB85C\x0020\x0020\xE014\x0020\xC744\x0020\xD130\xCE58\xD574\x0020\xC8FC\xC138\xC694\x002E",
		L"no font",
		L"To calibrate the Touch Screen, tap the\n"
		L"center of the �T marks with the stylus."
	},
	{ 	
		
		//�u����ʁv�ɐݒ肵�܂����B
		L"\x300C\xC544\xB798\x0020\x0020\xD654\xBA74\x300D\xC73C\xB85C\x0020\xC124\xC815\xD588\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"Default GBA screen\n"
		L"set to Touch Screen."
	},
	{ 	
		
		//�u�^�b�`�X�N���[���v��␳���܂����B
		L"\x300C\xD130\xCE58\xC2A4\xD06C\xB9B0\x300D\xC744\x0020\xC218\xC815\xD588\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"The Touch Screen has\n"
		L"been calibrated."
	},
	{ 	
		
		//�u���ʁv�ɐݒ肵�܂����B
		L"\x300C\xC704\x0020\xD654\xBA74\x300D\xC73C\xB85C\x0020\xC124\xC815\xD588\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"Default GBA screen\n"
		L"set to Top Screen."
	},
	{ 	
		
		//�u�I�[�g���[�h�v�ɐݒ肵�܂����B
		L"\x300C\xC624\xD1A0\x0020\xBAA8\xB4DC\x300D\xB85C\x0020\xC124\xC815\xD588\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"System start-up set to Auto."
	},
	{ 	
		
		//�u�}�j���A�����[�h�v�ɐݒ肵�܂����B
		L"\x300C\xB9E4\xB274\xC5BC\x0020\xBAA8\xB4DC\x300D\xB85C\x0020\xC124\xC815\xD588\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"System start-up set to Manual."
	},
	{ 	
		
		//�u�������v�ɐݒ肵�܂����B
		L"\x300C\xC601\xC5B4\x300D\xB85C\x0020\xC124\xC815\xD588\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"Language set to English."
	},
	{ 	
		
		//�u�t�����X���v�ɐݒ肵�܂����B
		L"\x300C\xD504\xB791\xC2A4\xC5B4\x300D\xB85C\x0020\xC124\xC815\xD588\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"Language set to French."
	},
	{ 	
		
		//�u�h�C�c���v�ɐݒ肵�܂����B
		L"\x300C\xB3C5\xC77C\xC5B4\x300D\xB85C\x0020\xC124\xC815\xD588\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"Language set to German."
	},
	{ 	
		
		//�u�C�^���A���v�ɐݒ肵�܂����B
		L"\x300C\xD55C\xAD6D\xC5B4\x300D\xB85C\x0020\xC124\xC815\xD588\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"Language set to Italian."
	},
	{ 	
		
		//�u���{��v�ɐݒ肵�܂����B
		L"\x300C\xC77C\xBCF8\xC5B4\x300D\xB85C\x0020\xC124\xC815\xD588\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"no font"
	},
	{ 	
		
		//�u�X�y�C�����v�ɐݒ肵�܂����B
		L"\x300C\xC2A4\xD398\xC778\xC5B4\x300D\xB85C\x0020\xC124\xC815\xD588\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"Language set to Spanish."
	},
	{ 	
		
		//�u���t�v��ݒ肵�܂����B
		L"\x300C\xB0A0\xC9DC\x300D\xB97C\x0020\xC124\xC815\xD588\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"Date setting saved."
	},
	{ 	
		
		//�u�����v��ݒ肵�܂����B
		L"\x300C\xC2DC\xAC01\x300D\xC744\x0020\xC124\xC815\xD588\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"Time setting saved."
	},
	{ 	
		
		//�u�A���[���v��ݒ肵�܂����B
		L"\x300C\xC54C\xB78C\x300D\xC744\x0020\xC124\xC815\xD588\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"Alarm setting saved."
	},
	{ 	
		
		//�u���[�U�[�l�[���v��ݒ肵�܂����B
		L"\x300C\xC720\xC800\x0020\xB124\xC784\x300D\xC744\x0020\xC124\xC815\xD588\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"User Name saved."
	},
	{ 	
		
		//�u�R�����g�v��ݒ肵�܂����B
		L"\x300C\xCF54\xBA58\xD2B8\x300D\xB97C\x0020\xC124\xC815\xD588\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"Message saved."
	},
	{ 	
		
		//�u���[�U�[�J���[�v��ݒ肵�܂����B
		L"\x300C\xC720\xC800\x0020\xCEEC\xB7EC\x300D\xB97C\x0020\xC124\xC815\xD588\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"Color setting saved."
	},
	{ 	
		
		//�u�a�����v��ݒ肵�܂����B
		L"\x300C\xC0DD\xC77C\x300D\xC744\x0020\xC124\xC815\xD588\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"Birthday setting saved."
	},
	{ 	
		
		//�����
		L"\xC544\xB798\x0020\x0020\xD654\xBA74",
		L"no font",
		L"Touch\n"
		L"Screen"
	},
	{ 	
		
		//����
		L"\xC704\x0020\xD654\xBA74",
		L"no font",
		L"Top\n"
		L"Screen"
	},
	{ 	
		
		//"�d������ꂽ��
		//DS���j���[�ŉ������邩
		//�I���ł��܂��B"
		L"\xC804\xC6D0\xC744\x0020\xB123\xC740\x0020\xD6C4\x000A\x0044\x0053\x0020\xBA54\xB274\xC5D0\xC11C\x0020\xBB34\xC5C7\xC744\x0020\xD560\xC9C0\x000A\xC120\xD0DD\xD560\x0020\xC218\x0020\xC788\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"The Main Menu will appear\n"
		L"automatically when you turn\n"
		L"the power on."
	},
	{ 	
		
		//�m �I�[�g���[�h
		L"\xE02D\x0020\xC624\xD1A0\x0020\xBAA8\xB4DC",
		L"\xE02D\x0020�I�[�g���[�h",
		L"\xE02D\x0020Auto Mode"
	},
	{ 	
		
		//"�d������ꂽ��
		//���傤���傤�ɉ�����
		//�����Ń\�t�g���N�����܂��B"
		L"\xC804\xC6D0\xC744\x0020\xB123\xC740\x0020\xD6C4\x000A\xC0C1\xD669\xC5D0\x0020\xB530\xB77C\xC11C\x000A\xC790\xB3D9\xC73C\xB85C\x0020\x0020\xC18C\xD504\xD2B8\xAC00\x0020\xC2E4\xD589\xB429\xB2C8\xB2E4\x002E",
		L"no font",
		L"If a DS Card or a Game Pak is\n"
		L"inserted when you power up, the\n"
		L"game will begin automatically."
	},
	{ 	
		
		//�n �}�j���A�����[�h
		L"\xE02E\x0020\xB9E4\xB274\xC5BC\x0020\xBAA8\xB4DC",
		L"\xE02E\x0020�}�j���A�����[�h",
		L"\xE02E\x0020Manual Mode"
	},
	{ 	
		//English
		L"\x0045\x006E\x0067\x006C\x0069\x0073\x0068",
		L"\x0045\x006E\x0067\x006C\x0069\x0073\x0068",
		L"\x0045\x006E\x0067\x006C\x0069\x0073\x0068",
	},
	{ 	
		//Francais
		L"\x0046\x0072\x0061\x006E\x00E7\x0061\x0069\x0073",
		L"\x0046\x0072\x0061\x006E\x00E7\x0061\x0069\x0073",
		L"\x0046\x0072\x0061\x006E\x00E7\x0061\x0069\x0073",
	},
	{ 	
		//Deutsch
		L"\x0044\x0065\x0075\x0074\x0073\x0063\x0068",
		L"\x0044\x0065\x0075\x0074\x0073\x0063\x0068",
		L"\x0044\x0065\x0075\x0074\x0073\x0063\x0068",
	},
	{ 	
		
		//\�m���
		L"\xD55C\xAD6D\xC5B4",
		L"\xD55C\xAD6D\xC5B4",
		L"\xD55C\xAD6D\xC5B4",
	},
	{ 	
		//���{��
		L"no font",
		L"no font",
		L"no font",
	},
	{ 	
		//Espanol
		L"\x0045\x0073\x0070\x0061\x00F1\x006F\x006C",
		L"\x0045\x0073\x0070\x0061\x00F1\x006F\x006C",
		L"\x0045\x0073\x0070\x0061\x00F1\x006F\x006C",
	},
	{ 	
		//���񂽂��J��-
		L"\xC120\xD0DD\x0020\xCEEC\xB7EC",
		L"���񂽂��J��-",
		L"Choose Color"
	},
	{ 	
		//"�␳���܂����B
		//�T���^�b�`���Ċm�F���ĉ������B"
		L"\xC218\xC815\xD588\xC2B5\xB2C8\xB2E4\x002E\x000A\xE014\x0020\xC744\x0020\xD130\xCE58\xD574\xC11C\x0020\xD655\xC778\xD574\x0020\xC8FC\xC138\xC694\x002E",
		L"no font",
		L"Calibration complete. Touch the\n"
		L"�T marks to test the calibration."
	},
	
	{ 	
		
		//�A�{�^���ŕ␳���L�����Z���ł��܂��B
		L"\xE001\x0020\xBC84\xD2BC\xC73C\xB85C\x0020\xC218\xC815\xC744\x0020\x000A\xCDE8\xC18C\xD560\x0020\xC218\x0020\xC788\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"Press �A to cancel."
	},
	{ 	
		
		//�d����؂��ďI�����܂��B
		L"\xC804\xC6D0\xC744\x0020\xB044\xACE0\x0020\xC885\xB8CC\xD569\xB2C8\xB2E4\x002E",
		L"no font",
		L"The system will now shut down."
	},
	{ 	
		
		//"���͂��I�����܂����B
		//�ݒ��ۑ����ēd����؂�܂��B"
		L"\xC785\xB825\xC774\x0020\xC885\xB8CC\xB418\xC5C8\xC2B5\xB2C8\xB2E4\x002E\x000A\xC124\xC815\xC744\x0020\xC800\xC7A5\xD558\xACE0\x0020\xC804\xC6D0\xC744\x0020\xB055\xB2C8\xB2E4\x002E",
		L"no font",
		L"In order for the new settings\n"
		L"to take effect, you must restart\n"
		L"your system. Shut down now?"
	},
	{ 	
		
		//"���߂ɖ{�̂̐ݒ�����܂��傤�B
		//��ʂ��^�b�`���邩
		//�����{�^���������ĉ������B"
		L"\xC6B0\xC120\x0020\xBCF8\xCCB4\x0020\xC124\xC815\xC744\x0020\xD569\xC2DC\xB2E4\x002E\x000A\xD654\xBA74\xC744\x0020\xD130\xCE58\xD558\xAC70\xB098\x000A\xC544\xBB34\x0020\xBC84\xD2BC\xC774\xB098\x0020\xB20C\xB7EC\x0020\xC8FC\xC138\xC694\x002E",
		L"no font",
		L"Before you begin, you must adjust\n"
		L"system settings. Please touch the\n"
		L"Touch Screen or press any button.\n"
	},
	{ 	
		//"�{�̂̐ݒ���m�F���ĉ������B
		//��ʂ��^�b�`���邩
		//�����{�^���������ĉ������B"
		L"\xBCF8\xCCB4\x0020\xC124\xC815\xC744\x0020\xD655\xC778\xD574\x0020\xC8FC\xC138\xC694\x002E\x000A\xD654\xBA74\xC744\x0020\xD130\xCE58\xD558\xAC70\xB098\x000A\xC544\xBB34\x0020\xBC84\xD2BC\xC774\xB098\x0020\xB20C\xB7EC\x0020\xC8FC\xC138\xC694\x002E",
		L"no font",
		L"Confirm your system settings.\n"
		L"Touch the Touch Screen\n"
		L"or press any button."
	},
	{ 	
		
		//"�u���{��v�ɐݒ肵�܂��B
		//��낵���ł����H"
		L"\x300C\xD55C\xAD6D\xC5B4\x300D\xB85C\x0020\xC124\xC815\xD558\xACA0\xC2B5\xB2C8\xAE4C\xFF1F",
		L"no font",
		L"This will change your settings to\n"
		L"English. Is this OK?"
	},
	{ 	
		
		//�g�p���錾���I�����ĉ������B
		L"\xC0AC\xC6A9\xD560\x0020\xC5B8\xC5B4\xB97C\x0020\xC120\xD0DD\xD574\x0020\xC8FC\xC138\xC694\x002E",
		L"no font",
		L"Select the language to use."
	},
	{ 	
		
		//�ݒ肪�������\��������܂��B�T���^�b�`���ĉ������B
		L"\xC124\xC815\xC774\x0020\xC9C0\xC6CC\xC84C\xC744\x0020\xAC00\xB2A5\xC131\xC774\x0020\xC788\xC2B5\xB2C8\xB2E4\x002E\x0020\x000A\xE014\x0020\xC744\x0020\xD130\xCE58\xD574\x0020\xC8FC\xC138\xC694\x002E",
		L"no font",
		L"System settings have been lost. Tap �T."
	},
	{ 	
		
		//0:00
		L"0:00",
		L"0:00",
		L"0:00",
	},
	{ 	
		
		//MM/DD
		L"MM/DD",
		L"MM/DD",
		L"MM/DD",
	},
	{ 	
		
		//-------------
		L"-------------",
		L"-------------",
		L"-------------",
	},
	{ 	
		
		//��
		L"\xAE08",
		L"no font",
		L"Fr",
	},
	{ 	
		
		//��
		L"\xC6D4",
		L"no font",
		L"Mo"
	},
	{ 	
		
		//�y
		L"\xD1A0",
		L"no font",
		L"Sa"
	},
	{ 	
		
		//��
		L"\xC77C",
		L"no font",
		L"Su"
	},
	{ 	
		
		//��
		L"\xBAA9",
		L"no font",
		L"Th"
	},
	{ 	
		//��
		L"\xD654",
		L"no font",
		L"Tu"
	},
	{ 	
		
		//��
		L"\xC218",
		L"no font",
		L"We"
	},
	{ 	
		//YYYY/MM
		L"YYYY/MM",
		L"YYYY/MM",
		L"MM/YYYY",
	},
	{ 	
		//YYYY/MM/DD
		L"YYYY/MM/DD",
		L"YYYY/MM/DD",
		L"MM/DD/YYYY",
	},
	{ 	
		//�L�����Z��
		L"\xCDE8\xC18C",
		L"�L�����Z��",
		L"Cancel"
	},
	{ 	
		//����
		L"\xACB0\xC815",
		L"no font",
		L"Confirm"
	},
	{ 	
		//"DS���C�����X�ʐM���J�n���܂��B
		//��낵���ł����H"
		L"\x0044\x0053\x0020\xBB34\xC120\x0020\xD1B5\xC2E0\xC744\x0020\xC2DC\xC791\xD558\xACA0\xC2B5\xB2C8\xAE4C\xFF1F",
		L"no font",
		L"Resume communication?"
	},
	{ 	
		//�L�����Z��
		L"\xCDE8\xC18C",
		L"�L�����Z��",
		L"Cancel"
	},
	{ 	
		//����
		L"\xACB0\xC815",
		L"no font",
		L"Confirm"
	},
	{ 	
		//�x��-���N�ƈ��S�̂��߂�
		L"\xACBD\xACE0\x0020\x2013\x0020\xAC74\xAC15\xACFC\x0020\xC548\xC804\xC744\x0020\xC704\xD558\xC5EC",
		L"no font",
		L"WARNING - HEALTH AND SAFETY"
	},
	{ 	
		//"�V�Ԃ܂��Ɏ戵�������́u���S�Ɏg�p���Ă�������
		//���߂Ɂc�v�����ǂ݂��������B�����ɂ́A���Ȃ���
		//���N�ƈ��S�̂��߂̑�؂ȓ��e��������Ă��܂��B"
		L"\xC0AC\xC6A9\xD558\xAE30\x0020\x0020\xC804\xC5D0\x0020\x0020\xC0AC\xC6A9\xC124\xBA85\xC11C\xC758\x0020\x0020\x300C\xC548\xC804\xC744\x000A\xC704\xD55C\x0020\x0020\xC8FC\xC758\xC0AC\xD56D\x300D\xC744\x0020\x0020\xC77D\xC5B4\x0020\x0020\xC8FC\xC2ED\xC2DC\xC624\x002E\x000A\xC5EC\xAE30\xC5D0\xB294\x0020\x0020\xB2F9\xC2E0\xC758\x0020\x0020\xAC74\xAC15\xACFC\x0020\x0020\xC548\xC804\xC744\x0020\x0020\xC704\xD55C\x000A\xC911\xC694\xD55C\x0020\x0020\xB0B4\xC6A9\xC774\x0020\x0020\xC801\xD600\x0020\x0020\xC788\xC2B5\xB2C8\xB2E4\x002E\x000A",
		L"no font",
		L"Before playing, read the health\nand safety precautions booklet\nfor important information \nabout your health and safety."
	},
	{ 	
		//"���̓��e�̓z�[���y�[�W�ł����邱�Ƃ��ł��܂��B
		//www.nintendo.co.jp/anzen/"
		L"\xC774\x0020\xB0B4\xC6A9\xC740\x0020\xD648\xD398\xC774\xC9C0\xC5D0\xC11C\xB3C4\x0020\xBCFC\x0020\xC218\x0020\xC788\xC2B5\xB2C8\xB2E4\x002E\x000A\x0077\x0077\x0077\x002E\x006E\x0069\x006E\x0074\x0065\x006E\x0064\x006F\x0063\x0061\x0075\x0074\x0069\x006F\x006E\x002E\x0063\x006F\x002E\x006B\x0072",
		L"no font",
		L"\x0054\x004F\x0020\x0047\x0045\x0054\x0020\x0041\x004E\x0020\x0045\x0058\x0054\x0052\x0041\x0020\x0043\x004F\x0050\x0059\x0020\x0046\x004F\x0052\x0020\x0059\x004F\x0055\x0052\x0020\x0052\x0045\x0047\x0049\x004F\x004E\x002C\x0020\x0047\x004F\x0020\x004F\x004E\x004C\x0049\x004E\x0045\x0020\x0041\x0054\x000A\x0077\x0077\x0077\x002E\x006E\x0069\x006E\x0074\x0065\x006E\x0064\x006F\x002E\x0063\x006F\x006D\x002F\x0068\x0065\x0061\x006C\x0074\x0068\x0073\x0061\x0066\x0065\x0074\x0079\x00\x2F"
	},
	{ 	
		//������ɂ͉�ʂ��^�b�`���Ă��������B
		L"\xACC4\xC18D\xD558\xB824\xBA74\x0020\xD654\xBA74\xC744\x0020\xD130\xCE58\xD574\x0020\xC8FC\xC2ED\xC2DC\xC624\x002E",
		L"no font",
		L"Touch the Touch Screen to continue"
	},

};

const int g_TextTable_Num = (sizeof(g_TextTable)/sizeof(g_TextTable[0]));

