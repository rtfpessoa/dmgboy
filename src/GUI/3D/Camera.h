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

#ifndef __CAMERA_H__
#define __CAMERA_H__

class Camera {
public:
    float x, y, z;
    float alpha, beta, gamma;
    
    Camera();
    Camera(Camera &src);
    Camera(float x, float y, float z, float alpha, float beta, float gamma);
    ~Camera();
    
    void Set(float x, float y, float z, float alpha, float beta, float gamma);
    void Apply();
    void CopyFrom(Camera &src);
    bool IsEqualTo(Camera &cam, float delta);
    void GoTo(Camera &cam, float filter);
    void UnrollAngles();
    
private:
    
};

    
#endif
