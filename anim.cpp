////////////////////////////////////////////////////
// animation-gl
// from template anim.cpp, version 4.1
////////////////////////////////////////////////////

/* OpenGL Includes */
#ifdef WIN32
#include <windows.h>
#include "GL/glew.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "GL/freeglut.h"
#elif __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#define glutInitContextVersion(a,b)
#define glutInitContextProfile(a)
#define glewExperimental int glewExperimentalAPPLE
#define glewInit()
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

/* System Includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

/* Application Includes */
#include "Ball.h"
#include "FrameSaver.h"
#include "Timer.h"
#include "Shapes.h"
#include "Tga.h"
#include "Angel/Angel.h"
#include "Angel/MatrixStack.h"

/* Global Variables - Courtesy of the template. */
// Framsaver Variables
FrameSaver FrSaver ;
Timer TM ;
// Arcball Variables
BallData *Arcball = NULL ;
int Width = 800;
int Height = 800 ;
int Button = -1 ;
float Zoom = 1 ;
int PrevY = 0 ;
// Global Program Flags
int Animate = 0 ;
int Recording = 0 ;
// Function Declarations
void resetArcball() ;
void save_image();
void instructions();
void set_colour(float r, float g, float b);
void load_textures();
// String Type Declation?
const int STRLEN = 100;
typedef char STR[STRLEN];
// Global Constants
#define PI 3.1415926535897
#define X 0
#define Y 1
#define Z 2
//texture
GLuint texture_cube;
GLuint texture_earth;
GLuint texture_bump;
// Structs that hold the Vertex Array Object index and number of vertices of each shape.
ShapeData cubeData;
ShapeData sphereData;
ShapeData coneData;
ShapeData cylData;
// Matrix Stack delcaration and shader variables.
MatrixStack  mvstack;
mat4         model_view;
GLint        uModelView, uProjection, uView;
GLint        uAmbient, uDiffuse, uSpecular, uLightPos, uShininess;
GLint        uTex, uEnableTex, uBumpTex, uEnableBumpTex;
// The eye point and look-at point.
// Currently unused. Use to control a camera with LookAt().
Angel::vec4 eye{0, 0.0, 50.0,1.0};
Angel::vec4 ref{0.0, 0.0, 0.0,1.0};
Angel::vec4 up{0.0,1.0,0.0,0.0};
// Time variable
double TIME = 0.0 ;

/* Function Implmentations */

// Render a solid cylinder  oriented along the Z axis. Both bases are of radius 1. 
// The bases of the cylinder are placed at Z = 0, and at Z = 1
void drawCylinder(void)
{
        glBindTexture( GL_TEXTURE_2D, texture_cube );
        glUniform1i( uEnableTex, 1 );
    glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view );
    glBindVertexArray( cylData.vao );
    glDrawArrays( GL_TRIANGLES, 0, cylData.numVertices );
	glUniform1i( uEnableTex, 0);

}

// Render a solid cone oriented along the Z axis with base radius 1. 
// The base of the cone is placed at Z = 0, and the top at Z = 1. 
void drawCone(void)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture( GL_TEXTURE_2D, texture_cube );
	glUniform1i( uEnableTex, 1 );
    glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view );
    glBindVertexArray( coneData.vao );
    glDrawArrays( GL_TRIANGLES, 0, coneData.numVertices );
	glUniform1i( uEnableTex, 0);
}


// this function draws a cube with dimensions 1,1,1
// centered around the origin.
void drawCube(void)
{
	glActiveTexture(GL_TEXTURE0);
    glBindTexture( GL_TEXTURE_2D, texture_cube );
    glUniform1i( uEnableTex, 1 );
	glActiveTexture(GL_TEXTURE1);
	glBindTexture( GL_TEXTURE_2D, texture_bump );
	glUniform1i ( uEnableBumpTex, 1);

    glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view );
    glBindVertexArray( cubeData.vao );
    glDrawArrays( GL_TRIANGLES, 0, cubeData.numVertices );
    glUniform1i( uEnableTex, 0 );
	glUniform1i( uEnableBumpTex, 0);
	glActiveTexture(GL_TEXTURE0);
}


// This function draws a sphere with radius 1
// centered around the origin.
void drawSphere(void)
{
    glBindTexture( GL_TEXTURE_2D, texture_earth);
    glUniform1i( uEnableTex, 1);
    glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view );
    glBindVertexArray( sphereData.vao );
    glDrawArrays( GL_TRIANGLES, 0, sphereData.numVertices );
    glUniform1i( uEnableTex, 0 );
}

// Resets the mouse to starting posisition?
void resetArcball()
{
    Ball_Init(Arcball);
    Ball_Place(Arcball,qOne,0.75);
}

// Key handler
void myKey(unsigned char key, int x, int y)
{
    float time ;
    switch (key) {
        case 'q':
        case 27:
            exit(0); 
        case 's':
            FrSaver.DumpPPM(Width,Height) ;
            break;
        case 'r':
            resetArcball() ;
            glutPostRedisplay() ;
            break ;
        case 'a': // togle animation
            Animate = 1 - Animate ;
            // reset the timer to point to the current time		
            time = TM.GetElapsedTime() ;
            TM.Reset() ;
            // printf("Elapsed time %f\n", time) ;
            break ;
        case '0':
            //reset your object
            break ;
        case 'm':
            if( Recording == 1 )
            {
                printf("Frame recording disabled.\n") ;
                Recording = 0 ;
            }
            else
            {
                printf("Frame recording enabled.\n") ;
                Recording = 1  ;
            }
            FrSaver.Toggle(Width);
            break ;
        case 'h':
        case '?':
            instructions();
            break;
    }
    glutPostRedisplay() ;

}

void load_textures(void)
{
	TgaImage coolImage;
    if (!coolImage.loadTGA("Textures/challenge.tga"))
    {
        printf("Error loading image file\n");
        exit(1);
    }
    
    TgaImage earthImage;
    if (!earthImage.loadTGA("Textures/earth.tga"))
    {
        printf("Error loading image file\n");
        exit(1);
    }


	TgaImage bumpImage;
	if(!bumpImage.loadTGA("Textures/bump.tga"))
	{
		printf("Error loading bump map file\n");
		exit(1);
	}
    
	glActiveTexture(GL_TEXTURE0);
    glGenTextures( 1, &texture_cube );
    glBindTexture( GL_TEXTURE_2D, texture_cube );
    
    glTexImage2D(GL_TEXTURE_2D, 0, 4, coolImage.width, coolImage.height, 0,
                 (coolImage.byteCount == 3) ? GL_BGR : GL_BGRA,
                 GL_UNSIGNED_BYTE, coolImage.data );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    
    
    glGenTextures( 1, &texture_earth );
    glBindTexture( GL_TEXTURE_2D, texture_earth );
    
    glTexImage2D(GL_TEXTURE_2D, 0, 4, earthImage.width, earthImage.height, 0,
                 (earthImage.byteCount == 3) ? GL_BGR : GL_BGRA,
                 GL_UNSIGNED_BYTE, earthImage.data );
    
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    
    // Set texture sampler variable to texture unit 0
    // (set in glActiveTexture(GL_TEXTURE0))
   
	glActiveTexture(GL_TEXTURE1);
    glGenTextures( 1, &texture_bump );
    glBindTexture( GL_TEXTURE_2D, texture_bump );

    glTexImage2D(GL_TEXTURE_2D, 0, 4, bumpImage.width, bumpImage.height, 0,
                 (bumpImage.byteCount == 3) ? GL_BGR : GL_BGRA,
                 GL_UNSIGNED_BYTE, bumpImage.data );
	glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );


 
    glUniform1i( uTex, 0);
	glUniform1i( uBumpTex, 1);
}

// Initialization Function
void myinit(void)
{
    // Load shaders and use the resulting shader program
    GLuint program = InitShader( "Shaders/vshader.glsl", "Shaders/fshader.glsl" );
    glUseProgram(program);

    // Generate vertex arrays for geometric shapes
    generateCube(program, &cubeData);
    generateSphere(program, &sphereData);
    generateCone(program, &coneData);
    generateCylinder(program, &cylData);

    uModelView  = glGetUniformLocation( program, "ModelView"  );
    uProjection = glGetUniformLocation( program, "Projection" );
    uView       = glGetUniformLocation( program, "View"       );

    glClearColor( 0.1, 0.1, 0.2, 1.0 ); // dark blue background

    uAmbient   = glGetUniformLocation( program, "AmbientProduct"  );
    uDiffuse   = glGetUniformLocation( program, "DiffuseProduct"  );
    uSpecular  = glGetUniformLocation( program, "SpecularProduct" );
    uLightPos  = glGetUniformLocation( program, "LightPosition"   );
    uShininess = glGetUniformLocation( program, "Shininess"       );
    uTex       = glGetUniformLocation( program, "Tex"             );
    uEnableTex = glGetUniformLocation( program, "EnableTex"       );
	uBumpTex   = glGetUniformLocation( program, "BumpTex"		  );
	uEnableBumpTex = glGetUniformLocation( program, "EnableBumpTex");


    glUniform4f(uAmbient,    0.2f,  0.2f,  0.2f, 1.0f);
    glUniform4f(uDiffuse,    0.6f,  0.6f,  0.6f, 1.0f);
    glUniform4f(uSpecular,   0.2f,  0.2f,  0.2f, 1.0f);
    glUniform4f(uLightPos,  15.0f, 15.0f, 30.0f, 0.0f);
    glUniform1f(uShininess, 100.0f);

    glEnable(GL_DEPTH_TEST);

    load_textures();
    
    Arcball = new BallData;
    Ball_Init(Arcball);
    Ball_Place(Arcball,qOne,0.75);
}

// sets all material properties to the given colour -- don't change
void set_colour(float r, float g, float b)
{
    float ambient  = 0.2f;
    float diffuse  = 0.6f;
    float specular = 0.2f;
    glUniform4f(uAmbient,  ambient*r,  ambient*g,  ambient*b,  1.0f);
    glUniform4f(uDiffuse,  diffuse*r,  diffuse*g,  diffuse*b,  1.0f);
    glUniform4f(uSpecular, specular*r, specular*g, specular*b, 1.0f);
}

// Display Function (Primary Changes)
void display(void)
{
    // Clear the screen with the background colour (set in myinit)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    model_view = mat4(1.0f);
    
    
    model_view *= Translate(0.0f, 0.0f, -15.0f);
    HMatrix r;
    Ball_Value(Arcball,r);

    mat4 mat_arcball_rot(
        r[0][0], r[0][1], r[0][2], r[0][3],
        r[1][0], r[1][1], r[1][2], r[1][3],
        r[2][0], r[2][1], r[2][2], r[2][3],
        r[3][0], r[3][1], r[3][2], r[3][3]);
    model_view *= mat_arcball_rot;
    
    mat4 view = model_view;
    
    
    //model_view = Angel::LookAt(eye, ref, up);//just the view matrix;

    glUniformMatrix4fv( uView, 1, GL_TRUE, model_view );

    // Previously glScalef(Zoom, Zoom, Zoom);
    model_view *= Scale(Zoom);

    // Draw Something
    set_colour(0.8f, 0.8f, 0.8f);
    drawSphere();

    // Previously glTranslatef(3,0,0);
    model_view *= Translate(3.0f, 0.0f, 0.0f);

    // Previously glScalef(3,3,3);
    model_view *= Scale(3.0f, 3.0f, 3.0f);

    drawCube();

    // And extra shapes!
    model_view *= Scale(1.0f/3.0f, 1.0f/3.0f, 1.0f/3.0f);
    model_view *= Translate(3.0f, 0.0f, 0.0f);
    set_colour(1.0f, 1.0f, 0.0f);
    drawCone();

    model_view *= Translate(-9.0f, 0.0f, 0.0f);
    set_colour(1.0f, 1.0f, 1.0f);
    drawCylinder();


    glutSwapBuffers();
    if(Recording == 1)
        FrSaver.DumpPPM(Width, Height) ;
}

// Window Resize Handler
void myReshape(int w, int h)
{
    Width = w;
    Height = h;

    glViewport(0, 0, w, h);

    mat4 projection = Perspective(50.0f, (float)w/(float)h, 1.0f, 1000.0f);
    glUniformMatrix4fv( uProjection, 1, GL_TRUE, projection );
}

// Prints instructions in console on launch
void instructions() 
{
    printf("Press:\n");
    printf("  s to save the image\n");
    printf("  r to restore the original view.\n") ;
    printf("  0 to set it to the zero state.\n") ;
    printf("  a to toggle the animation.\n") ;
    printf("  m to toggle frame dumping.\n") ;
    printf("  q to quit.\n");
}

// start or end interaction
void myMouseCB(int button, int state, int x, int y)
{
    Button = button ;
    if( Button == GLUT_LEFT_BUTTON && state == GLUT_DOWN )
    {
        HVect arcball_coords;
        arcball_coords.x = 2.0*(float)x/(float)Width-1.0;
        arcball_coords.y = -2.0*(float)y/(float)Height+1.0;
        Ball_Mouse(Arcball, arcball_coords) ;
        Ball_Update(Arcball);
        Ball_BeginDrag(Arcball);

    }
    if( Button == GLUT_LEFT_BUTTON && state == GLUT_UP )
    {
        Ball_EndDrag(Arcball);
        Button = -1 ;
    }
    if( Button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN )
    {
        PrevY = y ;
    }


    // Tell the system to redraw the window
    glutPostRedisplay() ;
}

// interaction (mouse motion)
void myMotionCB(int x, int y)
{
    if( Button == GLUT_LEFT_BUTTON )
    {
        HVect arcball_coords;
        arcball_coords.x = 2.0*(float)x/(float)Width - 1.0 ;
        arcball_coords.y = -2.0*(float)y/(float)Height + 1.0 ;
        Ball_Mouse(Arcball,arcball_coords);
        Ball_Update(Arcball);
        glutPostRedisplay() ;
    }
    else if( Button == GLUT_RIGHT_BUTTON )
    {
        if( y - PrevY > 0 )
            Zoom  = Zoom * 1.03 ;
        else 
            Zoom  = Zoom * 0.97 ;
        PrevY = y ;
        glutPostRedisplay() ;
    }
}


void idleCB(void)
{
    if( Animate == 1 )
    {
        // TM.Reset() ; // commenting out this will make the time run from 0
        // leaving 'Time' counts the time interval between successive calls to idleCB
        if( Recording == 0 )
            TIME = TM.GetElapsedTime() ;
        else
            TIME += 0.033 ; // save at 30 frames per second.
        
        eye.x = 20*sin(TIME);
        eye.z = 20*cos(TIME);
        
        printf("TIME %f\n", TIME) ;
        glutPostRedisplay() ; 
    }
}

// Main Function, program starting point:
int main(int argc, char** argv) 
{
    glutInit(&argc, argv);
    // If your code fails to run, uncommenting these lines may help.
    //glutInitContextVersion(3, 2);
    //glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition (0, 0);
    glutInitWindowSize(Width,Height);
    glutCreateWindow(argv[0]);
    printf("GL version %s\n", glGetString(GL_VERSION));
    glewExperimental = GL_TRUE;
    glewInit();
    
    myinit();

    glutIdleFunc(idleCB) ;
    glutReshapeFunc (myReshape);
    glutKeyboardFunc( myKey );
    glutMouseFunc(myMouseCB) ;
    glutMotionFunc(myMotionCB) ;
    instructions();

    glutDisplayFunc(display);
    glutMainLoop();

    TM.Reset() ;
    return 0;         // never reached
}
