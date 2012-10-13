/*
 This file is part of DMGBoy.
 
 DMGBoy is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 DMGBoy is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with DMGBoy.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Settings.h"
#include <wx/wx.h>

using namespace std;

Settings settings;

Settings::Settings()
{
    renderMethod = 0;
	greenScale	 = false;
	windowZoom	 = 1;
	soundEnabled	= true;
	soundSampleRate = 44100;
	
	padKeys[0]	= WXK_UP;	// Up
	padKeys[1]	= WXK_DOWN; // Down
	padKeys[2]	= WXK_LEFT; // Left
	padKeys[3]	= WXK_RIGHT;// Right
	padKeys[4]	= 'A';		// A
	padKeys[5]	= 'S';		// B
	padKeys[6]	= WXK_SHIFT;  // Select
	padKeys[7]	= WXK_RETURN; // Start
}

Settings SettingsGetCopy()
{
	return settings;
}

void SettingsSetNewValues(Settings newSettings)
{
	settings = newSettings;
}

int SettingsGetRenderMethod()
{
	return settings.renderMethod;
}

void SettingsSetGreenScale(int renderMethod)
{
	settings.renderMethod = renderMethod;
}

bool SettingsGetGreenScale()
{
	return settings.greenScale;
}

void SettingsSetGreenScale(bool greenScale)
{
	settings.greenScale = greenScale;
}

int SettingsGetWindowZoom()
{
	return settings.windowZoom;
}

void SettingsSetWindowZoom(int windowZoom)
{
	settings.windowZoom = windowZoom;
}

bool SettingsGetSoundEnabled()
{
	return settings.soundEnabled;
}

void SettingsSetSoundEnabled(bool enabled)
{
	settings.soundEnabled = enabled;
}

int SettingsGetSoundSampleRate()
{
	return settings.soundSampleRate;
}

void SettingsSetSoundSampleRate(int sampleRate)
{
	settings.soundSampleRate = sampleRate;
}

int* SettingsGetInput()
{
	return &settings.padKeys[0];
}

void SettingsSetInput(const int* padKeys)
{
	for (int i=0; i<8; i++)
	{
		settings.padKeys[i] = padKeys[i];
	}
}

string* SettingsGetRecentRoms()
{
	return &settings.recentRoms[0];
}

void SettingsSetRecentRoms(const string* recentRoms)
{
	for (int i=0; i<10; i++)
	{
		settings.recentRoms[i] = recentRoms[i];
	}
}
