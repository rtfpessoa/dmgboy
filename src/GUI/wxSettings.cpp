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

#include <wx/artprov.h>
#include <wx/bookctrl.h>
#include <wx/spinctrl.h>
#include <wx/config.h>
#include <wx/fileconf.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include "wxSettings.h"
#include "wxIDControls.h"
#include "Pad.h"
#include "Xpm/preferences1.xpm"


IMPLEMENT_CLASS(SettingsDialog, wxPropertySheetDialog)

BEGIN_EVENT_TABLE(SettingsDialog, wxPropertySheetDialog)
EVT_TEXT(ID_TEXTCTRL_UP, SettingsDialog::redefineKey)
END_EVENT_TABLE()

SettingsDialog::SettingsDialog(wxWindow* win)
{
	settings = SettingsGetCopy();

    SetExtraStyle(wxDIALOG_EX_CONTEXTHELP|wxWS_EX_VALIDATE_RECURSIVELY);

    bool useToolBook = true;

    if (useToolBook)
    {
        int sheetStyle = wxPROPSHEET_SHRINKTOFIT | wxPROPSHEET_BUTTONTOOLBOOK;

        SetSheetStyle(sheetStyle);
        //SetSheetInnerBorder(0);
        //SetSheetOuterBorder(0);

		wxBitmap preferences1(preferences1_xpm);

        // create a dummy image list with a few icons
        const wxSize imageSize(32, 32);

        m_imageList = new wxImageList(imageSize.GetWidth(), imageSize.GetHeight());
        m_imageList->Add(preferences1);
        m_imageList->
		Add(wxArtProvider::GetIcon(wxART_QUESTION, wxART_OTHER, imageSize));
        m_imageList->
		Add(wxArtProvider::GetIcon(wxART_WARNING, wxART_OTHER, imageSize));
        m_imageList->
		Add(wxArtProvider::GetIcon(wxART_ERROR, wxART_OTHER, imageSize));
    }

    Create(win, wxID_ANY, _("Preferences"), wxDefaultPosition, wxDefaultSize);

	CreateButtons(wxOK | wxCANCEL);

    wxBookCtrlBase* notebook = GetBookCtrl();
    notebook->SetImageList(m_imageList);

    wxPanel* generalSettings = CreateGeneralSettingsPage(notebook);
	wxPanel* padSettings = CreatePadSettingsPage(notebook);
    //wxPanel* aestheticSettings = CreateAestheticSettingsPage(notebook);

	notebook->AddPage(generalSettings, _("General"), true, 0);
    notebook->AddPage(padSettings, _("Pad"), false, 1);
    //notebook->AddPage(aestheticSettings, _("Aesthetics"), false, 2);

    LayoutDialog();
}

SettingsDialog::~SettingsDialog()
{
    delete m_imageList;
}

void SettingsDialog::redefineKey(wxCommandEvent &commandEvent)
{
	wxMessageBox(wxT("Key pressed"), wxT("Key"));
}

/*! * Transfer data to the window */
bool SettingsDialog::TransferDataToWindow()
{
	wxRadioBox* greenscaleCtrl = (wxRadioBox*) FindWindow(ID_GREENSCALE);
	wxChoice* winZoomCtrl = (wxChoice*) FindWindow(ID_WINZOOM);

	greenscaleCtrl->SetSelection(settings.greenScale);
	winZoomCtrl->SetSelection(settings.windowZoom - 1);

	return true;
}

/*! * Transfer data from the window */
bool SettingsDialog::TransferDataFromWindow()
{
	wxRadioBox* greenscaleCtrl = (wxRadioBox*) FindWindow(ID_GREENSCALE);
	wxChoice* winZoomCtrl = (wxChoice*) FindWindow(ID_WINZOOM);

	settings.greenScale = greenscaleCtrl->GetSelection();
	settings.windowZoom = winZoomCtrl->GetSelection()+1;

	SaveToFile();

	return true;
}

wxPanel* SettingsDialog::CreateGeneralSettingsPage(wxWindow* parent)
{
    wxPanel* panel = new wxPanel(parent, wxID_ANY);
	
	wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );

	// Paleta de colores (verde o gris)
	wxString grayOrGreenChoices[2];
    grayOrGreenChoices[0] = wxT("Grayscale");
    grayOrGreenChoices[1] = wxT("Greenscale");

    wxRadioBox* grayOrGreen = new wxRadioBox(panel, ID_GREENSCALE, wxT("Color palette:"),
												 wxDefaultPosition, wxDefaultSize, 2, grayOrGreenChoices, 1, wxRA_SPECIFY_COLS);

	// Tamaño de ventana
	wxArrayString winZoomChoices;
    winZoomChoices.Add(wxT("1x"));
    winZoomChoices.Add(wxT("2x"));
	winZoomChoices.Add(wxT("3x"));
	winZoomChoices.Add(wxT("4x"));

    wxBoxSizer* winZoomSizer = new wxBoxSizer( wxHORIZONTAL );

    wxChoice* choiceWinZoom = new wxChoice(panel, ID_WINZOOM, wxDefaultPosition, wxDefaultSize, winZoomChoices);

    winZoomSizer->Add(new wxStaticText(panel, wxID_ANY, wxT("Window size:")), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    winZoomSizer->Add(5, 5, 1, wxALL, 0);
    winZoomSizer->Add(choiceWinZoom, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);

    topSizer->Add(grayOrGreen, 0, wxGROW|wxALL, 0);
	topSizer->Add(winZoomSizer, 0, wxGROW|wxALL, 0);

    panel->SetSizerAndFit(topSizer);

    return panel;
}

wxPanel* SettingsDialog::CreatePadSettingsPage(wxWindow* parent)
{
    wxPanel* panel = new wxPanel(parent, wxID_ANY);
	
	wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer * upSizer = new wxBoxSizer( wxHORIZONTAL );
	wxStaticText * upLabel = new wxStaticText(panel, wxID_ANY, wxT("Up"));
	wxTextCtrl * upTextCtrl = new wxTextCtrl(panel, ID_TEXTCTRL_UP);
	
	upSizer->Add(upLabel, 0, wxGROW|wxALL, 0);
	upSizer->Add(upTextCtrl, 0, wxGROW|wxALL, 0);
	
	wxBoxSizer * downSizer = new wxBoxSizer( wxHORIZONTAL );
	wxStaticText * downLabel = new wxStaticText(panel, wxID_ANY, wxT("Down"));
	wxTextCtrl * downTextCtrl = new wxTextCtrl(panel, wxID_ANY);
	
	downSizer->Add(downLabel, 0, wxGROW|wxALL, 0);
	downSizer->Add(downTextCtrl, 0, wxGROW|wxALL, 0);
	
	wxBoxSizer * leftSizer = new wxBoxSizer( wxHORIZONTAL );
	wxStaticText * leftLabel = new wxStaticText(panel, wxID_ANY, wxT("Left"));
	wxTextCtrl * leftTextCtrl = new wxTextCtrl(panel, wxID_ANY);
	
	leftSizer->Add(leftLabel, 0, wxGROW|wxALL, 0);
	leftSizer->Add(leftTextCtrl, 0, wxGROW|wxALL, 0);
	
	wxBoxSizer * rightSizer = new wxBoxSizer( wxHORIZONTAL );
	wxStaticText * rightLabel = new wxStaticText(panel, wxID_ANY, wxT("Right"));
	wxTextCtrl * rightTextCtrl = new wxTextCtrl(panel, wxID_ANY);
	
	rightSizer->Add(rightLabel, 0, wxGROW|wxALL, 0);
	rightSizer->Add(rightTextCtrl, 0, wxGROW|wxALL, 0);
	
    topSizer->Add(upSizer, 0, wxGROW|wxALL, 0);
	topSizer->Add(downSizer, 0, wxGROW|wxALL, 0);
	topSizer->Add(leftSizer, 0, wxGROW|wxALL, 0);
	topSizer->Add(rightSizer, 0, wxGROW|wxALL, 0);
	
    panel->SetSizerAndFit(topSizer);
	
    return panel;
}

void SettingsDialog::SaveToFile()
{
	wxString configDir = wxStandardPaths::Get().GetUserDataDir();

	if (!wxFileName::DirExists(configDir))
		wxFileName::Mkdir(configDir, 0777, wxPATH_MKDIR_FULL);

	wxFileName configPath(configDir, wxT("config.ini"));

	// Guardar a disco
	wxFileConfig fileConfig(wxT("gbpablog"), wxT("pablogasco"), configPath.GetFullPath());

	fileConfig.Write(wxT("General/greenScale"), settings.greenScale);
	fileConfig.Write(wxT("General/windowZoom"), settings.windowZoom);
}

void SettingsDialog::LoadFromFile()
{
	wxString configDir = wxStandardPaths::Get().GetUserDataDir();
	wxFileName configPath(configDir, wxT("config.ini"));
	// Cargar de disco
	wxFileConfig fileConfig(wxT("gbpablog"), wxT("pablogasco"), configPath.GetFullPath());

	fileConfig.Read(wxT("General/greenScale"), &settings.greenScale);
	fileConfig.Read(wxT("General/windowZoom"), &settings.windowZoom);
}

wxPanel* SettingsDialog::CreateGeneralSettingsPage2(wxWindow* parent)
{
    wxPanel* panel = new wxPanel(parent, wxID_ANY);

    wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    //// LOAD LAST FILE

    wxBoxSizer* itemSizer3 = new wxBoxSizer( wxHORIZONTAL );
    wxCheckBox* checkBox3 = new wxCheckBox(panel, wxID_ANY, _("&Load last project on startup"), wxDefaultPosition, wxDefaultSize);
    itemSizer3->Add(checkBox3, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    item0->Add(itemSizer3, 0, wxGROW|wxALL, 0);

    //// AUTOSAVE

    wxString autoSaveLabel = _("&Auto-save every");
    wxString minsLabel = _("mins");

    wxBoxSizer* itemSizer12 = new wxBoxSizer( wxHORIZONTAL );
    wxCheckBox* checkBox12 = new wxCheckBox(panel, wxID_ANY, autoSaveLabel, wxDefaultPosition, wxDefaultSize);

#if wxUSE_SPINCTRL
    wxSpinCtrl* spinCtrl12 = new wxSpinCtrl(panel, wxID_ANY, wxEmptyString,
											wxDefaultPosition, wxSize(40, wxDefaultCoord), wxSP_ARROW_KEYS, 1, 60, 1);
#endif

    itemSizer12->Add(checkBox12, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
#if wxUSE_SPINCTRL
    itemSizer12->Add(spinCtrl12, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
#endif
    itemSizer12->Add(new wxStaticText(panel, wxID_STATIC, minsLabel), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    item0->Add(itemSizer12, 0, wxGROW|wxALL, 0);

    //// TOOLTIPS

    wxBoxSizer* itemSizer8 = new wxBoxSizer( wxHORIZONTAL );
    wxCheckBox* checkBox6 = new wxCheckBox(panel, wxID_ANY, _("Show &tooltips"), wxDefaultPosition, wxDefaultSize);
    itemSizer8->Add(checkBox6, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    item0->Add(itemSizer8, 0, wxGROW|wxALL, 0);

    topSizer->Add( item0, 1, wxGROW|wxALIGN_CENTRE|wxALL, 5 );

    panel->SetSizer(topSizer);
    topSizer->Fit(panel);

	wxColour color(wxSystemSettings::GetColour(wxSYS_COLOUR_BACKGROUND));
	panel->SetBackgroundColour(color);

    return panel;
}

wxPanel* SettingsDialog::CreateAestheticSettingsPage(wxWindow* parent)
{
    wxPanel* panel = new wxPanel(parent, wxID_ANY);

    wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    //// PROJECT OR GLOBAL
    wxString globalOrProjectChoices[2];
    globalOrProjectChoices[0] = _("&New projects");
    globalOrProjectChoices[1] = _("&This project");

    wxRadioBox* projectOrGlobal = new wxRadioBox(panel, wxID_ANY, _("&Apply settings to:"),
												 wxDefaultPosition, wxDefaultSize, 2, globalOrProjectChoices);
    item0->Add(projectOrGlobal, 0, wxGROW|wxALL, 5);

    projectOrGlobal->SetSelection(0);

    //// BACKGROUND STYLE
    wxArrayString backgroundStyleChoices;
    backgroundStyleChoices.Add(wxT("Colour"));
    backgroundStyleChoices.Add(wxT("Image"));
    wxStaticBox* staticBox3 = new wxStaticBox(panel, wxID_ANY, _("Background style:"));

    wxBoxSizer* styleSizer = new wxStaticBoxSizer( staticBox3, wxVERTICAL );
    item0->Add(styleSizer, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemSizer2 = new wxBoxSizer( wxHORIZONTAL );

    wxChoice* choice2 = new wxChoice(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, backgroundStyleChoices);

    itemSizer2->Add(new wxStaticText(panel, wxID_ANY, _("&Window:")), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    itemSizer2->Add(5, 5, 1, wxALL, 0);
    itemSizer2->Add(choice2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    styleSizer->Add(itemSizer2, 0, wxGROW|wxALL, 5);

#if wxUSE_SPINCTRL
    //// FONT SIZE SELECTION

    wxStaticBox* staticBox1 = new wxStaticBox(panel, wxID_ANY, _("Tile font size:"));
    wxBoxSizer* itemSizer5 = new wxStaticBoxSizer( staticBox1, wxHORIZONTAL );

    wxSpinCtrl* spinCtrl = new wxSpinCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition,
										  wxSize(80, wxDefaultCoord));
    itemSizer5->Add(spinCtrl, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    item0->Add(itemSizer5, 0, wxGROW|wxLEFT|wxRIGHT, 5);
#endif

    topSizer->Add( item0, 1, wxGROW|wxALIGN_CENTRE|wxALL, 5 );
    topSizer->AddSpacer(5);

    panel->SetSizer(topSizer);
    topSizer->Fit(panel);

    return panel;
}
