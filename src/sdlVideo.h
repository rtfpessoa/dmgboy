#ifndef __SDLVIDEO_H__
#define __SDLVIDEO_H__

void onVideoInit(int width, int height);
void onVideoPreDraw();
void onVideoPostDraw();
void onVideoRefreshScreen();
void onVideoDrawPixel(int color, int x, int y);
void onVideoClose();

#endif