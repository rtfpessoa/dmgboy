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

#include <string>
#include <wx/wfstream.h>
#include <wx/zipstrm.h>
#include <wx/stdpaths.h>
#include <wx/msgdlg.h>
#include "../Pad.h"
#include "../Sound.h"
#include "../Video.h"
#include "../CPU.h"
#include "Settings.h"
#include "EmulationThread.h"

using namespace std;

EmulationThread::EmulationThread()
{
    mutex = new wxMutex();

	sound = new Sound();
    video = new Video(NULL);
	cpu = new CPU(video, sound);
	cartridge = NULL;

    ApplySettings();

    SetState(NotStartedYet);
}

EmulationThread::~EmulationThread() {
    emuState = Stopped;
	delete cpu;
	delete video;
	delete sound;
	if (cartridge)
		delete cartridge;
}

void EmulationThread::SetState(enumEmuStates state)
{
    wxMutexLocker lock(*mutex);

    this->emuState = state;

    if (state == Playing)
        sound->SetEnabled(SettingsGetSoundEnabled());
    else
        sound->SetEnabled(false);

    if (state == Stopped)
    {
#ifdef MAKEGBLOG
        cpu->SaveLog();
#endif
        cpu->Reset();
    }
}

enumEmuStates EmulationThread::GetState()
{
    return this->emuState;
}

wxThread::ExitCode EmulationThread::Entry()
{
#ifdef INSTPROFILE
    static const int TIME_SECOND = 1000000;
    int count = 0;
    wxLongLong timeSpent = 0;
#endif

    while (!TestDestroy())
    {
	    {
            wxMutexLocker lock(*mutex);
            if (emuState == Playing) {
#ifdef INSTPROFILE
                swFrame.Start();
#endif

                cpu->ExecuteOneFrame();

#ifdef INSTPROFILE
                timeSpent += swFrame.TimeInMicro();
                count++;
#endif
            }
        } // Desbloquear el mutex

#ifdef INSTPROFILE
        if (timeSpent > TIME_SECOND) {
            std::cout << "FPS: " << count << std::endl;
            timeSpent = 0;
            count = 0;
        }
#endif
    }

    return 0;
}

bool EmulationThread::ChangeFile(wxString fileName)
{
    // Meter dentro de las llaves para que se desbloquee el mutex antes de
    // llamar a SetState
    {
        wxMutexLocker lock(*mutex);

        BYTE * buffer = NULL;
        unsigned long size = 0;
        bool isZip = false;

        if (!wxFileExists(fileName))
        {
            wxMessageBox(_("The file:\n")+fileName+_("\ndoesn't exist"), _("Error"));
            return false;
        }

        wxString fileLower = fileName.Lower();
        if (fileLower.EndsWith(wxT(".zip")))
        {
            isZip = true;
            this->LoadZip(fileName, &buffer, &size);
            if ((buffer == NULL) || (size == 0))
                return false;
        }
        else if (!fileLower.EndsWith(wxT(".gb")) && !fileLower.EndsWith(wxT(".gbc")))
        {
            wxMessageBox(_("Only gb, gbc and zip files allowed!"), _("Error"));
            return false;
        }


        // Si ha llegado aquí es que es un archivo permitido
        if (cartridge)
            delete cartridge;

        wxString battsDir = wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator()
        + wxT("Batts");

        if (!wxFileName::DirExists(battsDir))
            wxFileName::Mkdir(battsDir, 0777, wxPATH_MKDIR_FULL);

        battsDir += wxFileName::GetPathSeparator();

        if (isZip) {
            cartridge = new Cartridge(buffer, size, string(battsDir.mb_str()));
        }else {
            cartridge = new Cartridge(string(fileName.mb_str()), string(battsDir.mb_str()));
        }


        cpu->LoadCartridge(cartridge);
        cpu->Reset();
    }


	SetState(Playing);

    return true;
}

/*
 * Carga un fichero comprimido con zip y busca una rom de gameboy (un fichero con extension gb o gbc).
 * Si existe mas de una rom solo carga la primera. Si se ha encontrado, la rom se devuelve en un buffer
 * junto con su tamaño, sino las variables se dejan intactas
 */
void EmulationThread::LoadZip(const wxString zipPath, BYTE ** buffer, unsigned long * size)
{
	wxString fileInZip, fileLower;
	wxZipEntry* entry;
	wxFFileInputStream in(zipPath);
	wxZipInputStream zip(in);
	while ((entry = zip.GetNextEntry()))
	{
		fileInZip = entry->GetName();

		fileLower = fileInZip.Lower();
		if (fileLower.EndsWith(wxT(".gb")) || fileLower.EndsWith(wxT(".gbc")))
		{
			*size = zip.GetSize();
			*buffer = new BYTE[*size];
			zip.Read(*buffer, *size);
			delete entry;
			return;
		}
		else
		{
			delete entry;
			continue;
		}
	}

	// Archivo no encontrado
	wxMessageBox(_("GameBoy rom not found in the file:\n")+zipPath, _("Error"));
	return;
}

void EmulationThread::LoadState(std::string fileName, int id)
{
    wxMutexLocker lock(*mutex);

    cpu->LoadState(fileName, id);
}

void EmulationThread::SaveState(std::string fileName, int id)
{
    wxMutexLocker lock(*mutex);

    cpu->SaveState(fileName, id);
}

void EmulationThread::ApplySettings()
{
    wxMutexLocker lock(*mutex);

    PadSetKeys(SettingsGetInput());
    sound->ChangeSampleRate(SettingsGetSoundSampleRate());
    sound->SetEnabled(SettingsGetSoundEnabled());
}

void EmulationThread::SetScreen(IGBScreenDrawable * screen)
{
    wxMutexLocker lock(*mutex);
    
    video->SetScreen(screen);
}

void EmulationThread::UpdatePad()
{
    //wxMutexLocker lock(*mutex);
    if (emuState == Playing)
    {
        cpu->UpdatePad();
    }
}
