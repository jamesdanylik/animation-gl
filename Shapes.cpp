#include "Angel/Angel.h"
#include "Shapes.h"
#include <cmath>

typedef Angel::vec4 color4;
typedef Angel::vec4 point4;
typedef Angel::vec3 point3;
typedef Angel::vec2 point2;

void setVertexAttrib(GLuint program, 
                     GLfloat* points,    GLsizeiptr psize, 
                     GLfloat* normals,   GLsizeiptr nsize,
                     GLfloat* texcoords, GLsizeiptr tsize)
{
    GLuint buffer[2];
    glGenBuffers( 2, buffer );

    glBindBuffer( GL_ARRAY_BUFFER, buffer[0] );
    glBufferData( GL_ARRAY_BUFFER, psize, points, GL_STATIC_DRAW );
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

	glBindBuffer( GL_ARRAY_BUFFER, buffer[1] );
    glBufferData( GL_ARRAY_BUFFER, nsize, normals, GL_STATIC_DRAW );
    GLuint vNormal = glGetAttribLocation( program, "vNormal" );
    glEnableVertexAttribArray( vNormal );
    glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

    if (texcoords)
    {
        GLuint tbuffer;
        glGenBuffers( 1, &tbuffer );
        glBindBuffer( GL_ARRAY_BUFFER, tbuffer );
        glBufferData( GL_ARRAY_BUFFER, tsize, texcoords, GL_STATIC_DRAW );
        GLuint vTexCoords = glGetAttribLocation( program, "vTexCoords" );
        glEnableVertexAttribArray( vTexCoords );
        glVertexAttribPointer( vTexCoords, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
    }
    
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindVertexArray(0);
}


//----------------------------------------------------------------------------
// Cube (Uniform)

const int numCubeVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

point4 cubePoints [numCubeVertices];
point3 cubeNormals[numCubeVertices];
point2 cubeUV     [numCubeVertices];

// Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertices[8] = {
    point4( -0.5, -0.5,  0.5, 1.0 ),
    point4( -0.5,  0.5,  0.5, 1.0 ),
    point4(  0.5,  0.5,  0.5, 1.0 ),
    point4(  0.5, -0.5,  0.5, 1.0 ),
    point4( -0.5, -0.5, -0.5, 1.0 ),
    point4( -0.5,  0.5, -0.5, 1.0 ),
    point4(  0.5,  0.5, -0.5, 1.0 ),
    point4(  0.5, -0.5, -0.5, 1.0 )
};

// quad generates two triangles for each face and assigns normals and texture coordinates
//    to the vertices
int Index = 0;
void quad( int a, int b, int c, int d, const point3& normal)
{
    cubePoints[Index] = vertices[a]; cubeNormals[Index] = normal; 
    cubeUV[Index] = point2(0.0f, 1.0f); Index++;
    cubePoints[Index] = vertices[b]; cubeNormals[Index] = normal;
    cubeUV[Index] = point2(0.0f, 0.0f); Index++;
    cubePoints[Index] = vertices[c]; cubeNormals[Index] = normal;
    cubeUV[Index] = point2(1.0f, 0.0f); Index++;
    cubePoints[Index] = vertices[a]; cubeNormals[Index] = normal;
    cubeUV[Index] = point2(0.0f, 1.0f); Index++;
    cubePoints[Index] = vertices[c]; cubeNormals[Index] = normal;
    cubeUV[Index] = point2(1.0f, 0.0f); Index++;
    cubePoints[Index] = vertices[d]; cubeNormals[Index] = normal;
    cubeUV[Index] = point2(1.0f, 1.0f); Index++;
}

// generate 12 triangles: 36 vertices, 36 normals, 36 texture coordinates
void colorcube()
{
    quad( 1, 0, 3, 2, point3( 0.0f,  0.0f,  1.0f) );
    quad( 2, 3, 7, 6, point3( 1.0f,  0.0f,  0.0f) );
    quad( 3, 0, 4, 7, point3( 0.0f, -1.0f,  0.0f) );
    quad( 6, 5, 1, 2, point3( 0.0f,  1.0f,  0.0f) );
    quad( 4, 5, 6, 7, point3( 0.0f,  0.0f, -1.0f) );
    quad( 5, 4, 0, 1, point3(-1.0f,  0.0f,  0.0f) );
}

// initialization
void generateCube(GLuint program, ShapeData* cubeData)
{
    colorcube();
    cubeData->numVertices = numCubeVertices;

    // Create a vertex array object
    glGenVertexArrays( 1, &cubeData->vao );
    glBindVertexArray( cubeData->vao );

    // Set vertex attributes
    setVertexAttrib(program, 
        (float*)cubePoints,  sizeof(cubePoints), 
        (float*)cubeNormals, sizeof(cubeNormals),
        (float*)cubeUV,      sizeof(cubeUV));
}

// Cube (Multi-face)

const int numMCubeVerticies = 36;
point4	mCubePoints		[numMCubeVerticies];
point3	mCubeNormals	[numMCubeVerticies];
point2	mCubeUV			[numMCubeVerticies];

point4 mVertices[8] = vertices;

int mIndex = 0;
void mQuad( int a, int b, int c, int d, const point3& normal, int face)
{
    mCubePoints[mIndex] = mVertices[a]; mCubeNormals[mIndex] = normal;
    mCubeUV[mIndex] = point2(face*(1.0/6), 1.0f); mIndex++;
    mCubePoints[mIndex] = mVertices[b]; mCubeNormals[mIndex] = normal;
    mCubeUV[mIndex] = point2(face*(1.0/6), 0.0f); mIndex++;
    mCubePoints[mIndex] = mVertices[c]; mCubeNormals[mIndex] = normal;
    mCubeUV[mIndex] = point2((face+1)*(1.0/6), 0.0f); mIndex++;
    mCubePoints[mIndex] = mVertices[a]; mCubeNormals[mIndex] = normal;
    mCubeUV[mIndex] = point2(face*(1.0/6), 1.0f); mIndex++;
    mCubePoints[mIndex] = mVertices[c]; mCubeNormals[mIndex] = normal;
    mCubeUV[mIndex] = point2((face+1)*(1.0/6), 0.0f); mIndex++;
    mCubePoints[mIndex] = mVertices[d]; mCubeNormals[mIndex] = normal;
    mCubeUV[mIndex] = point2((face+1)*(1.0/6), 1.0f); mIndex++;
}

void mColorcube()
{
    mQuad( 1, 0, 3, 2, point3( 0.0f,  0.0f,  1.0f), 0 );
    mQuad( 2, 3, 7, 6, point3( 1.0f,  0.0f,  0.0f), 1 );
    mQuad( 3, 0, 4, 7, point3( 0.0f, -1.0f,  0.0f), 2 );
    mQuad( 6, 5, 1, 2, point3( 0.0f,  1.0f,  0.0f), 3 );
    mQuad( 4, 5, 6, 7, point3( 0.0f,  0.0f, -1.0f), 4 );
    mQuad( 5, 4, 0, 1, point3(-1.0f,  0.0f,  0.0f), 5 );
}

// initialization
void generateMCube(GLuint program, ShapeData* cubeData)
{
    mColorcube();
    cubeData->numVertices = numMCubeVerticies;

    // Create a vertex array object
    glGenVertexArrays( 1, &cubeData->vao );
    glBindVertexArray( cubeData->vao );

    // Set vertex attributes
    setVertexAttrib(program,
        (float*)mCubePoints,  sizeof(mCubePoints),
        (float*)mCubeNormals, sizeof(mCubeNormals),
        (float*)mCubeUV,      sizeof(mCubeUV));
}

// Wedge ////////////////////////////////////////////////////////////

const int numWedgeVertices = 24; //((3 faces)(2 traingles) 
            //+ (2 faces)(1 triangle))*(3 points/triangle)

point4 wedgePoints	[numWedgeVertices];
point3 wedgeNormals	[numWedgeVertices];
point2 wedgeUV		[numWedgeVertices];

int wIndex = 0;
void wQuad( int a, int b, int c, int d, const point3 &normal, int face)
{
	wedgePoints[wIndex] = vertices [a]; wedgeNormals[wIndex] = normal;
	wedgeUV[wIndex] = point2(face*(1.0/4), 1.0f); wIndex++;
    wedgePoints[wIndex] = vertices [b]; wedgeNormals[wIndex] = normal;
    wedgeUV[wIndex] = point2(face*(1.0/4), 0.0f); wIndex++;
    wedgePoints[wIndex] = vertices [c]; wedgeNormals[wIndex] = normal;
    wedgeUV[wIndex] = point2((face+1)*(1.0/4), 0.0f); wIndex++;
    wedgePoints[wIndex] = vertices [a]; wedgeNormals[wIndex] = normal;
    wedgeUV[wIndex] = point2(face*(1.0/4), 1.0f); wIndex++;
    wedgePoints[wIndex] = vertices [c]; wedgeNormals[wIndex] = normal;
    wedgeUV[wIndex] = point2((face+1)*(1.0/4), 0.0f); wIndex++;
    wedgePoints[wIndex] = vertices [d]; wedgeNormals[wIndex] = normal;
    wedgeUV[wIndex] = point2((face+1)*(1.0/4), 1.0f); wIndex++;
}

void wTriangle( int a, int b, int c, const point3 &normal)
{
	wedgePoints[wIndex] = vertices[a]; wedgeNormals[wIndex] = normal;
	wedgeUV[wIndex] = point2(3.0/4, 1.0f); wIndex++;
    wedgePoints[wIndex] = vertices[b]; wedgeNormals[wIndex] = normal;
    wedgeUV[wIndex] = point2(3.0/4, 0.0f); wIndex++;
    wedgePoints[wIndex] = vertices[c]; wedgeNormals[wIndex] = normal;
    wedgeUV[wIndex] = point2(1.0f, 1.0f); wIndex++;

}

void generateWedge(GLuint program, ShapeData *wedgeData)
{
	wQuad( 1, 0, 3, 2, point3( 0.0f, 0.0f, 1.0f), 0);
	wQuad( 2, 3, 7, 6, point3( 1.0f, 0.0f, 0.0f), 1);
	wQuad( 6, 7, 0, 1, point3( -sqrt(1.0/2), 0.0f, -sqrt(1.0/2)), 2);
	wTriangle(1, 2, 6, point3(0.0f, 1.0f, 0.0f));
	wTriangle(0, 3, 7, point3(0.0f, -1.0f,0.0f));

	wedgeData->numVertices = numWedgeVertices;

	glGenVertexArrays( 1, &wedgeData->vao);
	glBindVertexArray( wedgeData->vao);
	
	setVertexAttrib( program, 
        (float*)wedgePoints,  sizeof(wedgePoints),
        (float*)wedgeNormals, sizeof(wedgeNormals),
        (float*)wedgeUV,      sizeof(wedgeUV));

}

// Pyramid //////////////////////////////////////////////////////////
/*
const int numPyrVerticies = 18; //( (1 face)(2 triangles) + (4 faces)(1 triangle) ) 
								//* 3 points/traingle

point4 pVertices[5] 

point4 pyrPoints	[numPyrVerticies];
point3 pyrNormals	[numPyrVerticies];
point2 pyrUV		[numPyrVerticies];



int pIndex = 0;

void pTriangle( int a, int b, int c, const point3 &normal)
{
	pyrPoints[pIndex] = 
}
*/

//----------------------------------------------------------------------------
// Sphere approximation by recursive subdivision of a tetrahedron

const int N = 5;  // number of subdivisions
const int numSphereVertices = 16*256*3;  // number of resulting points

point4 spherePoints [numSphereVertices];
point3 sphereNormals[numSphereVertices];
point2 sphereUVs[numSphereVertices];

// four equally spaced points on the unit circle

point4 v[4] = {
    vec4(0.0, 0.0, 1.0, 1.0), 
    vec4(0.0, 0.942809, -0.333333, 1.0),
    vec4(-0.816497, -0.471405, -0.333333, 1.0),
    vec4(0.816497, -0.471405, -0.333333, 1.0)
};

static int k = 0;

// move a point to unit circle

point4 unit(const point4 &p)
{
    point4 c;
    double d=0.0;
    for(int i=0; i<3; i++) d+=p[i]*p[i];
    d=sqrt(d);
    if(d > 0.0) for(int i=0; i<3; i++) c[i] = p[i]/d;
    c[3] = 1.0;
    return c;
}

void triangle(point4 a, point4 b, point4 c)
{
    spherePoints[k] = a;
    k++;
    spherePoints[k] = b;
    k++;
    spherePoints[k] = c;
    k++;
}

void divide_triangle(point4 a, point4 b, point4 c, int n)
{
    point4 v1, v2, v3;
    if(n>0)
    {
        v1 = unit(a + b);
        v2 = unit(a + c);
        v3 = unit(b + c);   
        divide_triangle(a , v2, v1, n-1);
        divide_triangle(c , v3, v2, n-1);
        divide_triangle(b , v1, v3, n-1);
        divide_triangle(v1, v2, v3, n-1);
    }
    else triangle(a, b, c);
}

void tetrahedron(int n)
{
    divide_triangle(v[0], v[1], v[2], n);
    divide_triangle(v[3], v[2], v[1], n);
    divide_triangle(v[0], v[3], v[1], n);
    divide_triangle(v[0], v[3], v[2], n);
}

// initialization

void generateSphere(GLuint program, ShapeData* sphereData)
{
    tetrahedron(N);

    sphereData->numVertices = numSphereVertices;

    // Normals
    for (int i = 0; i < numSphereVertices; i++)
    {
        sphereNormals[i] = point3(spherePoints[i].x, spherePoints[i].y, spherePoints[i].z);
    }
    
    // TexCoords
    double u1, v1, u2, v2, u3, v3;
    for (int i = 0; i < (numSphereVertices/3); i++)
    {

		// Fix for bad UV mapping before!  Huzzah!
        v1 = 0.5 - asin(spherePoints[i*3].y)/M_PI ; //0~1
        u1 = 0.5*(atan2(spherePoints[i*3].z,spherePoints[i*3].x)/M_PI + 1); //0~1

		v2 = 0.5 - asin(spherePoints[(i*3)+1].y)/M_PI;
		u2 = 0.5*(atan2(spherePoints[(i*3)+1].z, spherePoints[(i*3)+1].x)/M_PI + 1);
		if ( u2 < 0.75 && u1 > 0.75 && u1-u2 > .2 )
			u2 += 1.0;
		else if ( u2 > 0.75 && u1 < 0.75 && u2-u1 > .2)
			u2 -= 1.0;


		v3 = 0.5 - asin(spherePoints[(i*3)+2].y)/M_PI;
		u3 = 0.5*(atan2(spherePoints[(i*3)+2].z, spherePoints[(i*3)+2].x)/M_PI + 1);
		if ( u3 < 0.75 && u2 > 0.75 && u2-u3 > .2)
			u3 += 1.0;
		else if (u3 > 0.75 && u2 < 0.75 && u3-u2 > .2)
			u3 -= 1.0;

        sphereUVs[i*3] = point2(u1,v1);
		sphereUVs[(i*3)+1] = point2(u2,v2);
		sphereUVs[(i*3)+2] = point2(u3,v3);
    }

    // Create a vertex array object
    glGenVertexArrays( 1, &sphereData->vao );
    glBindVertexArray( sphereData->vao );

    // Set vertex attributes
    setVertexAttrib(program,
        (float*)spherePoints,  sizeof(spherePoints),
        (float*)sphereNormals, sizeof(sphereNormals),
        (float*)sphereUVs, sizeof(sphereUVs));
}

//----------------------------------------------------------------------------
// Cone

const int numConeDivisions = 32;
const int numConeVertices = numConeDivisions * 6;

point4 conePoints [numConeVertices];
point3 coneNormals[numConeVertices];
//point2 coneUVs[numConeVertices];

point2 circlePoints[numConeDivisions];

void makeCircle(point2* dest, int numDivisions)
{
    for (int i = 0; i < numDivisions; i++)
    {
        float a = i * 2.0f * M_PI / numDivisions;
        dest[i] = point2(cosf(a), sinf(a));
    }
}

void makeConeWall(point4* destp, point3* destn, int numDivisions, float z1, float z2, int& Index, int dir)
{
    for (int i = 0; i < numDivisions; i++)
    {
        point3 p1(circlePoints[i].x, circlePoints[i].y, z1);
        point3 p2(0.0f, 0.0f, z2);
        point3 p3(circlePoints[(i+1)%numDivisions].x, circlePoints[(i+1)%numDivisions].y, z1);
        if (dir == -1)
        {
            point3 temp = p1;
            p1 = p3;
            p3 = temp;
        }
        point3 n = cross(p1-p2, p3-p2);
        destp[Index] = p1; destn[Index] = n; Index++;
        destp[Index] = p2; destn[Index] = n; Index++;
        destp[Index] = p3; destn[Index] = n; Index++;
    }    
}

void generateCone(GLuint program, ShapeData* coneData)
{
    makeCircle(circlePoints, numConeDivisions);
    int Index = 0;
    makeConeWall(conePoints, coneNormals, numConeDivisions, 1.0f, 1.0f, Index, 1);
    makeConeWall(conePoints, coneNormals, numConeDivisions, 1.0f, -1.0f, Index, -1);
    
    coneData->numVertices = numConeVertices;

    // Create a vertex array object
    glGenVertexArrays( 1, &coneData->vao );
    glBindVertexArray( coneData->vao );
    
    // Set vertex attributes
    setVertexAttrib(program,
                    (float*)conePoints,  sizeof(conePoints),
                    (float*)coneNormals, sizeof(coneNormals),
                    0, 0);
}

//----------------------------------------------------------------------------
// Cylinder

const int numCylDivisions = 32;
const int numCylVertices = numCylDivisions * 12;

point4 cylPoints [numCylVertices];
point3 cylNormals[numCylVertices];
//point2 cylUVs[numCylVerticies];

void generateCylinder(GLuint program, ShapeData* cylData)
{
    makeCircle(circlePoints, numCylDivisions);
    int Index = 0;
    makeConeWall(cylPoints, cylNormals, numCylDivisions, 1.0f, 1.0f, Index, 1);
    makeConeWall(cylPoints, cylNormals, numCylDivisions, -1.0f, -1.0f, Index, -1);
    
    for (int i = 0; i < numCylDivisions; i++)
    {
        int i2 = (i+1)%numCylDivisions;
        point3 p1(circlePoints[i2].x, circlePoints[i2].y, -1.0f);
        point3 p2(circlePoints[i2].x, circlePoints[i2].y, 1.0f);
        point3 p3(circlePoints[i].x,  circlePoints[i].y,  1.0f);
        //point3 n = cross(p3-p2, p1-p2);
        cylPoints[Index] = p1; cylNormals[Index] = point3(p1.x, p1.y, 0.0f); Index++;
        cylPoints[Index] = p2; cylNormals[Index] = point3(p2.x, p2.y, 0.0f); Index++;
        cylPoints[Index] = p3; cylNormals[Index] = point3(p3.x, p3.y, 0.0f); Index++;
        p1 = point3(circlePoints[i2].x, circlePoints[i2].y, -1.0f);
        p2 = point3(circlePoints[i].x,  circlePoints[i].y,  1.0f);
        p3 = point3(circlePoints[i].x,  circlePoints[i].y,  -1.0f);
        //n = cross(p3-p2, p1-p2);
        cylPoints[Index] = p1; cylNormals[Index] = point3(p1.x, p1.y, 0.0f); Index++;
        cylPoints[Index] = p2; cylNormals[Index] = point3(p2.x, p2.y, 0.0f); Index++;
        cylPoints[Index] = p3; cylNormals[Index] = point3(p3.x, p3.y, 0.0f); Index++;
    }
    
    cylData->numVertices = numCylVertices;
    
    // Create a vertex array object
    glGenVertexArrays( 1, &cylData->vao );
    glBindVertexArray( cylData->vao );
    
    // Set vertex attributes
    setVertexAttrib(program,
                    (float*)cylPoints,  sizeof(cylPoints),
                    (float*)cylNormals, sizeof(cylNormals),
                    0, 0);
}













