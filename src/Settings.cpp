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

#include "Settings.h"
#include <wx/wx.h>

using namespace std;

Settings settings;

Settings::Settings()
{
	greenScale	= false;
	windowZoom	= 1;
	
	padKeys[0]	= WXK_UP;	// Up
	padKeys[1]	= WXK_DOWN; // Down
	padKeys[2]	= WXK_LEFT; // Left
	padKeys[3]	= WXK_RIGHT;// Right
	padKeys[4]	= 'A';		// A
	padKeys[5]	= 'S';		// B
	padKeys[6]	= 'Q';		// Select
	padKeys[7]	= 'W';		// Start
}

Settings SettingsGetCopy()
{
	return settings;
}

void SettingsSetNewValues(Settings newSettings)
{
	settings = newSettings;
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
