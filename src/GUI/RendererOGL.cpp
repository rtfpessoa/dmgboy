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

#include <wx/wx.h>
#include <wx/setup.h>
//#include <wx/dcclient.h>
#include <wx/stdpaths.h>

#if !wxUSE_GLCANVAS
#error "OpenGL required: set wxUSE_GLCANVAS to 1 and rebuild the library"
#endif

#ifdef __WXMAC__
#include <OpenGL/glu.h>
#else
#include <GL/glew.h>
#endif

#include "RendererOGL.h"
#include "MainFrame.h"
#include "Settings.h"
#include "../Def.h"

#define PI 3.141592653589793f


/*----------------------------------------------------------------
 Implementation of RendererOGL
 -----------------------------------------------------------------*/

BEGIN_EVENT_TABLE(RendererOGL, wxGLCanvas)
EVT_SIZE(RendererOGL::OnSize)
EVT_PAINT(RendererOGL::OnPaint)
EVT_ERASE_BACKGROUND(RendererOGL::OnEraseBackground)
EVT_LEFT_DCLICK(RendererOGL::OnDoubleClick)
EVT_LEFT_DOWN(RendererOGL::OnMouseLeftDown)
EVT_LEFT_UP(RendererOGL::OnMouseLeftUp)
EVT_RIGHT_DOWN(RendererOGL::OnMouseRightDown)
EVT_RIGHT_UP(RendererOGL::OnMouseRightUp)
EVT_MOTION(RendererOGL::OnMouseMove)
EVT_MOUSEWHEEL(RendererOGL::OnMouseWheel)
EVT_KEY_DOWN(RendererOGL::OnKeyPressed)
END_EVENT_TABLE()

void RendererOGL::OnKeyPressed(wxKeyEvent &ev)
{
    
}

int attribList[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_SAMPLE_BUFFERS, 1, WX_GL_SAMPLES, 2, WX_GL_DEPTH_SIZE, 16, 0 };

RendererOGL::RendererOGL(MainFrame *parent, wxWindowID id,
						   const wxPoint& pos, const wxSize& size, long style, const wxString& name)
: wxGLCanvas(parent, id, attribList, pos, size, style|wxFULL_REPAINT_ON_RESIZE, name)
{
    m_initialized = false;
    m_parent      = parent;
    m_glList      = 0;
	m_fov         = 28.0f;
    m_zNear       = 20.0f;
    m_zFar        = 1000.0f;
    m_glContext   = NULL;
    m_mouseNewX = m_mouseNewY = m_mouseOldX = m_mouseOldY = 0;
    m_mouseWheel = 0;
    m_mouseLeft = m_mouseRight = false;
    m_restoreTo2D = m_restoreTo3D = false;
    m_filter = 0.1f;
    
    float h = 42.75f;
    float radFov = m_fov * PI / 180.0f;
    float dist = (h/2.0f) / (float)tan(radFov/2.0f);
    m_minZ = -(dist+16);
    
    m_camera3D.Set(0, -2, -300, -30, 30, 0);
    m_camera2D.Set(0, -32.9f, m_minZ, 0, 0, 0);
    m_camera.CopyFrom(m_camera2D);

	SetWinRenderer(parent, this);
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
}

RendererOGL::~RendererOGL()
{
    delete m_glContext;
}

void RendererOGL::OnPaint( wxPaintEvent& event)
{
	wxPaintDC dc(this);
    Render();
}

void RendererOGL::OnSize(wxSizeEvent& event)
{
    Refresh();
}

void RendererOGL::OnEraseBackground(wxEraseEvent& event)
{
	// Do nothing, to avoid flashing.
}

void RendererOGL::InitGL()
{
#ifndef __WXMAC__
	glewInit();
#endif

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHT2);
    
    float position1[] = {0.0f, 0.0f, 1.0f, 0.0f};
    float position2[] = {-5.77f, 5.77f, -5.0f, 0.0f};
    float position3[] = {5.77f, -5.77f, -5.0f, 0.0f};
    float diffuse[]  = {1.0f, 1.0f, 1.0f, 1.0f};
    
    glLightfv(GL_LIGHT0, GL_POSITION, position1);
    glLightfv(GL_LIGHT1, GL_POSITION, position2);
    glLightfv(GL_LIGHT2, GL_POSITION, position3);
    
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse);
    
    glEnable(GL_DEPTH_TEST);
	
	glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
	glEnable(GL_TEXTURE_2D);
    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
	
    glGenTextures(1, &m_textureID);
	glBindTexture(GL_TEXTURE_2D, m_textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, GB_SCREEN_W, GB_SCREEN_H, 0, GL_RGB, GL_UNSIGNED_BYTE, frontBuffer);
    
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    wxString cwd = wxGetCwd();
	wxStandardPaths::Get().DontIgnoreAppSubDir();
	wxString dir = wxStandardPaths::Get().GetResourcesDir();
    wxSetWorkingDirectory(dir);

    if (wxFileExists("gb.3di"))
        m_geo = GeoLoad("gb.3di");
    else if (wxFileExists("gb.obj"))
        m_geo = GeoLoad("gb.obj");

    wxSetWorkingDirectory(cwd);
}

void RendererOGL::SetPerspective() {
    
    int winW, winH, w, h, x, y;
    GetClientSize(&winW, &winH);
    
    float gbAspectRatio = (float)GB_SCREEN_W / GB_SCREEN_H;
    float wAspectRatio = (float)winW / winH;
    
    if (gbAspectRatio < wAspectRatio)
    {
        w = winH * gbAspectRatio;
        h = winH;
        x = (winW - w) / 2;
        y = 0;
    }
    else
    {
        w = winW;
        h = winW / gbAspectRatio;
        x = 0;
        y = (winH - h) / 2;
    }
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(m_fov, wAspectRatio, m_zNear, m_zFar);
    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, (GLint) winW, (GLint) winH);
}

void RendererOGL::ScreenCreate() {
    m_glList = glGenLists(1);
    glNewList(m_glList, GL_COMPILE_AND_EXECUTE);
    
    glBegin(GL_QUADS);
    glNormal3f( 0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-23.75f, 54.225f, 16.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-23.75f, 11.475f, 16.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 23.75f, 11.475f, 16.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 23.75f, 54.225f, 16.0f);
    glEnd();
    
    glEndList();
}

void RendererOGL::SetGLContext()
{
    if (!m_glContext)
        m_glContext = new wxGLContext(this, NULL);
    SetCurrent(*m_glContext);
}

void RendererOGL::OnDoubleClick(wxMouseEvent &event)
{
    m_parent->OnDoubleClick(event);
}

void RendererOGL::OnMouseLeftDown(wxMouseEvent &event) {
    m_mouseLeft = true;
}

void RendererOGL::OnMouseLeftUp(wxMouseEvent &event) {
    m_mouseLeft = false;
}

void RendererOGL::OnMouseRightDown(wxMouseEvent &event) {
    m_mouseRight = true;
}

void RendererOGL::OnMouseRightUp(wxMouseEvent &event) {
    m_mouseRight = false;
}

void RendererOGL::OnMouseMove(wxMouseEvent &event) {
    m_mouseNewX = event.GetX();
    m_mouseNewY = event.GetY();
}

void RendererOGL::OnMouseWheel(wxMouseEvent &event) {
    m_mouseWheel += event.GetWheelRotation();
}

void RendererOGL::OnChangeView() {
    if (m_camera.IsEqualTo(m_camera2D, 0.01f)) {
        m_restoreTo3D = true;
        m_restoreTo2D = false;
    }
    else if (m_camera.IsEqualTo(m_camera3D, 0.01f)){
        m_restoreTo2D = true;
        m_restoreTo3D = false;
    }
    else if (m_restoreTo2D) {
        m_restoreTo3D = true;
        m_restoreTo2D = false;
    }
    else {
        m_restoreTo2D = true;
        m_restoreTo3D = false;
    }
}

bool RendererOGL::RestoreTo(Camera &camSrc, Camera &camDst) {
    camSrc.UnrollAngles();
    camSrc.GoTo(camDst, m_filter);
    
    return !camSrc.IsEqualTo(camDst, 0.01f);
}

void RendererOGL::MoveCamera(Camera &cam) {
    float x, y, z, alpha, beta;
    x = y = z = alpha = beta = 0;
    
    if (m_restoreTo2D)
        m_restoreTo2D = RestoreTo(cam, m_camera2D);
    else if (m_restoreTo3D)
            m_restoreTo3D = RestoreTo(cam, m_camera3D);
    else {
        if (m_mouseLeft) {
            cam.alpha += 0.1f*(m_mouseNewX - m_mouseOldX);
            cam.beta  += 0.1f*(m_mouseNewY - m_mouseOldY);
            if (cam.beta > 90)
                cam.beta = 90;
            else if (cam.beta < -90)
                cam.beta = -90;
        }
        
        if (m_mouseRight) {
            cam.x += 0.1f*(m_mouseNewX - m_mouseOldX);
            cam.y -= 0.1f*(m_mouseNewY - m_mouseOldY);
        }
        
        cam.z += m_mouseWheel;
        m_mouseWheel = 0;
        if (cam.z > m_minZ)
            cam.z = m_minZ;
        else if (cam.z < -(m_zFar-100))
            cam.z = -(m_zFar-100);
    }
    m_mouseOldX = m_mouseNewX;
    m_mouseOldY = m_mouseNewY;
}

void RendererOGL::Render()
{
	if(!IsShown())
        return;

    SetGLContext();
    
    // Init OpenGL once, but after SetCurrent
    if (!m_initialized)
    {
        InitGL();
        ScreenCreate();
        m_initialized = true;
    }
	
    SetPerspective();
	
    // clear color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glLoadIdentity();
    MoveCamera(m_camera);
    m_camera.Apply();
    
    if (!m_camera.IsEqualTo(m_camera2D, 0.01f))
        GeoDraw(m_geo);
    ScreenDraw();
	
    glFlush();
    SwapBuffers();

}

void RendererOGL::ScreenDraw() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, GB_SCREEN_W, GB_SCREEN_H, GL_RGB, GL_UNSIGNED_BYTE, frontBuffer);
    
    float ambient[] = {1.0f, 1.0f, 1.0f, 1.0f};
    float diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
    float specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    float shininess = 128.0f;
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
    
    glCallList(m_glList);
}
