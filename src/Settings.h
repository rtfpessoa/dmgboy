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

#ifndef __SETTINGS_H__
#define __SETTINGS_H__

class Settings
{
public:
	bool greenScale;
	int windowZoom;
	
	int padKeys[8];
public:
	Settings();
};

Settings SettingsGetCopy();

bool SettingsGetGreenScale();
int  SettingsGetWindowZoom();
int* SettingsGetInput();

void SettingsSetNewValues(Settings newSettings);

void SettingsSetGreenScale(bool greenScale);
void SettingsSetWindowZoom(int windowZoom);
void SettingsSetInput(int* padKeys);

#endif
