#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <OpenGL/glu.h>
#include "Obj.h"
#include "wxImageLoader.h"
#define MAXCHAR 4098

ObjGeo ObjLoad(const char *filename){
    char line[MAXCHAR], temp[MAXCHAR];
    Vec3D vec3D;
    Vec2D vec2D;
    Face face;
    unsigned int currentMat;
    
    ObjGeo geo;
    geo.vertices   = ArrayCreate<Vec3D>();
    geo.normals    = ArrayCreate<Vec3D>();
    geo.texCoords  = ArrayCreate<Vec2D>();
    geo.faces      = ArrayCreate<Face>();
    geo.materials  = ArrayCreate<Material>(10);
    
    FILE *file;
    
    if ((file = fopen(filename,"r"))){
        while (fgets(line, MAXCHAR, file)){
            if (!memcmp(line, "v ", 2)){
                    sscanf(line+1, "%f%f%f", &vec3D.x, &vec3D.y, &vec3D.z);
                    ArrayAdd(geo.vertices, &vec3D);
            }
            else if (!memcmp(line,"vn",2)){
                    sscanf(line+2, "%f%f%f", &vec3D.x, &vec3D.y, &vec3D.z);
                    ArrayAdd(geo.normals, &vec3D);
            }
            else if (!memcmp(line, "vt", 2)){
                    sscanf(line+2, "%f%f", &vec2D.u, &vec2D.v);
                    ArrayAdd(geo.texCoords, &vec2D);
            }
            else if (!memcmp(line,"f ",2)){
                    face.points = GetFace(line);
                    face.IdMaterial = currentMat;
                    ArrayAdd(geo.faces, &face);
            }
            else if (!memcmp(line, "mtllib", 6)){
                    sscanf(line+7, "%s", temp);
                    geo.materials = MtlLoad(temp);
            }
            else if (!memcmp(line, "usemtl", 6)){
                    memcpy(temp, line+7, MAXCHAR);
                    temp[strlen(temp)-1] = '\0';
                    currentMat = MatGetID(geo.materials, temp);
            }
        }
        fclose(file);
    }
    else
        printf("OBJ file not found\n");
    
    return geo;    
}    

Array GetFace(char *line){
    
    Array points = ArrayCreate<Point>(3);
    Point p;
    char *tuple = strtok(line+2," ");
    while (tuple){
        p.IdVertex = p.IdNormal = p.IdTexCoord = 0;
        
        if(!strchr(tuple, '/')){
            //CASO A
            sscanf(tuple, "%d", &p.IdVertex);
        }    
        else if(strstr(tuple, "//")){
            //CASO C
            sscanf(tuple, "%d//%d", &p.IdVertex, &p.IdNormal);
        }    
        else if(strchr(tuple, '/') == strrchr(tuple, '/')){
            sscanf(tuple, "%d/%d", &p.IdVertex, &p.IdTexCoord);
        }    
        else{
            //CASO D
            sscanf(tuple, "%d/%d/%d", &p.IdVertex, &p.IdTexCoord, &p.IdNormal);
        }
        ArrayAdd(points, &p);
        tuple = strtok(NULL, " \n");
        if(tuple && strchr(tuple, '\r'))
            tuple = NULL;
    }
    return points;
}

void ObjClear(ObjGeo &geo){
    unsigned long i;
    
    ArrayClear(geo.vertices);
    ArrayClear(geo.normals);
    ArrayClear(geo.texCoords);
    Face *face = ArrayPtr<Face>(geo.faces);
    for (i=0; i<ArrayLength(geo.faces); i++){
        ArrayClear(face[i].points);
    }
    ArrayClear(geo.faces);
    ArrayClear(geo.materials);
}    

/*
void CreateVBO(objGeom *geom) {
    GLuint id;
    glGenBuffers(1, &id);
    
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(geom->vertices.buffer), geom->vertices.buffer, GL_STATIC_DRAW);
    
    for (i=0;i<geom->caras.elements;i++){
        glGenBuffers(1, &id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(geom->caras.buffer.points.buffer), geom->caras.buffer.points.buffer, GL_STATIC_DRAW);
        geom->caras.vboFace = id;
    }
    
    geom->vboVertices = id[0];
}

void DrawVBO() {
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, geom.vboID);
    for (i=0;i<geom.caras.elements;i++){
        glDrawElements(GL_TRIANGLES, geom.caras, GL_UNSIGNED_BYTE, 0);
    }
}
*/

void ObjDraw(const ObjGeo &geo){
    unsigned long i,j;
    unsigned int idNormal, idTexCoord, idVertex;
    int idMaterial = -1;
    const Vec3D *vertices = ArrayPtr<Vec3D>(geo.vertices);
    const Vec3D *normals  = ArrayPtr<Vec3D>(geo.normals);
    const Vec2D *texCoord = ArrayPtr<Vec2D>(geo.texCoords);
    const Material *materials = ArrayPtr<Material>(geo.materials);
    const Face *face = ArrayPtr<Face>(geo.faces);
    const Point *point;
    
    // Modo inmediato
    for (i=0; i<ArrayLength(geo.faces); i++){
        point = ArrayPtr<Point>(face[i].points);
        if (face[i].IdMaterial && (idMaterial != face[i].IdMaterial)) {
            idMaterial = face[i].IdMaterial;
            MatApply(materials[idMaterial-1]);
        }
        glBegin(GL_POLYGON);
        for (j=0; j<ArrayLength(face[i].points); j++){
            idNormal   = point[j].IdNormal;
            idTexCoord = point[j].IdTexCoord;
            idVertex   = point[j].IdVertex;
            
            if (idNormal > 0)
                glNormal3fv((float*)&normals[idNormal-1]);
            if (idTexCoord > 0)
                glTexCoord2fv((float*)&texCoord[idTexCoord-1]);
            glVertex3fv((float*)&vertices[idVertex-1]);
        }
        glEnd();
    }
}


void ObjScale(ObjGeo &geo, float s){
    unsigned long i;
    
    Vec3D *vertices = ArrayPtr<Vec3D>(geo.vertices);
    for (i=0; i<ArrayLength(geo.vertices); i++){
        vertices[i].x *= s;
        vertices[i].y *= s;
        vertices[i].z *= s;
    }    
}   

Material MatCreate(const char *name){
    Material mat;
    
    mat.name = (char*)malloc(sizeof(char)*strlen(name));
    strncpy(mat.name, name, strlen(name)-1);
    mat.name[strlen(name)-1]='\0';
    
    mat.texture = NULL;
    
    mat.texID = -1;
    
    mat.amb.r = mat.amb.g = mat.amb.b = mat.amb.a = 1;
    mat.spe.r = mat.spe.g = mat.spe.b = mat.spe.a = 1;
    mat.dif.r = mat.dif.g = mat.dif.b = mat.dif.a = 1;
    
    mat.bright = 64;
    mat.illum = 2;
    
    return mat;
}     

int MatGetID(const Array &materials, char *name){
    int i,j,res;
    const Material *m = ArrayPtr<Material>(materials);
    
    res = 0;
    j = 1;
    for(i=0; i<ArrayLength(materials); i++){
        if (!strcmp(m[i].name, name))
            res = j;
        j++;    
    }
    return res;
}

Array MtlLoad(const char* filename){
    //Esta funcion lee el fichero de materiales especificado y rellena un array
    //con todos los materiales encontrados, una vez hecho esto retorna el array
    char line[MAXCHAR], *copy;
    Array materials = ArrayCreate<Material>(5);
    Material m;
    int initial = 0;
    
    FILE *file;
    
    if ((file = fopen(filename, "r"))){
        while (fgets(line, MAXCHAR, file)){ //Mientras queden lineas sigue leyendo
            if (!strchr(line, '#')){
                if (!memcmp(line, "newmtl", 6)){ //Si es la palabra newmtl, entonces...
                    //anyadir el material anterior (menos la primera vez)
                    if (initial){
                        ArrayAdd(materials, &m);
                    }
                    initial = 1;
                    m = MatCreate(line+7);
                }
                else if (strstr(line, "Ka ")){
                    sscanf(line, " Ka %f %f %f", &m.amb.r, &m.amb.g, &m.amb.b);
                }
                else if (strstr(line, "map_Kd")){
                    char *name;
                    name = strchr(line, 'm');
                    copy = (char*)malloc(sizeof(char)*strlen(name));
                    sscanf(name, "map_Kd %s\n", copy);
                    m.texture = copy;
                }
                else if (strstr(line, "Kd ")){
                    sscanf(line, " Kd %f %f %f", &m.dif.r, &m.dif.g, &m.dif.b);
                }
                else if (strstr(line, "Ks ")){
                    sscanf(line, " Ks %f %f %f", &m.spe.r, &m.spe.g, &m.spe.b);
                }
                else if (strstr(line, "illum ")){
                    sscanf(line, " illum %d", &m.illum);
                }
                else if (strstr(line, "Ns ")){
                    sscanf(line, " Ns %d", &m.bright);
                }
                else if (strstr(line, "Tr ")){ //Hay que anyadir la trasparencia a Kd
                    sscanf(line, " Tr %f", &m.dif.a);
                }
                else if (strstr(line, "d ")){ //Hay que anyadir la trasparencia a Kd
                    sscanf(line, " d %f", &m.dif.a);
                }
            }    
        }
        ArrayAdd(materials,&m);
        fclose(file);
    }else
        printf("Mtl file not found\n");
    
    return materials;
}

void PrintMaterials(const Array &materials){
    unsigned long i;
    
    const Material *m = ArrayPtr<Material>(materials);
    for (i=0; i<ArrayLength(materials); i++){
        printf("Material: %s\n",  m[i].name);
        printf("Texture: %s\n",   m[i].texture);
        printf("IdTexture: %d\n", m[i].texID);
        printf("%f, %f, %f\n\n",  m[i].amb.r, m[i].amb.g, m[i].amb.b);
    }
}    

void LoadTextures(Array &materials){
    unsigned long i;
    
    Material *m = ArrayPtr<Material>(materials);
    for (i=0; i<ArrayLength(materials); i++)
        if (m[i].texture != NULL)
            m[i].texID = LoadTexture(m[i].texture);
}

unsigned int LoadTexture(const char* fileName){
    int imageWidth, imageHeight, textureWidth, textureHeight;
    
    return LoadImage(fileName, &imageWidth, &imageHeight, &textureWidth, &textureHeight);
}    

void MatApply(const Material &mat) {
    
    float ambient[] = {mat.amb.r, mat.amb.g, mat.amb.b, 1.0f};
    float diffuse[] = {mat.dif.r, mat.dif.g, mat.dif.b, 1.0f};
    //float emission[] = {0.0, 0.0, 0.0, 0.0};
    float specular[] = {mat.spe.r, mat.spe.g, mat.spe.b, 0.0f};
    if(mat.illum == 1) {
        specular[0] = specular[1] = specular[2] = specular[3] = 0.0f;
    }    
    float shininess = mat.bright;
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    //glMaterialfv(GL_FRONT, GL_EMISSION, emission);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
    
    int filtrado = 1;
    if (mat.texID == -1){
        glDisable(GL_TEXTURE_2D);
    }    
    else{
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, mat.texID);
        if (filtrado == 0){
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        }    
        else{
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        }    
    }

}
