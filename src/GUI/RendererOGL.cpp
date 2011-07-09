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

#include "wx/wx.h"

#if !wxUSE_GLCANVAS
#error "OpenGL required: set wxUSE_GLCANVAS to 1 and rebuild the library"
#endif

#include "RendererOGL.h"
#include "../Settings.h"
#include "Def.h"


/*----------------------------------------------------------------
 Implementation of RendererOGL
 -----------------------------------------------------------------*/
IMPLEMENT_CLASS(RendererOGL, wxGLCanvas)

BEGIN_EVENT_TABLE(RendererOGL, wxGLCanvas)
EVT_SIZE(RendererOGL::OnSize)
EVT_PAINT(RendererOGL::OnPaint)
EVT_ERASE_BACKGROUND(RendererOGL::OnEraseBackground)
END_EVENT_TABLE()

RendererOGL::RendererOGL(wxWindow *parent, wxWindowID id,
						   const wxPoint& pos, const wxSize& size, long style, const wxString& name)
: wxGLCanvas(parent, (wxGLCanvas*) NULL, id, pos, size, style|wxFULL_REPAINT_ON_RESIZE , name ),
  RendererBase(this)
{
    initialized = false;
    m_gllist = 0;
	
	SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    windowParent = parent;
}

void RendererOGL::Render()
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

void RendererOGL::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    Render();
}

void RendererOGL::OnSize(wxSizeEvent& event)
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

void RendererOGL::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
	// Do nothing, to avoid flashing.
}

void RendererOGL::InitGL()
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
