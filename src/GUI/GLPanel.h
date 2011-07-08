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

#ifndef __GLPANEL_H__
#define __GLPANEL_H__


/*******************************************************************************
 * GLPanel Class
 *******************************************************************************/

#include "wx/glcanvas.h"
#include "../IGBScreenDrawable.h"
#include "Def.h"

#if wxUSE_GLCANVAS

class GLPanel: public wxGLCanvas, public IGBScreenDrawable {
	DECLARE_CLASS(GLPanel)
	DECLARE_EVENT_TABLE()
	
private:
    bool   initialized;
	int selPalette;
    BYTE * imgBuf;
	wxWindow * windowParent;
    GLuint m_gllist;
	
public:
    GLPanel( wxWindow *parent, wxWindowID id = wxID_ANY,
				 const wxPoint& pos = wxDefaultPosition,
				 const wxSize& size = wxDefaultSize,
				 long style = 0, const wxString& name = _T("GLPanel") );
	
    GLPanel();
	
	void CreateScreen();
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
	
	void ChangeSize();
	void ChangePalette(bool original);
	void OnPreDraw();
	void OnPostDraw();
	void OnDrawPixel(int idColor, int x, int y);
	void OnRefreshScreen();
	void OnClear();
	
    void Render();
    void InitGL();
};

#endif // #if wxUSE_GLCANVAS


#endif
