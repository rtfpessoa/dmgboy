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

#ifdef __WXMAC__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

#include "RendererOGL.h"
#include "../Settings.h"
#include "../Def.h"

#define PI 3.141592653589793f


/*----------------------------------------------------------------
 Implementation of RendererOGL
 -----------------------------------------------------------------*/
IMPLEMENT_CLASS(RendererOGL, wxGLCanvas)

BEGIN_EVENT_TABLE(RendererOGL, wxGLCanvas)
EVT_SIZE(RendererOGL::OnSize)
EVT_PAINT(RendererOGL::OnPaint)
EVT_ERASE_BACKGROUND(RendererOGL::OnEraseBackground)
EVT_LEFT_DCLICK(RendererOGL::OnDoubleClick)
END_EVENT_TABLE()

int attribList[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0 };

RendererOGL::RendererOGL(wxWindow *parent, wxWindowID id,
						   const wxPoint& pos, const wxSize& size, long style, const wxString& name)
// Debido a una incoherencia en el API de wxWidgets hay que realizar dos codigos distintos, uno para
// mac y otro para linux y windows
#ifndef __WXMAC__
: wxGLCanvas(parent, id, attribList, pos, size, style|wxFULL_REPAINT_ON_RESIZE, name)
#else
: wxGLCanvas(parent, (wxGLCanvas*) NULL, id, pos, size, style|wxFULL_REPAINT_ON_RESIZE , name)
#endif
{
    initialized = false;
    windowParent = parent;
    m_gllist = 0;
	fov = 50.0;
#ifndef __WXMAC__
    glContext = NULL;
#endif

	SetWinRenderer(parent, this);
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
}

RendererOGL::~RendererOGL()
{
#ifndef __WXMAC__
    delete glContext;
#endif
}


void RendererOGL::Render()
{
	if(!IsShown())
        return;
	
    SetGLContext();
    
    // Init OpenGL once, but after SetCurrent
    if (!initialized)
    {
        InitGL();
        initialized = true;
    }
	
    // set GL viewport
    int winW, winH, w, x;
    GetClientSize(&winW, &winH);
    
    w = winH * GB_SCREEN_W / GB_SCREEN_H;
    x = (winW - w) / 2;
    glViewport(x, 0, (GLint) w, (GLint) winH);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //glFrustum(-0.5f, 0.5f, -0.5f, 0.5f, 1.0f, 3.0f);
	gluPerspective(fov, (GLdouble)GB_SCREEN_W/GB_SCREEN_H, 1.0, 500.0);
	
    glMatrixMode(GL_MODELVIEW);
	
    /* clear color and depth buffers */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, GB_SCREEN_W, GB_SCREEN_H, GL_RGB, GL_UNSIGNED_BYTE, frontBuffer);
	
    if( m_gllist == 0 )
    {
		float w2 = GB_SCREEN_W / 2.0f;
		float h2 = GB_SCREEN_H / 2.0f;
		
        m_gllist = glGenLists( 1 );
        glNewList( m_gllist, GL_COMPILE_AND_EXECUTE );
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
    else
    {
		static float rot = 0;
		float radFov = fov * PI / 180.0f;
		float dist = ((float)GB_SCREEN_H/2.0f) / tan(radFov/2.0f); 
		
		glLoadIdentity();
		
		glTranslatef(0.0f, 0.0f, -(dist+(float)GB_SCREEN_W/2.0f));
		//glRotatef(30.0f, 1.0f, 0.0f, 0.0f);
		//glRotatef(-rot, 0.0f, 1.0f, 0.0f);
		
		rot+=1.0;
		
        glCallList(m_gllist);
    }
	
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
	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);// Set Texture Max Filter
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);// Set Texture Min Filter
	
	//glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);		// Set The Texture Generation Mode For S To Sphere Mapping
	//glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);		// Set The Texture Generation Mode For T To Sphere Mapping
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, GB_SCREEN_W, GB_SCREEN_H, 0, GL_RGB, GL_UNSIGNED_BYTE, frontBuffer);
}

void RendererOGL::SetGLContext()
{
    
#ifndef __WXMAC__
    if (!glContext)
        glContext = new wxGLContext(this, NULL);
    SetCurrent(*glContext);
#else
    if (!GetContext())
        return;
    else
        SetCurrent();
#endif
}

void RendererOGL::OnDoubleClick(wxMouseEvent &event)
{
    this->windowParent->ProcessEvent(event);
}
