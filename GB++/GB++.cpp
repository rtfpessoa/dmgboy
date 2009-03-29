// GB++.cpp: define el punto de entrada de la aplicación de consola.
//

#include "stdafx.h"
#include "Cartridge.h"
#include "Registers.h"
#include "Memory.h"
#include "CPU.h"
#include "Video.h"
#include "Pad.h"

int _tmain(int argc, _TCHAR* argv[])
{
	//Cartridge c("D:\\Descargas\\GB\\ROMS\\32K\\Alleyway (JUA) [!].gb");
	//Cartridge c("D:\\Descargas\\GB\\ROMS\\32K\\Asteroids (UE).gb");
	//Cartridge c("D:\\Descargas\\GB\\ROMS\\32K\\Boxxle (V1.1) (U) [!].gb");
	//Cartridge c("D:\\Descargas\\GB\\ROMS\\32K\\Boxxle II (U).gb");						
	Cartridge c("D:\\Descargas\\GB\\ROMS\\32K\\Brainbender (U).gb");
	//Cartridge c("D:\\Descargas\\GB\\ROMS\\32K\\Bubble Ghost (U).gb");
	//Cartridge c("D:\\Descargas\\GB\\ROMS\\32K\\Castelian (U).gb");
	//Cartridge c("D:\\Descargas\\GB\\ROMS\\32K\\Catrap (U).gb");						
	//Cartridge c("D:\\Descargas\\GB\\ROMS\\32K\\Cool Ball (U).gb");
	//Cartridge c("D:\\Descargas\\GB\\ROMS\\32K\\Crystal Quest (U).gb");
	//Cartridge c("D:\\Descargas\\GB\\ROMS\\32K\\Daedalian Opus (U).gb");
	//Cartridge c("D:\\Descargas\\GB\\ROMS\\32K\\Dr. Mario (JUE) (v1.1).gb");			
	//Cartridge c("D:\\Descargas\\GB\\ROMS\\32K\\Flipull (U).gb");
	//Cartridge c("D:\\Descargas\\GB\\ROMS\\32K\\Game of Harmony, The (U).gb");
	//Cartridge c("D:\\Descargas\\GB\\ROMS\\32K\\Heiankyo Alien (U).gb");				
	//Cartridge c("D:\\Descargas\\GB\\ROMS\\32K\\Hyper Lode Runner (JU) (v1.1).gb");
	//Cartridge c("D:\\Descargas\\GB\\ROMS\\32K\\Kwirk (UA) [!].gb");					
	//Cartridge c("D:\\Descargas\\GB\\ROMS\\32K\\Loopz (JUE).gb");
	//Cartridge c("D:\\Descargas\\GB\\ROMS\\32K\\Missile Command (UE).gb");
	//Cartridge c("D:\\Descargas\\GB\\ROMS\\32K\\Motocross Maniacs (U).gb");
	//Cartridge c("D:\\Descargas\\GB\\ROMS\\32K\\NFL Football (U).gb");
	//Cartridge c("D:\\Descargas\\GB\\ROMS\\32K\\Pipe Dream (U).gb");
	//Cartridge c("D:\\Descargas\\GB\\ROMS\\32K\\Q Billion (U).gb");
	//Cartridge c("D:\\Descargas\\GB\\ROMS\\32K\\Serpent (U).gb");
	//Cartridge c("D:\\Descargas\\GB\\ROMS\\32K\\Shanghai (U).gb");
	//Cartridge c("D:\\Descargas\\GB\\ROMS\\32K\\Spot (U).gb");
	//Cartridge c("D:\\Descargas\\GB\\ROMS\\32K\\Tasmania Story (U).gb");
	//Cartridge c("D:\\Descargas\\GB\\ROMS\\32K\\Tennis (JUE).gb");
	//Cartridge c("D:\\Descargas\\GB\\ROMS\\32K\\Tesserae (U).gb");
	//Cartridge c("D:\\Descargas\\GB\\ROMS\\32K\\Tetris (JUE) (v1.1).gb");
	//Cartridge c("D:\\Descargas\\GB\\ROMS\\32K\\World Bowling (U).gb");

	//Cartridge c("D:\\Descargas\\GB\\ROMS\\PDRoms\\AsciiWars.gb");
	//Cartridge c("D:\\Descargas\\GB\\ROMS\\PDRoms\\Big Scroller Demo (PD).gb");
	//Cartridge c("D:\\Descargas\\GB\\ROMS\\PDRoms\\BLEM! (PD).gb");
	//Cartridge c("D:\\Descargas\\GB\\ROMS\\PDRoms\\Bounce.gb");
	//Cartridge c("D:\\Descargas\\GB\\ROMS\\PDRoms\\BOXING.GB");
	//Cartridge c("D:\\Descargas\\GB\\ROMS\\PDRoms\\CardShuffle.gb");					
	//Cartridge c("D:\\Descargas\\GB\\ROMS\\PDRoms\\DanLaser.gb");
	//Cartridge c("D:\\Descargas\\GB\\ROMS\\PDRoms\\DownhillSki.gb");
	//Cartridge c("D:\\Descargas\\GB\\ROMS\\PDRoms\\GAME1.GB");
	//Cartridge c("D:\\Descargas\\GB\\ROMS\\PDRoms\\GAY.GB");
	//Cartridge c("D:\\Descargas\\GB\\ROMS\\PDRoms\\GB-TicTacToe.gb");							
	//Cartridge c("D:\\Descargas\\GB\\ROMS\\PDRoms\\GuessTheNumber_1.gb");							
	//Cartridge c("D:\\Descargas\\GB\\ROMS\\PDRoms\\GuessTheNumber_2.gb");							
	//Cartridge c("D:\\Descargas\\GB\\ROMS\\PDRoms\\Minesweeper.gb");
	//Cartridge c("D:\\Descargas\\GB\\ROMS\\PDRoms\\PaddleMission.gb");
	//Cartridge c("D:\\Descargas\\GB\\ROMS\\PDRoms\\Pong.gb");
	//Cartridge c("D:\\Descargas\\GB\\ROMS\\PDRoms\\Puzzle.gb");
	//Cartridge c("D:\\Descargas\\GB\\ROMS\\PDRoms\\Quiz.gb");


	//Tiene MBC - Cartridge c("D:\\Descargas\\GB\\ROMS\\PDRoms\\Pikakilla.gb");
	
	//c.Print(0, 0x1a0);
	
	system("PAUSE");

	Video v;
	Pad p;

	CPU cpu(&v, &p);
	cpu.CargarCartridge(&c);
	try
	{
		cpu.Run();
	}
	catch(exception e)
	{
		cerr << e.what();
	}
	
	v.Close();

	system("PAUSE");
	return 0;
}

