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
