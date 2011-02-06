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

#include <iostream>
#include <wx/cmdline.h>
#include <wx/filename.h>
#include "MainApp.h"
#include "../Def.h"


IMPLEMENT_CLASS(MainApp, wxApp)
IMPLEMENT_APP(MainApp)

static const wxCmdLineEntryDesc g_cmdLineDesc[] =
{
	{ wxCMD_LINE_SWITCH, wxT("h"), wxT("help"), wxT("displays this help"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_HELP },
	{ wxCMD_LINE_SWITCH, wxT("v"), wxT("version"), wxT("print version"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_PARAM, NULL, NULL, wxT("input file"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{ wxCMD_LINE_NONE, NULL, NULL, NULL, wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL }
};

bool MainApp::OnInit() {

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
    frame->Show();
	
    // Our MainFrame is the Top Window
    SetTopWindow(frame);
	
    // initialization should always succeed
    return true;
}

int MainApp::OnRun() {
    // initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "unable to init SDL: " << SDL_GetError() << '\n';

        return -1;
    }
	
    // start the main loop
    return wxApp::OnRun();
}

int MainApp::OnExit() {
    SDL_Quit();
	
    // return the standard exit code
    return wxApp::OnExit();
}

void MainApp::MacOpenFile(const wxString &fileName)
{
	frame->ChangeFile(fileName);
}
