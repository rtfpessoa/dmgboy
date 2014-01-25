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

#include <stdio.h>
#include <cmath>

#include "wx/image.h"
#include "wx/wx.h"

#ifdef __WXMAC__
#include "OpenGL/gl.h"
#else
#include <GL/gl.h>
#endif

#include "wxImageLoader.h"

void LoadImagePow2(wxImage* img, int* texWidth, int* texHeight) {
    // note: must make a local copy before passing the data to OpenGL, as GetData() returns RGB
    // and we want the Alpha channel if it's present. Additionally OpenGL seems to interpret the
    // data upside-down so we need to compensate for that.
    GLubyte *bitmapData = img->GetData();
    GLubyte *alphaData = img->GetAlpha();
    
    int bytesPerPixel = img->HasAlpha() ?  4 : 3;
    
    int imgWidth  = img->GetWidth();
    int imgHeight = img->GetHeight();
    int imgSize = imgWidth * imgHeight * bytesPerPixel;
    GLubyte *imgData = new GLubyte[imgSize];
    
    int revVal = imgHeight-1;
    
    for(int y=0; y<imgHeight; y++)
    {
        for(int x=0; x<imgWidth; x++)
        {
            int offset = (x+y*imgWidth)*bytesPerPixel;
            
            imgData[offset+0] = bitmapData[(x+(revVal-y)*imgWidth)*3 + 0];
            imgData[offset+1] = bitmapData[(x+(revVal-y)*imgWidth)*3 + 1];
            imgData[offset+2] = bitmapData[(x+(revVal-y)*imgWidth)*3 + 2];
            
            if(bytesPerPixel == 4)
                imgData[offset+3] = alphaData[x+(revVal-y)*imgWidth];
        }//next
    }//next
    
    // if yes, everything is fine
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 bytesPerPixel,
                 imgWidth,
                 imgHeight,
                 0,
                 img->HasAlpha() ?  GL_RGBA : GL_RGB,
                 GL_UNSIGNED_BYTE,
                 imgData);
    
    (*texWidth)  = imgWidth;
    (*texHeight) = imgHeight;
    
    delete [] imgData;
}

void LoadImageNotPow2(wxImage* img, int* texWidth, int* texHeight, double expWidth, double expHeight) {
    int imgWidth  = img->GetWidth();
    int imgHeight = img->GetHeight();
    
    int newWidth  = (int)std::pow(2.0, (int)(std::ceil(expWidth)));
    int newHeight = (int)std::pow(2.0, (int)(std::ceil(expHeight)));
    
    GLubyte	*bitmapData = img->GetData();
    GLubyte *alphaData  = img->GetAlpha();
    
    int oldBytesPerPixel = 3;
    int bytesPerPixel = img->HasAlpha() ?  4 : 3;
    
    int imageSize = newWidth * newHeight * bytesPerPixel;
    GLubyte	*imgData = new GLubyte[imageSize];
    
    int revVal = imgHeight-1;
    
    for(int y=0; y<newHeight; y++)
    {
        for(int x=0; x<newWidth; x++)
        {
            int offset = (x+y*newWidth)*bytesPerPixel;
            
            if(x<imgWidth && y<imgHeight){
                imgData[offset+0] = bitmapData[(x+(revVal-y)*imgWidth)*oldBytesPerPixel + 0];
                imgData[offset+1] = bitmapData[(x+(revVal-y)*imgWidth)*oldBytesPerPixel + 1];
                imgData[offset+2] = bitmapData[(x+(revVal-y)*imgWidth)*oldBytesPerPixel + 2];
                
                if(bytesPerPixel==4)
                    imgData[offset+3] = alphaData[x+(revVal-y)*imgWidth];
            }
            else {
                imgData[offset+0] = 0;
                imgData[offset+1] = 0;
                imgData[offset+2] = 0;
                if(bytesPerPixel == 4)
                    imgData[offset+3] = 0;
            }
            
        }//next
    }//next
    
    
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 img->HasAlpha() ?  4 : 3,
                 newWidth,
                 newHeight,
                 0,
                 img->HasAlpha() ?  GL_RGBA : GL_RGB,
                 GL_UNSIGNED_BYTE,
                 imgData);
    
    (*texWidth)  = newWidth;
    (*texHeight) = newHeight;
    
    delete [] imgData;
}

GLuint LoadImage(wxString path, int* imgWidth, int* imgHeight, int* texWidth, int* texHeight)
{
	// the first time, init image handlers (remove this part if you do it somewhere else in your app)
	static bool isFirstTime = true;
	if(isFirstTime) {
		wxInitAllImageHandlers();
		isFirstTime = false;
	}
	
	// check the file exists
	if(!wxFileExists(path)) {
		printf("Failed to load resource image\n");
		return -1;
	}
    
    GLuint id;
	glGenTextures(1, &id);
	
	glBindTexture(GL_TEXTURE_2D, id);
    
	wxImage *img = new wxImage(path);
	
	(*imgWidth)  = img->GetWidth();
	(*imgHeight) = img->GetHeight();
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
    /*
     * Many graphics card require that textures be power of two.
     * Below is a simple implementation, probably not optimal but working.
     * If your texture sizes are not restricted to power of 2s, you can
     * of course adapt the bit below as needed.
     */
    
	double expWidth  = std::log((double)(*imgWidth))/std::log(2.0);
	double expHeight = std::log((double)(*imgHeight))/std::log(2.0);
	
    // check if image dimensions are a power of two
    if((int)expWidth == expWidth && (int)expHeight == expHeight)
        LoadImagePow2(img, texWidth, texHeight);
	else // texture is not a power of two. We need to resize it
        LoadImageNotPow2(img, texWidth, texHeight, expWidth, expHeight);
    
    delete img;
	
	return id;
	
}