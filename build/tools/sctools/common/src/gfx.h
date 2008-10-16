#ifndef	_GFX_H_
#define	_GFX_H_


#ifdef __cplusplus
extern "C" {
#endif

void Gfx_Init(void);
void Gfx_Render(int main_no, int sub_no);

#ifdef __cplusplus
}
#endif



#define MAX_VRAM_NUM  (NUM_OF_SCREEN)

extern TEXT_CTRL *tc[NUM_OF_SCREEN];
extern int vram_num_main;
extern int vram_num_sub;


#endif /* _GFX_H_ */
