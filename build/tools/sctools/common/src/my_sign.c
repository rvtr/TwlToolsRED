#include <twl.h>
#include <stddef.h>
#include <string.h>
#include "my_rsa_sign.h"
#include "my_aes.h"
#include "my_sign.h"

#define AES_KEY_BIT_LEN 256
#define AES_KEY_BYTE_LEN (AES_KEY_BIT_LEN/8)

static u8 my_sign_aes_key[AES_KEY_BYTE_LEN] = {
  0x02,0xB6,0x01,0xD8,0x01,0x80,0x01,0x77,0xB4,0x01,0xCB,0x01,0xBD,0x5F,0x18,0x0F,
  0xF6,0x39,0x9C,0xC6,0x90,0xAC,0xC1,0x0D,0x03,0x74,0x6E,0x8D,0xD1,0xBA,0x37,0x46
};

static u8 my_sign_aes_iv[AES_BLOCK_SIZE] = {
  0xC3,0x85,0x93,0xFE,0xA8,0x2D,0xBF,0xFB,0xED,0x42,0xE0,0x42,0xFD,0x17,0x04,0xB0
};



static BOOL my_sign_check(MY_SIGN_SIGNATURE *encrypted_sign, u8 *buf, int buf_size)
{
  u8 hash_temp[MY_SIGN_HASH_SIZE] ATTRIBUTE_ALIGN(32);
  MY_SIGN_SIGNATURE tmp_sign ATTRIBUTE_ALIGN(32);
  int outlen;
  int i;

  if( encrypted_sign == NULL ) {
    OS_TPrintf("func=%s line=%d error!\n",__FUNCTION__,__LINE__);
    return FALSE;
  }
  if( buf == NULL ) {
    OS_TPrintf("func=%s line=%d error!\n",__FUNCTION__,__LINE__);
    return FALSE;
  }
  if( buf_size == 0 ) {
    OS_TPrintf("func=%s line=%d error!\n",__FUNCTION__,__LINE__);
    return FALSE;
  }
  memset(hash_temp, 0, MY_SIGN_HASH_SIZE );
  memset((void *)&tmp_sign, 0, sizeof(MY_SIGN_SIGNATURE));
  SHA256(buf, (u32)buf_size, hash_temp);
  
  outlen = RsaTestDecrypt((char *)encrypted_sign, sizeof(MY_SIGN_SIGNATURE), 
			  (char *)&tmp_sign, sizeof(MY_SIGN_SIGNATURE));

  //  OS_TPrintf("decrypt outlen = %d\n",outlen);

  if( outlen < MY_SIGN_HASH_SIZE ) {
    OS_TPrintf("%s %d:RSA Decrypt error! outlen=%d\n",__FUNCTION__, __LINE__,outlen);
    return FALSE;
  }    
  
  for( i = 0 ; i < MY_SIGN_HASH_SIZE ; i++ ) {
    //    OS_TPrintf("hash[0x%02x] chk=0x%02x cal=0x%02x\n",i,tmp_sign.hash[i],hash_temp[i]);
    if( tmp_sign.hash[i] != hash_temp[i] ) {
      OS_TPrintf("Hash check error i=%d chk=0x%02x cal=0x%02x\n", i , tmp_sign.hash[i] , hash_temp[i] );
      return FALSE;
    }
  }
  return TRUE;
}


void my_sign_FS_InitFile(MY_SIGN_File *msfile)
{
  msfile->open_flag = FALSE;
}

BOOL my_sign_FS_SeekFile(MY_SIGN_File *msfile, int offset, int whence) 
{
  if( msfile->open_flag != TRUE ) {
    return FALSE; /* error */
  }
  switch( whence )  {
  case FS_SEEK_SET:
    msfile->pos = offset;
    break;
  case FS_SEEK_CUR:
    msfile->pos += offset;
    break;
  case FS_SEEK_END:
    msfile->pos = msfile->header.org_file_size + offset;
    break;
  }
  if( msfile->pos < 0 ) {
    msfile->pos = 0;
  }
  else if( msfile->pos >= msfile->header.org_file_size ) {
    msfile->pos = msfile->header.org_file_size - 1;
  }
  return TRUE;
}

BOOL my_sign_FS_SeekFileToBegin(MY_SIGN_File *msfile)
{
  if( msfile->open_flag != TRUE ) {
    return FALSE; /* error */
  }
  msfile->pos = 0;
  return TRUE;
}

u32 my_sign_FS_GetLength(MY_SIGN_File *msfile)
{
  if( msfile->open_flag != TRUE ) {
    return 0; /* error */
  }
  return (u32)(msfile->header.org_file_size);
}

BOOL my_sign_FS_OpenFile(MY_SIGN_File *msfile, char *path)
{
  MY_SIGN_SIGNATURE temp_sign;
  //  MY_SIGN_SIGNATURE *L2_sign_table_temp;
  //  int rest_readlen = 0;
  //  int outlen = 0;
  int readlen;
  int L2_sign_table_size;

  if( msfile == NULL ) {
    return FALSE;
  }

  RsaTestInit();

  FS_InitFile(&(msfile->f));
  if( FALSE == FS_OpenFileEx(&(msfile->f), path, FS_FILEMODE_R) ) {
    OS_TPrintf("%s Failed Open File %s\n",__FUNCTION__,path);
    return FALSE;
  }

  /* 
     FILE format:
     -----
     MY_SIGN_HEADER header;
     MY_SIGN_SIGNATURE header_sign;
     L2_sign_table[0];
     L2_sign_table[1];
     L2_sign_table[2];
     L2_sign_table[ ];
     .
     .
     aes_enc_data_block[0](32KB)
     aes_enc_data_block[1](32KB)
     aes_enc_data_block[2](32KB)
     .
     .
  */

  readlen = FS_ReadFile(&(msfile->f), &(msfile->header), sizeof(MY_SIGN_HEADER) );
  if( readlen != sizeof(MY_SIGN_HEADER) ) {
    OS_TPrintf("%s Failed read File readlen=%d\n",__FUNCTION__, readlen);
    return FALSE;
  }

  //  OS_TPrintf("%s %d\n",__FUNCTION__, __LINE__);

  /* ヘッダーの読み込み */
  readlen = FS_ReadFile(&(msfile->f), &temp_sign, sizeof(MY_SIGN_SIGNATURE) );
  if( readlen != sizeof(MY_SIGN_SIGNATURE) ) {
    OS_TPrintf("%s Failed read File readlen=%d\n",__FUNCTION__, readlen);
    return FALSE;
  }

  /* ヘッダーの署名チェック */
  if( FALSE == my_sign_check(&temp_sign, (u8 *)&(msfile->header), sizeof(MY_SIGN_HEADER)) ) {
    OS_TPrintf("line=%d Header signature chech error!\n",__LINE__);
    return FALSE;
  }

#if 0
  OS_TPrintf("header.magic_code = 0x%08x\n",(int)msfile->header.magic_code); 
  OS_TPrintf("header.org_file_size = %d\n",(int)msfile->header.org_file_size); 
  OS_TPrintf("header.num_of_block = %d\n",(int)msfile->header.num_of_block);
#endif

  //  rest_readlen = (int)msfile->header.org_file_size;

  //    header.file_offset_L2_sign_table;
  //    header.file_offset_data_block;
  L2_sign_table_size = (int)(msfile->header.num_of_block * sizeof(MY_SIGN_SIGNATURE));
  msfile->L2_sign_table = (MY_SIGN_SIGNATURE *)OS_Alloc( (u32)L2_sign_table_size );
  if(msfile->L2_sign_table == NULL ) {
    OS_TPrintf("%s Failed alloc line=%d\n",__LINE__);
    return FALSE;
  }

  //  L2_sign_table_temp = msfile->L2_sign_table; 


  readlen = FS_ReadFile(&(msfile->f), (u8 *)(msfile->L2_sign_table), L2_sign_table_size );
  if( readlen != L2_sign_table_size ) {
    OS_TPrintf("%s Failed read File readlen=%d\n",__FUNCTION__, readlen);
    return FALSE;
  }

  /* L2署名テーブルの署名チェック */
  if( FALSE == my_sign_check(&(msfile->header.L2_sign), (u8 *)(msfile->L2_sign_table), L2_sign_table_size) ) {
    OS_TPrintf("L2_sign Hash check error!\n");
    return FALSE;
  }


  msfile->block_buf_in = (u8 *)OS_Alloc( MY_SIGN_BLOCK_SIZE );
  if( msfile->block_buf_in == NULL ) {
    return FALSE;
  }
  msfile->block_buf_out = (u8 *)OS_Alloc( MY_SIGN_BLOCK_SIZE );
  if( msfile->block_buf_out == NULL ) {
    return FALSE;
  }
  
  msfile->open_flag = TRUE;
  msfile->pos = 0;  /* original file pos */

  //  OS_TPrintf("%s %d\n",__FUNCTION__, __LINE__);

 end:
  return TRUE;
}


int my_sign_FS_ReadFile(MY_SIGN_File *msfile, u8 *buf, int buf_size)
{
  int i;
  int block_no;
  int enc_file_pos;
  MY_SIGN_SIGNATURE *L2_sign_table_temp;
  AES_KEY aes_key;
  u8 aes_key_buf[AES_KEY_BYTE_LEN];
  unsigned char aes_iv[ AES_BLOCK_SIZE ];
  int block_buf_out_pos;
  int user_buf_size;
  u8 *user_buf_ptr;
  int num = 0;
  int readlen;
  int temp_size;
  BOOL cache_valid_flag;

  if( msfile->open_flag != TRUE ) {
    return -1; /* error */
  }

  user_buf_size = buf_size;
  user_buf_ptr = buf;

  if( msfile->pos >= (int)(msfile->header.org_file_size) ) { 
    return 0;
  }

  while( user_buf_size > 0 ) {
    block_no = msfile->pos / MY_SIGN_BLOCK_SIZE;
    block_buf_out_pos  = msfile->pos % MY_SIGN_BLOCK_SIZE;

    if( block_no >= (int)msfile->header.num_of_block ) {
      OS_TPrintf("%s %d pos = %d block_no = %d\n",__FUNCTION__, __LINE__,msfile->pos, block_no);
      break;
    }
  
    //  OS_TPrintf("%s %d pos = %d block_no = %d\n",__FUNCTION__, __LINE__,msfile->pos, block_no);

    enc_file_pos = (int)msfile->header.file_offset_data_block + block_no * MY_SIGN_BLOCK_SIZE;

    cache_valid_flag = FALSE;
    if( msfile->now_cache == TRUE ) {
      if( msfile->cache_msfile == (void *)msfile ) {
	if( msfile->cache_block_no == block_no ) {
	  cache_valid_flag = TRUE;
	}
      }
    }

    if( cache_valid_flag == FALSE ) {
      if( FALSE == FS_SeekFile(&(msfile->f), enc_file_pos , FS_SEEK_SET) ) {
	OS_TPrintf("%s %d Failed seek File pos=%d\n",__FUNCTION__, __LINE__, enc_file_pos );
	return -1;
      }
      
      readlen = FS_ReadFile(&(msfile->f), msfile->block_buf_in, MY_SIGN_BLOCK_SIZE);
      if( readlen != MY_SIGN_BLOCK_SIZE ) {
	OS_TPrintf("%s %d Failed read File readlen=%d\n",__FUNCTION__, __LINE__, readlen);
	return -1;
      }
      
      L2_sign_table_temp = msfile->L2_sign_table + block_no;
      
      /*  データブロックの署名チェック */
      if( FALSE == my_sign_check( L2_sign_table_temp, msfile->block_buf_in, MY_SIGN_BLOCK_SIZE) ) {
	OS_TPrintf("Data Hash check Error!\n");
	return -1;
      }

      /* AESキーのセット */
#if 1
      for( i = 0 ; i < AES_KEY_BYTE_LEN ; i++ ) {
	aes_key_buf[i] = my_sign_aes_key[i];
      }
      for( i = 0 ; i < AES_BLOCK_SIZE ; i++ ) {
	aes_iv[i] = my_sign_aes_iv[i];
      }
#else
      for( i = 0 ; i < AES_KEY_BYTE_LEN ; i++ ) {
	aes_key_buf[i] = (u8)i;
      }
      for( i = 0 ; i < AES_BLOCK_SIZE ; i++ ) {
	aes_iv[i] = (u8)i;
      }
#endif
      AES_set_decrypt_key(aes_key_buf, AES_KEY_BIT_LEN, &aes_key);
      
      memset(msfile->block_buf_out, 0 , MY_SIGN_BLOCK_SIZE);
      
      /* AES復号化 */
      for( i = 0 ; i < (MY_SIGN_BLOCK_SIZE / AES_BLOCK_SIZE) ; i++ ) {
	AES_cbc_encrypt( &(msfile->block_buf_in[AES_BLOCK_SIZE*i]), &(msfile->block_buf_out[AES_BLOCK_SIZE*i]), 
			 AES_BLOCK_SIZE, &aes_key, aes_iv, AES_DECRYPT );
	
      }
      msfile->now_cache = TRUE;
      msfile->cache_msfile = (void *)msfile;
      msfile->cache_block_no = block_no;
      //      OS_TPrintf("%s %d cache change\n",__FUNCTION__, __LINE__);
    }
      
    temp_size = (MY_SIGN_BLOCK_SIZE - block_buf_out_pos);
    if( (msfile->pos + temp_size) >= (int)msfile->header.org_file_size ) { 
      temp_size = (int)msfile->header.org_file_size - msfile->pos;
    }
    if( temp_size > user_buf_size ) {
      temp_size = user_buf_size;
    }

    memcpy(user_buf_ptr, &(msfile->block_buf_out[block_buf_out_pos]), (u32)temp_size );
    user_buf_ptr += temp_size;
    block_buf_out_pos += temp_size;
    msfile->pos += temp_size;
    user_buf_size -= temp_size;
    num += temp_size;

  }
  //  OS_TPrintf("%s %d num=%d\n",__FUNCTION__, __LINE__,num);
 end:
  return num;
}

BOOL my_sign_FS_CloseFile(MY_SIGN_File *msfile)
{
  if( msfile ) {
    if( msfile->L2_sign_table ) {
      OS_Free( msfile->L2_sign_table );
    }
    if( msfile->block_buf_in ) {
      OS_Free( msfile->block_buf_in );
    }
    if( msfile->block_buf_out ) {
      OS_Free( msfile->block_buf_out );
    }
  }
  (void)FS_CloseFile(&(msfile->f));
  return TRUE;
}



