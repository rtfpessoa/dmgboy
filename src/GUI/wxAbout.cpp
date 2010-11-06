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

#include <wx/wx.h>
#include <wx/hyperlink.h>   // hyperlink support
#include "wxAbout.h"

#include "gb64.xpm"          // app icon bitmap


const wxString APP_NAME = wxT("gbpablog");
const wxString APP_VENDOR = wxT("wyoGuide");
const wxString APP_VERSION = wxT("0.7");
const wxString APP_MAINT = wxT("Pablo Gasco");
const wxString APP_LICENCE = wxT("GPL v3");
const wxString APP_COPYRIGTH = wxT("(C) 2010 Pablo Gasco");
const wxString APP_WEBSITE = wxT("http://code.google.com/p/gbpablog/");


AboutDialog::AboutDialog (wxWindow *parent)
: wxDialog (parent, -1, _("About ..."),
			wxDefaultPosition, wxDefaultSize,
			wxDEFAULT_DIALOG_STYLE) {

    // sets the application icon
    SetTitle (_("About ..."));

    // about info
    wxGridSizer *aboutinfo = new wxGridSizer (2, 3, 3);
    aboutinfo->Add (new wxStaticText(this, -1, _("Version: ")));
    aboutinfo->Add (new wxStaticText(this, -1, APP_VERSION));
    aboutinfo->Add (new wxStaticText(this, -1, _("Written by: ")));
    aboutinfo->Add (new wxStaticText(this, -1, APP_MAINT));
    aboutinfo->Add (new wxStaticText(this, -1, _("Licence type: ")));
    aboutinfo->Add (new wxStaticText(this, -1, APP_LICENCE));
    aboutinfo->Add (new wxStaticText(this, -1, _("wxWidgets: ")));
    aboutinfo->Add (new wxStaticText(this, -1, wxVERSION_STRING));

    // about icontitle//info
    wxBoxSizer *aboutpane = new wxBoxSizer (wxHORIZONTAL);
    aboutpane->Add (new wxStaticBitmap (this, -1, wxBitmap (gb64_xpm)),
                    0, wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 40);
    aboutpane->Add (aboutinfo, 1, wxEXPAND);
    aboutpane->Add (60, 0);

    // about complete
    wxBoxSizer *totalpane = new wxBoxSizer (wxVERTICAL);
    totalpane->Add (0, 20);
    wxStaticText *appname = new wxStaticText (this, -1, APP_NAME);
    appname->SetFont (wxFont (24, wxDEFAULT, wxNORMAL, wxBOLD));
    totalpane->Add (appname, 0, wxALIGN_CENTER | wxLEFT | wxRIGHT, 40);
    totalpane->Add (0, 10);
    totalpane->Add (aboutpane, 0, wxALIGN_CENTER | wxLEFT | wxRIGHT | wxBOTTOM, 10);
    wxHyperlinkCtrl *website = new wxHyperlinkCtrl (this, wxID_ANY, APP_WEBSITE, APP_WEBSITE);
    totalpane->Add (website, 0, wxALIGN_CENTER | wxLEFT | wxRIGHT | wxBOTTOM, 10);

    wxButton *okButton = new wxButton (this, wxID_OK, _("OK"));
    okButton->SetDefault();
    totalpane->Add (okButton, 0, wxALIGN_CENTER | wxALL, 10);

    SetSizerAndFit (totalpane);

    CentreOnParent();
    ShowModal();
}
