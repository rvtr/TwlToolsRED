/*===============================================================*//**
 	@file		Pad.cpp
 	@brief		パッド情報取得ライブラリ
 	@author		Kazumasa Hirata
 
 	(C) Copyright FLOOR Co., Ltd. All rights reserved 
*//*==============================================================*/

//----------------------------------------------------------------
//	インクルード
//----------------------------------------------------------------
#include "Pad.hpp"

#include <nitro.h>
#include <algorithm>		//std::fill

namespace LOB
{

//----------------------------------------------------------------
//	宣言
//----------------------------------------------------------------

//----------------------------------------------------------------
//	定義
//----------------------------------------------------------------

	RawReader::RawReader()
		:	m_Data( PAD_Read() )
	{
	}

	RawReader::RawReader( u16 init )
		:	m_Data( init )
	{
	}

	u16 RawReader::Update()
	{
		m_Data = PAD_Read();
		return m_Data;
	}

	TriggerReader::TriggerReader()
		:	m_Trig( Cont() )
	{
	}

	TriggerReader::TriggerReader( u16 init )
		:	m_Base( init )
		,	m_Trig( Cont() )
	{
	}

	u16 TriggerReader::Update()
	{
		const u16 PrevCont = m_Base.Get();								// 前のやつ退避
		const u16 NewCont  = m_Base.Update();							// 新しいやつを読み出す
		const u16 result   = (u16)(NewCont & ( NewCont ^ PrevCont ));	// トリガ更新
		m_Trig = result;
		return result;
	}

	RepeatReader::RepeatReader( u16 Wait , u16 Interval )
		:	m_Rept( Trig() )
		,	m_Wait( Wait )
		,	m_Interval( Interval )
	{
		std::fill( &m_Counter[0] ,  &m_Counter[ NUM_KEYS ] , 0 );
	}

	RepeatReader::RepeatReader( u16 init )
		:	m_Base( init )
		,	m_Rept( Trig() )
		,	m_Wait( defaultWait )
		,	m_Interval( defaultInterval )
	{
		std::fill( &m_Counter[0] ,  &m_Counter[ NUM_KEYS ] , 0 );
	}

	RepeatReader::RepeatReader( u16 Wait , u16 Interval , u16 init )
		:	m_Base( init )
		,	m_Rept( Trig() )
		,	m_Wait( Wait )
		,	m_Interval( Interval )
	{
		std::fill( &m_Counter[0] ,  &m_Counter[ NUM_KEYS ] , 0 );
	}

	u16 RepeatReader::Update()
	{
		const unsigned trig 	= m_Base.Update();	// トリガー更新
		const unsigned cont 	= m_Base.Cont();
		const unsigned interval = m_Interval;
		const int wait = m_Wait;
		unsigned rept = 0;
		unsigned mask = 1 << ( NUM_KEYS - 1 );
		s16* pCounter = &m_Counter[ NUM_KEYS - 1 ];
		do {
			if( cont & mask )
			{
				int c;	// カウンター更新値
				if( !( trig & mask ) )
				{// 押しっぱなし
					c = *pCounter;	// カウンタ読み出し
					if( c != wait )
					{
						++c;	// インクリメント
					}else
					{// リピートトリガー発生
						rept |= mask;
						c -= interval;	// まき戻し
					}
				}else
				{// 押しはじめ
					rept |= mask;
					c = 0;
				}
				*pCounter = (s16)c;
			}
			--pCounter;
		} while( mask >>= 1 );
		m_Rept = (u16)rept;
		return (u16)rept;
	}

	void RepeatReader::SetWait( u16 Wait )
	{
		const unsigned cont = m_Base.Cont();
		unsigned mask = 1 << ( NUM_KEYS - 1 );
		s16* pCounter = &m_Counter[ NUM_KEYS - 1 ];
		const int adjust = -m_Wait + Wait;
		do {
			if( cont & mask )
			{// リピート中
				*pCounter += adjust;	// 元の m_Wait との相対値を保持
			}
			--pCounter;
		} while( mask >>= 1 );
		m_Wait = Wait;
	}

	void RepeatReader::SetInterval( u16 Interval )
	{
		m_Interval = Interval;
	}

}	//namespace LOB

