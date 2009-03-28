// GB++.cpp: define el punto de entrada de la aplicación de consola.
//

#include "stdafx.h"
#include "Cartucho.h"
#include "Registros.h"
#include "Memoria.h"
#include "CPU.h"
#include "Video.h"

int _tmain(int argc, _TCHAR* argv[])
{
	//Cartucho c("D:\\Descargas\\GB\\ROMS\\32K\\Alleyway (JUA) [!].gb");
	//Cartucho c("D:\\Descargas\\GB\\ROMS\\32K\\Asteroids (UE).gb");
	//Cartucho c("D:\\Descargas\\GB\\ROMS\\32K\\Boxxle (V1.1) (U) [!].gb");
	//Cartucho c("D:\\Descargas\\GB\\ROMS\\32K\\Boxxle II (U).gb");						
	//Cartucho c("D:\\Descargas\\GB\\ROMS\\32K\\Brainbender (U).gb");
	//Cartucho c("D:\\Descargas\\GB\\ROMS\\32K\\Bubble Ghost (U).gb");
	Cartucho c("D:\\Descargas\\GB\\ROMS\\32K\\Castelian (U).gb");
	//Cartucho c("D:\\Descargas\\GB\\ROMS\\32K\\Catrap (U).gb");						
	//Cartucho c("D:\\Descargas\\GB\\ROMS\\32K\\Cool Ball (U).gb");
	//Cartucho c("D:\\Descargas\\GB\\ROMS\\32K\\Crystal Quest (U).gb");
	//Cartucho c("D:\\Descargas\\GB\\ROMS\\32K\\Daedalian Opus (U).gb");
	//Cartucho c("D:\\Descargas\\GB\\ROMS\\32K\\Dr. Mario (JUE) (v1.1).gb");			
	//Cartucho c("D:\\Descargas\\GB\\ROMS\\32K\\Flipull (U).gb");
	//Cartucho c("D:\\Descargas\\GB\\ROMS\\32K\\Game of Harmony, The (U).gb");
	//Cartucho c("D:\\Descargas\\GB\\ROMS\\32K\\Heiankyo Alien (U).gb");				
	//Cartucho c("D:\\Descargas\\GB\\ROMS\\32K\\Hyper Lode Runner (JU) (v1.1).gb");
	//Cartucho c("D:\\Descargas\\GB\\ROMS\\32K\\Kwirk (UA) [!].gb");					
	//Cartucho c("D:\\Descargas\\GB\\ROMS\\32K\\Loopz (JUE).gb");
	//Cartucho c("D:\\Descargas\\GB\\ROMS\\32K\\Missile Command (UE).gb");
	//Cartucho c("D:\\Descargas\\GB\\ROMS\\32K\\Motocross Maniacs (U).gb");
	//Cartucho c("D:\\Descargas\\GB\\ROMS\\32K\\NFL Football (U).gb");
	//Cartucho c("D:\\Descargas\\GB\\ROMS\\32K\\Pipe Dream (U).gb");
	//Cartucho c("D:\\Descargas\\GB\\ROMS\\32K\\Q Billion (U).gb");
	//Cartucho c("D:\\Descargas\\GB\\ROMS\\32K\\Serpent (U).gb");
	//Cartucho c("D:\\Descargas\\GB\\ROMS\\32K\\Shanghai (U).gb");
	//Cartucho c("D:\\Descargas\\GB\\ROMS\\32K\\Spot (U).gb");
	//Cartucho c("D:\\Descargas\\GB\\ROMS\\32K\\Tasmania Story (U).gb");
	//Cartucho c("D:\\Descargas\\GB\\ROMS\\32K\\Tennis (JUE).gb");
	//Cartucho c("D:\\Descargas\\GB\\ROMS\\32K\\Tesserae (U).gb");
	//Cartucho c("D:\\Descargas\\GB\\ROMS\\32K\\Tetris (JUE) (v1.1).gb");
	//Cartucho c("D:\\Descargas\\GB\\ROMS\\32K\\World Bowling (U).gb");

	//Cartucho c("D:\\Descargas\\GB\\ROMS\\PDRoms\\AsciiWars.gb");
	//Cartucho c("D:\\Descargas\\GB\\ROMS\\PDRoms\\Big Scroller Demo (PD).gb");
	//Cartucho c("D:\\Descargas\\GB\\ROMS\\PDRoms\\BLEM! (PD).gb");
	//Cartucho c("D:\\Descargas\\GB\\ROMS\\PDRoms\\Bounce.gb");
	//Cartucho c("D:\\Descargas\\GB\\ROMS\\PDRoms\\BOXING.GB");
	//Cartucho c("D:\\Descargas\\GB\\ROMS\\PDRoms\\CardShuffle.gb");					
	//Cartucho c("D:\\Descargas\\GB\\ROMS\\PDRoms\\DanLaser.gb");
	//Cartucho c("D:\\Descargas\\GB\\ROMS\\PDRoms\\DownhillSki.gb");
	//Cartucho c("D:\\Descargas\\GB\\ROMS\\PDRoms\\GAME1.GB");
	//Cartucho c("D:\\Descargas\\GB\\ROMS\\PDRoms\\GAY.GB");
	//Cartucho c("D:\\Descargas\\GB\\ROMS\\PDRoms\\GB-TicTacToe.gb");							
	//Cartucho c("D:\\Descargas\\GB\\ROMS\\PDRoms\\GuessTheNumber_1.gb");							
	//Cartucho c("D:\\Descargas\\GB\\ROMS\\PDRoms\\GuessTheNumber_2.gb");							
	//Cartucho c("D:\\Descargas\\GB\\ROMS\\PDRoms\\Minesweeper.gb");
	//Cartucho c("D:\\Descargas\\GB\\ROMS\\PDRoms\\PaddleMission.gb");
	//Cartucho c("D:\\Descargas\\GB\\ROMS\\PDRoms\\Pong.gb");
	//Cartucho c("D:\\Descargas\\GB\\ROMS\\PDRoms\\Puzzle.gb");
	//Cartucho c("D:\\Descargas\\GB\\ROMS\\PDRoms\\Quiz.gb");


	//Tiene MBC - Cartucho c("D:\\Descargas\\GB\\ROMS\\PDRoms\\Pikakilla.gb");
	
	//c.Imprimir(0, 0x1a0);
	
	system("PAUSE");

	Video v;

	CPU cpu(&v);
	cpu.CargarCartucho(&c);
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

