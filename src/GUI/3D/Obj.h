#ifndef _OBJLOAD_H_
#define _OBJLOAD_H_

#include "array.h"

typedef struct{
    float x,y,z;
}Vec3D;

typedef struct{
    float u,v;
}Vec2D;

typedef struct{
    unsigned int IdVertex, IdNormal, IdTexCoord;
}Point;

typedef struct{
    Array points;
    unsigned int IdMaterial;
}Face;

typedef struct{
    Array vertices;
    Array normals;
    Array texCoords;
    Array faces;
    Array materials;
    GLuint vboVertices;
    GLuint vboNormals;
    GLuint vboTexCoords;
    GLuint *vboIndices;
    Array numIndices;
}ObjGeo;

typedef struct{
    float r,g,b,a;
}RGBA;

typedef struct{
    char *name;
    char *texture;
    unsigned int texID;
    RGBA amb;
    RGBA dif;
    RGBA spe;
    int bright;
    int illum;
}Material;

ObjGeo ObjLoad(const char *filename);
void   ObjScale(ObjGeo &geo, float s);
void   ObjClear(ObjGeo &geo);
void   ObjDraw(const ObjGeo &geo);
void   ObjCreateVBO(ObjGeo &geo);

Array MtlLoad(const char *filename);
Array GetFace(char *line);

Material MatCreate(const char *name);
int      MatGetID(const Array &materials, char *name);
void     MatApply(const Material &mat);

void PrintMaterials(const Array &materials);

unsigned int LoadTexture(const char* filename);
void LoadTextures(Array &materials);

class Obj {
public:
    Obj(const char *filename);
    ~Obj();
    
    void Scale(float s);
    void Draw();
    
private:
    int GetMatID(Array materiales, const char *name);
};


#endif
