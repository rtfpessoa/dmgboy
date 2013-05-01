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

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/statbmp.h>
#include <wx/button.h>
#include <wx/hyperlink.h>   // hyperlink support
#ifdef __WXMSW__
#include "SDL.h"
#endif
#include "AboutDialog.h"
#include "Xpm/gb64.xpm"          // app icon bitmap
#include "../Def.h"


AboutDialog::AboutDialog (wxWindow *parent)
: wxDialog (parent, -1, _("About ..."),
			wxDefaultPosition, wxDefaultSize,
			wxDEFAULT_DIALOG_STYLE) {

    // sets the application icon
    SetTitle (_("About ..."));

#ifdef __WXMSW__
	const SDL_version* sdlVersion = SDL_Linked_Version();
	wxString stringSDLVersion;
	stringSDLVersion << (int)sdlVersion->major << wxT(".") << (int)sdlVersion->minor << wxT(".") << (int)sdlVersion->patch;
#endif

    // about info
    wxFlexGridSizer *aboutinfo = new wxFlexGridSizer (2, 3, 3);
    aboutinfo->Add (new wxStaticText(this, wxID_ANY, _("Version: ")));
    aboutinfo->Add (new wxStaticText(this, wxID_ANY, wxT(APP_VERSION)));
    aboutinfo->Add (new wxStaticText(this, wxID_ANY, _("Written by: ")));
    aboutinfo->Add (new wxStaticText(this, wxID_ANY, wxT(APP_MAINT)));
    aboutinfo->Add (new wxStaticText(this, wxID_ANY, _("Licence type: ")));
    aboutinfo->Add (new wxStaticText(this, wxID_ANY, wxT(APP_LICENCE)));
    aboutinfo->Add (new wxStaticText(this, wxID_ANY, _("wxWidgets: ")));
    aboutinfo->Add (new wxStaticText(this, wxID_ANY, wxVERSION_STRING));
#ifdef __WXMSW__
	aboutinfo->Add (new wxStaticText(this, wxID_ANY, _("SDL: ")));
    aboutinfo->Add (new wxStaticText(this, wxID_ANY, stringSDLVersion));
#endif
	aboutinfo->Add (new wxStaticText(this, wxID_ANY, _("Thanks to: ")));
	aboutinfo->Add (new wxStaticText(this, wxID_ANY, _("Authors of Pan Docs")));
	aboutinfo->Add (new wxStaticText(this, wxID_ANY, wxT("")));
	aboutinfo->Add (new wxStaticText(this, wxID_ANY, wxT("bcrew1375 (Miracle GB)")));
	aboutinfo->Add (new wxStaticText(this, wxID_ANY, wxT("")));
	aboutinfo->Add (new wxStaticText(this, wxID_ANY, wxT("AntonioND (GiiBii)")));
	aboutinfo->Add (new wxStaticText(this, wxID_ANY, wxT("")));
	aboutinfo->Add (new wxStaticText(this, wxID_ANY, wxT("Shay Green (Gb_Snd_Emu)")));

    // about icontitle//info
    wxBoxSizer *aboutpane = new wxBoxSizer (wxHORIZONTAL);
    aboutpane->Add (new wxStaticBitmap (this, wxID_ANY, wxBitmap (gb64_xpm)),
                    0, wxRIGHT, 10);
    aboutpane->Add (aboutinfo, 0, wxEXPAND);
    //aboutpane->Add (60, 0);

    // about complete
    wxBoxSizer *totalpane = new wxBoxSizer (wxVERTICAL);
    totalpane->Add (0, 20);
    wxStaticText *appname = new wxStaticText (this, wxID_ANY, wxT(APP_NAME));
    appname->SetFont (wxFont (24, wxDEFAULT, wxNORMAL, wxBOLD));
    totalpane->Add (appname, 0, wxALIGN_CENTER);
    totalpane->Add (0, 10);
    totalpane->Add (aboutpane, 0, wxALIGN_CENTER | wxRIGHT | wxLEFT, 20);
	totalpane->Add (0, 10);
    wxHyperlinkCtrl *website = new wxHyperlinkCtrl (this, wxID_ANY, wxT(APP_WEBSITE), wxT(APP_WEBSITE));
    totalpane->Add (website, 0, wxALIGN_CENTER);
	totalpane->Add (0, 10);

    wxButton *okButton = new wxButton (this, wxID_OK, _("OK"));
    okButton->SetDefault();
    totalpane->Add (okButton, 0, wxALIGN_CENTER | wxALL, 10);

    SetSizerAndFit (totalpane);

    CentreOnParent();
    ShowModal();
}
