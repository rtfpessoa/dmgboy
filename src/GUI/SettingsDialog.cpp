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
#include "SettingsDialog.h"
#include "IDControls.h"
#include "InputTextCtrl.h"
#include "../Pad.h"
#include "Xpm/preferences1.xpm"


IMPLEMENT_CLASS(SettingsDialog, wxPropertySheetDialog)

BEGIN_EVENT_TABLE(SettingsDialog, wxPropertySheetDialog)
END_EVENT_TABLE()

SettingsDialog::SettingsDialog(wxWindow* win)
{
	settings = SettingsGetCopy();
	
	m_imageList = NULL;
	
    bool useToolBook = false;

	int sheetStyle = wxPROPSHEET_SHRINKTOFIT;
	
    if (useToolBook)
    {
        sheetStyle |= wxPROPSHEET_BUTTONTOOLBOOK;

        //SetSheetInnerBorder(0);
        //SetSheetOuterBorder(0);

		wxBitmap preferences1(preferences1_xpm);

        // create a dummy image list with a few icons
        const wxSize imageSize(32, 32);

        m_imageList = new wxImageList(imageSize.GetWidth(), imageSize.GetHeight());
        m_imageList->Add(preferences1);
        m_imageList->Add(wxArtProvider::GetIcon(wxART_QUESTION, wxART_OTHER, imageSize));
    }
	
	SetSheetStyle(sheetStyle);
	
	Create(win, wxID_ANY, _("Preferences"));
	
	wxBookCtrlBase* notebook = GetBookCtrl();
	
	if (useToolBook)
		notebook->SetImageList(m_imageList);

	CreateButtons(wxOK | wxCANCEL);

    wxPanel* generalSettings = CreateGeneralSettingsPage(notebook);
	wxPanel* soundSettings = CreateSoundSettingsPage(notebook);
	wxPanel* inputSettings = CreateInputSettingsPage(notebook);

	notebook->AddPage(generalSettings, _("General"), true);
	notebook->AddPage(soundSettings, _("Sound"), false);
    notebook->AddPage(inputSettings, _("Input"), false);

    LayoutDialog();
}

SettingsDialog::~SettingsDialog()
{
	if (m_imageList)
		delete m_imageList;
}

/*! * Transfer data to the window */
bool SettingsDialog::TransferDataToWindow()
{
	settings = SettingsGetCopy();
	
	wxRadioBox* greenscaleCtrl = (wxRadioBox*) FindWindow(ID_GREENSCALE);
	wxChoice* winZoomCtrl = (wxChoice*) FindWindow(ID_WINZOOM);
	InputTextCtrl* upCtrl = (InputTextCtrl*) FindWindow(ID_TEXTCTRL_UP);
	InputTextCtrl* downCtrl = (InputTextCtrl*) FindWindow(ID_TEXTCTRL_DOWN);
	InputTextCtrl* leftCtrl = (InputTextCtrl*) FindWindow(ID_TEXTCTRL_LEFT);
	InputTextCtrl* rightCtrl = (InputTextCtrl*) FindWindow(ID_TEXTCTRL_RIGHT);
	InputTextCtrl* aCtrl = (InputTextCtrl*) FindWindow(ID_TEXTCTRL_A);
	InputTextCtrl* bCtrl = (InputTextCtrl*) FindWindow(ID_TEXTCTRL_B);
	InputTextCtrl* selectCtrl = (InputTextCtrl*) FindWindow(ID_TEXTCTRL_SELECT);
	InputTextCtrl* startCtrl = (InputTextCtrl*) FindWindow(ID_TEXTCTRL_START);

	greenscaleCtrl->SetSelection(settings.greenScale);
	winZoomCtrl->SetSelection(settings.windowZoom - 1);
	
	upCtrl->OnChangeKey(	settings.padKeys[0]);
	downCtrl->OnChangeKey(	settings.padKeys[1]);
	leftCtrl->OnChangeKey(	settings.padKeys[2]);
	rightCtrl->OnChangeKey(	settings.padKeys[3]);
	aCtrl->OnChangeKey(		settings.padKeys[4]);
	bCtrl->OnChangeKey(		settings.padKeys[5]);
	selectCtrl->OnChangeKey(settings.padKeys[6]);
	startCtrl->OnChangeKey(	settings.padKeys[7]);

	return true;
}

/*! * Transfer data from the window */
bool SettingsDialog::TransferDataFromWindow()
{
	wxRadioBox* greenscaleCtrl = (wxRadioBox*) FindWindow(ID_GREENSCALE);
	wxChoice* winZoomCtrl = (wxChoice*) FindWindow(ID_WINZOOM);
	InputTextCtrl* upCtrl = (InputTextCtrl*) FindWindow(ID_TEXTCTRL_UP);
	InputTextCtrl* downCtrl = (InputTextCtrl*) FindWindow(ID_TEXTCTRL_DOWN);
	InputTextCtrl* leftCtrl = (InputTextCtrl*) FindWindow(ID_TEXTCTRL_LEFT);
	InputTextCtrl* rightCtrl = (InputTextCtrl*) FindWindow(ID_TEXTCTRL_RIGHT);
	InputTextCtrl* aCtrl = (InputTextCtrl*) FindWindow(ID_TEXTCTRL_A);
	InputTextCtrl* bCtrl = (InputTextCtrl*) FindWindow(ID_TEXTCTRL_B);
	InputTextCtrl* selectCtrl = (InputTextCtrl*) FindWindow(ID_TEXTCTRL_SELECT);
	InputTextCtrl* startCtrl = (InputTextCtrl*) FindWindow(ID_TEXTCTRL_START);

	settings.greenScale = greenscaleCtrl->GetSelection();
	settings.windowZoom = winZoomCtrl->GetSelection()+1;

	settings.padKeys[0] = upCtrl->keyCode;
	settings.padKeys[1] = downCtrl->keyCode;
	settings.padKeys[2] = leftCtrl->keyCode;
	settings.padKeys[3] = rightCtrl->keyCode;
	settings.padKeys[4] = aCtrl->keyCode;
	settings.padKeys[5] = bCtrl->keyCode;
	settings.padKeys[6] = selectCtrl->keyCode;
	settings.padKeys[7] = startCtrl->keyCode;
	
	SaveToFile();

	return true;
}

wxPanel* SettingsDialog::CreateGeneralSettingsPage(wxWindow* parent)
{
    wxPanel* panel = new wxPanel(parent, wxID_ANY);

	wxStaticText * grayGreenLabel = new wxStaticText(panel, wxID_ANY, wxT("Color palette:"));
	wxBoxSizer * grayGreenSizer = new wxBoxSizer(wxVERTICAL);
	grayGreenSizer->Add(grayGreenLabel, 0, wxUP, 7);
	wxString grayGreenChoices[2];
    grayGreenChoices[0] = wxT("Grayscale");
    grayGreenChoices[1] = wxT("Greenscale");
    wxRadioBox* grayGreenRadioBox = new wxRadioBox(panel, ID_GREENSCALE, wxT(""),
												 wxDefaultPosition, wxDefaultSize, 2, grayGreenChoices, 1, wxRA_SPECIFY_COLS);

	wxStaticText * winZoomLabel = new wxStaticText(panel, wxID_ANY, wxT("Window size:"));
    wxChoice* winZoomChoice = new wxChoice(panel, ID_WINZOOM);
	winZoomChoice->Append(wxT("1x"));
	winZoomChoice->Append(wxT("2x"));
	winZoomChoice->Append(wxT("3x"));
	winZoomChoice->Append(wxT("4x"));

	wxFlexGridSizer *grid = new wxFlexGridSizer(2, 3, 5);
	grid->Add(grayGreenSizer);
	grid->Add(grayGreenRadioBox);
	grid->Add(winZoomLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
	grid->Add(winZoomChoice);
	
	wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
	topSizer->Add(grid, 0, wxALL, 10);
	
    panel->SetSizerAndFit(topSizer);

    return panel;
}

wxPanel* SettingsDialog::CreateSoundSettingsPage(wxWindow* parent)
{
    wxPanel* panel = new wxPanel(parent, wxID_ANY);
	
	wxStaticText * enabledLabel = new wxStaticText(panel, wxID_ANY, wxT("Enabled:"));
	wxCheckBox * enabledCheckBox = new wxCheckBox(panel, wxID_ANY, wxT(""));
	enabledCheckBox->SetValue(true);
	
	wxStaticText * sampleRateLabel = new wxStaticText(panel, wxID_ANY, wxT("Sample Rate:"));
    wxChoice* choiceSampleRate = new wxChoice(panel, wxID_ANY);
	choiceSampleRate->Append(wxT("22050 Hz"));
	choiceSampleRate->Append(wxT("32000 Hz"));
	choiceSampleRate->Append(wxT("44100 Hz"));
	choiceSampleRate->Append(wxT("48000 Hz"));
	choiceSampleRate->SetSelection(0);
	
	wxFlexGridSizer *grid = new wxFlexGridSizer(2, 3, 5);
	
	grid->Add(enabledLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
	grid->Add(enabledCheckBox);
	grid->Add(sampleRateLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
	grid->Add(choiceSampleRate);
	
	wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
	topSizer->Add(grid, 0, wxALL, 10);
	
    panel->SetSizerAndFit(topSizer);
	
    return panel;
}

wxPanel* SettingsDialog::CreateInputSettingsPage(wxWindow* parent)
{
	wxPanel* panel = new wxPanel(parent, wxID_ANY);
	
	
    wxStaticText * upLabel = new wxStaticText(panel, wxID_ANY, wxT("Up:"));
	InputTextCtrl * upTextCtrl = new InputTextCtrl(panel, ID_TEXTCTRL_UP);
	
	wxStaticText * downLabel = new wxStaticText(panel, wxID_ANY, wxT("Down:"));
	InputTextCtrl * downTextCtrl = new InputTextCtrl(panel, ID_TEXTCTRL_DOWN);
	
	wxStaticText * leftLabel = new wxStaticText(panel, wxID_ANY, wxT("Left:"));
	InputTextCtrl * leftTextCtrl = new InputTextCtrl(panel, ID_TEXTCTRL_LEFT);
	
	wxStaticText * rightLabel = new wxStaticText(panel, wxID_ANY, wxT("Right:"));
	InputTextCtrl * rightTextCtrl = new InputTextCtrl(panel, ID_TEXTCTRL_RIGHT);
	
	wxStaticText * aLabel = new wxStaticText(panel, wxID_ANY, wxT("A:"));
	InputTextCtrl * aTextCtrl = new InputTextCtrl(panel, ID_TEXTCTRL_A);
	
	wxStaticText * bLabel = new wxStaticText(panel, wxID_ANY, wxT("B:"));
	InputTextCtrl * bTextCtrl = new InputTextCtrl(panel, ID_TEXTCTRL_B);
	
	wxStaticText * selectLabel = new wxStaticText(panel, wxID_ANY, wxT("Select:"));
	InputTextCtrl * selectTextCtrl = new InputTextCtrl(panel, ID_TEXTCTRL_SELECT);
	
	wxStaticText * startLabel = new wxStaticText(panel, wxID_ANY, wxT("Start:"));
	InputTextCtrl * startTextCtrl = new InputTextCtrl(panel, ID_TEXTCTRL_START);
	
	wxFlexGridSizer *grid = new wxFlexGridSizer(2, 3, 5);
	grid->Add(upLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
	grid->Add(upTextCtrl);
	grid->Add(downLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
	grid->Add(downTextCtrl);
	grid->Add(leftLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
	grid->Add(leftTextCtrl);
	grid->Add(rightLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
	grid->Add(rightTextCtrl);
	grid->Add(aLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
	grid->Add(aTextCtrl);
	grid->Add(bLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
	grid->Add(bTextCtrl);
	grid->Add(selectLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
	grid->Add(selectTextCtrl);
	grid->Add(startLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
	grid->Add(startTextCtrl);
	
	wxBoxSizer * widthSizer = new wxBoxSizer(wxVERTICAL);
	widthSizer->Add(grid, 0, wxRIGHT|wxLEFT, 20);
	
	wxBoxSizer * topSizer = new wxBoxSizer(wxVERTICAL);
	topSizer->Add(widthSizer, 0, wxALL, 10);
	
	panel->SetSizerAndFit(topSizer);
	
    return panel;
}

void SettingsDialog::SaveToFile(bool reloadSettings)
{
	if (reloadSettings)
		settings = SettingsGetCopy();
		
	wxString configDir = wxStandardPaths::Get().GetUserDataDir();

	if (!wxFileName::DirExists(configDir))
		wxFileName::Mkdir(configDir, 0777, wxPATH_MKDIR_FULL);

	wxFileName configPath(configDir, wxT("config.ini"));

	// Guardar a disco
	wxFileConfig fileConfig(wxT("gbpablog"), wxT("pablogasco"), configPath.GetFullPath());

	fileConfig.Write(wxT("General/greenScale"), settings.greenScale);
	fileConfig.Write(wxT("General/windowZoom"), settings.windowZoom);
	
	fileConfig.Write(wxT("Input/up"), settings.padKeys[0]);
	fileConfig.Write(wxT("Input/down"), settings.padKeys[1]);
	fileConfig.Write(wxT("Input/left"), settings.padKeys[2]);
	fileConfig.Write(wxT("Input/right"), settings.padKeys[3]);
	fileConfig.Write(wxT("Input/a"), settings.padKeys[4]);
	fileConfig.Write(wxT("Input/b"), settings.padKeys[5]);
	fileConfig.Write(wxT("Input/select"), settings.padKeys[6]);
	fileConfig.Write(wxT("Input/start"), settings.padKeys[7]);
	
	wxString auxString[10];
	for (int i=0; i<10; i++)
	{
		auxString[i] = wxString(settings.recentRoms[i].c_str(), wxConvUTF8);
	}
	
	fileConfig.Write(wxT("RecentRoms/01"), auxString[0]);
	fileConfig.Write(wxT("RecentRoms/02"), auxString[1]);
	fileConfig.Write(wxT("RecentRoms/03"), auxString[2]);
	fileConfig.Write(wxT("RecentRoms/04"), auxString[3]);
	fileConfig.Write(wxT("RecentRoms/05"), auxString[4]);
	fileConfig.Write(wxT("RecentRoms/06"), auxString[5]);
	fileConfig.Write(wxT("RecentRoms/07"), auxString[6]);
	fileConfig.Write(wxT("RecentRoms/08"), auxString[7]);
	fileConfig.Write(wxT("RecentRoms/09"), auxString[8]);
	fileConfig.Write(wxT("RecentRoms/10"), auxString[9]);
}

void SettingsDialog::LoadFromFile()
{
	wxString configDir = wxStandardPaths::Get().GetUserDataDir();
	wxFileName configPath(configDir, wxT("config.ini"));
	// Cargar de disco
	wxFileConfig fileConfig(wxT("gbpablog"), wxT("pablogasco"), configPath.GetFullPath());

	fileConfig.Read(wxT("General/greenScale"), &settings.greenScale);
	fileConfig.Read(wxT("General/windowZoom"), &settings.windowZoom);
	
	fileConfig.Read(wxT("Input/up"), &settings.padKeys[0]);
	fileConfig.Read(wxT("Input/down"), &settings.padKeys[1]);
	fileConfig.Read(wxT("Input/left"), &settings.padKeys[2]);
	fileConfig.Read(wxT("Input/right"), &settings.padKeys[3]);
	fileConfig.Read(wxT("Input/a"), &settings.padKeys[4]);
	fileConfig.Read(wxT("Input/b"), &settings.padKeys[5]);
	fileConfig.Read(wxT("Input/select"), &settings.padKeys[6]);
	fileConfig.Read(wxT("Input/start"), &settings.padKeys[7]);
	
	wxString auxString[10];
	fileConfig.Read(wxT("RecentRoms/01"), &auxString[0]);
	fileConfig.Read(wxT("RecentRoms/02"), &auxString[1]);
	fileConfig.Read(wxT("RecentRoms/03"), &auxString[2]);
	fileConfig.Read(wxT("RecentRoms/04"), &auxString[3]);
	fileConfig.Read(wxT("RecentRoms/05"), &auxString[4]);
	fileConfig.Read(wxT("RecentRoms/06"), &auxString[5]);
	fileConfig.Read(wxT("RecentRoms/07"), &auxString[6]);
	fileConfig.Read(wxT("RecentRoms/08"), &auxString[7]);
	fileConfig.Read(wxT("RecentRoms/09"), &auxString[8]);
	fileConfig.Read(wxT("RecentRoms/10"), &auxString[9]);
	
	for (int i=0; i<10; i++)
	{
		settings.recentRoms[i] = auxString[i].mb_str();
	}
}
