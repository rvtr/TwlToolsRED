/*---------------------------------------------------------------------------*
  Project:  MainMemoryDumper
  File:     main.c

  Copyright 2011 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date::            $
  $Rev$
  $Author$
 *---------------------------------------------------------------------------*/

#include <twl.h>
#include "wh.h"
#include "misc_simple.h"
#include "data.h"
#include "mmdumper.h"
#include "mmd_parent.h"
#include "mmd_client.h"

// extern data-----------------------------------------------------------------

// define data-----------------------------------------------------------------
#define WH_GGID           SDK_MAKEGGID_SYSTEM(0xfe)

// function's prototype-------------------------------------------------------
static void printHeader(void);
static void printParentOrClient(void);
static void actionParentOrClient(void);

static void TraceWH(const char *fmt, ...);

// global variable-------------------------------------------------------------

// static variable-------------------------------------------------------------
static MMDumperStage stage;

static u16 cursor;

static MenuPos pos[] =
{
    { TRUE, 1, 3 },
    { TRUE, 1, 4 },
    { TRUE, 1, 5 },
    { TRUE, 1, 6 },
    { TRUE, 1, 7 },
    { TRUE, 1, 8 },
    { TRUE, 1, 9 },
    { TRUE, 1, 10 },
    { TRUE, 1, 11 },
    { TRUE, 1, 12 },
    { TRUE, 1, 13 },
    { TRUE, 1, 14 },
};

// const data------------------------------------------------------------------
static const char *strStage[] =
{
    "",
    "(Parent)",
    "(Client)",
    "",
    "",
    "!ERROR!"
};

static const char *strParentOrClient[] =
{
    "Parent for dumping",
    "Client for storing"
};
static const MenuParam menuParentOrClient =
{
    sizeof(strParentOrClient)/sizeof(char*),
    TXT_COLOR_BLACK,
    TXT_COLOR_DARKGREEN,
    TXT_COLOR_DARKGRAY,
    pos,
    strParentOrClient
};


// ============================================================================
// function's description
// ============================================================================
void mmdumperInit(void)
{
    stage = MMD_STAGE_INIT;
    cursor = 0;

    WH_SetGgid(WH_GGID);
    WH_SetDebugOutput(TraceWH);
}

void mmdumperEnd(void)
{
    WH_Finalize();
    stage = MMD_STAGE_END;
}

BOOL mmdumperMain(void)
{
    printHeader();

    switch (stage)
    {
    case MMD_STAGE_INIT:
        if ( OS_IsRunOnTwl() )
        {
            printParentOrClient();
            actionParentOrClient();
        }
        else
        {
            stage = MMD_STAGE_PARENT;
        }
        break;
    case MMD_STAGE_PARENT:
        printParent();
        actionParent();
        break;
    case MMD_STAGE_CLIENT:
        printClient();
        actionClient();
        break;
    case MMD_STAGE_PRE_END:
        mmdumperEnd();
        stage = MMD_STAGE_END;
        break;
    case MMD_STAGE_END:
        myDp_Printf(1, 3, TXT_COLOR_BLACK, MAIN_SCREEN, "Done.");
        break;
    }
    if ( stage == MMD_STAGE_ERROR )
    {
        return FALSE;
    }
    return TRUE;
}

void printWMError(const char* strWMError)
{
    myDp_Printf(1, 18, TXT_COLOR_RED, MAIN_SCREEN, "WMError: %s", strWMError);
    mmdumperEnd();
    stage = MMD_STAGE_ERROR;
}

u16 cursorUpDown(u16 now, u16 nums)
{
    if ( DEMO_IS_TRIG( PAD_KEY_UP ) )
    {
        return (now + nums - 1) % nums;
    }
    if ( DEMO_IS_TRIG( PAD_KEY_DOWN ) )
    {
        return (now + 1) % nums;
    }
    return now;
}


static void printHeader(void)
{
    myDp_Printf(1, 1, TXT_COLOR_BLUE, MAIN_SCREEN, "Main Memory Dumper %s", strStage[stage]);
}

#include <twl/ltdmain_begin.h>
static void printParentOrClient(void)
{
    myDp_DrawMenu(cursor, MAIN_SCREEN, &menuParentOrClient);
}

static void actionParentOrClient(void)
{
    cursor = cursorUpDown(cursor, (u16)menuParentOrClient.num);

    if ( DEMO_IS_TRIG( PAD_BUTTON_A ) )
    {
        switch (cursor)
        {
        case 0: stage = MMD_STAGE_PARENT; break;
        case 1: stage = MMD_STAGE_CLIENT; break;
        }
    }
}
#include <twl/ltdmain_end.h>

static void TraceWH(const char *fmt, ...)
{
    va_list vlist;
    char temp[SCREEN_SIZE+1];
    temp[sizeof(temp)-1] = 0;
    va_start(vlist, fmt);
    STD_TVSNPrintf(temp, sizeof(temp) - 1, fmt, vlist);
    va_end(vlist);
    myDp_Write(1, 20, TXT_COLOR_RED, MAIN_SCREEN, temp);
    OS_TPrintf("(1,20) %s\n", temp);
}
