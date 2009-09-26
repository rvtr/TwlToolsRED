#pragma once

namespace MasterEditorTWL {

// -------------------------------------------------------------------
// Type : enum class
// Name : ECFormResult
//
// Description : �t�H�[����̃G���[��錾
// -------------------------------------------------------------------
enum class ECFormResult
{
	NOERROR = 0,
	// ����̕K�v���Ȃ��G���ȃG���[
	ERROR,
	// �t�@�C������
	ERROR_FILE_OPEN,
	ERROR_FILE_READ,
	ERROR_FILE_WRITE,
	ERROR_FILE_COPY,
	ERROR_FILE_EXIST,
	ERROR_FILE_SIGN,	// �t�@�C���ɏ����������Ȃ�����

}; //enum class ECFormResult

} // end of namespace MasterEditorTWL
