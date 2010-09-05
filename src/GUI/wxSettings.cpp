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

#include "wxSettings.h"
#include "wxIDControls.h"
#include "gb16.xpm"
#include <wx/artprov.h>
#include <wx/bookctrl.h>

IMPLEMENT_CLASS(SettingsDialog, wxPropertySheetDialog)

BEGIN_EVENT_TABLE(SettingsDialog, wxPropertySheetDialog)
END_EVENT_TABLE()

SettingsDialog::SettingsDialog(wxWindow* win)
{
    SetExtraStyle(wxDIALOG_EX_CONTEXTHELP|wxWS_EX_VALIDATE_RECURSIVELY);
	
    int tabImage1 = -1;
    int tabImage2 = -1;
	
    bool useToolBook = true;
    int resizeBorder = wxRESIZE_BORDER;
	
    if (useToolBook)
    {
        resizeBorder = 0;
        tabImage1 = 0;
        tabImage2 = 1;
		
        int sheetStyle = wxPROPSHEET_SHRINKTOFIT | wxPROPSHEET_TOOLBOOK;
        //sheetStyle |= wxPROPSHEET_BUTTONTOOLBOOK;
		
        SetSheetStyle(sheetStyle);
        SetSheetInnerBorder(0);
        SetSheetOuterBorder(0);
		
        // create a dummy image list with a few icons
        const wxSize imageSize(32, 32);
		
        m_imageList = new wxImageList(imageSize.GetWidth(), imageSize.GetHeight());
        m_imageList->
		Add(wxArtProvider::GetIcon(wxART_INFORMATION, wxART_OTHER, imageSize));
        m_imageList->
		Add(wxArtProvider::GetIcon(wxART_QUESTION, wxART_OTHER, imageSize));
        m_imageList->
		Add(wxArtProvider::GetIcon(wxART_WARNING, wxART_OTHER, imageSize));
        m_imageList->
		Add(wxArtProvider::GetIcon(wxART_ERROR, wxART_OTHER, imageSize));
    }
    else
        m_imageList = NULL;
	
    Create(win, wxID_ANY, _("Preferences"), wxDefaultPosition, wxDefaultSize,
		   wxDEFAULT_DIALOG_STYLE| (int)wxPlatform::IfNot(wxOS_WINDOWS_CE, resizeBorder)
		   );
	
    // If using a toolbook, also follow Mac style and don't create buttons
    if (!useToolBook)
        CreateButtons(wxOK | wxCANCEL |
					  (int)wxPlatform::IfNot(wxOS_WINDOWS_CE, wxHELP)
					  );
	
    wxBookCtrlBase* notebook = GetBookCtrl();
    notebook->SetImageList(m_imageList);
	
    //wxPanel* generalSettings = CreateGeneralSettingsPage(notebook);
    //wxPanel* aestheticSettings = CreateAestheticSettingsPage(notebook);
	
    //notebook->AddPage(generalSettings, _("General"), true, tabImage1);
    //notebook->AddPage(aestheticSettings, _("Aesthetics"), false, tabImage2);
	
    LayoutDialog();
}

SettingsDialog::~SettingsDialog()
{
    delete m_imageList;
}