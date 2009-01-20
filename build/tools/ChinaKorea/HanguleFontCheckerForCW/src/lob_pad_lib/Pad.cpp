/*===============================================================*//**
 	@file		Pad.cpp
 	@brief		�p�b�h���擾���C�u����
 	@author		Kazumasa Hirata
 
 	(C) Copyright FLOOR Co., Ltd. All rights reserved 
*//*==============================================================*/

//----------------------------------------------------------------
//	�C���N���[�h
//----------------------------------------------------------------
#include "Pad.hpp"

#include <nitro.h>
#include <algorithm>		//std::fill

namespace LOB
{

//----------------------------------------------------------------
//	�錾
//----------------------------------------------------------------

//----------------------------------------------------------------
//	��`
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
		const u16 PrevCont = m_Base.Get();								// �O�̂�ޔ�
		const u16 NewCont  = m_Base.Update();							// �V�������ǂݏo��
		const u16 result   = (u16)(NewCont & ( NewCont ^ PrevCont ));	// �g���K�X�V
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
		const unsigned trig 	= m_Base.Update();	// �g���K�[�X�V
		const unsigned cont 	= m_Base.Cont();
		const unsigned interval = m_Interval;
		const int wait = m_Wait;
		unsigned rept = 0;
		unsigned mask = 1 << ( NUM_KEYS - 1 );
		s16* pCounter = &m_Counter[ NUM_KEYS - 1 ];
		do {
			if( cont & mask )
			{
				int c;	// �J�E���^�[�X�V�l
				if( !( trig & mask ) )
				{// �������ςȂ�
					c = *pCounter;	// �J�E���^�ǂݏo��
					if( c != wait )
					{
						++c;	// �C���N�������g
					}else
					{// ���s�[�g�g���K�[����
						rept |= mask;
						c -= interval;	// �܂��߂�
					}
				}else
				{// �����͂���
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
			{// ���s�[�g��
				*pCounter += adjust;	// ���� m_Wait �Ƃ̑��Βl��ێ�
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

