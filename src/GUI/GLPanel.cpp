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

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if !wxUSE_GLCANVAS
#error "OpenGL required: set wxUSE_GLCANVAS to 1 and rebuild the library"
#endif

#include "GLPanel.h"
#include "../Settings.h"
#include "Def.h"


/*----------------------------------------------------------------
 Implementation of Test-GLCanvas
 -----------------------------------------------------------------*/
IMPLEMENT_CLASS(GLPanel, wxGLCanvas)

BEGIN_EVENT_TABLE(GLPanel, wxGLCanvas)
EVT_SIZE(GLPanel::OnSize)
EVT_PAINT(GLPanel::OnPaint)
EVT_ERASE_BACKGROUND(GLPanel::OnEraseBackground)
END_EVENT_TABLE()

static BYTE palettes[][4][3] =		{
	{
		{ 16,  57,  16},
		{ 49,  99,  49},
		{140, 173,  16},
		{156, 189,  16}
	},
	{
		{  0,   0,   0},
		{ 85,  85,  85},
		{170, 170, 170},
		{255, 255, 255}
	}
};


GLPanel::GLPanel(wxWindow *parent, wxWindowID id,
						   const wxPoint& pos, const wxSize& size, long style, const wxString& name)
: wxGLCanvas(parent, (wxGLCanvas*) NULL, id, pos, size, style|wxFULL_REPAINT_ON_RESIZE , name )
{
    initialized = false;
    m_gllist = 0;
	
	imgBuf = NULL;
	SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    windowParent = parent;
	
	CreateScreen();
	ChangeSize();
}

void GLPanel::CreateScreen() {
	imgBuf = new BYTE[GB_SCREEN_W*GB_SCREEN_H*3];
	OnClear();
	ChangePalette(SettingsGetGreenScale());
}

void GLPanel::ChangeSize()
{
	int zoom = SettingsGetWindowZoom();
    
	wxSize size(GB_SCREEN_W*zoom, GB_SCREEN_H*zoom);
    SetMinSize(size);
    SetMaxSize(size);
}

void GLPanel::OnRefreshScreen()
{
	// refresh the panel
    Refresh(false);
	Update();
}

void GLPanel::ChangePalette(bool original)
{
	if (original)
		selPalette = 0;
	else
		selPalette = 1;
}

void GLPanel::OnClear()
{
	int size = GB_SCREEN_W*GB_SCREEN_H*3;
	memset(imgBuf, 0, size);
}

void GLPanel::OnPreDraw()
{
	
}

void GLPanel::OnPostDraw()
{
	
}

void GLPanel::OnDrawPixel(int idColor, int x, int y)
{
	BYTE colorR = palettes[selPalette][idColor][0];
	BYTE colorG = palettes[selPalette][idColor][1];
	BYTE colorB = palettes[selPalette][idColor][2];
	
	int sizeLine = GB_SCREEN_W * 3;
	int offsetX = x * 3;
	int offsetY = y * sizeLine;
	int offsetBuf = offsetY + offsetX;
	
	imgBuf[offsetBuf + 0] = colorR;
	imgBuf[offsetBuf + 1] = colorG;
	imgBuf[offsetBuf + 2] = colorB;
}

void GLPanel::Render()
{
    wxPaintDC dc(this);
	
    if (!GetContext()) return;

	
    SetCurrent();
    // Init OpenGL once, but after SetCurrent
    if (!initialized)
    {
        InitGL();
        initialized = true;
    }
	
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-0.5f, 0.5f, -0.5f, 0.5f, 1.0f, 3.0f);
    glMatrixMode(GL_MODELVIEW);
	
    /* clear color and depth buffers */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, GB_SCREEN_W, GB_SCREEN_H, GL_RGB, GL_UNSIGNED_BYTE, imgBuf);
	
    if( m_gllist == 0 )
    {
        m_gllist = glGenLists( 1 );
        glNewList( m_gllist, GL_COMPILE_AND_EXECUTE );
        /* draw six faces of a cube */
        glBegin(GL_QUADS);
		
        glNormal3f( 0.0f, 0.0f, 1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 0.5f, 0.5f, 0.5f); glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, 0.5f, 0.5f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f,-0.5f, 0.5f); glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.5f,-0.5f, 0.5f);
		
        glNormal3f( 0.0f, 0.0f,-1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f,-0.5f,-0.5f); glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f,-0.5f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.5f, 0.5f,-0.5f); glTexCoord2f(1.0f, 0.0f); glVertex3f( 0.5f,-0.5f,-0.5f);
		
        glNormal3f( 0.0f, 1.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f( 0.5f, 0.5f, 0.5f); glTexCoord2f(0.0f, 1.0f); glVertex3f( 0.5f, 0.5f,-0.5f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f, 0.5f,-0.5f); glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, 0.5f, 0.5f);
		
        glNormal3f( 0.0f,-1.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f);glVertex3f(-0.5f,-0.5f,-0.5f); glTexCoord2f(0.0f, 1.0f); glVertex3f( 0.5f,-0.5f,-0.5f);
        glTexCoord2f(1.0f, 1.0f);glVertex3f( 0.5f,-0.5f, 0.5f); glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f,-0.5f, 0.5f);
		
        glNormal3f( 1.0f, 0.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f( 0.5f, 0.5f, 0.5f); glTexCoord2f(0.0f, 1.0f); glVertex3f( 0.5f,-0.5f, 0.5f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.5f,-0.5f,-0.5f); glTexCoord2f(1.0f, 0.0f); glVertex3f( 0.5f, 0.5f,-0.5f);
		
        glNormal3f(-1.0f, 0.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f,-0.5f,-0.5f); glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f,-0.5f, 0.5f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f, 0.5f, 0.5f); glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, 0.5f,-0.5f);
        glEnd();
		
        glEndList();
    }
    else
    {
        glCallList(m_gllist);
    }
	
    glFlush();
    SwapBuffers();
}

void GLPanel::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    Render();
}

void GLPanel::OnSize(wxSizeEvent& event)
{
    // this is also necessary to update the context on some platforms
    wxGLCanvas::OnSize(event);
	
    // set GL viewport (not called by wxGLCanvas::OnSize on all platforms...)
    int w, h;
    GetClientSize(&w, &h);
    if (GetContext())
    {
        SetCurrent();
        glViewport(0, 0, (GLint) w, (GLint) h);
    }
}

void GLPanel::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
	// Do nothing, to avoid flashing.
}

void GLPanel::InitGL()
{
    SetCurrent();
	
    /* set viewing projection */
    glMatrixMode(GL_PROJECTION);
    glFrustum(-0.5f, 0.5f, -0.5f, 0.5f, 1.0f, 3.0f);
	
    /* position viewer */
    glMatrixMode(GL_MODELVIEW);
    glTranslatef(0.0f, 0.0f, -2.0f);
	
    /* position object */
    glRotatef(30.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(30.0f, 0.0f, 1.0f, 0.0f);
	
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
	
	glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
	glEnable(GL_TEXTURE_2D);
	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);// Set Texture Max Filter
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);// Set Texture Min Filter
	
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);		// Set The Texture Generation Mode For S To Sphere Mapping
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);		// Set The Texture Generation Mode For T To Sphere Mapping
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, GB_SCREEN_W, GB_SCREEN_H, 0, GL_RGB, GL_UNSIGNED_BYTE, imgBuf);
}
