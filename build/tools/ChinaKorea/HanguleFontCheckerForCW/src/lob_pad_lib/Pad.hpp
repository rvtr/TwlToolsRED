/*===============================================================*//**
 	@file		Pad.hpp
 	@brief		�p�b�h���擾���C�u����
 	@author		Kazumasa Hirata
 
 	(C) Copyright FLOOR Co., Ltd. All rights reserved 
*//*==============================================================*/

#if !defined( LOB_PAD_HPP_INCLUDED )
#define LOB_PAD_HPP_INCLUDED

//----------------------------------------------------------------
//	�C���N���[�h
//----------------------------------------------------------------
#include <nitro.h>
        
//----------------------------------------------------------------
//	�錾
//----------------------------------------------------------------
namespace LOB
{
	/*------------------------------------------------*//**
	 *	@brief	�p�b�h�̌��ݏ�Ԃ��擾����N���X
	 *//*------------------------------------------------*/
	class RawReader{
		u16 m_Data;
	public:
		/**
		 *	@brief	�R���X�g���N�^
		 */
		RawReader();
		
		/**
		 *	@brief	�R���X�g���N�^
		 *  @param	u16 init	�������l
		 */
		explicit RawReader( u16 init );
		
		/**
		 *	@brief	�X�V
		 *  @return �L�[���
		 */
		u16 Update();						
		
		/**
		 *	@brief	�擾
		 *  @return �L�[���
		 */
		u16 Get() const { return m_Data; } 	
	};

	/*------------------------------------------------*//**
	 *	@brief	�p�b�h�̃g���K�[�i�����ꂽ�u�ԁj�����擾����N���X
	 *//*------------------------------------------------*/
	class TriggerReader{
		RawReader m_Base;
		u16 m_Trig;
	public:
		/**
		 *	@brief	�R���X�g���N�^
		 */
		TriggerReader();
		
		/**
		 *	@brief	�R���X�g���N�^
		 *  @param	u16 init	�������l
		 */
		explicit TriggerReader( u16 init );

		/**
		 *	@brief	�X�V
		 *  @return �L�[���
		 */
		u16 Update();	

		/**
		 *	@brief	�p�b�h�̌��ݏ�Ԃ̎擾
		 *  @return �L�[���
		 */
		u16 Cont() const { return m_Base.Get(); }	
		
		/**
		 *	@brief	�g���K�[��Ԃ̎擾
		 *  @return �L�[���
		 */
		u16 Get()  const { return m_Trig; }			
	};

	/*------------------------------------------------*//**
	 *	@class	RepeatReader
	 *	@brief	�p�b�h�̃g���K�[�i�����ꂽ�u�ԁj�����擾�����N���X
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
		 *	@brief	�R���X�g���N�^
		 *  @param  Wait		�E�F�C�g�l
		 *  @param  Interval	�C���^�[�o���l
		 */
		explicit RepeatReader( u16 Wait = defaultWait 
							 , u16 Interval = defaultInterval );
		/**
		 *	@brief	�R���X�g���N�^
		 *  @param  init		�����L�[�l
		 */
		explicit RepeatReader( u16 init );
		
		/**
		 *	@brief	�R���X�g���N�^
		 *  @param  Wait		�E�F�C�g�l
		 *  @param  Interval	�C���^�[�o���l
		 *  @param  init		�����L�[�l
		 */
		RepeatReader( u16 Wait, u16 Interval, u16 init );

		/**
		 *	@brief	�X�V
		 *  @return �L�[���
		 */
		u16 Update();	

		/**
		 *	@brief	�p�b�h��Ԃ̎擾
		 *  @return �L�[���
		 */
		u16 Cont() const { return m_Base.Cont(); }	
		
		/**
		 *	@brief	�g���K�[��Ԃ̎擾
		 *  @return �L�[���
		 */
		u16 Trig() const { return m_Base.Get(); }	
		
		/**
		 *	@brief	�L�[���s�[�g��Ԃ̎擾
		 *  @return �L�[���
		 */
		u16 Get()  const { return m_Rept; }	

		/**
		 *	@brief	�L�[�E�F�C�g�l�̐ݒ�
		 *  @param  Wait		�E�F�C�g�l
		 */
		void SetWait( u16 Wait );
		
		/**
		 *	@brief	�L�[�C���^�[�o���l�̐ݒ�
		 *  @param  Interval	�C���^�[�o���l
		 */
		void SetInterval( u16 Interval );
		
		/**
		 *	@brief	�L�[�E�F�C�g�l�̎擾
		 *  @return  Wait		�E�F�C�g�l
		 */
		u16 GetWait() const 	{ return m_Wait; }
		
		/**
		 *	@brief	�L�[�C���^�[�o���l�̎擾
		 *  @return  Interval	�C���^�[�o���l
		 */
		u16 GetInterval() const { return m_Interval; }
	};

}	//namespace LOB

#endif	//#if !defined( LOB_PAD_HPP_INCLUDED )

