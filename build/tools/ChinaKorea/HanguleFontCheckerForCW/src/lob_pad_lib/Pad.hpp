/*===============================================================*//**
 	@file		Pad.hpp
 	@brief		パッド情報取得ライブラリ
 	@author		Kazumasa Hirata
 
 	(C) Copyright FLOOR Co., Ltd. All rights reserved 
*//*==============================================================*/

#if !defined( LOB_PAD_HPP_INCLUDED )
#define LOB_PAD_HPP_INCLUDED

//----------------------------------------------------------------
//	インクルード
//----------------------------------------------------------------
#include <nitro.h>
        
//----------------------------------------------------------------
//	宣言
//----------------------------------------------------------------
namespace LOB
{
	/*------------------------------------------------*//**
	 *	@brief	パッドの現在状態を取得するクラス
	 *//*------------------------------------------------*/
	class RawReader{
		u16 m_Data;
	public:
		/**
		 *	@brief	コンストラクタ
		 */
		RawReader();
		
		/**
		 *	@brief	コンストラクタ
		 *  @param	u16 init	初期化値
		 */
		explicit RawReader( u16 init );
		
		/**
		 *	@brief	更新
		 *  @return キー状態
		 */
		u16 Update();						
		
		/**
		 *	@brief	取得
		 *  @return キー状態
		 */
		u16 Get() const { return m_Data; } 	
	};

	/*------------------------------------------------*//**
	 *	@brief	パッドのトリガー（押された瞬間）情報を取得するクラス
	 *//*------------------------------------------------*/
	class TriggerReader{
		RawReader m_Base;
		u16 m_Trig;
	public:
		/**
		 *	@brief	コンストラクタ
		 */
		TriggerReader();
		
		/**
		 *	@brief	コンストラクタ
		 *  @param	u16 init	初期化値
		 */
		explicit TriggerReader( u16 init );

		/**
		 *	@brief	更新
		 *  @return キー状態
		 */
		u16 Update();	

		/**
		 *	@brief	パッドの現在状態の取得
		 *  @return キー状態
		 */
		u16 Cont() const { return m_Base.Get(); }	
		
		/**
		 *	@brief	トリガー状態の取得
		 *  @return キー状態
		 */
		u16 Get()  const { return m_Trig; }			
	};

	/*------------------------------------------------*//**
	 *	@class	RepeatReader
	 *	@brief	パッドのトリガー（押された瞬間）情報を取得すうクラス
	 *//*------------------------------------------------*/
	class RepeatReader{
		TriggerReader m_Base;

		u16 m_Rept;

		u16 m_Wait;
		u16 m_Interval;

		static const unsigned NUM_KEYS = 10;
		u32 dummy;
		s16 m_Counter[ NUM_KEYS ];

	public:
		static const u16 defaultWait = 30;
		static const u16 defaultInterval = 4;

		/**
		 *	@brief	コンストラクタ
		 *  @param  Wait		ウェイト値
		 *  @param  Interval	インターバル値
		 */
		explicit RepeatReader( u16 Wait = defaultWait 
							 , u16 Interval = defaultInterval );
		/**
		 *	@brief	コンストラクタ
		 *  @param  init		初期キー値
		 */
		explicit RepeatReader( u16 init );
		
		/**
		 *	@brief	コンストラクタ
		 *  @param  Wait		ウェイト値
		 *  @param  Interval	インターバル値
		 *  @param  init		初期キー値
		 */
		RepeatReader( u16 Wait, u16 Interval, u16 init );

		/**
		 *	@brief	更新
		 *  @return キー状態
		 */
		u16 Update();	

		/**
		 *	@brief	パッド状態の取得
		 *  @return キー状態
		 */
		u16 Cont() const { return m_Base.Cont(); }	
		
		/**
		 *	@brief	トリガー状態の取得
		 *  @return キー状態
		 */
		u16 Trig() const { return m_Base.Get(); }	
		
		/**
		 *	@brief	キーリピート状態の取得
		 *  @return キー状態
		 */
		u16 Get()  const { return m_Rept; }	

		/**
		 *	@brief	キーウェイト値の設定
		 *  @param  Wait		ウェイト値
		 */
		void SetWait( u16 Wait );
		
		/**
		 *	@brief	キーインターバル値の設定
		 *  @param  Interval	インターバル値
		 */
		void SetInterval( u16 Interval );
		
		/**
		 *	@brief	キーウェイト値の取得
		 *  @return  Wait		ウェイト値
		 */
		u16 GetWait() const 	{ return m_Wait; }
		
		/**
		 *	@brief	キーインターバル値の取得
		 *  @return  Interval	インターバル値
		 */
		u16 GetInterval() const { return m_Interval; }
	};

}	//namespace LOB

#endif	//#if !defined( LOB_PAD_HPP_INCLUDED )

