#ifdef __WXMAC__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif
#include "Camera.h"

Camera::Camera() {
    x = y = z = 0;
    alpha = beta = gamma = 0;
}

Camera::Camera(Camera &src) {
    CopyFrom(src);
}

Camera::Camera(float x, float y, float z, float alpha, float beta, float gamma) {
    Set(x, y, z, alpha, beta, gamma);
}

Camera::~Camera() {
    
}

void Camera::Set(float x, float y, float z, float alpha, float beta, float gamma) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->alpha = alpha;
    this->beta  = beta;
    this->gamma = gamma;
}

bool Camera::IsEqualTo(Camera &cam, float delta) {
    if ((alpha < (cam.alpha+delta)) && (alpha > (cam.alpha-delta)) &&
        (beta  < (cam.beta +delta)) && (beta  > (cam.beta -delta)) &&
        (gamma < (cam.gamma+delta)) && (gamma > (cam.gamma-delta)) &&
        (x     < (cam.x    +delta)) && (x     > (cam.x    -delta)) &&
        (y     < (cam.y    +delta)) && (y     > (cam.y    -delta)) &&
        (z     < (cam.z    +delta)) && (z     > (cam.z    -delta))) {
        
        return true;
    }
    else
        return false;
}

void Camera::GoTo(Camera &dst, float filter) {
    alpha = alpha * (1-filter) + dst.alpha * (filter);
    beta  = beta  * (1-filter) + dst.beta  * (filter);
    gamma = gamma * (1-filter) + dst.gamma * (filter);
    x     = x     * (1-filter) + dst.x     * (filter);
    y     = y     * (1-filter) + dst.y     * (filter);
    z     = z     * (1-filter) + dst.z     * (filter);
}

void Camera::UnrollAngles() {
    while (alpha > +360) alpha -= 360;
    while (alpha < -360) alpha += 360;
    while (beta  > +360) beta  -= 360;
    while (beta  < -360) beta  += 360;
    while (gamma > +360) gamma -= 360;
    while (gamma < -360) gamma += 360;
}

void Camera::Apply() {
    glTranslatef(x, y, z);
    glRotatef(beta,  1, 0, 0);
    glRotatef(alpha, 0, 1, 0);
    glRotatef(gamma, 0, 0, 1);
}

void Camera::CopyFrom(Camera &src) {
    x = src.x;
    y = src.y;
    z = src.z;
    alpha = src.alpha;
    beta  = src.beta;
    gamma = src.gamma;
}
