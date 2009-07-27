/* 
 [ROM用] make_tad_table.exe -dir . -o table_file.txt -var MAKEROM_TAD_ROMFILES -fdir tads -mk Makefile.inc
 [SD用] make_tad_table.exe -dir . -o table_file.txt -fdir tads_sd
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h> 
#include <unistd.h> 

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

/* tadのデータは基本的にビッグエンディアン */

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

static u16 roundUp2( const u16 v, const u16 align )
{
	u16 r = ((v + align - 1) / align) * align;
	return r;
}

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

  tad_info->hdrSize     = reverseEndian4( read4bytes(fp) );	// 基本的にビッグエンディアン
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
    printf( "hdrSize     %d\n", tad_info->hdrSize );
    printf( "tadType     %c%c\n", tad_info->tadType>>8, tad_info->tadType&0xFF );
    printf( "tadVersion  %d\n", tad_info->tadVersion );
    printf( "certSize    %d\n", tad_info->certSize );
    printf( "crlSize     %d\n", tad_info->crlSize );
    printf( "ticketSize  %d\n", tad_info->ticketSize );
    printf( "tmdSize     %d\n", tad_info->tmdSize );
    printf( "contentSize %d\n", tad_info->contentSize );
    printf( "metaSize    %d\n", tad_info->metaSize );
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


#if 0
static write_tad_table_form(FILE *fp, u32 tid_hi, u32 tid_lo, char *filename)
{
  fprintf(fp, "0x%08x%08x, %d , %d , rom:/tads/%s\n",tid_hi, tid_lo, 0 , 0 , filename);
}
#endif

static void read_file_and_print_titleid( char *path , char *d_name, FILE *fp_out, FILE *fp_mk)
{
  FILE *fp_in = NULL;
  TAD_INFO tad_info;
  u8 *tmd = NULL;
  u32 titleid_hi = 0;
  u32 titleid_lo = 0;

  fp_in = fopen(path,"rb");	/* fseek効かすため */
  if( fp_in == NULL ) {
    fprintf(stderr, "error: file open %s\n",path);
    goto end_file;
  }
    
  if( FALSE == read_tad_info(fp_in, &tad_info) ) {
    //    fprintf(stderr, "error:%s %d\n",__FUNCTION__,__LINE__);
    goto end_file;
  }


  fseek( fp_in, 0, SEEK_END );
  if( tad_info.fileSize != ftell( fp_in ) ) {
    printf( "file size is not expected size(=%d)", tad_info.fileSize );
    goto end_file;
  }
  
  tmd = alloc_and_read( fp_in, tad_info.tmdOffset, tad_info.tmdSize );


  titleid_hi = reverseEndian4( *((u32 *)( tmd + 0x18C )) );
  titleid_lo = reverseEndian4( *((u32 *)( tmd + 0x18C + 4 )) );
  

  if( debug_print_flag ) {
    printf("inputfile = %s\n", path);
  }

  if( fp_out ) {
    fprintf(fp_out, "0x%08x%08x, %d , %d , rom:/%s,\n", titleid_hi,  titleid_lo, 0 , 0 , d_name);
    if( fp_mk ) {
      fprintf(fp_mk, "\t\t%s \\\n", d_name);
    }
  }
  else {
    printf("0x%08x%08x, %d , %d , rom:/%s,\n", titleid_hi,  titleid_lo, 0 , 0 , d_name);
  }


 end_file:
  if(tmd) {
    free(tmd);
  }
  
  if( fp_in ) {
    fclose(fp_in);
  }

}

int main(int argc, char **argv)
{
  FILE *fp_in = NULL;
  FILE *fp_out = NULL;
  FILE *fp_mk = NULL;
  char *infile = NULL;
  char *outfile = NULL;
  char *dir_name = NULL;
  char *mkfile = NULL;
  char *var_name = NULL;
  char *file_dir = NULL;

  BOOL read_file_flag = FALSE;
  BOOL write_file_flag = FALSE;
  BOOL dir_read_flag = FALSE;
  BOOL mk_file_flag = FALSE;
  BOOL var_name_flag = FALSE;
  BOOL file_dir_flag = FALSE;

  char *prog;
  int badops = 0;
  TAD_INFO tad_info;
  u8 *ticket = NULL;
  u8 *tmd = NULL;
  u8 *content = NULL;
  u64 titleid = 0;
  u32 titleid_hi = 0;
  u32 titleid_lo = 0;
  u16 groupid = 0;

  DIR *dir;
  struct dirent *dr;
  struct stat st;
  char *full_path;
  char rom_file_full_path[256];


  prog=argv[0];
  argc--;
  argv++;

  while (argc >= 1) {
    if (strcmp(*argv,"-dir") == 0  && !dir_read_flag ) {
      if (--argc < 1) {
	goto bad;
      }
      dir_name = *++argv;
      dir_read_flag = TRUE;
    }
    else if (strcmp(*argv,"-var") == 0 && !var_name_flag ) {
      if (--argc < 1) {
	goto bad;
      }
      var_name = *++argv;
      var_name_flag = TRUE;
    }
    else if (strcmp(*argv,"-fdir") == 0 && !file_dir_flag ) {
      if (--argc < 1) {
	goto bad;
      }
      file_dir = *++argv;
      file_dir_flag = TRUE;
    }
    else if (strcmp(*argv,"-o") == 0 && !write_file_flag ) {
      if (--argc < 1) {
	goto bad;
      }
      outfile = *++argv;
      write_file_flag = TRUE;
    }
    else if ( strcmp(*argv,"-mk") == 0 && !mk_file_flag ) {
      if (--argc < 1) {
	goto bad;
      }
      mkfile = *++argv;
      mk_file_flag = TRUE;
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
    fprintf(stderr, "%s -dir dirname -o outfile -mk mkfile\n",prog);
    goto end;
  }

  if( dir_read_flag == TRUE) {

    if( debug_print_flag ) {
      printf("dir name = %s\n", dir_name);
    }
    dir = opendir(dir_name);
    if( dir == NULL ) {
      fprintf(stderr, "error: dir open %s\n",dir_name);
      goto end;
    }


    if( write_file_flag ) {
      fp_out = fopen(outfile,"wb");	/* fseek効かすため */
      if( fp_out == NULL ) {
	fprintf(stderr, "error: file open %s\n",outfile);
	goto end;
      }
    }

    if( mk_file_flag ) {
      fp_mk = fopen(mkfile,"wb");	/* fseek効かすため */
      if( fp_mk == NULL ) {
	fprintf(stderr, "error: file open %s\n",mkfile);
	goto end;
      }
      if( var_name_flag ) {
	fprintf(fp_mk, "%s = \\\n",var_name);
      }
      else {
	fprintf(fp_mk, "MAKEROM_TAD_ROMFILES = \\\n");
      }
    }


    

    while( (dr = readdir(dir)) != NULL ) {
      if (!strcmp(dr->d_name, ".") || !strcmp(dr->d_name, "..")) {
	continue;
      }
	
      full_path = malloc( strlen(dir_name) + strlen(dr->d_name) + 2);
      strcpy(full_path, dir_name);
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
	  if( file_dir_flag ) {
	    strcpy( rom_file_full_path, file_dir);
	    strcat( rom_file_full_path, "/");
	    strcat( rom_file_full_path, dr->d_name);
	    read_file_and_print_titleid( full_path ,rom_file_full_path, fp_out , fp_mk );
	  }
	  else {
	    read_file_and_print_titleid( full_path ,dr->d_name, fp_out , fp_mk );
	  }
	}
      }

      free( full_path);

    }
    if( dir ) {
      (void)closedir( dir );
    }

    if( write_file_flag ) {
      if( fp_out ) {
	fclose(fp_out);
	fp_out = NULL;
      }
    }
    
    if( mk_file_flag ) {
      if( fp_mk ) {
	fclose(fp_mk);
	fp_mk = NULL;
      }
    }

  }
  else {
    fp_in = fopen(infile,"rb");	/* fseek効かすため */
    if( fp_in == NULL ) {
      fprintf(stderr, "error: file open %s\n",infile);
      goto end;
    }
    
    if( FALSE == read_tad_info(fp_in, &tad_info) ) {
      fprintf(stderr, "%s infile\n",prog);
      goto end_file;
    }


    fseek( fp_in, 0, SEEK_END );
    if( tad_info.fileSize != ftell( fp_in ) ) {
      printf( "file size is not expected size(=%d)", tad_info.fileSize );
      goto end_file;
    }
  

    ticket = alloc_and_read( fp_in, tad_info.ticketOffset, tad_info.ticketSize );
    tmd = alloc_and_read( fp_in, tad_info.tmdOffset, tad_info.tmdSize );
    content = alloc_and_read( fp_in, tad_info.contentOffset, tad_info.contentSize );


    //  memcpy( (void *)&titleid, (void *)(tmd + 0x18C), 8 );
    //  memcpy( (void *)&groupid, (void *)(tmd + 0x198), 2 );
    //  printf("titleid = 0x%08x %08x\n", ((titleid) >> 32), (0xffffffff & titleid));

    titleid_hi = reverseEndian4( *((u32 *)( tmd + 0x18C )) );
    titleid_lo = reverseEndian4( *((u32 *)( tmd + 0x18C + 4 )) );

    groupid = reverseEndian2( *((u16 *)( tmd + 0x198)) );

    printf("inputfile = %s\n", infile);

    printf("titleid = 0x%08x %08x ", titleid_hi,  titleid_lo);
    printf("[");
    print_gamecode(titleid_lo);
    printf("]");
    printf("\n");

  end_file:
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

  }
 end:
  return 0;
}





