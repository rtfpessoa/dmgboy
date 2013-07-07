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

#include <wx/artprov.h>
#include <wx/bookctrl.h>
#include <wx/spinctrl.h>
#include <wx/radiobox.h>
#include <wx/choice.h>
#include <wx/checkbox.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/config.h>
#include "SettingsDialog.h"
#include "IDControls.h"
#include "InputTextCtrl.h"
#include "../Pad.h"


BEGIN_EVENT_TABLE(SettingsDialog, wxPropertySheetDialog)
END_EVENT_TABLE()

SettingsDialog::SettingsDialog(wxWindow* win)
{
	m_settings = SettingsGetCopy();

	SetSheetStyle(wxPROPSHEET_SHRINKTOFIT);
	
	Create(win, wxID_ANY, _("Preferences"));
	
	wxBookCtrlBase* notebook = GetBookCtrl();

	CreateButtons(wxOK | wxCANCEL);

    wxPanel* videoSettings = CreateVideoSettingsPage(notebook);
	wxPanel* soundSettings = CreateSoundSettingsPage(notebook);
	wxPanel* inputSettings = CreateInputSettingsPage(notebook);

	notebook->AddPage(videoSettings, _("Video"), true);
	notebook->AddPage(soundSettings, _("Sound"), false);
    notebook->AddPage(inputSettings, _("Input"), false);

    LayoutDialog();
}

SettingsDialog::~SettingsDialog()
{
    
}

/*! * Transfer data to the window */
bool SettingsDialog::TransferDataToWindow()
{
	m_settings = SettingsGetCopy();
	
    wxRadioBox* renderCtrl =     (wxRadioBox*)	  FindWindow(ID_RENDERMETHOD);
	wxRadioBox* greenscaleCtrl = (wxRadioBox*)	  FindWindow(ID_GREENSCALE);
	wxChoice* winZoomCtrl =		 (wxChoice*)	  FindWindow(ID_WINZOOM);
	wxCheckBox* soundEnabledCtrl = (wxCheckBox*)  FindWindow(ID_SOUND_ENABLED);
	wxChoice* soundSRCtrl =		 (wxChoice*)	  FindWindow(ID_SOUND_SR);
	InputTextCtrl* upCtrl =		 (InputTextCtrl*) FindWindow(ID_TEXTCTRL_UP);
	InputTextCtrl* downCtrl =	 (InputTextCtrl*) FindWindow(ID_TEXTCTRL_DOWN);
	InputTextCtrl* leftCtrl =	 (InputTextCtrl*) FindWindow(ID_TEXTCTRL_LEFT);
	InputTextCtrl* rightCtrl =	 (InputTextCtrl*) FindWindow(ID_TEXTCTRL_RIGHT);
	InputTextCtrl* aCtrl =		 (InputTextCtrl*) FindWindow(ID_TEXTCTRL_A);
	InputTextCtrl* bCtrl =		 (InputTextCtrl*) FindWindow(ID_TEXTCTRL_B);
	InputTextCtrl* selectCtrl =	 (InputTextCtrl*) FindWindow(ID_TEXTCTRL_SELECT);
	InputTextCtrl* startCtrl =	 (InputTextCtrl*) FindWindow(ID_TEXTCTRL_START);

    renderCtrl->SetSelection(m_settings.renderMethod);
	greenscaleCtrl->SetSelection(m_settings.greenScale);
	winZoomCtrl->SetSelection(m_settings.windowZoom - 1);
	
	soundEnabledCtrl->SetValue(m_settings.soundEnabled);
	int sampleRates[] = { 22050, 32000, 44100, 48000 };
	int idSampleRate = 2;
	for (int i=0; i<4; i++)
	{
		if (m_settings.soundSampleRate == sampleRates[i])
			idSampleRate = i;
	}
	soundSRCtrl->SetSelection(idSampleRate);
	
	upCtrl->OnChangeKey(	m_settings.padKeys[0]);
	downCtrl->OnChangeKey(	m_settings.padKeys[1]);
	leftCtrl->OnChangeKey(	m_settings.padKeys[2]);
	rightCtrl->OnChangeKey(	m_settings.padKeys[3]);
	aCtrl->OnChangeKey(		m_settings.padKeys[4]);
	bCtrl->OnChangeKey(		m_settings.padKeys[5]);
	selectCtrl->OnChangeKey(m_settings.padKeys[6]);
	startCtrl->OnChangeKey(	m_settings.padKeys[7]);

	return true;
}

/*! * Transfer data from the window */
bool SettingsDialog::TransferDataFromWindow()
{
    wxRadioBox* renderCtrl =     (wxRadioBox*)	  FindWindow(ID_RENDERMETHOD);
	wxRadioBox* greenscaleCtrl = (wxRadioBox*)	  FindWindow(ID_GREENSCALE);
	wxChoice* winZoomCtrl =		 (wxChoice*)	  FindWindow(ID_WINZOOM);
	wxCheckBox* soundEnabledCtrl = (wxCheckBox*)  FindWindow(ID_SOUND_ENABLED);
	wxChoice* soundSRCtrl =		 (wxChoice*)	  FindWindow(ID_SOUND_SR);
	InputTextCtrl* upCtrl =		 (InputTextCtrl*) FindWindow(ID_TEXTCTRL_UP);
	InputTextCtrl* downCtrl =	 (InputTextCtrl*) FindWindow(ID_TEXTCTRL_DOWN);
	InputTextCtrl* leftCtrl =	 (InputTextCtrl*) FindWindow(ID_TEXTCTRL_LEFT);
	InputTextCtrl* rightCtrl =	 (InputTextCtrl*) FindWindow(ID_TEXTCTRL_RIGHT);
	InputTextCtrl* aCtrl =		 (InputTextCtrl*) FindWindow(ID_TEXTCTRL_A);
	InputTextCtrl* bCtrl =		 (InputTextCtrl*) FindWindow(ID_TEXTCTRL_B);
	InputTextCtrl* selectCtrl =	 (InputTextCtrl*) FindWindow(ID_TEXTCTRL_SELECT);
	InputTextCtrl* startCtrl =	 (InputTextCtrl*) FindWindow(ID_TEXTCTRL_START);

    m_settings.renderMethod = renderCtrl->GetSelection();
	m_settings.greenScale = greenscaleCtrl->GetSelection();
	m_settings.windowZoom = winZoomCtrl->GetSelection()+1;
	m_settings.soundEnabled = soundEnabledCtrl->GetValue();
	int sampleRates[] = { 22050, 32000, 44100, 48000 };
	int idSampleRate = soundSRCtrl->GetSelection();
	m_settings.soundSampleRate = sampleRates[idSampleRate];

	m_settings.padKeys[0] = upCtrl->keyCode;
	m_settings.padKeys[1] = downCtrl->keyCode;
	m_settings.padKeys[2] = leftCtrl->keyCode;
	m_settings.padKeys[3] = rightCtrl->keyCode;
	m_settings.padKeys[4] = aCtrl->keyCode;
	m_settings.padKeys[5] = bCtrl->keyCode;
	m_settings.padKeys[6] = selectCtrl->keyCode;
	m_settings.padKeys[7] = startCtrl->keyCode;
	
    SettingsSetNewValues(m_settings);
	SettingsSaveToFile();

	return true;
}

wxPanel* SettingsDialog::CreateVideoSettingsPage(wxWindow* parent)
{
    wxPanel* panel = new wxPanel(parent, wxID_ANY);

    wxStaticText * renderLabel = new wxStaticText(panel, wxID_ANY, _("Render method:"));
	wxString renderChoices[2];
    renderChoices[0] = _("Software (Slower)");
    renderChoices[1] = _("OpenGL (Faster)");
    wxRadioBox* renderRadioBox = new wxRadioBox(panel, ID_RENDERMETHOD, wxT(""),
                                                   wxDefaultPosition, wxDefaultSize, 2, renderChoices, 1, wxRA_SPECIFY_COLS);

	wxStaticText * grayGreenLabel = new wxStaticText(panel, wxID_ANY, _("Color palette:"));
	wxString grayGreenChoices[2];
    grayGreenChoices[0] = _("Grayscale");
    grayGreenChoices[1] = _("Greenscale");
    wxRadioBox* grayGreenRadioBox = new wxRadioBox(panel, ID_GREENSCALE, wxT(""),
												 wxDefaultPosition, wxDefaultSize, 2, grayGreenChoices, 1, wxRA_SPECIFY_COLS);

	wxStaticText * winZoomLabel = new wxStaticText(panel, wxID_ANY, _("Window size:"));
    wxChoice* winZoomChoice = new wxChoice(panel, ID_WINZOOM);
	winZoomChoice->Append(wxT("1x"));
	winZoomChoice->Append(wxT("2x"));
	winZoomChoice->Append(wxT("3x"));
	winZoomChoice->Append(wxT("4x"));

	wxFlexGridSizer *grid = new wxFlexGridSizer(2, 3, 5);
    grid->Add(renderLabel, 0, wxUP, 7);
    grid->Add(renderRadioBox);
	grid->Add(grayGreenLabel, 0, wxUP, 7);
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
	
	wxStaticText * enabledLabel = new wxStaticText(panel, wxID_ANY, _("Enabled:"));
	wxCheckBox * enabledCheckBox = new wxCheckBox(panel, ID_SOUND_ENABLED, wxT(""));
	enabledCheckBox->SetValue(true);
	
	wxStaticText * sampleRateLabel = new wxStaticText(panel, wxID_ANY, _("Sample Rate:"));
    wxChoice* sampleRateChoice = new wxChoice(panel, ID_SOUND_SR);
	sampleRateChoice->Append(wxT("22050 Hz"));
	sampleRateChoice->Append(wxT("32000 Hz"));
	sampleRateChoice->Append(wxT("44100 Hz"));
	sampleRateChoice->Append(wxT("48000 Hz"));
	sampleRateChoice->SetSelection(0);
	
	wxFlexGridSizer *grid = new wxFlexGridSizer(2, 3, 5);
	grid->Add(enabledLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
	grid->Add(enabledCheckBox);
	grid->Add(sampleRateLabel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0);
	grid->Add(sampleRateChoice);
	
	wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
	topSizer->Add(grid, 0, wxALL, 10);
	
    panel->SetSizerAndFit(topSizer);
	
    return panel;
}

wxPanel* SettingsDialog::CreateInputSettingsPage(wxWindow* parent)
{
	wxPanel* panel = new wxPanel(parent, wxID_ANY);
	
    wxStaticText * upLabel = new wxStaticText(panel, wxID_ANY, _("Up:"));
	InputTextCtrl * upTextCtrl = new InputTextCtrl(panel, ID_TEXTCTRL_UP);
	
	wxStaticText * downLabel = new wxStaticText(panel, wxID_ANY, _("Down:"));
	InputTextCtrl * downTextCtrl = new InputTextCtrl(panel, ID_TEXTCTRL_DOWN);
	
	wxStaticText * leftLabel = new wxStaticText(panel, wxID_ANY, _("Left:"));
	InputTextCtrl * leftTextCtrl = new InputTextCtrl(panel, ID_TEXTCTRL_LEFT);
	
	wxStaticText * rightLabel = new wxStaticText(panel, wxID_ANY, _("Right:"));
	InputTextCtrl * rightTextCtrl = new InputTextCtrl(panel, ID_TEXTCTRL_RIGHT);
	
	wxStaticText * aLabel = new wxStaticText(panel, wxID_ANY, _("A:"));
	InputTextCtrl * aTextCtrl = new InputTextCtrl(panel, ID_TEXTCTRL_A);
	
	wxStaticText * bLabel = new wxStaticText(panel, wxID_ANY, _("B:"));
	InputTextCtrl * bTextCtrl = new InputTextCtrl(panel, ID_TEXTCTRL_B);
	
	wxStaticText * selectLabel = new wxStaticText(panel, wxID_ANY, _("Select:"));
	InputTextCtrl * selectTextCtrl = new InputTextCtrl(panel, ID_TEXTCTRL_SELECT);
	
	wxStaticText * startLabel = new wxStaticText(panel, wxID_ANY, _("Start:"));
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
	
	wxBoxSizer * topSizer = new wxBoxSizer(wxVERTICAL);
	topSizer->Add(0, 10);
	topSizer->Add(grid, 0, wxRIGHT|wxLEFT, 30);
	topSizer->Add(0, 10);
	
	panel->SetSizerAndFit(topSizer);
	
    return panel;
}

void SettingsDialog::Reload() {
    m_settings = SettingsGetCopy();
}

void SettingsDialog::AcceptValues() {
    SettingsSetNewValues(m_settings);
}
