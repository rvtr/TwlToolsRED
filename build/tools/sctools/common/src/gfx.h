#ifndef	_GFX_H_
#define	_GFX_H_


#ifdef __cplusplus
extern "C" {
#endif

void Gfx_Init(void);
void Gfx_Render(int main_no, int sub_no);
void Gfx_Set_BG1_Color(u16 col);
void Gfx_Set_BG1_line_Color(int vram_num, int num_of_line, u16 col);

#ifdef __cplusplus
}
#endif



#define MAX_VRAM_NUM  (NUM_OF_SCREEN)

extern TEXT_CTRL *tc[NUM_OF_SCREEN];
extern int vram_num_main;
extern int vram_num_sub;


#endif /* _GFX_H_ */
