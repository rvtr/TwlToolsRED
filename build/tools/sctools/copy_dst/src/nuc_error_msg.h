#ifndef _NUC_ERROR_MSG_H_
#define _NUC_ERROR_MSG_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include <twl/types.h>

/*---------------------------------------------------------------------------*
    定数
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
    型
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
    関数
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         GetPrivateMsg

  Description:  開発者向けのエラーのメッセージを取得します。

  Arguments:    error_code

  Returns:      文字列へのポインタ
 *---------------------------------------------------------------------------*/
const char* GetPrivateMsg(int error_code);

/*---------------------------------------------------------------------------*
  Name:         GetPublicMsg

  Description:  ユーザー向けのエラーのメッセージを取得します。

  Arguments:    error_code

  Returns:      文字列へのポインタ
 *---------------------------------------------------------------------------*/
const char* GetPublicMsg(int error_code);

#ifdef	__cplusplus
}          /* extern "C" */
#endif

#endif  /* _NUC_ERROR_MSG_H_ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
