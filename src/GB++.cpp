// GB++.cpp: define el punto de entrada de la aplicación de consola.
//
#include "Cartridge.h"
#include <iostream>
#include "Video.h"
#include "Registers.h"
#include "Memory.h"
#include "CPU.h"
#include "Pad.h"

//Para evitar que el gcc en windows de un mensaje de error de referencia al linkar
//WinMain16 no encontrada
#undef main

int main ( int argc, char** argv )
{
	string basePath;

#ifdef WIN32
	basePath = "D:/Descargas/GB/ROMS/";
#else
	basePath = "/media/disk/Descargas/GB/ROMS/";
#endif

	Cartridge c(basePath + "32K/Alleyway (JUA) [!].gb");
	//Cartridge c(basePath + "32K/Asteroids (UE).gb");
	//Cartridge c(basePath + "32K/Boxxle (V1.1) (U) [!].gb");
	//Cartridge c(basePath + "32K/Boxxle II (U).gb");
	//Cartridge c(basePath + "32K/Brainbender (U).gb");
	//Cartridge c(basePath + "32K/Bubble Ghost (U).gb");
	//Cartridge c(basePath + "32K/Castelian (U).gb");
	//Cartridge c(basePath + "32K/Catrap (U).gb");
	//Cartridge c(basePath + "32K/Cool Ball (U).gb");
	//Cartridge c(basePath + "32K/Crystal Quest (U).gb");
	//Cartridge c(basePath + "32K/Daedalian Opus (U).gb");
	//Cartridge c(basePath + "32K/Dr. Mario (JUE) (v1.1).gb");
	//Cartridge c(basePath + "32K/Flipull (U).gb");
	//Cartridge c(basePath + "32K/Game of Harmony, The (U).gb");
	//Cartridge c(basePath + "32K/Heiankyo Alien (U).gb");
	//Cartridge c(basePath + "32K/Hyper Lode Runner (JU) (v1.1).gb");
	//Cartridge c(basePath + "32K/Kwirk (UA) [!].gb");
	//Cartridge c(basePath + "32K/Loopz (JUE).gb");
	//Cartridge c(basePath + "32K/Missile Command (UE).gb");
	//Cartridge c(basePath + "32K/Motocross Maniacs (U).gb");
	//Cartridge c(basePath + "32K/NFL Football (U).gb");
	//Cartridge c(basePath + "32K/Pipe Dream (U).gb");
	//Cartridge c(basePath + "32K/Q Billion (U).gb");
	//Cartridge c(basePath + "32K/Serpent (U).gb");
	//Cartridge c(basePath + "32K/Shanghai (U).gb");
	//Cartridge c(basePath + "32K/Spot (U).gb");
	//Cartridge c(basePath + "32K/Tasmania Story (U).gb");
	//Cartridge c(basePath + "32K/Tennis (JUE).gb");
	//Cartridge c(basePath + "32K/Tesserae (U).gb");
	//Cartridge c(basePath + "32K/Tetris (JUE) (v1.1).gb");
	//Cartridge c(basePath + "32K/World Bowling (U).gb");

	//Cartridge c(basePath + "PDRoms/AsciiWars.gb");
	//Cartridge c(basePath + "PDRoms/Big Scroller Demo (PD).gb");
	//Cartridge c(basePath + "PDRoms/BLEM! (PD).gb");
	//Cartridge c(basePath + "PDRoms/Bounce.gb");
	//Cartridge c(basePath + "PDRoms/BOXING.GB");
	//Cartridge c(basePath + "PDRoms/CardShuffle.gb");
	//Cartridge c(basePath + "PDRoms/DanLaser.gb");
	//Cartridge c(basePath + "PDRoms/DownhillSki.gb");
	//Cartridge c(basePath + "PDRoms/GAME1.GB");
	//Cartridge c(basePath + "PDRoms/GAY.GB");
	//Cartridge c(basePath + "PDRoms/GB-TicTacToe.gb");
	//Cartridge c(basePath + "PDRoms/GuessTheNumber_1.gb");
	//Cartridge c(basePath + "PDRoms/GuessTheNumber_2.gb");
	//Cartridge c(basePath + "PDRoms/Minesweeper.gb");
	//Cartridge c(basePath + "PDRoms/PaddleMission.gb");
	//Cartridge c(basePath + "PDRoms/Pong.gb");
	//Cartridge c(basePath + "PDRoms/Puzzle.gb");
	//Cartridge c(basePath + "PDRoms/Quiz.gb");


	//Tiene MBC - Cartridge c(basePath + "PDRoms/Pikakilla.gb");

	if (!c.isLoaded())
	{
		cout << "Press to finish ..." << endl;
		cin.get();

		return 0;
	}

    cout << "Press to continue ..." << endl;
    cin.get();
    Video v;
	Pad p;

	CPU cpu(&v, &p);
	cpu.LoadCartridge(&c);
	try
	{
		cpu.Run();
	}
	catch(exception e)
	{
		cerr << e.what();
	}

	v.Close();

    cout << "Press to finish ..." << endl;
    cin.get();

	return 0;
}

