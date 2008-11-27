#ifndef _TEXT_H_
#define _TEXT_H_

#define X_LINE_MAX (32)
#define Y_LINE_MAX (24)

#define LINE_BUF_X_SIZE  256
#define LINE_BUF_Y_SIZE  24

#define DEFAULT_TEXT_PALETTE_NO 0x0f

#define M_TEXT_COLOR_BLACK          0
#define M_TEXT_COLOR_RED            1
#define M_TEXT_COLOR_GREEN          2
#define M_TEXT_COLOR_BLUE           3
#define M_TEXT_COLOR_YELLOW         4
#define M_TEXT_COLOR_PURPLE         5
#define M_TEXT_COLOR_LIGHTBLUE      6
#define M_TEXT_COLOR_DARKRED        7
#define M_TEXT_COLOR_DARKGREEN      8
#define M_TEXT_COLOR_DARKBLUE       9
#define M_TEXT_COLOR_ORANGE         0xa
#define M_TEXT_COLOR_BROWN          0xb
#define M_TEXT_COLOR_PINK           0xc
#define M_TEXT_COLOR_GRAY           0xd
#define M_TEXT_COLOR_DARKGRAY       0xe
#define M_TEXT_COLOR_WHITE          0x0f

/* ƒIƒŒƒ“ƒW‚Æ‚©‚Ù‚µ‚¢ */


typedef struct LINE_BUF_s {
  int buf[LINE_BUF_X_SIZE];
  u16 col[LINE_BUF_X_SIZE];
  int last_count;
  int cur_count;
  struct LINE_BUF_s *prev;
  struct LINE_BUF_s *next;
  int blink_flag;
} LINE_BUF;

typedef struct {
  int *num_x,*num_y;
  LINE_BUF *start;
  LINE_BUF *cur;
  int virtual_x;
  int virtual_y;
  int display_offset_x;
  int display_offset_y;
  int display_newline;
} TEXT_BUF;




// #define TEXT_BUFFER_SIZE 128
// typedef char (MY_ARRAY)[TEXT_BUFFER_SIZE] ;
//  MY_ARRAY buffer;

typedef struct {
  TEXT_BUF text_buf;
  u16 *screen;
  u16 x_line;
  u16 y_line;
  u16 palette;
  u16 padding;
} TEXT_CTRL;



#ifdef __cplusplus
extern "C" {
#endif

int init_text_buf_sys(void *heap_start, void *heap_end);


void m_putchar(TEXT_CTRL *tc, int c);
void m_set_palette(TEXT_CTRL *tc, u16 num);
u16 m_get_palette(TEXT_CTRL *tc);
void init_text(TEXT_CTRL *tc, u16 *screen, u16 palette);
void m_set_display_offset_y(TEXT_CTRL *tc, int offset);
int m_get_display_offset_y(TEXT_CTRL *tc);
void m_set_display_offset_x(TEXT_CTRL *tc, int offset);
int m_get_display_offset_x(TEXT_CTRL *tc);

void text_buf_to_vram(TEXT_CTRL *tc);
void text_display_newline_off(TEXT_CTRL *tc);
void text_display_newline_on(TEXT_CTRL *tc);
void text_blink_current_line(TEXT_CTRL *tc);
void text_blink_clear(TEXT_CTRL *tc);

#ifdef __cplusplus
}
#endif

#endif /* _TEXT_H_ */
