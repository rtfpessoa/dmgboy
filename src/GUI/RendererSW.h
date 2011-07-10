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

#ifndef __RENDERERSW_H__
#define __RENDERERSW_H__

#include <wx/wx.h>
#include "RendererBase.h"

/*******************************************************************************
 * RendererSW Class
 *******************************************************************************/

class RendererSW : public wxPanel, public RendererBase {
    DECLARE_CLASS(RendererSW)
    DECLARE_EVENT_TABLE()

private:
	void OnPaint(wxPaintEvent &);
    void OnEraseBackground(wxEraseEvent &);
    
public:
	RendererSW(wxWindow *parent);
    ~RendererSW();
};

#endif
