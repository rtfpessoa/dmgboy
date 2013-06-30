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

#include <iostream>
#include <wx/cmdline.h>
#include <wx/filename.h>
#include <wx/log.h>
#ifdef __WXMSW__
#include <SDL.h>
#endif
#include "MainApp.h"
#include "MainFrame.h"
#include "SettingsDialog.h"
#include "../Settings.h"
#include "../Def.h"

IMPLEMENT_APP(MainApp)

static const wxCmdLineEntryDesc g_cmdLineDesc[] =
{
	{ wxCMD_LINE_SWITCH, "h", "help", "displays this help", wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_HELP },
	{ wxCMD_LINE_SWITCH, "v", "version", "print version", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_PARAM, NULL, NULL, "input file", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_NONE, NULL, NULL, NULL, wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL }
};

void MainApp::ChangeLanguage(long language)
{
    wxLocale* locale;
    
    // load language if possible, fall back to english otherwise
    if(wxLocale::IsAvailable(language))
    {
        locale = new wxLocale(language);
        
#ifdef __WXGTK__
        // add locale search paths
        locale->AddCatalogLookupPathPrefix(wxT("/usr"));
        locale->AddCatalogLookupPathPrefix(wxT("/usr/local"));
        wxStandardPaths* paths = (wxStandardPaths*) &wxStandardPaths::Get();
        wxString prefix = paths->GetInstallPrefix();
        locale->AddCatalogLookupPathPrefix( prefix );
#endif
        
        locale->AddCatalog(APP_NAME);
        
        if(! locale->IsOk() )
        {
            std::cerr << "selected language is wrong" << std::endl;
            delete locale;
            locale = new wxLocale( wxLANGUAGE_ENGLISH );
            language = wxLANGUAGE_ENGLISH;
        }
    }
    else
    {
        std::cout << "The selected language is not supported by your system."
        << "Try installing support for this language." << std::endl;
        locale = new wxLocale( wxLANGUAGE_ENGLISH );
        language = wxLANGUAGE_ENGLISH;
    }
    
}

bool MainApp::OnInit() {
    Settings settings = SettingsDialog::LoadFromFile();
	SettingsSetNewValues(settings);
    
    ChangeLanguage(settings.language);
    
	wxString cmdFilename = wxT("");
	wxCmdLineParser cmdParser(g_cmdLineDesc, argc, argv);
	int res;
	{
		wxLogNull log;
		// Pass false to suppress auto Usage() message
		res = cmdParser.Parse(false);
	}

	// Check if the user asked for command-line help
	if (res == -1 || res > 0 || cmdParser.Found(wxT("h")))
	{
		cmdParser.Usage();
		return false;
	}

	// Check if the user asked for the version
	if (cmdParser.Found(wxT("v")))
	{
#ifndef __WXMSW__
		wxLog::SetActiveTarget(new wxLogStderr);
#endif
		wxString msg = wxT("");
		msg << wxT(APP_NAME) << wxT(" ") << wxT(APP_VERSION);

		wxLogMessage(wxT("%s"), msg.c_str());
		return false;
	}

	// Check for a filename
	if (cmdParser.GetParamCount() > 0)
	{
		cmdFilename = cmdParser.GetParam(0);

		// Under Windows when invoking via a document
		// in Explorer, we are passed the short form.
		// So normalize and make the long form.
		wxFileName fName(cmdFilename);
		fName.Normalize(wxPATH_NORM_LONG|wxPATH_NORM_DOTS|wxPATH_NORM_TILDE|wxPATH_NORM_ABSOLUTE);
		cmdFilename = fName.GetFullPath();
	}

    // create the MainFrame
    frame = new MainFrame(cmdFilename);
    frame->Centre();
    frame->Show(true);

    // Our MainFrame is the Top Window
    SetTopWindow(frame);

    // initialization should always succeed
    return true;
}

int MainApp::OnRun() {
#ifdef __WXMSW__
    // initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "unable to init SDL: " << SDL_GetError() << '\n';

        return -1;
    }
#endif
    // start the main loop
    return wxApp::OnRun();
}

int MainApp::OnExit() {
#ifdef __WXMSW__
    SDL_Quit();
#endif

    // return the standard exit code
    return wxApp::OnExit();
}

void MainApp::MacOpenFile(const wxString &fileName)
{
	frame->ChangeFile(fileName);
}
