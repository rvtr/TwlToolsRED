/*---------------------------------------------------------------------------*
  Project:  TwlSDK - - makelst
  File:     searcharg.c

  Copyright 2006-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "searcharg.h"

char*  output_fname = NULL;
char*  genuine_fname = NULL;
char*  magicon_fname = NULL;
char*  log_fname = NULL;


void SA_Usage( void)
{
    fprintf( stderr, "Analyzing Tool\n");
    fprintf( stderr, "Usage: makelst [-o output-file] [--g genuine-srl-file] [--m magicon-srl-file]\n\n");
    fprintf( stderr, "Usage: makelst [-o output-file] [--g genuine-srl-file] [--l access-log-file]\n\n");
    exit( 1);
}


/*à¯êîÇâêÕÇ∑ÇÈ*/
void SA_searchopt( int argc, char* argv[])
{
    int n;
    struct option optionInfo[] = {
        { "genuine", required_argument, NULL, 'g'},
        { "magicon", required_argument, NULL, 'm'},
        { "log",     required_argument, NULL, 'l'},
        { NULL, 0, NULL, 0}
    };

    if( argc <= 1) {
        SA_Usage();
    }
  
    while( (n = getopt_long( argc, argv, "do:h", &optionInfo[0], NULL))
           != -1)
    {
        switch( n) {
        case 'd':
//          dbg_print_flag = 1;
          break;
        case 'o':
          if( output_fname != NULL) {
              fprintf( stderr, "ERROR! redefined output filename.\n");
              SA_Usage();
          }
          output_fname = optarg;
          break;
        case 'g': // "--genuine"
          if( genuine_fname != NULL) {
              fprintf( stderr, "ERROR! redefined genuine filename.\n");
              SA_Usage();
          }
          genuine_fname = optarg;
          break;
        case 'm': // "--magicon"
          if( magicon_fname != NULL) {
              fprintf( stderr, "ERROR! redefined magicon filename.\n");
              SA_Usage();
          }
          magicon_fname = optarg;
          break;
        case 'l': // "--log"
          if( log_fname != NULL) {
              fprintf( stderr, "ERROR! redefined log filename.\n");
              SA_Usage();
          }
          log_fname = optarg;
          break;
        case 'h':
          SA_Usage();
          break;
        default: // '?'
          SA_Usage();
          break;
        }
    }
}

