#ifndef GPU_H
#define GPU_H
void gpu_video_mode(unsigned int n);
void gpu_vert_range(unsigned int top, unsigned int bottom);
void gpu_horiz_range(unsigned int start, unsigned int end);
void gpu_display_offset(unsigned int x, unsigned int y);
void gpu_reset(void);
#endif
