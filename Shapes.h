#ifndef _SHAPES_H_
#define _SHAPES_H_

#ifdef __APPLE__
#define glGenVertexArrays glGenVertexArraysAPPLE
#define glBindVertexArray glBindVertexArrayAPPLE
#endif

struct ShapeData
{
    GLuint vao;
    int numVertices;
};

void generateCube(GLuint program, ShapeData* cubeData);
void generateMCube(GLuint program, ShapeData* cubeData);
void generateSphere(GLuint program, ShapeData* sphereData);
void generateCone(GLuint program, ShapeData* coneData);
void generateCylinder(GLuint program, ShapeData* cylData);
void generateWedge(GLuint program, ShapeData* wedgeData);
void generatePyramid(GLuint program, ShapeData* pyramidData);
void generateICube(GLuint program, ShapeData* iCubeData);
void generateISphere(GLuint program, ShapeData* iSphereData);
void generateDecal(GLuint program, ShapeData* decalData);
void generateSD(GLuint program, ShapeData* sDData);
void generateSPyramid(GLuint program, ShapeData* pyrSData);
#endif
