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

#ifndef __RENDEREROGL_H__
#define __RENDEREROGL_H__


/*******************************************************************************
 * RendererOGL Class
 *******************************************************************************/

#include "wx/glcanvas.h"
#include "RendererBase.h"
#include "Def.h"

#if wxUSE_GLCANVAS

class RendererOGL: public wxGLCanvas, public RendererBase {
	DECLARE_CLASS(RendererOGL)
	DECLARE_EVENT_TABLE()
	
private:
    bool initialized;
	wxWindow * windowParent;
    GLuint m_gllist;
	
	void InitGL();
	
public:
    RendererOGL( wxWindow *parent, wxWindowID id = wxID_ANY,
				 const wxPoint& pos = wxDefaultPosition,
				 const wxSize& size = wxDefaultSize,
				 long style = 0, const wxString& name = _T("OGLRenderer") );
	
	void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
	
    void Render();
};

#endif // #if wxUSE_GLCANVAS


#endif
