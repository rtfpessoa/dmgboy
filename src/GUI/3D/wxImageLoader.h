#ifndef __WXIMAGELOADER_H__
#define __WXIMAGELOADER_H__

#include "wx/wx.h"

GLuint LoadImage(wxString path, int* imageWidth, int* imageHeight, int* textureWidth, int* textureHeight);

#endif