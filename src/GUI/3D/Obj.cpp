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
    geo.vertices   = ArrayCreate(sizeof(Vec3D),50);
    geo.normals    = ArrayCreate(sizeof(Vec3D),50);
    geo.texCoords  = ArrayCreate(sizeof(Vec2D),50);
    geo.faces      = ArrayCreate(sizeof(Face),50);
    geo.materials  = ArrayCreate(sizeof(Material),50);
    
    //material m = (material*) &obj.materiales.buffer;
    
    FILE *file;
    
    if ((file = fopen(filename,"r"))){
        while (fgets(line, MAXCHAR, file)){
            if (!memcmp(line, "v ", 2)){
                    sscanf(line+1, "%f%f%f", &vec3D.x, &vec3D.y, &vec3D.z);
                    ArrayAdd(&geo.vertices, &vec3D);
            }
            else if (!memcmp(line,"vn",2)){
                    sscanf(line+2, "%f%f%f", &vec3D.x, &vec3D.y, &vec3D.z);
                    ArrayAdd(&geo.normals, &vec3D);
            }
            else if (!memcmp(line, "vt", 2)){
                    sscanf(line+2, "%f%f", &vec2D.u, &vec2D.v);
                    ArrayAdd(&geo.texCoords, &vec2D);
            }
            else if (!memcmp(line,"f ",2)){
                    face.points = GetFace(line);
                    face.IdMaterial = currentMat;
                    ArrayAdd(&geo.faces, &face);
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
    
    Array points = ArrayCreate(sizeof(Point), 3);
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
        ArrayAdd(&points, &p);
        tuple = strtok(NULL, " \n");
    }
    return points;
}

void ObjClear(ObjGeo *geo){
    unsigned long i;
    Face *face = (Face*) &(geo->faces.buffer);
    
    ArrayClear(&(geo->vertices));
    ArrayClear(&(geo->normals));
    ArrayClear(&(geo->texCoords));
    for (i=0;i<geo->faces.elements;i++){
        ArrayClear(&(face[i].points));
    }
    ArrayClear(&(geo->faces));
    ArrayClear(&(geo->materials));
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

void ObjDraw(ObjGeo geo){
    unsigned long i,j;
    Vec3D *vertices = (Vec3D*) geo.vertices.buffer;
    Vec3D *normals  = (Vec3D*) geo.normals.buffer;
    Vec2D *texCoord = (Vec2D*) geo.texCoords.buffer;
    Material *materials = (Material*) geo.materials.buffer;
    Face *face = (Face*) geo.faces.buffer;
    Point *point;
    
    // Modo inmediato
    for (i=0;i<geo.faces.elements;i++){
        point = (Point*) face[i].points.buffer;
        if (face[i].IdMaterial)
            MatApply(materials[face[i].IdMaterial-1]);
        glBegin(GL_TRIANGLES);
        for (j=0;j<face[i].points.elements;j++){
            if (point[j].IdNormal)
                glNormal3fv((float*)&normals[point[j].IdNormal-1]);
            if (point[j].IdTexCoord)
                glTexCoord2fv((float*)&texCoord[point[j].IdTexCoord-1]);
            glVertex3fv((float*)&vertices[point[j].IdVertex-1]);
        }
        glEnd();
    }
}


void ObjScale(ObjGeo geo, float s){
    Vec3D *vertices = (Vec3D*) geo.vertices.buffer;
    unsigned long i;
    for (i=0;i<geo.vertices.elements;i++){
        vertices[i].x *= s;
        vertices[i].y *= s;
        vertices[i].z *= s;
    }    
}   

Material MatCreate(char *name){
    Material mat;
    
    mat.name = (char*)malloc(sizeof(char)*strlen(name));
    strncpy(mat.name, name,strlen(name)-1);
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

int MatGetID(Array materiales, char *name){
    int i,j,res;
    Material *m = (Material*) materiales.buffer;
    
    res=0;
    j=1;
    for(i=0; i<materiales.elements; i++){
        if (!strcmp(m[i].name, name))
            res=j;
        j++;    
    }
    return res;
}

Array MtlLoad(const char* filename){
    //Esta funcion lee el fichero de materiales especificado y rellena un array
    //con todos los materiales encontrados, una vez hecho esto retorna el array
    char linea[MAXCHAR], *copia;
    Array materiales = ArrayCreate(sizeof(Material),3);
    Material m;
    int inicial = 0;
    
    FILE *fichero;
    
    if ((fichero = fopen(filename, "r"))){
        while (fgets(linea, MAXCHAR, fichero)){ //Mientras queden lineas sigue leyendo
            if (!strchr(linea, '#')){
                if (!memcmp(linea, "newmtl", 6)){ //Si es la palabra newmtl, entonces...
                    //anyadir el material anterior (menos la primera vez)
                    if (inicial){
                        ArrayAdd(&materiales, &m);
                    }
                    inicial=1;
                    m = MatCreate(linea+7);
                }
                else if (strstr(linea, "Ka ")){
                    sscanf(linea, " Ka %f %f %f", &m.amb.r, &m.amb.g, &m.amb.b); 
                }
                else if (strstr(linea, "map_Kd")){
                    char *name;
                    name = strchr(linea, 'm');
                    copia = (char*)malloc(sizeof(char)*strlen(name));
                    sscanf(name, "map_Kd %s\n", copia);
                    m.texture = copia;
                }
                else if (strstr(linea, "Kd ")){
                    sscanf(linea, " Kd %f %f %f", &m.dif.r, &m.dif.g, &m.dif.b);
                }
                else if (strstr(linea, "Ks ")){
                    sscanf(linea, " Ks %f %f %f", &m.spe.r, &m.spe.g, &m.spe.b);
                }
                else if (strstr(linea, "illum ")){
                    sscanf(linea, " illum %d", &m.illum);
                }
                else if (strstr(linea, "Ns ")){
                    sscanf(linea, " Ns %d", &m.bright);
                }
                else if (strstr(linea, "Tr ")){ //Hay que anyadir la trasparencia a Kd
                    sscanf(linea, " Tr %f", &m.dif.a);
                }
                else if (strstr(linea, "d ")){ //Hay que anyadir la trasparencia a Kd
                    sscanf(linea, " d %f", &m.dif.a);
                }
            }    
        }
        ArrayAdd(&materiales,&m);
    }else
        printf("Mtl file not found\n");
    
    return materiales;            
}

void PrintMaterials(Array materials){
    Material *m = (Material*) materials.buffer;
    int i;
    for (i=0; i<materials.elements; i++){
        printf("Material: %s\n",  m[i].name);
        printf("Texture: %s\n",   m[i].texture);
        printf("IdTexture: %d\n", m[i].texID);
        printf("%f, %f, %f\n\n",  m[i].amb.r, m[i].amb.g, m[i].amb.b);
    }
}    

void LoadTextures(Array materials){
    int i;
    Material *m = (Material*) materials.buffer;
    
    for (i=0; i<materials.elements; i++)
        if (m[i].texture != NULL)
            m[i].texID = LoadTexture(m[i].texture);
}

unsigned int LoadTexture(char* fileName){
    int imageWidth, imageHeight, textureWidth, textureHeight;
    
    return LoadImage(fileName, &imageWidth, &imageHeight, &textureWidth, &textureHeight);
}    

void MatApply(Material mat) {
    
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
