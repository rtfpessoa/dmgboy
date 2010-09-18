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

Settings settings;

Settings::Settings()
{
	greenscale = false;
}

Settings SettingsGetCopy()
{
	return settings;
}

void SettingsSetNewValues(Settings newSettings)
{
	settings = newSettings;
}

bool SettingsGetGreenscale()
{
	return settings.greenscale;
}

void SettingsSetGreenScale(bool greenscale)
{
	settings.greenscale = greenscale;
}
