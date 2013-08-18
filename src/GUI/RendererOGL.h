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

#ifndef __RENDEREROGL_H__
#define __RENDEREROGL_H__


/*******************************************************************************
 * RendererOGL Class
 *******************************************************************************/

#include "wx/glcanvas.h"
#include "RendererBase.h"

#if wxUSE_GLCANVAS

struct Camera{
    float x, y, z;
    float alpha, beta, gamma;
};

class MainFrame;

class RendererOGL: public wxGLCanvas, public RendererBase {

public:
    RendererOGL( MainFrame *parent, wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0, const wxString& name = _T("OGLRenderer") );
    
    ~RendererOGL();
	
	void OnPaint(wxPaintEvent&);
    void OnSize(wxSizeEvent&);
    void OnEraseBackground(wxEraseEvent&);
    void OnDoubleClick(wxMouseEvent&);
    void OnKeyPressed(wxKeyEvent &);
    void OnMouseLeftDown(wxMouseEvent &event);
    void OnMouseLeftUp(wxMouseEvent &event);
    void OnMouseRightDown(wxMouseEvent &event);
    void OnMouseRightUp(wxMouseEvent &event);
    void OnMouseMove(wxMouseEvent &event);
    void OnMouseWheel(wxMouseEvent &event);
    void OnChangeView();
	
    void Render();
	
protected:
	DECLARE_EVENT_TABLE()
    
private:
    bool         m_initialized;
	MainFrame   *m_parent;
    wxGLContext *m_glContext;
    GLuint       m_glList;
	float        m_fov;
    Camera       m_camera;
    Camera       m_camera2D;
    Camera       m_camera3D;
    int          m_mouseOldX, m_mouseOldY;
    int          m_mouseNewX, m_mouseNewY;
    int          m_mouseWheel;
    bool         m_mouseLeft, m_mouseRight;
    bool         m_restoreView;
    float        m_defaultZ;
    float        m_filter;
	
	void InitGL();
    void SetGLContext();
    void SetPerspective();
    void CreateCube();
    void MoveCamera(Camera &cam);
    void ApplyCamera(Camera &cam);
    bool CamsAreEqual(Camera &cam1, Camera &cam2, float delta);
    void FilterCam(Camera &cam, Camera &dstCamera, float filter);
    void UnrollAnglesCam(Camera &cam);
};

#endif // #if wxUSE_GLCANVAS


#endif
