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
#include <string.h>
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
Timer TM ;  // the animation timer
Timer PTM;  // the program timer
float AV_FPS;
float INST_FPS;
float numFrames;
float last_time;
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

const int max_textures = 11;
const int max_filename_length = 30;
const char texture_filenames[max_textures][max_filename_length] = {
			"cobblestone.tga",		//0
			"cobblestone_bump.tga",	//1
			"colorcube.tga",		//2
			"moon.tga",				//3
			"moon_bump.tga",		//4
			"colorwedge.tga",		//5
			"colorpyramid.tga",		//6
			"starfield_skybox.tga",  //7
			"starfield_skysphere.tga", //8
			"transparency_test.tga", //9
			"titlecrawl.tga" //10
};
GLuint gl_textures[max_textures];
TgaImage texture_images[max_textures];


// Structs that hold the Vertex Array Object index and number of vertices of each shape.
ShapeData cubeData;
ShapeData sphereData;
ShapeData coneData;
ShapeData cylData;
ShapeData mCubeData;
ShapeData wedgeData;
ShapeData pyramidData;
ShapeData iCubeData;
ShapeData iSphereData;
ShapeData decalData;
// Matrix Stack delcaration and shader variables.
MatrixStack  mvstack;
mat4         model_view;
GLint        uModelView, uProjection, uView;
GLint        uAmbient, uDiffuse, uSpecular, uLightPos, uShininess;
GLint        uTex, uEnableTex, uBumpTex, uEnableBumpTex, uEnableSkybox;
// The eye point and look-at point.
// Currently unused. Use to control a camera with LookAt().
Angel::vec4 eye{0, 0.0, 50.0,1.0};
Angel::vec4 ref{0.0, 0.0, 0.0,1.0};
Angel::vec4 up{0.0,1.0,0.0,0.0};
struct Cam 
{
	double x;
	double y;
	double z;
};

Cam Camera;
// Time variable
double TIME = 0.0 ;
double OFFSET = 0.0;

/* Function Implmentations */

// DECAL DRAW METHODS ///////////////////////////////////////////////
// Render a pure quad facing +z to display decals/text and the like.

void drawDecal(void)
{
    glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view );
    glBindVertexArray( decalData.vao );
    glDrawArrays( GL_TRIANGLES, 0, decalData.numVertices );
}

void drawDecal(GLuint diffuse)
{
    glBindTexture( GL_TEXTURE_2D, diffuse);
    glUniform1i( uEnableTex, 1);
    drawDecal();
    glUniform1i( uEnableTex, 0);
}

void drawDecal(GLuint diffuse, GLuint bump)
{
    glActiveTexture(GL_TEXTURE1);
    glBindTexture( GL_TEXTURE_2D, bump);
    glUniform1i( uEnableBumpTex, 1);
    glActiveTexture(GL_TEXTURE0);
    drawDecal(diffuse);
    glUniform1i( uEnableBumpTex, 0);
}


// Render a solid cylinder  oriented along the Z axis. Both bases are of radius 1. 
// The bases of the cylinder are placed at Z = 0, and at Z = 1
void drawCylinder(void)
{
    glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view );
    glBindVertexArray( cylData.vao );
    glDrawArrays( GL_TRIANGLES, 0, cylData.numVertices );
}

void drawCylinder(GLuint diffuse)
{
    glBindTexture( GL_TEXTURE_2D, diffuse);
    glUniform1i( uEnableTex, 1);
    drawCylinder();
    glUniform1i( uEnableTex, 0);
}

void drawCylinder(GLuint diffuse, GLuint bump)
{
    glActiveTexture(GL_TEXTURE1);
    glBindTexture( GL_TEXTURE_2D, bump);
    glUniform1i( uEnableBumpTex, 1);
    glActiveTexture(GL_TEXTURE0);
    drawCylinder(diffuse);
    glUniform1i( uEnableBumpTex, 0);
}

// CONE DRAW METHODS/////////////////////////////////////////////////
// Render a solid cone oriented along the Z axis with base radius 1. 
// The base of the cone is placed at Z = 0, and the top at Z = 1. 
void drawCone(void)
{
    glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view );
    glBindVertexArray( coneData.vao );
    glDrawArrays( GL_TRIANGLES, 0, coneData.numVertices );
}

void drawCone(GLuint diffuse)
{
    glBindTexture( GL_TEXTURE_2D, diffuse);
    glUniform1i( uEnableTex, 1);
    drawCone();
    glUniform1i( uEnableTex, 0);
}

void drawCone(GLuint diffuse, GLuint bump)
{
    glActiveTexture(GL_TEXTURE1);
    glBindTexture( GL_TEXTURE_2D, bump);
    glUniform1i( uEnableBumpTex, 1);
    glActiveTexture(GL_TEXTURE0);
    drawCone(diffuse);
    glUniform1i( uEnableBumpTex, 0);
}


// PYRAMID DRAW METHODS /////////////////////////////////////////////
void drawPyramid(void)
{
	glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view);
	glBindVertexArray( pyramidData.vao );
	glDrawArrays( GL_TRIANGLES, 0, pyramidData.numVertices);
}

void drawPyramid(GLuint diffuse)
{
    glBindTexture( GL_TEXTURE_2D, diffuse);
    glUniform1i( uEnableTex, 1);
	drawPyramid();
    glUniform1i( uEnableTex, 0);
}

void drawPyramid(GLuint diffuse, GLuint bump)
{
    glActiveTexture(GL_TEXTURE1);
    glBindTexture( GL_TEXTURE_2D, bump);
    glUniform1i( uEnableBumpTex, 1);
    glActiveTexture(GL_TEXTURE0);
	drawPyramid(diffuse);
    glUniform1i( uEnableBumpTex, 0);
}


// CUBE DRAW METHODS ////////////////////////////////////////////////
void drawCube(void)
{
	glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view );
    glBindVertexArray( cubeData.vao );
    glDrawArrays( GL_TRIANGLES, 0, cubeData.numVertices );
}

void drawCube(GLuint diffuse)
{
	glBindTexture( GL_TEXTURE_2D, diffuse);
	glUniform1i( uEnableTex, 1);
	drawCube();
	glUniform1i( uEnableTex, 0);	
}

void drawCube( GLuint diffuse, GLuint bump)
{
	glActiveTexture(GL_TEXTURE1);
	glBindTexture( GL_TEXTURE_2D, bump);
	glUniform1i( uEnableBumpTex, 1);
	glActiveTexture(GL_TEXTURE0);
	drawCube(diffuse);
	glUniform1i( uEnableBumpTex, 0);
}

// MCUBE DRAW METHODS ///////////////////////////////////////////////
void drawMCube(void)
{
	glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view);
	glBindVertexArray( mCubeData.vao );
	glDrawArrays( GL_TRIANGLES, 0, mCubeData.numVertices );
}

void drawMCube(GLuint diffuse)
{
    glBindTexture( GL_TEXTURE_2D, diffuse);
    glUniform1i( uEnableTex, 1);
    drawMCube();
    glUniform1i( uEnableTex, 0);
}

void drawMCube( GLuint diffuse, GLuint bump)
{
    glActiveTexture(GL_TEXTURE1);
    glBindTexture( GL_TEXTURE_2D, bump);
    glUniform1i( uEnableBumpTex, 1);
    glActiveTexture(GL_TEXTURE0);
    drawMCube(diffuse);
    glUniform1i( uEnableBumpTex, 0);
}

// ICUBE DRAW METHODS ///////////////////////////////////////////////
void drawICube(void)
{
    glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view );
    glBindVertexArray( iCubeData.vao );
    glDrawArrays( GL_TRIANGLES, 0, iCubeData.numVertices );
}

void drawICube(GLuint diffuse)
{
    glBindTexture( GL_TEXTURE_2D, diffuse);
    glUniform1i( uEnableTex, 1);
    drawICube();
    glUniform1i( uEnableTex, 0);
}

void drawICube( GLuint diffuse, GLuint bump)
{
    glActiveTexture(GL_TEXTURE1);
    glBindTexture( GL_TEXTURE_2D, bump);
    glUniform1i( uEnableBumpTex, 1);
    glActiveTexture(GL_TEXTURE0);
    drawICube(diffuse);
    glUniform1i( uEnableBumpTex, 0);
}


// WEDGE DRAW METHODS ///////////////////////////////////////////////
void drawWedge(void)
{
	glUniformMatrix4fv(uModelView, 1, GL_TRUE, model_view);
	glBindVertexArray( wedgeData.vao );
	glDrawArrays( GL_TRIANGLES, 0, wedgeData.numVertices );
}

void drawWedge(GLuint diffuse)
{
    glBindTexture( GL_TEXTURE_2D, diffuse);
    glUniform1i( uEnableTex, 1);
    drawWedge();
    glUniform1i( uEnableTex, 0);
}

void drawWedge( GLuint diffuse, GLuint bump)
{
    glActiveTexture(GL_TEXTURE1);
    glBindTexture( GL_TEXTURE_2D, bump);
    glUniform1i( uEnableBumpTex, 1);
    glActiveTexture(GL_TEXTURE0);
    drawWedge(diffuse);
    glUniform1i( uEnableBumpTex, 0);
}

// SPHERE DRAW METHODS //////////////////////////////////////////////
// This function draws a sphere with radius 1
// centered around the origin.
void drawSphere(void)
{
    glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view );
    glBindVertexArray( sphereData.vao );
    glDrawArrays( GL_TRIANGLES, 0, sphereData.numVertices );
}

void drawSphere(GLuint diffuse)
{
    glBindTexture( GL_TEXTURE_2D, diffuse);
    glUniform1i( uEnableTex, 1);
    drawSphere();
    glUniform1i( uEnableTex, 0);
}

void drawSphere( GLuint diffuse, GLuint bump)
{
    glActiveTexture(GL_TEXTURE1);
    glBindTexture( GL_TEXTURE_2D, bump);
    glUniform1i( uEnableBumpTex, 1);
    glActiveTexture(GL_TEXTURE0);
    drawSphere(diffuse);
    glUniform1i( uEnableBumpTex, 0);
}

// ISPHERE DRAW METHODS //////////////////////////////////////////////
// This function draws a sphere with radius 1
// centered around the origin.
void drawISphere(void)
{
    glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view );
    glBindVertexArray( iSphereData.vao );
    glDrawArrays( GL_TRIANGLES, 0, iSphereData.numVertices );
}

void drawISphere(GLuint diffuse)
{
    glBindTexture( GL_TEXTURE_2D, diffuse);
    glUniform1i( uEnableTex, 1);
    drawISphere();
    glUniform1i( uEnableTex, 0);
}

void drawISphere( GLuint diffuse, GLuint bump)
{
    glActiveTexture(GL_TEXTURE1);
    glBindTexture( GL_TEXTURE_2D, bump);
    glUniform1i( uEnableBumpTex, 1);
    glActiveTexture(GL_TEXTURE0);
    drawISphere(diffuse);
    glUniform1i( uEnableBumpTex, 0);
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
    switch (key) {
        case 'q':
        case 27:
			printf("\n");
            exit(0); 
        case 's':
            FrSaver.DumpPPM(Width,Height) ;
            break;
        case 'r':
            resetArcball() ;
            glutPostRedisplay() ;
            break ;
        case 'a': // togle animation
			if (Animate)
				OFFSET += TM.GetElapsedTime();
            Animate = 1 - Animate ;
            // reset the timer to point to the current time		
			numFrames = 0.0;
            TM.Reset() ;
            // printf("Elapsed time %f\n", time) ;
            break ;
        case '0':
            //reset your object
			OFFSET = 0.0f;
			numFrames = 0.0f;
			TIME = 0.0f;
			TM.Reset();
            glutPostRedisplay() ;
            break ;
        case 'm':
            if( Recording == 1 )
            {
                //printf("Frame recording disabled.\n") ;
                Recording = 0 ;
            }
            else
            {
                //printf("Frame recording enabled.\n") ;
                Recording = 1  ;
            }
            FrSaver.Toggle(Width);
            break ;
        case 'h':
        case '?':
            instructions();
            break;
		case 'o':
			Camera.z += 0.2;
			break;
		case 'u':
			Camera.z -= 0.2;
			break;
		case 'j':
			Camera.x -= 0.2;
			break;
		case 'l':
			Camera.x += 0.2;
			break;
		case 'i':
			Camera.y += 0.2;
			break;
		case 'k':
			Camera.y -= 0.2;
			break;
    }
    glutPostRedisplay() ;

}

void load_textures(void)
{

	for( int i = 0; i < max_textures; i++)
	{
		char filename[max_filename_length+10];
		filename[0] = '\0';
		strcat(filename, "Textures/");
		strcat(filename, texture_filenames[i]);
		bool is_bump_map = false;
		if(!texture_images[i].loadTGA(filename))
		{
			printf("Failed to load texture: %s.\n", texture_filenames[i]);
			exit(1);
		}
		if(strstr(texture_filenames[i], "bump") != NULL)
			is_bump_map = true;

		if(is_bump_map)
			glActiveTexture(GL_TEXTURE1);
		else
			glActiveTexture(GL_TEXTURE0);

		glGenTextures( 1, &gl_textures[i]);
		glBindTexture( GL_TEXTURE_2D, gl_textures[i] );
		glTexImage2D(GL_TEXTURE_2D, 0, 4, texture_images[i].width, texture_images[i].height, 
					0, (texture_images[i].byteCount == 3) ? GL_BGR : GL_BGRA, 
					GL_UNSIGNED_BYTE, texture_images[i].data);
		glGenerateMipmap(GL_TEXTURE_2D);
    	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	}
 
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
	generateMCube(program, &mCubeData);
	generateWedge(program, &wedgeData);
	generatePyramid(program, &pyramidData);
	generateICube(program, &iCubeData);
	generateISphere(program, &iSphereData);
	generateDecal(program, &decalData);

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
	uEnableSkybox = glGetUniformLocation( program, "EnableSkybox");

    glUniform4f(uAmbient,    0.2f,  0.2f,  0.2f, 1.0f);
    glUniform4f(uDiffuse,    0.6f,  0.6f,  0.6f, 1.0f);
    glUniform4f(uSpecular,   0.2f,  0.2f,  0.2f, 1.0f);
    glUniform4f(uLightPos,  15.0f, 15.0f, 30.0f, 0.0f);
    glUniform1f(uShininess, 100.0f);

    glEnable(GL_DEPTH_TEST);

    load_textures();
	Camera.x = 0.0f; Camera.y= 0.0f; Camera.z = -20.0f;
	AV_FPS = 0.0; numFrames = 0.0; last_time = 0.0; 

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
	numFrames += 1.0;
	INST_FPS = 1.0/ (PTM.GetElapsedTime());
	if( Animate )
		AV_FPS = numFrames/TIME;	
	else
		AV_FPS = INST_FPS;
	PTM.Reset();
	printf("\rTIME:  %.2fs  AV.FPS:  %.2f  INST.FPS:  %.2f    ", TIME, AV_FPS, INST_FPS) ;	
    // Clear the screen with the background colour (set in myinit)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    model_view = mat4(1.0f);
    
	if ( 0.00 <= TIME && TIME < 8.00) //titlecard 1
	{

	}
	else if ( 8.00 <= TIME && TIME < 16.00 ) //titlecard 2
	{

	}
	else if (16.00 <= TIME && TIME < 25.00 ) // along time ago
	{

	}
	else if ( 25.00 <= TIME && TIME < 150.00 ) // title crawl & opening ships
	{

	}
	else if ( 150.00 <= TIME && TIME < 153.00 ) // forward engines hit
    {
	}
    model_view *= RotateX(25.0);
	model_view *= Translate(Camera.x, Camera.y, Camera.z);
    

    //mat4 view = model_view;
    
    
    //model_view = Angel::LookAt(eye, ref, up);//just the view matrix;

    glUniformMatrix4fv( uView, 1, GL_TRUE, model_view );

    // Previously glScalef(Zoom, Zoom, Zoom);
    //model_view *= Scale(Zoom);
	mvstack.push(model_view);
	mvstack.push(model_view);
    mvstack.push(model_view);

	model_view *= Translate(-Camera.x, -Camera.y, -Camera.z);
	model_view *= Scale(1000.0f);
	glUniform1i( uEnableSkybox, 1);
	drawISphere(gl_textures[8]);
	glUniform1i( uEnableSkybox, 0);
	model_view *= Scale(1/200.0);

	model_view = mvstack.pop();
    // Draw Something
    set_colour(0.8f, 0.8f, 0.8f);
    drawSphere(gl_textures[2]);

    // Previously glTranslatef(3,0,0);
    model_view *= Translate(3.0f, 0.0f, 0.0f);

    // Previously glScalef(3,3,3);
	mvstack.push(model_view);
	model_view *= RotateY(20.0f * TIME);
    model_view *= Scale(3.0f, 6.0f, 3.0f);
    drawCube(gl_textures[0], gl_textures[1]);
	model_view = mvstack.pop();

    // And extra shapes!
    //model_view *= Scale(1.0f/3.0f, 1.0f/3.0f, 1.0f/3.0f);
    model_view *= Translate(3.0f, 0.0f, 0.0f);
    //set_colour(1.0f, 1.0f, 0.0f);
    drawCone(gl_textures[2]);

    model_view *= Translate(-9.0f, 0.0f, 0.0f);
    set_colour(1.0f, 1.0f, 1.0f);
    drawCylinder(gl_textures[2]);

	model_view *= Translate(-3.0f, 0.0f, 0.0f);
	drawMCube(gl_textures[2]);

	model_view *= Translate(-3.0f, 0.0f, 0.0f);
	drawWedge(gl_textures[5]);

	model_view *= Translate(-3.0f, 0.0f, 0.0f);
	model_view *= Scale(1.5f, 3.0f, 0.5f);
	model_view *= RotateY(45.0);	
	drawPyramid(gl_textures[6]);

	model_view = mvstack.pop();
	model_view *= Translate(0.0, -9.0f, 0.0f);
	model_view *= RotateX(-25.0);
	model_view *= Translate(0.0f, 0.0, 18.8f - TIME*TIME);
	model_view *= Scale(6.0,3.0,1.0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUniform1i( uEnableSkybox, 1);
	drawDecal(gl_textures[9]);

	model_view = mvstack.pop();
	model_view *= Translate(0.0f, -10.0f, 50.0f - 2.0*TIME);
	model_view *= Scale(20.0, 1.0, 60.0);
	model_view *= RotateX(-90.0);
	drawDecal(gl_textures[10]);

    glUniform1i( uEnableSkybox, 0);
	glDisable(GL_BLEND);
	
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
	printf("  i/k, j/l, u/o, to manually move the camera.\n");
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
        //TM.Reset() ; // commenting out this will make the time run from 0
        // leaving 'Time' counts the time interval between successive calls to idleCB
        if( Recording == 0 )
            TIME = TM.GetElapsedTime() + OFFSET;
        else
            TIME += 0.033 + OFFSET; // save at 30 frames per second.        

        eye.x = 20*sin(TIME);
        eye.z = 20*cos(TIME);
        
        
		if (Recording)
			printf("[REC]  ");
		else
			printf("       ");
		fflush(stdout);
        glutPostRedisplay() ; 
    }
	else
	{
	    fflush(stdout);
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
	//glutSpecialFunc(myKey);
    glutMouseFunc(myMouseCB) ;
    glutMotionFunc(myMotionCB) ;
    instructions();

    glutDisplayFunc(display);
    glutMainLoop();

    TM.Reset() ;
    return 0;         // never reached
}
