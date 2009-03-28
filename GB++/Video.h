#ifndef __VIDEO_H__
#define __VIDEO_H__

#include "SDL.h"
#include "Memoria.h"

class Video
{
private:
	Memoria *mem;
	SDL_Surface *screen, *hideScreen;
public:
	Video();
	~Video(void);
	void SetMem(Memoria *mem);
	void RefreshScreen();
	//void ActualizarPantalla();
	void UpdateBG(BYTE line);
	//void actualizarBG(BYTE scanLine);
	void Close();
private:
	void DrawPixel(SDL_Surface *screen, Uint8 R, Uint8 G, Uint8 B, BYTE x, BYTE y);
	void ObtainPalette(BYTE * palette);
	//void DrawTile(WORD dir_tile, BYTE row, BYTE col);
	/*int getIdTile(int xTile, int yTile, int mapAddress, int tileAddress);
	int getTile(int idTile, int tileAddress, int bitX, int bitY);
	int id2color(int id, int direccion);*/
};

#endif