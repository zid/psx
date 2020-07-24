#ifndef GPU_H
#define GPU_H
void gpu_copy_rect_from_cd(unsigned int x, unsigned int y, unsigned int w, unsigned int h);
void gpu_send_packet(unsigned int *l);
void gpu_send_list(void *p);
void gpu_display_enable(unsigned int n);
void gpu_draw_offset(int x, int y);
void gpu_clip_area(unsigned int x, unsigned int y, unsigned int x2, unsigned int y2);
void gpu_draw_mode(unsigned int n);
void gpu_video_mode(unsigned int n);
void gpu_vert_range(unsigned int top, unsigned int bottom);
void gpu_horiz_range(unsigned int start, unsigned int end);
void gpu_display_offset(unsigned int x, unsigned int y);
void gpu_reset(void);
#endif
