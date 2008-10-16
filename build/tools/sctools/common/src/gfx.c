#include        "font.h"
#include        "text.h"
#include        "mprintf.h"
#include        "gfx.h"

static GXOamAttr g_oam[128];
#define VRAM_SIZE 2*32*24

/* DISPLAY Control */

// #define TEXT_HEAPBUF_SIZE 0x16000
#define TEXT_HEAPBUF_SIZE 0x80000 /* 512KByte */
static u8 text_heap_buffer[TEXT_HEAPBUF_SIZE];

static TEXT_CTRL textctrl[NUM_OF_SCREEN];
TEXT_CTRL *tc[NUM_OF_SCREEN];
static int vram_num_main = 1;
static int vram_num_sub = 0;
static void   VBlankIntr( void );
static u32 v_blank_intr_counter = 0;

static u32 g_screen[MAX_VRAM_NUM][VRAM_SIZE/sizeof(u32)];


void Gfx_Init(void)
{
  int i;
  u16 palette_no = 0x0f; 
  int line_buf_count;

  FX_Init();
  GX_Init();
  GX_DispOff();
  GXS_DispOff();



  line_buf_count = init_text_buf_sys((void *)&(text_heap_buffer[0]), 
				     (void *)&(text_heap_buffer[TEXT_HEAPBUF_SIZE]));

  // OS_TPrintf("Init start 1\n");
  for( i = 0 ; i < NUM_OF_SCREEN ; i++) {
    tc[i] = &(textctrl[i]);
    init_text(tc[i],  (u16 *)&(g_screen[i]),  palette_no);
  }



  
  // VRAM������
  GX_SetBankForLCDC(GX_VRAM_LCDC_ALL);
  MI_CpuClearFast((void*)HW_LCDC_VRAM,HW_LCDC_VRAM_SIZE);
  (void)GX_DisableBankForLCDC();
  MI_CpuFillFast((void*)HW_OAM,192,HW_OAM_SIZE);
  MI_CpuClearFast((void*)HW_PLTT,HW_PLTT_SIZE);
  MI_CpuFillFast((void*)HW_DB_OAM,192,HW_DB_OAM_SIZE);
  MI_CpuClearFast((void*)HW_DB_PLTT,HW_DB_PLTT_SIZE);


  
  /* BG0�\���ݒ� */
  GX_SetBankForBG(GX_VRAM_BG_128_A);
  G2_SetBG0Control(GX_BG_SCRSIZE_TEXT_256x256,
		   GX_BG_COLORMODE_16,
		   GX_BG_SCRBASE_0xf000,	// SCR base block 31
		   GX_BG_CHARBASE_0x00000,	// CHR base block 0
		   GX_BG_EXTPLTT_01);
  G2_SetBG0Priority(0);
  G2_BG0Mosaic(FALSE);
  // 2D�\���ݒ�
  GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS,GX_BGMODE_0,GX_BG0_AS_2D);
  GX_LoadBG0Char(d_CharData,0,sizeof(d_CharData));
  GX_LoadBGPltt(d_PaletteData,0,sizeof(d_PaletteData));
  //  GX_SetBankForARM7(GX_VRAM_ARM7_128_D);
  // OBJ�\���ݒ�
  GX_SetBankForOBJ(GX_VRAM_OBJ_128_B);
  GX_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_2D);
  GX_LoadOBJ(d_IconCharData,0,sizeof(d_IconCharData));
  GX_LoadOBJPltt(d_IconPaletteData,0,sizeof(d_IconPaletteData));
  
  GX_SetVisiblePlane(GX_PLANEMASK_BG0 | GX_PLANEMASK_OBJ );  


  /* BG1�\���ݒ� */
  GX_SetBankForSubBG( GX_VRAM_SUB_BG_48_HI );
  G2S_SetBG0Control(
		    GX_BG_SCRSIZE_TEXT_256x256 ,
		    GX_BG_COLORMODE_16 ,
		    GX_BG_SCRBASE_0xf800 ,	// SCR �x�[�X�u���b�N 31
		    GX_BG_CHARBASE_0x10000 ,	// CHR �x�[�X�u���b�N 0
		    GX_BG_EXTPLTT_01
		    );
  G2S_SetBG0Priority( 0 );
  G2S_BG0Mosaic( FALSE );
  GXS_SetGraphicsMode(GX_BGMODE_0);
  GXS_LoadBG0Char(d_CharData,0,sizeof(d_CharData));
  GXS_LoadBGPltt(d_PaletteData,0,sizeof(d_PaletteData));

  GX_SetBankForSubOBJ(GX_VRAM_SUB_OBJ_128_D);
  GXS_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_2D);


  GXS_LoadOBJ(d_IconCharData,0,sizeof(d_IconCharData));
  GXS_LoadOBJPltt(d_IconPaletteData,0,sizeof(d_IconPaletteData));
  GXS_SetVisiblePlane( GX_PLANEMASK_BG0 | GX_PLANEMASK_OBJ );
  

  // LCD�\���J�n
  GX_DispOn();
  GXS_DispOn();
  
  for(i=0;i<128;i++){
    G2_SetOBJPosition(&(g_oam[i]),256,192);
  }

  G2_SetBG0Offset( 0, 0 );
  G2S_SetBG0Offset( 0, 0 );


  // for V Blank
  (void)OS_SetIrqFunction( OS_IE_V_BLANK, VBlankIntr );
  (void)OS_EnableIrqMask( OS_IE_V_BLANK );
  (void)GX_VBlankIntr( TRUE );

}

static void VBlankIntr(void)
{
  if( vram_num_main < NUM_OF_SCREEN ) {
    DC_FlushRange( (void *)&(g_screen[vram_num_main]),VRAM_SIZE);
  }
  GX_LoadBG0Scr( &(g_screen[vram_num_main]),0,VRAM_SIZE);

  if( vram_num_sub < NUM_OF_SCREEN ) {
    DC_FlushRange( (void *)&(g_screen[vram_num_sub]),VRAM_SIZE);
  }
  GXS_LoadBG0Scr( (void *)&(g_screen[vram_num_sub]) , 0 , VRAM_SIZE );

  // ���zOAM��VRAM�ɔ��f
  DC_FlushRange(g_oam,sizeof(g_oam));
  GX_LoadOAM(g_oam,0,sizeof(g_oam));
  //---- ���荞�݃`�F�b�N�t���O
  OS_SetIrqCheckFlag( OS_IE_V_BLANK );
  v_blank_intr_counter++;
}



void Gfx_Render(int main_no, int sub_no)
{
  vram_num_main = main_no;
  vram_num_sub = sub_no;

  if( -1 < vram_num_main && vram_num_main < NUM_OF_SCREEN ) {
    text_buf_to_vram(tc[vram_num_main]);
  }
  if( -1 < vram_num_sub && vram_num_sub < NUM_OF_SCREEN ) {
    text_buf_to_vram(tc[vram_num_sub]);
  }
}
