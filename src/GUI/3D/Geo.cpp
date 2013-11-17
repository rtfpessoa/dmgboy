#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <OpenGL/glu.h>
#include "Geo.h"
#include "wxImageLoader.h"
#define MAXCHAR 4098

using namespace std;

void LoadIndexedBuffers(string fileName, Array &vertices, Array &normals, Array &texCoords,
                   Array &indices, Array &materials);
void GeoFillVBOBuffers(Geo &geo, Array &outVertices, Array &outNormals, Array &outTexCoords, Array &outIndices);
void GeoClearVBOArrays(Array &outVertices, Array &outNormals, Array &outTexCoords, Array &outIndices);

bool strEndsWith(const char *str1, const char *str2) {
    int str1Len = strlen(str1);
    int str2Len = strlen(str2);
    
    if (str1Len >= str2Len && strcmp(str1 + str1Len - str2Len, str2) == 0)
        return true;
    else
        return false;
}

void ObjLoad(const char *filename, Geo &geo) {
    char line[MAXCHAR], temp[MAXCHAR];
    Vec3D vec3D;
    Vec2D vec2D;
    Face face;
    unsigned int currentMat;
    
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
}

void File3diLoad(const char *filename, Geo &geo) {
    Array vertices   = ArrayCreate<Vec3D>(500);
    Array normals    = ArrayCreate<Vec3D>(500);
    Array texCoords  = ArrayCreate<Vec2D>(500);
    Array indices    = ArrayCreate<Array>();
    
    LoadIndexedBuffers(filename, vertices, normals, texCoords,
                       indices, geo.materials);
    GeoFillVBOBuffers(geo, vertices, normals, texCoords, indices);
    GeoClearVBOArrays(vertices, normals, texCoords, indices);
}

Geo GeoLoad(const char *filename) {
    char extension[] = ".3di";
    
    Geo geo;
    geo.vboIndices = NULL;
    geo.vertices   = ArrayCreate<Vec3D>(500);
    geo.normals    = ArrayCreate<Vec3D>(500);
    geo.texCoords  = ArrayCreate<Vec2D>(500);
    geo.faces      = ArrayCreate<Face>(300);
    geo.materials  = ArrayCreate<Material>(10);
    
    if (strEndsWith(filename, extension)) {
        File3diLoad(filename, geo);
    } else {
        ObjLoad(filename, geo);
    }
    
    LoadTextures(geo.materials);
    
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

void GeoClear(Geo &geo){
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

void SaveArray(ofstream *file, const Array &a) {
    unsigned int length;
    const void *data;
    
    length = ArrayLength(a);
    data   = ArrayPtr(a);
    file->write((char *)&length, sizeof(unsigned int));
    file->write((char *)data, ArrayBytes(a));
}

void SaveArrayIndices(ofstream *file, const Array &a) {
    unsigned int length = ArrayLength(a);
    file->write((char *)&length, sizeof(unsigned int));
    for (int i=0; i<length; i++) {
        const Array *indices = ArrayAtPtr<Array>(a, i);
        SaveArray(file, *indices);
    }
}

void SaveArrayMats(ofstream *file, const Array &a) {
    unsigned int length = ArrayLength(a);
    file->write((char *)&length, sizeof(unsigned int));
    for (int i=0; i<length; i++) {
        Material mat = ArrayAt<Material>(a, i);
        
        size_t nameLen = strlen(mat.name);
        file->write((char *)&nameLen, sizeof(size_t));
        file->write(mat.name, sizeof(char)*nameLen);
        
        size_t textureLen = 0;
        if (mat.texture == NULL)
            file->write((char *)&textureLen, sizeof(size_t));
        else {
            textureLen = strlen(mat.texture);
            file->write((char *)&textureLen, sizeof(size_t));
            file->write(mat.texture, sizeof(char)*textureLen);
        }
        
        file->write((char *)&mat.amb, sizeof(RGBA));
        file->write((char *)&mat.dif, sizeof(RGBA));
        file->write((char *)&mat.spe, sizeof(RGBA));
        
        file->write((char *)&mat.bright, sizeof(int));
        file->write((char *)&mat.illum, sizeof(int));
    }
}

void SaveIndexedBuffers(string fileName, const Array &vertices, const Array &normals, const Array &texCoords,
                        const Array &indices, const Array &materials) {
    
    ofstream * file = new ofstream(fileName.c_str(), ios::out|ios::binary|ios::trunc);
    
    if (file && file->is_open())
	{
        SaveArray(file, vertices);
        SaveArray(file, normals);
        SaveArray(file, texCoords);
        
        SaveArrayIndices(file, indices);
        SaveArrayMats(file, materials);
        
		file->close();
	}
	
	if (file)
		delete file;
}

void LoadArray(ifstream *file, Array &a, size_t elementSize) {
    unsigned int length;
    void *data = NULL;
    
    file->read((char *)&length, sizeof(unsigned int));
    data = malloc(elementSize*length);
    file->read((char *)data, elementSize*length);
    
    a.buffer = data;
    a.elementSize = elementSize;
    a.elements = length;
    a.capacity = length;
}

void LoadArrayIndices(ifstream *file, Array &a) {
    unsigned int length;
    file->read((char *)&length, sizeof(unsigned int));
    
    for (int i=0; i<length; i++) {
        Array indices = ArrayCreate<unsigned int>(1000);
        LoadArray(file, indices, sizeof(unsigned int));
        ArrayAdd(a, &indices);
    }
}

void LoadArrayMats(ifstream *file, Array &a) {
    unsigned int length;
    file->read((char *)&length, sizeof(unsigned int));
    
    for (int i=0; i<length; i++) {
        Material mat;
        
        size_t nameLen = 0;
        file->read((char *)&nameLen, sizeof(size_t));
        mat.name = (char *)malloc(sizeof(char)*nameLen+1);
        file->read(mat.name, sizeof(char)*nameLen);
        mat.name[nameLen] = '\0';
        
        size_t textureLen = 0;
        file->read((char *)&textureLen, sizeof(size_t));
        if (textureLen == 0)
            mat.texture = NULL;
        else {
            mat.texture = (char *)malloc(sizeof(char)*textureLen+1);
            file->read(mat.texture, sizeof(char)*textureLen);
            mat.texture[textureLen] = '\0';
        }
        
        file->read((char *)&mat.amb, sizeof(RGBA));
        file->read((char *)&mat.dif, sizeof(RGBA));
        file->read((char *)&mat.spe, sizeof(RGBA));
        
        file->read((char *)&mat.bright, sizeof(int));
        file->read((char *)&mat.illum, sizeof(int));
        
        mat.texID = -1;
        
        ArrayAdd(a, &mat);
    }
}

void LoadIndexedBuffers(string fileName, Array &vertices, Array &normals, Array &texCoords,
                        Array &indices, Array &materials)
{
	ifstream * file = new ifstream(fileName.c_str(), ios::in|ios::binary);
	
	if (file && file->is_open())
	{
        LoadArray(file, vertices, sizeof(Vec3D));
        LoadArray(file, normals, sizeof(Vec3D));
        LoadArray(file, texCoords, sizeof(Vec2D));
        
        LoadArrayIndices(file, indices);
        LoadArrayMats(file, materials);
		
		file->close();
	}
	else
	{
		printf("Unable to open the geo file");
	}
    
    if (file)
        delete file;
}

bool AreEqual(float a, float b, float delta = 0.01f){
    return fabs(a-b) < delta;
}

int ArraySearchPoint(const Vec3D &vertex, const Vec3D &normal, const Vec2D &texCoord,
                   const Array &outVertices, const Array &outNormals, const Array &outTexCoords) {
    unsigned int i;
    
    for (i=0; i<ArrayLength(outVertices); i++) {
        
        Vec3D outVertex = ArrayAt<Vec3D>(outVertices, i);
        Vec3D outNormal = ArrayAt<Vec3D>(outNormals, i);
        Vec2D outTexCoord = ArrayAt<Vec2D>(outTexCoords, i);
        
        if (AreEqual(vertex.x, outVertex.x) && AreEqual(vertex.y, outVertex.y) && AreEqual(vertex.z, outVertex.z) &&
            AreEqual(normal.x, outNormal.x) && AreEqual(normal.y, outNormal.y) && AreEqual(normal.z, outNormal.z) &&
            AreEqual(texCoord.u, outTexCoord.u) && AreEqual(texCoord.v, outTexCoord.v)) {
                return i;
            }
    }
        
    return -1;
}

void CreateIndexedBuffers(const Array &inVertices, const Array &inNormals, const Array &inTexCoords, const Array &inFaces,
                          Array &outVertices, Array &outNormals, Array &outTexCoords, Array &outIndices)
{
    unsigned int i, j, numIndices;
    
    numIndices = 0;
    for (i=0; i<ArrayLength(inFaces); i++) {
        Face face = ArrayAt<Face>(inFaces, i);
        for (j=0; j<ArrayLength(face.points); j++) {

            Array *indices = ArrayAtPtr<Array>(outIndices, face.IdMaterial-1);
            Point p = ArrayAt<Point>(face.points, j);
            
            Vec3D vertex = ArrayAt<Vec3D>(inVertices, p.IdVertex-1);
            Vec3D normal = ArrayAt<Vec3D>(inNormals, p.IdNormal-1);
            Vec2D texCoord = ArrayAt<Vec2D>(inTexCoords, p.IdTexCoord-1);
            
            int index = ArraySearchPoint(vertex, normal, texCoord, outVertices, outNormals, outTexCoords);
            if (index >= 0) {
                ArrayAdd(*indices, &index);
            } else {
                ArrayAdd(outVertices, &vertex);
                ArrayAdd(outNormals, &normal);
                ArrayAdd(outTexCoords, &texCoord);
                index = ArrayLength(outVertices)-1;
                ArrayAdd(*indices, &index);
            }
        }
    }
}

void GeoFillVBOBuffers(Geo &geo, Array &outVertices, Array &outNormals, Array &outTexCoords, Array &outIndices) {
    glGenBuffers(1, &geo.vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, geo.vboVertices);
    glBufferData(GL_ARRAY_BUFFER, ArrayBytes(outVertices), ArrayPtr(outVertices), GL_STATIC_DRAW);
    
    glGenBuffers(1, &geo.vboNormals);
    glBindBuffer(GL_ARRAY_BUFFER, geo.vboNormals);
    glBufferData(GL_ARRAY_BUFFER, ArrayBytes(outNormals), ArrayPtr(outNormals), GL_STATIC_DRAW);
    
    glGenBuffers(1, &geo.vboTexCoords);
    glBindBuffer(GL_ARRAY_BUFFER, geo.vboTexCoords);
    glBufferData(GL_ARRAY_BUFFER, ArrayBytes(outTexCoords), ArrayPtr(outTexCoords), GL_STATIC_DRAW);
    
    // vboIndices
    geo.numIndices = ArrayCreate<unsigned int>(10);
    geo.vboIndices = (GLuint *) malloc(sizeof(GLuint) * ArrayLength(outIndices));
    glGenBuffers(ArrayLength(outIndices), geo.vboIndices);
    for (int i=0; i<ArrayLength(outIndices); i++) {
        Array indices = ArrayAt<Array>(outIndices, i);
        unsigned int length = ArrayLength(indices);
        ArrayAdd(geo.numIndices, &length);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geo.vboIndices[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, ArrayBytes(indices), ArrayPtr(indices), GL_STATIC_DRAW);
    }
}

void GeoClearVBOArrays(Array &outVertices, Array &outNormals, Array &outTexCoords, Array &outIndices) {
    for (int i=0; i<ArrayLength(outIndices); i++) {
        Array indices = ArrayAt<Array>(outIndices, i);
        ArrayClear(indices);
    }
    ArrayClear(outVertices);
    ArrayClear(outNormals);
    ArrayClear(outTexCoords);
    ArrayClear(outIndices);
}

void GeoCreateVBO(Geo &geo) {
    Array outVertices = ArrayCreate<Vec3D>(300);
    Array outNormals  = ArrayCreate<Vec3D>(300);
    Array outTexCoords = ArrayCreate<Vec2D>(300);
    Array outIndices = ArrayCreate<Array>(10);
    for (int i=0; i<ArrayLength(geo.materials); i++) {
        Array indices = ArrayCreate<unsigned int>(1000);
        ArrayAdd(outIndices, &indices);
    }
    
    CreateIndexedBuffers(geo.vertices, geo.normals, geo.texCoords, geo.faces,
                         outVertices, outNormals, outTexCoords, outIndices);
    GeoFillVBOBuffers(geo, outVertices, outNormals, outTexCoords, outIndices);
    GeoClearVBOArrays(outVertices, outNormals, outTexCoords, outIndices);
}

void DrawVBO(const Geo &geo) {
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    
    glBindBuffer(GL_ARRAY_BUFFER, geo.vboVertices);
    glVertexPointer(3, GL_FLOAT, 0, 0);
    
    glBindBuffer(GL_ARRAY_BUFFER, geo.vboNormals);
    glNormalPointer(GL_FLOAT, 0, 0);
    
    glBindBuffer(GL_ARRAY_BUFFER, geo.vboTexCoords);
    glTexCoordPointer(2, GL_FLOAT, 0, 0);
    
    for (int i=0; i<ArrayLength(geo.numIndices); i++) {
        MatApply(ArrayAt<Material>(geo.materials, i));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geo.vboIndices[i]);
        unsigned int numIndices = ArrayAt<unsigned int>(geo.numIndices, i);
        glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
    }
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void DrawImmediate(const Geo &geo){
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

void GeoDraw(const Geo &geo){
    
    if (geo.vboIndices != NULL)
        DrawVBO(geo);
    else
        DrawImmediate(geo);
    
}

void GeoScale(Geo &geo, float s){
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
