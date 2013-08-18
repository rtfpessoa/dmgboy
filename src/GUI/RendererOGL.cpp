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

#include <wx/setup.h>
#include <wx/dcclient.h>

#if !wxUSE_GLCANVAS
#error "OpenGL required: set wxUSE_GLCANVAS to 1 and rebuild the library"
#endif

#ifdef __WXMAC__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
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

int attribList[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0 };

RendererOGL::RendererOGL(MainFrame *parent, wxWindowID id,
						   const wxPoint& pos, const wxSize& size, long style, const wxString& name)
: wxGLCanvas(parent, id, attribList, pos, size, style|wxFULL_REPAINT_ON_RESIZE, name)
{
    m_initialized = false;
    m_parent      = parent;
    m_glList      = 0;
	m_fov         = 50.0f;
    m_glContext   = NULL;
    m_camera.x = m_camera.y = m_camera.z = 0;
    m_camera.alpha = m_camera.beta = m_camera.gamma = 0;
    m_mouseNewX = m_mouseNewY = m_mouseOldX = m_mouseOldY = 0;
    m_mouseWheel = 0;
    m_mouseLeft = m_mouseRight = false;
    m_restoreView = false;
    m_filter = 0.1f;
    
    float radFov = m_fov * PI / 180.0f;
    float dist = (GB_SCREEN_H/2.0f) / (float)tan(radFov/2.0f);
    m_defaultZ = -(dist+(float)GB_SCREEN_W/2.0f);
    
    m_camera.z = m_defaultZ;
    
    m_camera2D.alpha = 0;
    m_camera2D.beta = 0;
    m_camera2D.gamma = 0;
    m_camera2D.x = 0;
    m_camera2D.y = 0;
    m_camera2D.z = m_defaultZ;

	SetWinRenderer(parent, this);
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
}

RendererOGL::~RendererOGL()
{
    delete m_glContext;
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
        CreateCube();
        m_initialized = true;
    }
	
    SetPerspective();
	
    /* clear color and depth buffers */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, GB_SCREEN_W, GB_SCREEN_H, GL_RGB, GL_UNSIGNED_BYTE, frontBuffer);
    
    glLoadIdentity();
    MoveCamera(m_camera);
    ApplyCamera(m_camera);
    
    glCallList(m_glList);
	
    glFlush();
    SwapBuffers();
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
    
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_LIGHTING);
    //glEnable(GL_LIGHT0);
	
	glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
	glEnable(GL_TEXTURE_2D);
	
    //glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);// Set Texture Max Filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);// Set Texture Min Filter
	
	//glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);		// Set The Texture Generation Mode For S To Sphere Mapping
	//glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);		// Set The Texture Generation Mode For T To Sphere Mapping
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, GB_SCREEN_W, GB_SCREEN_H, 0, GL_RGB, GL_UNSIGNED_BYTE, frontBuffer);
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
	//gluPerspective(m_fov, (GLdouble)gbAspectRatio, 1.0, 500.0);
    gluPerspective(m_fov, wAspectRatio, 1.0, 5000.0);
    glMatrixMode(GL_MODELVIEW);
    //glViewport(x, y, (GLint) w, (GLint) h);
    glViewport(0, 0, (GLint) winW, (GLint) winH);
}

void RendererOGL::CreateCube() {
    float w2 = GB_SCREEN_W / 2.0f;
    float h2 = GB_SCREEN_H / 2.0f;
    
    m_glList = glGenLists( 1 );
    glNewList( m_glList, GL_COMPILE_AND_EXECUTE );
    /* draw six faces of a cube */
    glBegin(GL_QUADS);
    
    glNormal3f( 0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-w2, h2, w2); glTexCoord2f(0.0f, 1.0f); glVertex3f(-w2,-h2, w2);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( w2,-h2, w2); glTexCoord2f(1.0f, 0.0f); glVertex3f( w2, h2, w2);
    
    glNormal3f( 0.0f, 0.0f,-1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( w2, h2,-w2); glTexCoord2f(0.0f, 1.0f); glVertex3f( w2,-h2,-w2);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-w2,-h2,-w2); glTexCoord2f(1.0f, 0.0f); glVertex3f(-w2, h2,-w2);
    
    glNormal3f( 1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( w2, h2, w2); glTexCoord2f(0.0f, 1.0f); glVertex3f( w2,-h2, w2);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( w2,-h2,-w2); glTexCoord2f(1.0f, 0.0f); glVertex3f( w2, h2,-w2);
    
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-w2, h2,-w2); glTexCoord2f(0.0f, 1.0f); glVertex3f(-w2,-h2,-w2);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-w2,-h2, w2); glTexCoord2f(1.0f, 0.0f); glVertex3f(-w2, h2, w2);
    
    glNormal3f( 0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-w2, h2,-w2); glTexCoord2f(0.0f, 1.0f); glVertex3f(-w2, h2, w2);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( w2, h2, w2); glTexCoord2f(1.0f, 0.0f); glVertex3f( w2, h2,-w2);
    
    glNormal3f( 0.0f,-1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-w2,-h2, w2); glTexCoord2f(0.0f, 1.0f); glVertex3f(-w2,-h2,-w2);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( w2,-h2,-w2); glTexCoord2f(1.0f, 0.0f); glVertex3f( w2,-h2, w2);
    
    glEnd();
    
    glEndList();
}

void RendererOGL::ApplyCamera(Camera &cam) {
    glTranslatef(cam.x, cam.y, cam.z);
    glRotatef(cam.beta,  1, 0, 0);
    glRotatef(cam.alpha, 0, 1, 0);
    glRotatef(cam.gamma, 0, 0, 1);
}

void RendererOGL::MoveCamera(Camera &cam) {
    if (m_restoreView) {
        UnrollAnglesCam(cam);
        FilterCam(cam, m_camera2D, m_filter);
        
        if (CamsAreEqual(cam, m_camera2D, 0.1f))
            m_restoreView = false;
    }
        
    if (m_mouseLeft) {
        cam.alpha += 0.1f*(m_mouseNewX - m_mouseOldX);
        cam.beta  += 0.1f*(m_mouseNewY - m_mouseOldY);
        if (cam.beta > 90)
            cam.beta = 90;
        else if (cam.beta < -90)
            cam.beta = -90;
    }
    
    cam.z += m_mouseWheel;
    m_mouseWheel = 0;
    
    if (m_mouseRight) {
        cam.x += 0.1f*(m_mouseNewX - m_mouseOldX);
        cam.y += 0.1f*(m_mouseNewY - m_mouseOldY);
    }
    
    m_mouseOldX = m_mouseNewX;
    m_mouseOldY = m_mouseNewY;
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
    m_restoreView = true;
}

bool RendererOGL::CamsAreEqual(Camera &cam1, Camera &cam2, float delta) {
    if ((cam1.alpha < (cam2.alpha+delta)) && (cam1.alpha > (cam2.alpha-delta)) &&
        (cam1.beta  < (cam2.beta +delta)) && (cam1.beta  > (cam2.beta -delta)) &&
        (cam1.gamma < (cam2.gamma+delta)) && (cam1.gamma > (cam2.gamma-delta)) &&
        (cam1.x     < (cam2.x    +delta)) && (cam1.x     > (cam2.x    -delta)) &&
        (cam1.y     < (cam2.y    +delta)) && (cam1.y     > (cam2.y    -delta)) &&
        (cam1.z     < (cam2.z    +delta)) && (cam1.z     > (cam2.z    -delta))) {
        
        return true;
    }
    else
        return false;
}

void RendererOGL::FilterCam(Camera &cam, Camera &dstCamera, float filter) {
    cam.alpha = cam.alpha * (1-filter) + dstCamera.alpha * (filter);
    cam.beta  = cam.beta  * (1-filter) + dstCamera.beta  * (filter);
    cam.gamma = cam.gamma * (1-filter) + dstCamera.gamma * (filter);
    cam.x     = cam.x     * (1-filter) + dstCamera.x     * (filter);
    cam.y     = cam.y     * (1-filter) + dstCamera.y     * (filter);
    cam.z     = cam.z     * (1-filter) + dstCamera.z     * (filter);
}

void RendererOGL::UnrollAnglesCam(Camera &cam) {
    while (cam.alpha > 360)
        cam.alpha -= 360;
    while (cam.alpha < -360)
        cam.alpha += 360;
    while (cam.beta > 360)
        cam.beta -= 360;
    while (cam.beta < -360)
        cam.beta += 360;
    while (cam.gamma > 360)
        cam.gamma -= 360;
    while (cam.gamma < -360)
        cam.gamma += 360;
}
