/*
 This file is part of gbpablog.
 
 gbpablog is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 gbpablog is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with gbpablog.  If not, see <http://www.gnu.org/licenses/>.
 */

// GB++.cpp: define el punto de entrada de la aplicación de consola.
//
#include <iostream>
#include "Cartridge.h"
#include "Video.h"
#include "CPU.h"
#include "GBException.h"
#include "SDL.h"

using namespace std;

//Para evitar que el gcc en windows de un mensaje de error de referencia al linkar
//WinMain16 no encontrada
#ifdef WIN32
#undef main
#endif

//TODO:
//	- Controlar la velocidad de pintado
//	- Guardar a disco duro la memoria de los cartuchos que llevan batería
//	- Permitir Savestates en cualquier momento
//	- Integrar interfaz gráfica (QT)
//	- Poder cambiar los botones del pad en cualquier momento
//	- Sonido
//	- Permitir cargar roms comprimidas en zip y 7z
//		http://www.winimage.com/zLibDll/minizip.html
//		http://www.boost.org/doc/libs/1_41_0/libs/iostreams/doc/index.html
//OK:
//	- Revisar compilación en gcc
//	- Permitir cargar una rom por línea de comandos

string selectROM(int argc, char*argv[])
{
	string basePath, fullPath;

	cout << "argc: " << argc << endl;

	if (argc >= 2)
	{
		cout << "argv[1]: " << argv[1] << endl;
		if (argc > 2) {
			cout << "Number of arguments incorrect" << endl;
		}
		else {
			fullPath = argv[1];
			return fullPath;
		}

	}

#ifdef WIN32
	basePath = "//.psf/Home/Documents/Programacion/GB/ROMS/";
#elif __MAC__
    basePath = "/Users/pablo/Documents/Programacion/GB/ROMS/";
#else
	basePath = "/media/disk/Descargas/GB/ROMS/";
#endif

	//fullPath = basePath + "32K/Alleyway (JUA) [!].gb";
	//fullPath = basePath + "32K/Asteroids (UE).gb";
	//fullPath = basePath + "32K/Boxxle (V1.1) (U) [!].gb"; // Pass:BXBX
	//fullPath = basePath + "32K/Boxxle II (U).gb";
	//fullPath = basePath + "32K/Brainbender (U).gb";
	//fullPath = basePath + "32K/Bubble Ghost (U).gb";
	//fullPath = basePath + "32K/Castelian (U).gb";
	//fullPath = basePath + "32K/Catrap (U).gb"; // Password: 2ARW
	//fullPath = basePath + "32K/Cool Ball (U).gb";
	//fullPath = basePath + "32K/Crystal Quest (U).gb";
	//fullPath = basePath + "32K/Daedalian Opus (U).gb";
	//fullPath = basePath + "32K/Dr. Mario (JUE) (v1.1).gb";
	//fullPath = basePath + "32K/Flipull (U).gb";
	//fullPath = basePath + "32K/Game of Harmony, The (U).gb";
	//fullPath = basePath + "32K/Heiankyo Alien (U).gb";
	//fullPath = basePath + "32K/Hyper Lode Runner (JU) (v1.1).gb";
	//fullPath = basePath + "32K/Kwirk (UA) [!].gb";
	//fullPath = basePath + "32K/Loopz (JUE).gb";
	//fullPath = basePath + "32K/Missile Command (UE).gb";
	//fullPath = basePath + "32K/Motocross Maniacs (U).gb";
	//fullPath = basePath + "32K/NFL Football (U).gb";
	//fullPath = basePath + "32K/Pipe Dream (U).gb";
	//fullPath = basePath + "32K/Q Billion (U).gb";
	//fullPath = basePath + "32K/Serpent (U).gb";
	//fullPath = basePath + "32K/Shanghai (U).gb";
	//fullPath = basePath + "32K/Spot (U).gb";
	//fullPath = basePath + "32K/Tasmania Story (U).gb";
	//fullPath = basePath + "32K/Tennis (JUE).gb";
	//fullPath = basePath + "32K/Tesserae (U).gb";
	//fullPath = basePath + "32K/Tetris (JUE) (v1.1).gb";
	//fullPath = basePath + "32K/World Bowling (U).gb";

	//fullPath = basePath + "128K/4 in 1 Funpak Vol. II (U).gb";

	//fullPath = basePath + "PDRoms/AsciiWars.gb";
	//fullPath = basePath + "PDRoms/Big Scroller Demo (PD).gb";
	//fullPath = basePath + "PDRoms/BLEM! (PD).gb";
	//fullPath = basePath + "PDRoms/Bounce.gb";
	//fullPath = basePath + "PDRoms/BOXING.GB";
	//fullPath = basePath + "PDRoms/CardShuffle.gb";
	//fullPath = basePath + "PDRoms/DanLaser.gb";
	//fullPath = basePath + "PDRoms/DownhillSki.gb";
	//fullPath = basePath + "PDRoms/GAME1.GB";
	//fullPath = basePath + "PDRoms/GAY.GB";
	//fullPath = basePath + "PDRoms/GB-TicTacToe.gb";
	//fullPath = basePath + "PDRoms/GuessTheNumber_1.gb";
	//fullPath = basePath + "PDRoms/GuessTheNumber_2.gb";
	//fullPath = basePath + "PDRoms/Minesweeper.gb";
	//fullPath = basePath + "PDRoms/PaddleMission.gb";
	//fullPath = basePath + "PDRoms/Pong.gb";
	//fullPath = basePath + "PDRoms/Puzzle.gb";
	//fullPath = basePath + "PDRoms/Quiz.gb";
	//fullPath = basePath + "PDRoms/Pikakilla.gb";
	//fullPath = basePath + "PDRoms/Runtime - Test Rom (PD) [b1].gb";

	//fullPath = basePath + "MBC1/Amazing Penguin (U).gb";
	//fullPath = basePath + "MBC1/Amazing Spider-Man, The (UE).gb";
	fullPath = basePath + "MBC1/Super Mario Land (JUE) (v1.1).gb";
	//fullPath = basePath + "MBC1/Castlevania Adventure, The (U).gb";
	//fullPath = basePath + "MBC1/Pac-Man (U).gb";
	//fullPath = basePath + "MBC1/Yoshi (U).gb";
	//fullPath = basePath + "MBC1/Street Fighter II (UE) (v1.1).gb";
	//fullPath = basePath + "MBC1/Ninja Gaiden Shadow (U).gb";
	//fullPath = basePath + "MBC1/Mortal Kombat (UE).gb";
	//fullPath = basePath + "MBC1/Mortal Kombat II (U).gb";
	//fullPath = basePath + "MBC1/Mortal Kombat I & II (U).gb";
	//fullPath = basePath + "MBC1/Megaman (U).gb";
	//fullPath = basePath + "MBC1/Kirby's Dream Land (UE).gb";
	//fullPath = basePath + "MBC1/King of Fighters '95, The (U).gb";
	//fullPath = basePath + "MBC1/Killer Instinct (UE).gb";
	//fullPath = basePath + "MBC1/FIFA International Soccer (U) (M4).gb";
	//fullPath = basePath + "MBC1/Double Dragon (UE).gb";
	//fullPath = basePath + "MBC1/Contra - The Alien Wars (U).gb";
	//fullPath = basePath + "MBC1/Chase H.Q. (UE) (v1.1).gb";
	//fullPath = basePath + "MBC1/Castlevania Legends (U).gb";
	//fullPath = basePath + "MBC1/Bionic Commando (U).gb";
	//fullPath = basePath + "MBC1/Battletoads (UE).gb";

	//fullPath = basePath + "MBC1/RAM/Legend of Zelda, The - Link's Awakening (UE) (v1.2).gb";
	//fullPath = basePath + "MBC1/RAM/Donkey Kong Land (UE).gb";
	//fullPath = basePath + "MBC1/RAM/Donkey Kong (JUE) (v1.1).gb";
	//fullPath = basePath + "MBC1/RAM/Metroid II - Return of Samus (JUE).gb";
	//fullPath = basePath + "MBC1/RAM/Super Mario Land 2 - 6 Golden Coins (UE) (v1.2).gb";
	//fullPath = basePath + "MBC1/RAM/InfoGenius Systems - Personal Organizer (U).gb";

	//fullPath = basePath + "MBC2/Final Fantasy Legend (Sa-Ga) (U) [!].gb";
	//fullPath = basePath + "MBC2/Lazlos' Leap (U).gb";
	//fullPath = basePath + "MBC2/Final Fantasy Adventure (U).gb";

	//fullPath = basePath + "MBC3/Wario Land II (UE).gb";

	//fullPath = basePath + "MBC5/Pokemon Azul (Blue) (S) [S].gb";

	return fullPath;
}

int main(int argc, char*argv[])
{
	Cartridge c(selectROM(argc, argv));

	if (!c.IsLoaded())
	{
		cout << "Press to finish ..." << endl;
		cin.get();

		return 0;
	}

    Video v;

	CPU cpu(&v, &c);
	try
	{
		cpu.Run();
	}
	catch(GBException &e)
	{
		if (e.GetType() != Exit)
			cerr << e.what() << endl;
	}

	cpu.SaveLog();

	v.Close();

	return 0;
}
