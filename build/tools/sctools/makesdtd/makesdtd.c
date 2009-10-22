/* 
 [ROM�p] make_sdtad_table.exe -dir test-tad -odir [dir] -var MAKEROM_TAD_ROMFILES -fdir tads -mk Makefile.inc
 [SD�p] make_sdtad_table.exe -sd -dir test-tad -odir [dir] -fdir sdtads

 table_file.txt ���O�Œ�

 makesdtd.exe -indir [inputdir] -odir sdtads
 makesdtd.exe -indir test-tad -odir sdtads

*/


/*
  #include <unistd.h>
  char *getcwd(char *buf, size_t size);
  char *getwd(char *buf);
  char *get_current_dir_name(void);
*/


#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h> 
#include <unistd.h> 

#include "my_sign.h"

typedef unsigned char   u8;
typedef unsigned short u16;
typedef unsigned long  u32;
typedef unsigned long long u64;
typedef int BOOL;

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif


/* �O���[�o���f�[�^ */

#define MAX_TITLE_IDS 2048
static int num_of_title_ids = 0;
static u32 title_id_hi_array[MAX_TITLE_IDS];
static u32 title_id_lo_array[MAX_TITLE_IDS];
static u16 title_ver_array[MAX_TITLE_IDS];
static u16 title_gid_array[MAX_TITLE_IDS];
static char title_rom_file_full_path[MAX_TITLE_IDS][256];
static char key_file_path[256];


/* tad�̃f�[�^�͊�{�I�Ƀr�b�O�G���f�B�A�� */

typedef struct {
  u32 hdrSize;
  u16 tadType;
  u16 tadVersion;
  u32 certSize;
  u32 crlSize;
  u32 ticketSize;
  u32 tmdSize;
  u32 contentSize;
  u32 metaSize;    

  u32 certOffset;
  u32 crlOffset;
  u32 ticketOffset;
  u32 tmdOffset;
  u32 contentOffset;
  u32 metaOffset;
  u32 fileSize;
} TAD_INFO;

typedef struct NAMiTADHeader
{
  u32 hdrSize;        // Size of TADHeader1 96
  u8  tadType[2];
  u16 tadVersion;		// TAD_VERSION_1
  u32 certSize;
  u32 crlSize;
  u32 ticketSize;
  u32 tmdSize;
  u32 contentSize;
  u32 metaSize;
} NAMiTADHeader;

typedef u64 OSTitleId;
typedef OSTitleId NAMTitleId;

typedef struct NAMTitleInfo
{
  NAMTitleId  titleId;
  u16 companyCode;
  u16 version;
  u32 publicSaveSize;
  u32 privateSaveSize;
  u32 blockSize;
} NAMTitleInfo;

typedef struct NAMTadInfo
{
    NAMTitleInfo titleInfo;
    u32     fileSize;
} NAMTadInfo;


#define ES_APP_ENC_HANDLE       6

#define ES_ROOT_NAME            "Root"
#define ES_CA_PREFIX            "CA"
#define ES_XS_PREFIX            "XS"
#define ES_MS_PREFIX            "MS"
#define ES_CP_PREFIX            "CP"
#define ES_APP_CERT_PREFIX      "AP"

#define ES_BUF_SIZE             256

typedef u32  ESId;                 /* 32-bit device identity */
typedef u32  ESContentId;          /* 32-bit content identity */
typedef u64  ESTitleId;            /* 64-bit title identity */
typedef u64  ESTicketId;           /* 64-bit ticket id */
typedef u8   ESVersion;            /* 8-bit data structure version */
typedef u16  ESTitleVersion;       /* 16-bit title version */
typedef ESTitleId ESSysVersion;    /* 64-bit system software version */
typedef u32  ESTitleType;          /* title type */
typedef u16  ESContentType;        /* content type */
typedef u8   ESTmdReserved[62];    /* reserved field in TMD structure */
typedef u8   ESTicketReserved[47]; /* reserved field in eTicket structure */
typedef u8   ESSysAccessMask[2];   /* 16 bit cidx Mask to indicate which 

                                      content can be accessed by sys app */
#if !defined(__ES_INTERNAL__)
/* IOSC types */
typedef u8   IOSCAesKey[16];
typedef u8   IOSCHash[20];
typedef u8   IOSCName[64];
typedef u8   IOSCSigDummy[60];
typedef u8   IOSCCertPad[52];
typedef u8   IOSCEccCertPad[4];
typedef u8   IOSCRsaSig2048[256];
typedef enum {
    IOSC_SIG_RSA4096 = 0x00010000,  /* RSA 4096 bit signature */
    IOSC_SIG_RSA2048,  /* RSA 2048 bit signature */
    IOSC_SIG_ECC       /* ECC signature 512 bits*/
} IOSCCertSigType;
typedef enum {
    IOSC_PUBKEY_RSA4096,  /* RSA 4096 bit key */
    IOSC_PUBKEY_RSA2048,  /* RSA 2048 bit key */
    IOSC_PUBKEY_ECC       /* ECC pub key 512 bits*/
} IOSCCertPubKeyType;
typedef u8   IOSCEccSig[60];
typedef u8   IOSCEccPublicKey[60];

typedef struct {
    IOSCCertSigType sigType;
    IOSCRsaSig2048  sig;
    IOSCSigDummy    dummy;
    IOSCName        issuer;
} IOSCSigRsa2048;

typedef u8   IOSCEccPrivatePad[2];
typedef u8   IOSCEccPublicPad[4];
typedef struct {
    IOSCCertSigType  sigType;
    IOSCEccSig       sig;
    IOSCEccPublicPad eccPad;
    IOSCSigDummy     dummy;
    IOSCName         issuer;
} IOSCSigEcc;

typedef u8   IOSCDeviceId[64];
typedef u8   IOSCServerId[64];
typedef struct {
    IOSCCertPubKeyType pubKeyType;
    union {
        IOSCServerId serverId;
        IOSCDeviceId deviceId;
    } name;
    u32 date;
} IOSCCertHeader;
typedef struct {
    IOSCSigEcc          sig;        /* ECC signature struct */
    IOSCCertHeader      head;
    IOSCEccPublicKey    pubKey;     /* 60 byte ECC public key */
    IOSCEccPublicPad    eccPad;
    IOSCCertPad         pad;
    IOSCEccCertPad      pad2;
} IOSCEccEccCert;
#endif

#define ES_TMD_VERSION             0
#define ES_TICKET_VERSION          0

/* TMD View */
#define ES_MAX_CONTENT             512

#define ES_TITLE_TYPE_NC_TITLE     0         /* NC title */
#define ES_TITLE_TYPE_NG_TITLE     1         /* NG title */
#define ES_TITLE_TYPE_DS_TITLE     2         /* DS title for NC */
#define ES_TITLE_TYPE_STREAM       4         /* stream title */
#define ES_TITLE_TYPE_DATA         8         /* data title */
#define ES_CONTENT_TYPE_SHARED     (1<<15)   /* shared content */
#define ES_CONTENT_TYPE_OPTIONAL   (1<<14)   /* optional content */
#define ES_CONTENT_TYPE_DISC       (1<< 1)   /* disc content */
#define ES_CONTENT_TYPE_NC_EXE     0         /* NC merged elf */
#define ES_CONTENT_TYPE_NG_EXE     1         /* NG content */

#define ES_LICENSE_MASK      0x0f /*  allow 16 licensing types */
#define ES_LICENSE_PERMANENT 0x00 /* e.g.,  regular game with permanent rights */
#define ES_LICENSE_DEMO      0x01 /* e.g.,  demo game with permanent rights */
#define ES_LICENSE_TRIAL     0x02 /* e.g.,  regular game with limited rights */
#define ES_LICENSE_RENTAL    0x03 /* TBD */
#define ES_LICENSE_SUBSCRIPTION 0x04 /* TBD */    
#define ES_LICENSE_GIFT_MASK 0x80 /* this is a gift */

/* TMD */
typedef struct {
    ESContentId    cid;    /* 32 bit content id */
    u16            index;  /* content index, unique per title */
    ESContentType  type;   /* content type*/
    u64            size;   /* unencrypted content size in bytes */
    IOSCHash       hash;   /* 160-bit SHA1 hash of the content */
} ESContentMeta;

typedef struct {
    ESVersion      version;  /* TMD version number */
    ESVersion      caCrlVersion;  /* CA CRL version number */
    ESVersion      signerCrlVersion; /* signer CRL version number */
    ESSysVersion   sysVersion;  /* required system software version number */

    ESTitleId      titleId;      /* 64 bit title id */
    ESTitleType    type;         /* 32 bit title type */
    u16            groupId;
    ESTmdReserved  reserved;     /* 62 bytes reserved info for Nintendo */
    u32            accessRights; /* title's access rights to use 
                                    system resources */
    ESTitleVersion titleVersion; /* 16 bit title version */
    u16            numContents;  /* number of contents per title */
    u16            bootIndex;    /* boot content index */
} ESTitleMetaHeader;

typedef struct {
    IOSCSigRsa2048     sig;      /* RSA 2048bit sign of all the data in 
                                    the TMD file */
    ESTitleMetaHeader  head;
    ESContentMeta      contents[ES_MAX_CONTENT];
} ESTitleMeta;

/* TMD View */
typedef struct {
    ESVersion        version;           /* TMD data structure version */
    ESSysVersion     sysVersion;        /* required system software 
                                           version number */
    ESTitleId        titleId;           /* 64 bit title id */
    ESTitleType      type;              /* 32 bit title type */
    u16              groupId;
    ESTmdReserved    reserved;          /* 62 bytes reserved info */
    ESTitleVersion   titleVersion;      /* 16 bit title version */
    u16              numContents;       /* number of contents in the title */
} ESTmdViewHeader;

typedef struct {
    ESContentId      cid;               /* 32 bit content id */
    u16              index;             /* 16 bit content index */
    ESContentType    type;              /* 16 bit content type */
    u64              size;              /* 64 bit content size */
} ESCmdView;

typedef struct {
    ESTmdViewHeader head;
    ESCmdView       contents[ES_MAX_CONTENT];
} ESTmdView;



typedef struct NAMiTadParams
{
    u32 cert;
    u32 crl;
    u32 ticket;
    u32 tmd;
    u32 content;
    u32 meta;
}
NAMiTadParams;

//---- tad �\�����
typedef struct NAMiTadInfo
{
    NAMiTadParams sizes;        // �e�̈�̃T�C�Y
    NAMiTadParams offsets;      // �t�@�C���擪����e�̈�ւ̃I�t�Z�b�g
}
NAMiTadInfo;

#if 0
static my_NAM_ReadTadInfoWithFile(FILE *fp, TAD_INFO *tad_info)
{
 NAMiTadInfo tadInfo;
 ESTitleMeta tmd;
 int result;

 result = my_sign_ReadTadHeader(fp, &tadInfo);

 // TMD ��ǂ�
 fread(&tmd, tmd-size, fp);
 result = NAMi_Load_sign( fp, (void*)tmd, tadInfo.sizes.tmd,
			  tadInfo.offsets.tmd,


 NAMi_CopyTmdReservedInfo(&pInfo->titleInfo, &pTmd->head.reserved);
 pInfo->titleInfo.titleId            = NAMi_EndianU64(pTmd->head.titleId);
 pInfo->titleInfo.companyCode        = NAMi_EndianU16(pTmd->head.groupId);
 pInfo->titleInfo.version            = NAMi_EndianU16(pTmd->head.titleVersion);
 pInfo->titleInfo.blockSize          = my_NAMi_CalcTitleBlocksFromTmd(pTmd);
 pInfo->fileSize                     = my_sign_FS_GetLength(pTadFile);
 
 NAMi_Free(pTmd);

}
#endif

#if 0
static void test(void)
{
  NAMiTadInfo* pInfo;
  // ESTitleMeta tmd;
#if 0
  typedef struct {
    ESVersion      version;  /* TMD version number */
    ESVersion      caCrlVersion;  /* CA CRL version number */
    ESVersion      signerCrlVersion; /* signer CRL version number */
    ESSysVersion   sysVersion;  /* required system software version number */
    
    ESTitleId      titleId;      // 8 
    ESTitleType    type;         // 4
    u16            groupId;      // 2
    ESTmdReserved  reserved;     /* 62 bytes reserved info for Nintendo */
    u32            accessRights; /* title's access rights to use 
                                    system resources */
    ESTitleVersion titleVersion; /* 16 bit title version */
    u16            numContents;  /* number of contents per title */
    u16            bootIndex;    /* boot content index */
  } ESTitleMetaHeader;
#endif

  printf("TMD head offset = %d 0x%04x\n", offsetof(ESTitleMeta, head), offsetof(ESTitleMeta, head));
  printf("ESTitleMetaHeader titleId offset = %d 0x%04x\n", offsetof(ESTitleMetaHeader ,titleId ), offsetof(ESTitleMetaHeader ,titleId));
  
  printf("ESTitleMetaHeader titleVersion offset = %d 0x%04x\n", offsetof(ESTitleMetaHeader ,titleVersion ), offsetof(ESTitleMetaHeader ,titleVersion));
  
  
  printf("ESTitleMetaHeader groupId offset = %d 0x%04x\n", offsetof(ESTitleMetaHeader ,groupId ), offsetof(ESTitleMetaHeader ,groupId));
  
  printf("ESTitleMetaHeader numContents offset = %d 0x%04x\n", offsetof(ESTitleMetaHeader ,numContents ), offsetof(ESTitleMetaHeader ,numContents));
  
  
  
  printf("NAMiTADHeader = %d 0x%04x\n",sizeof(NAMiTADHeader),sizeof(NAMiTADHeader));
  printf("ESTmdView = %d 0x%04x\n",sizeof(ESTmdView),sizeof(ESTmdView));
  printf("ESTmdViewHeader = %d 0x%04x\n",sizeof(ESTmdViewHeader),sizeof(ESTmdViewHeader));
  printf("ESCmdView = %d 0x%04x\n",sizeof(ESCmdView),sizeof(ESCmdView));
#if 0
  
  printf(" = %d 0x%04x\n",sizeof(),sizeof());
  printf(" = %d 0x%04x\n",sizeof(),sizeof());
  printf(" = %d 0x%04x\n",sizeof(),sizeof());
  printf(" = %d 0x%04x\n",sizeof(),sizeof());
  printf(" = %d 0x%04x\n",sizeof(),sizeof());
#endif
  
}
#endif
 
 




 




static BOOL debug_print_flag = FALSE;


static u32 reverseEndian4( const u32 v )
{
  u32 ret = (v<<24) | ((v<<8) & 0x00FF0000) | ((v>>8) & 0x0000FF00) | (v>>24);
  return ret;
}

static u16 reverseEndian2( const u16 v )
{
  u16 ret = (v<<8) | (v>>8);
  return ret;
}

static u32 roundUp4( const u32 v, const u32 align )
{
	u32 r = ((v + align - 1) / align) * align;
	return r;
}

#if 0
/* unused */
static u16 roundUp2( const u16 v, const u16 align )
{
	u16 r = ((v + align - 1) / align) * align;
	return r;
}
#endif

static u16 read2bytes(FILE *fp)
{
  u16 buf;
  if( 2 != fread(&buf,1,2,fp) ) {
    fprintf(stderr, "Error: %s %s %d\n",__FILE__,__FUNCTION__,__LINE__);
    exit(-1);
  }
  return buf;
} 

static u32 read4bytes(FILE *fp)
{
  u32 buf;
  if( 4 != fread(&buf,1,4,fp) ) {
    fprintf(stderr, "Error: %s %s %d\n",__FILE__,__FUNCTION__,__LINE__);
    exit(-1);
  }
  return buf;
} 


static BOOL read_tad_info(FILE *fp, TAD_INFO *tad_info)
{

  tad_info->hdrSize     = reverseEndian4( read4bytes(fp) );	// ��{�I�Ƀr�b�O�G���f�B�A��
  tad_info->tadType     = reverseEndian2( read2bytes(fp) );
  tad_info->tadVersion  = reverseEndian2( read2bytes(fp) );
  tad_info->certSize    = reverseEndian4( read4bytes(fp) );
  tad_info->crlSize     = reverseEndian4( read4bytes(fp) );
  tad_info->ticketSize   = reverseEndian4( read4bytes(fp) );
  tad_info->tmdSize      = reverseEndian4( read4bytes(fp) );
  tad_info->contentSize = reverseEndian4( read4bytes(fp) );
  tad_info->metaSize    = reverseEndian4( read4bytes(fp) );

  if( tad_info->hdrSize != 32) {
    return FALSE;
  }

  if( debug_print_flag ) {
    printf( "hdrSize     %d\n", (int)tad_info->hdrSize );
    printf( "tadType     %c%c\n", tad_info->tadType>>8, tad_info->tadType&0xFF );
    printf( "tadVersion  %d\n", (int)tad_info->tadVersion );
    printf( "certSize    %d\n", (int)tad_info->certSize );
    printf( "crlSize     %d\n", (int)tad_info->crlSize );
    printf( "ticketSize  %d\n", (int)tad_info->ticketSize );
    printf( "tmdSize     %d\n", (int)tad_info->tmdSize );
    printf( "contentSize %d\n", (int)tad_info->contentSize );
    printf( "metaSize    %d\n", (int)tad_info->metaSize );
  }


  tad_info->certOffset 		= roundUp4( tad_info->hdrSize , 64 );
  tad_info->crlOffset  		= roundUp4( tad_info->certOffset 	 + tad_info->certSize , 64 );
  tad_info->ticketOffset 	= roundUp4( tad_info->crlOffset 	 + tad_info->crlSize , 64 );
  tad_info->tmdOffset  		= roundUp4( tad_info->ticketOffset + tad_info->ticketSize , 64 );
  tad_info->contentOffset	= roundUp4( tad_info->tmdOffset 	 + tad_info->tmdSize , 64 );
  tad_info->metaOffset 		= roundUp4( tad_info->contentOffset+ tad_info->contentSize , 64 );
  tad_info->fileSize		= roundUp4( tad_info->metaOffset 	 + tad_info->metaSize , 64 );

  return TRUE;
}


static u8 *alloc_and_read(FILE *fp, int offset, int size)
{
  u8 *buf;
  if( fp == NULL || size < 1 ) {
    fprintf(stderr, "Error %s open file\n",__FUNCTION__);
    return NULL;
  }

  buf = malloc(size);
  if( buf == NULL ) {
    fprintf(stderr, "Error %s memory allocate \n",__FUNCTION__);
    return NULL;
  }
  
  if( -1 == fseek( fp, offset, SEEK_SET ) ) {
    fprintf(stderr, "Error: %s %s %d fseek offset=%d\n",__FILE__,__FUNCTION__,__LINE__,offset);
    return NULL;
  }
  if( size != fread(buf,1,size,fp) ) {
    fprintf(stderr, "Error: %s %s %d fread size=%d\n",__FILE__,__FUNCTION__,__LINE__,size);
    return NULL;
  }    
  return buf;
}

#if 0
/* unused */
static BOOL saveFile(char *name, char *buf, int size)
{
  FILE *fp;
  fp = fopen(name, "wb");
  if( fp == NULL ) {
    fprintf(stderr, "Error %s %d file open\n",__FUNCTION__,__LINE__);
    return FALSE;
  }
  if( fwrite(buf, 1, size, fp) != size ) {
    fprintf(stderr, "Error %s %d file write\n",__FUNCTION__,__LINE__);
    return FALSE;
  } 
  fclose( fp );
  return TRUE;
}
#endif

#if 0
static void print_gamecode(u32 tid_lo)
{
  char gamecode[5];

  char *str;
  str = gamecode;
  *str++ = (char)((tid_lo >> 24) & 0xff);
  *str++ = (char)((tid_lo >> 16) & 0xff);
  *str++ = (char)((tid_lo >> 8) & 0xff);
  *str++ = (char)(tid_lo  & 0xff);
  *str  = '\0';
  printf( "%s", gamecode );
}
#endif


#if 0
static write_tad_table_form(FILE *fp, u32 tid_hi, u32 tid_lo, char *filename)
{
  fprintf(fp, "0x%08x%08x, %d , %d , rom:/tads/%s\n",tid_hi, tid_lo, 0 , 0 , filename);
}
#endif

static BOOL read_file_and_print_titleid( char *path , char *d_name, FILE *fp_out, FILE *fp_mk)
{
  FILE *fp_in = NULL;
  TAD_INFO tad_info;
  u8 *tmd = NULL;
  u32 titleid_hi = 0;
  u32 titleid_lo = 0;
  u16 t_ver;
  u16 gid;
  int i;
  BOOL ret_flag = TRUE;


  fp_in = fopen(path,"rb");	/* fseek���������� */
  if( fp_in == NULL ) {
    fprintf(stderr, "error: file open %s\n",path);
    ret_flag = FALSE;
    goto end_file; 
 }

  /* TAD�t�@�C�����ǂ����`�F�b�N */
  if( FALSE == read_tad_info(fp_in, &tad_info) ) {
    //    fprintf(stderr, "error:%s %d\n",__FUNCTION__,__LINE__);
    ret_flag = FALSE;
    goto end_file;
  }


  fseek( fp_in, 0, SEEK_END );
  if( tad_info.fileSize != ftell( fp_in ) ) {
    printf( "file size is not expected size(=%d)", (int)tad_info.fileSize );
    ret_flag = FALSE;
    goto end_file;
  }
  
  tmd = alloc_and_read( fp_in, tad_info.tmdOffset, tad_info.tmdSize );


  titleid_hi = reverseEndian4( *((u32 *)( tmd + 0x18C )) );
  titleid_lo = reverseEndian4( *((u32 *)( tmd + 0x18C + 4 )) );

  t_ver = reverseEndian2( *((u16 *)( tmd + 0x1DC )) ); 
  gid  = reverseEndian2( *((u16 *)( tmd + 0x198)) ); 
#if 0
  printf("0x%08x%08x ver=0x%04x gid=0x%04x\n",titleid_hi,  titleid_lo, 
	 t_ver,groupId);
#endif

  if( debug_print_flag ) {
    printf("inputfile = %s\n", path);
  }


  /* �����Ń^�C�g���h�c�̏d���`�F�b�N���� */
  for( i = 0 ; i < num_of_title_ids ; i++ ) {
    if( (title_id_hi_array[i] == titleid_hi) && (title_id_lo_array[i] == titleid_lo) ) {
      if( title_ver_array[i] == t_ver ) {
	/* �o�[�W�������܂߂ďd�����Ă�I */
	fprintf(stderr, "Error: title exists redundantly\n");
      }
      else {
	/* �o�[�W�����Ⴂ�����Ǐd�����Ă� */
	fprintf(stderr, "Warning: title exists in different versions\n");
      }

      fprintf(stderr, "  0x%08x%08x, 0x%04x , 0x%04x  %s\n", 
	      (int)title_id_hi_array[i], (int)title_id_lo_array[i], 
	      title_ver_array[i], title_gid_array[i], title_rom_file_full_path[i] );
      fprintf(stderr, "  0x%08x%08x, 0x%04x , 0x%04x  %s\n", (int)titleid_hi,  (int)titleid_lo, t_ver , gid , path);
    }
  }


  title_id_hi_array[ num_of_title_ids ] = titleid_hi;
  title_id_lo_array[ num_of_title_ids ] = titleid_lo;
  title_ver_array[ num_of_title_ids ] = t_ver;
  title_gid_array[ num_of_title_ids ] = gid;
  strcpy( title_rom_file_full_path[num_of_title_ids], path);

  num_of_title_ids++;

  if( fp_out ) {
    fprintf(fp_out, "0x%08x%08x, 0x%04x , 0x%04x , sdmc:/%s,\n", 
	    (int)titleid_hi,  (int)titleid_lo, t_ver , gid , d_name);
    if( fp_mk ) {
      fprintf(fp_mk, "\t\t%s \\\n", d_name);
    }
  }
  else {
    printf("0x%08x%08x, 0x%04x , 0x%04x , sdmc:/%s,\n",
	   (int)titleid_hi,  (int)titleid_lo, t_ver , gid , d_name);
  }
  
 end_file:
  if(tmd) {
    free(tmd);
  }
  
  if( fp_in ) {
    fclose(fp_in);
  }

  return ret_flag;
}

int main(int argc, char **argv)
{
  int i;
  FILE *fp_in = NULL;
  FILE *fp_out = NULL;
  FILE *fp_mk = NULL;
  char *infile = NULL;
  char *outfile = NULL;

  char *outdir_name = NULL;
  char *indir_name = NULL;

  BOOL read_file_flag = FALSE;
  BOOL write_file_flag = FALSE;
  BOOL write_dir_flag = FALSE;
  BOOL dir_read_flag = FALSE;

  char *prog;
  int badops = 0;
  u8 *ticket = NULL;
  u8 *tmd = NULL;
  u8 *content = NULL;
  //  u64 titleid = 0;
#if 0
  TAD_INFO tad_info;
  u32 titleid_hi = 0;
  u32 titleid_lo = 0;
  u16 groupid = 0;
#endif
  DIR *indir;
  DIR *outdir;
  struct dirent *dr;
  struct stat st;
  char *full_path;
  char rom_file_full_path[256];
  BOOL func_ret_flag;
  char out_file_full_path[256];
  char table_file_full_path[256];
  char en_table_file_full_path[256];
  int len;
  int pos;


  prog=argv[0];
  argc--;
  argv++;


  //  printf("d = %s\n", prog);

  
  //  strcpy(key_file_path, ".\\");
  strcat(key_file_path, prog);

  len = strlen(key_file_path);
  for( pos = len - 1 ; pos > 0 ; pos--) {
    if( key_file_path[pos] == '\\' ) {
      strcpy( &(key_file_path[pos]), "\\tad1024.der");
      break;
    }
  }


  printf("key_file_path = %s\n",key_file_path );
  

  while (argc >= 1) {
    if (strcmp(*argv,"-indir") == 0  && !dir_read_flag ) {
      if (--argc < 1) {
	goto bad;
      }
      indir_name = *++argv;
      dir_read_flag = TRUE;
    }
    else if (strcmp(*argv,"-odir") == 0 && !write_dir_flag ) {
      if (--argc < 1) {
	goto bad;
      }
      outdir_name = *++argv;
      write_dir_flag = TRUE;
    }
    else if (strcmp(*argv,"-d") == 0 ) {
      debug_print_flag = TRUE;
    }
    else if ( !read_file_flag ) {
      infile = *argv;
      read_file_flag = TRUE;
    }
    else {
      goto bad;
    }
    argc--;
    argv++;
  }



  if (badops) {
  bad:
    fprintf(stderr, "%s -indir dirname -odir dirname\n",prog);
    goto end;
  }


    if( debug_print_flag ) {
      printf("dir name = %s\n", indir_name);
    }
    indir = opendir(indir_name);
    if( indir == NULL ) {
      fprintf(stderr, "error: dir open %s\n",indir_name);
      goto end;
    }


    if( write_dir_flag ) {
      outdir = opendir(outdir_name);
      if( outdir == NULL ) {
	//	fprintf(stderr, "error: dir open %s\n",indir_name);
	/*
	  EACCES 
	  �A�N�Z�X�������Ȃ��B 
	  EBADF 
	  fd ���ǂݏo���p�ɃI�[�v�����ꂽ�A�L���ȃt�@�C���f�B�X�N���v�^�ł͂Ȃ��B 
	  EMFILE 
	  �v���Z�X���g�p���̃t�@�C���f�B�X�N���v�^����������B 
	  ENFILE 
	  �V�X�e���ŃI�[�v������Ă���t�@�C������������B 
	  ENOENT 
	  �f�B���N�g�������݂��Ȃ����A�܂��� name ���󕶎���ł���B 
	  ENOMEM 
	  ���߂����s����̂ɏ[���ȃ��������Ȃ��B 
	  ENOTDIR 
	  name �̓f�B���N�g���ł͂Ȃ��B 
	*/
	if( -1 ==  mkdir(outdir_name) ) {
	  fprintf(stderr, "error: mkdir %s\n",indir_name);
	  goto end;
	}

      }
      else {
	closedir(outdir);
      }



      strcpy(table_file_full_path, outdir_name);
      strcat(table_file_full_path, "/table_file.txt");
      outfile = table_file_full_path;

      strcpy(en_table_file_full_path, outdir_name);
      strcat(en_table_file_full_path, "/en_sdtad_table.txt");

      fp_out = fopen(outfile,"wb");	/* fseek���������� */
      if( fp_out == NULL ) {
	fprintf(stderr, "error: file open %s\n",outfile);
	goto end;
      }
      write_file_flag = TRUE;
    }

    

    if( 0 != cryptopc_init( key_file_path ) ) {
      fprintf(stderr, "error: cryptopc\n");
      goto end;
    }


    /* �`�F�b�N�p�̃e�[�u��������(����񂯂�) */
    num_of_title_ids = 0;
    for( i = 0 ; i < MAX_TITLE_IDS ; i++ ) {
      title_id_hi_array[i] = 0;
      title_id_lo_array[i] = 0;
      title_ver_array[i] = 0;
      title_gid_array[i] = 0;
      title_rom_file_full_path[i][0] = 0;
    }
    
    /* �f�B���N�g������TAD�t�@�C�������X�g�ɂ��ĕ\������B */
    while( (dr = readdir(indir)) != NULL ) {
      if (!strcmp(dr->d_name, ".") || !strcmp(dr->d_name, "..")) {
	continue;
      }
	
      full_path = malloc( strlen(indir_name) + strlen(dr->d_name) + 2);
      strcpy(full_path, indir_name);
      strcat(full_path, "/");
      strcat(full_path, dr->d_name);

      // printf("%s\n", full_path);
      if ( stat(full_path, &st) != 0 ) {
	free( full_path);
	continue;
      }

      if (S_ISDIR(st.st_mode) == 1) {
	if( debug_print_flag ) {
	  printf("DIR  %s\n",  dr->d_name);
	}
      }
      else {
	if( debug_print_flag ) {
	  printf("FILE %s\n",  dr->d_name);
	}
	if( st.st_size >= 32 ) { 
	  //  strcpy( rom_file_full_path, file_dir);
	  //  strcat( rom_file_full_path, "/");
	  // strcat( rom_file_full_path, "en_");
	  strcpy( rom_file_full_path, "sdtads/en_");
	  strcat( rom_file_full_path, dr->d_name);
	  func_ret_flag = read_file_and_print_titleid( full_path ,rom_file_full_path, fp_out , fp_mk );

	  if( func_ret_flag == TRUE ) {
	    strcpy(out_file_full_path, outdir_name);
	    strcat(out_file_full_path, "/en_");
	    strcat(out_file_full_path, dr->d_name);
	    //	    printf("cryptopc tad1024.der %s %s\n",full_path, out_file_full_path);
	    putchar('.');
	    (void)cryptopc(full_path, out_file_full_path);
	    /* 
	       tad�t�@�C���������̂Ńe�[�u���t�@�C���ɓo�^���ꂽ�B
	       �Ȃ̂ňÍ��������łɂ���B
	    */
	  }
	}
      }
      free( full_path);
    }
    putchar('\n');

    if( indir ) {
      (void)closedir( indir );
    }

    if( write_file_flag ) {
      if( fp_out ) {
	fclose(fp_out);
	fp_out = NULL;
      }
      /* table_file.txt ���Í������� */
      (void)cryptopc(table_file_full_path, en_table_file_full_path);
      // char en_table_file_full_path[256];
    }

 end:

    if( ticket ) {
      free(ticket);
    }
    if(tmd) {
      free(tmd);
    }
    if(content) {
      free(content);
    }
    if( fp_in ) {
      fclose(fp_in);
    }
    if( fp_out ) {
      fclose(fp_out);
    }

    if( fp_mk ) {
      fclose(fp_mk);
    }

    cryptopc_end();


  return 0;
}





