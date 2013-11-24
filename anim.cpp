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
#define __NOAUDIO__ // define no audio in windows
#elif __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#define glutInitContextVersion(a,b)
#define glutInitContextProfile(a)
#define glewExperimental int glewExperimentalAPPLE
#define glewInit()
#define __NOAUDIO__ // define no audio in mac
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
#include <algorithm>

// Stuff for audio support!  Pretty nifty!  But
// definitely only for LINUX systems! 
// (More specifically, requires support for ALSA,
// which should be present on all linux systems.
//
// Uncommenting the following line will build  
// with sudio support removed in LINUX as well:
//#define __NOAUDIO__
#ifndef __NOAUDIO__
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
pid_t audioPID;
#endif

/* Application Includes */
#include "Ball.h"
#include "FrameSaver.h"
#include "Timer.h"
#include "Timings.h"
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
const int max_textures = 17;
const int max_filename_length = 30;
const char texture_filenames[max_textures][max_filename_length] = {
			"cobblestone.tga",		//0
			"cobblestone_bump.tga",	//1
			"clouds.tga",			//2 
			"moon.tga",				//3
			"cobblechuck.tga",		//4 
			"mars.tga",				//5
			"spaceship_bump.tga",		//6  
			"titlestart.tga",      //7
			"starfield_skysphere.tga", //8
			"starwarstitle.tga", 	//9 
			"titlecrawl.tga", 		//10
			"alongtimeago.tga", 	//11
			"earth.tga", 			//12
			"flames.tga",			//13
			"titlecard.tga",    //14
			"rebelspaceship.tga",
			"spaceship.tga"		 //15
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
ShapeData sDData;
ShapeData pyrSData;
// Matrix Stack delcaration and shader variables.
MatrixStack  mvstack;
mat4         model_view;
GLint        uModelView, uProjection, uView;
GLint        uAmbient, uDiffuse, uSpecular, uLightPos, uShininess;
GLint        uTex, uEnableTex, uBumpTex, uEnableBumpTex, uEnableSkybox, uEnableFade;
GLfloat		 uFade;
// The eye point and look-at point.
// Currently unused. Use to control a camera with LookAt().
Angel::vec4 eye{0, 0.0, -10.0,1.0};
Angel::vec4 ref{0.0, 0.0, 0.0,1.0};
Angel::vec4 up{0.0,1.0,0.0,0.0};
struct Cam 
{
	double x;
	double y;
	double z;
	double Rx;
	double Ry;
	double Rz;
};
Cam Camera;
// Time variables
double TIME = 0.0 ;
double OFFSET = 0.0;

int audioRunning = 0;

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

void drawSPyramid(void)
{
    glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view);
    glBindVertexArray( pyrSData.vao );
    glDrawArrays( GL_TRIANGLES, 0, pyrSData.numVertices);
}

void drawSPyramid(GLuint diffuse)
{
    glBindTexture( GL_TEXTURE_2D, diffuse);
    glUniform1i( uEnableTex, 1);
    drawSPyramid();
    glUniform1i( uEnableTex, 0);
}

void drawSPyramid(GLuint diffuse, GLuint bump)
{
    glActiveTexture(GL_TEXTURE1);
    glBindTexture( GL_TEXTURE_2D, bump);
    glUniform1i( uEnableBumpTex, 1);
    glActiveTexture(GL_TEXTURE0);
    drawSPyramid(diffuse);
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

// DRAW SD //////////////////////////////////////////////////////////
void drawSD(void)
{
    glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view );
    glBindVertexArray( sDData.vao );
    glDrawArrays( GL_TRIANGLES, 0, sDData.numVertices );
}

void drawSD(GLuint diffuse)
{
    glBindTexture( GL_TEXTURE_2D, diffuse);
    glUniform1i( uEnableTex, 1);
    drawSD();
    glUniform1i( uEnableTex, 0);
}

void drawSD( GLuint diffuse, GLuint bump)
{
    glActiveTexture(GL_TEXTURE1);
    glBindTexture( GL_TEXTURE_2D, bump);
    glUniform1i( uEnableBumpTex, 1);
    glActiveTexture(GL_TEXTURE0);
    drawSD(diffuse);
    glUniform1i( uEnableBumpTex, 0);
}



// Resets the mouse to starting posisition?
void resetArcball()
{
    Ball_Init(Arcball);
    Ball_Place(Arcball,qOne,0.75);
}

void jumpTime(double time)
{
	OFFSET = time;
	TIME = time;
	numFrames = 0.0f;
}

double sinBetween(double startTime, double endTime,
				  double startLoc, double endLoc, 
                  double TIME )
{
	double locDiff = endLoc - startLoc;
	double scaleFactor = (TIME-startTime)/(endTime-startTime);
	double scale = sin((scaleFactor)*(M_PI)-(M_PI/2));
	
	return startLoc + locDiff * (scale+1)/2;
	

}

double linearBetween(double startTime, double endTime,
					 double startLoc, double endLoc,
					 double TIME )
{
	double locDiff = endLoc - startLoc;
	double scaleFactor = (TIME-startTime)/(endTime-startTime);
	return startLoc + locDiff*scaleFactor;
}

// Key handler
void myKey(unsigned char key, int x, int y)
{
    switch (key) {
        case 'q':
        case 27:
			printf("\n");
			#ifndef __NOAUDIO__
			if ( audioRunning )
				kill(audioPID, SIGKILL);
			#endif
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
			{
				#ifndef __NOAUDIO__
				if( audioRunning )
					kill(audioPID, SIGKILL);
				audioRunning = 0;
				#endif
				OFFSET += TM.GetElapsedTime();
			}
			else if (TIME == 0.0)
			{
                #ifndef __NOAUDIO__
				audioPID = fork();
				audioRunning = 1;
				if( audioPID == 0) 
				{
					int fd = open("/dev/null", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
					dup2(fd,1);
					dup2(fd,2);
					close(fd);
					audioRunning = 1;
					execl("/usr/bin/aplay", "aplay", "first5mins.wav", (char*) 0);
					_exit(0);
				}
				#endif
			}
            Animate = 1 - Animate ;
            // reset the timer to point to the current time		
			numFrames = 0.0;
            TM.Reset() ;
            // printf("Elapsed time %f\n", time) ;
            break ;
        case '0':
            //reset your object
            #ifndef __NOAUDIO__
			if ( audioRunning )
                kill(audioPID, SIGKILL);
			#endif
			jumpTime(SCENE_0_START);
			TM.Reset();
            glutPostRedisplay() ;
            break ;
		case '1':
			#ifndef __NOAUDIO__
            if ( audioRunning )
                kill(audioPID, SIGKILL);
			#endif
            jumpTime(SCENE_1_START);
            TM.Reset();
            glutPostRedisplay() ;
            break ;
		case '2':
			#ifndef __NOAUDIO__
         	if ( audioRunning )
                kill(audioPID, SIGKILL);
			#endif
            jumpTime(SCENE_3_START);
            TM.Reset();
            glutPostRedisplay() ;
            break ;
		case '3':
			#ifndef __NOAUDIO__
            if ( audioRunning )
                kill(audioPID, SIGKILL);
			#endif
			jumpTime(117.0);
            //jumpTime(SCENE_3_START);
            TM.Reset();
            glutPostRedisplay() ;
            break ;
		case '4':
			#ifndef __NOAUDIO__
            if ( audioRunning )
                kill(audioPID, SIGKILL);
			#endif
            jumpTime(SCENE_4_START);
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

void default_camera()
{
    Camera.x = 0.0f; Camera.y= 0.0f; Camera.z = -20.0f;
    Camera.Rx = 0.0; Camera.Ry = 0.0f; Camera. Rz = 0.0f;
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
	generateSD(program, &sDData);
	generateSPyramid(program, &pyrSData);

    uModelView  = glGetUniformLocation( program, "ModelView"  );
    uProjection = glGetUniformLocation( program, "Projection" );
    uView       = glGetUniformLocation( program, "View"       );

    glClearColor( 0.0, 0.0, 0.0, 1.0 ); // dark blue background

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
	uEnableFade= glGetUniformLocation( program, "EnableFade"	);
	uFade	   = glGetUniformLocation( program, "Fade"			);

    glUniform4f(uAmbient,    0.2f,  0.2f,  0.2f, 1.0f);
    glUniform4f(uDiffuse,    0.6f,  0.6f,  0.6f, 1.0f);
    glUniform4f(uSpecular,   0.2f,  0.2f,  0.2f, 1.0f);
    glUniform4f(uLightPos,  15.0f, 15.0f, 30.0f, 0.0f);
    glUniform1f(uShininess, 100.0f);
	glUniform1f(uFade, 1.0f);

    glEnable(GL_DEPTH_TEST);

    load_textures();
	Camera.x = 0.0f; Camera.y= 0.0f; Camera.z = -20.0f;
	Camera.Rx = 25.0; Camera.Ry = 0.0f; Camera. Rz = 0.0f;
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

void place_camera()
{
    model_view *= RotateX(Camera.Rx);
    model_view *= RotateY(Camera.Ry);
    model_view *= RotateZ(Camera.Rz);
    model_view *= Translate(Camera.x, Camera.y, Camera.z);
	glUniformMatrix4fv( uView, 1, GL_TRUE, model_view );
}

void draw_stars()
{
	mvstack.push(model_view);
	model_view *= Translate(-Camera.x, -Camera.y, -Camera.z);
	model_view *= RotateX(-45.0);
	model_view *= RotateY(90.0);
    model_view *= Scale(1000.0f);
    glUniform1i( uEnableSkybox, 1);
    drawISphere(gl_textures[8]);
    glUniform1i( uEnableSkybox, 0);
    model_view *= Scale(1/1000.0);
	model_view = mvstack.pop();
}

void draw_title_crawl()
{
	double titleFade = 1.0f;
	glUniform1f(uFade, (float)titleFade);
	double titleWait = 10.0f;
	double titleTime = 5.0f;
	if (TIME > SCENE_3_START + titleWait && TIME < SCENE_3_START + titleWait + titleTime)
	{
		titleFade = sinBetween(SCENE_3_START + titleWait, SCENE_3_START + titleWait + titleTime,
							   1.0, 0, TIME);
		glUniform1f(uFade, (float)titleFade);
	}
	else if (TIME > SCENE_3_START + titleWait + titleTime)
	{
		titleFade = 0.0f;
		glUniform1f(uFade, (float)titleFade);
	}
	mvstack.push(model_view);
	model_view *= Translate(0.0, -9.0f, 0.0f);
    model_view *= RotateX(-25.0);
    model_view *= Translate(0.0f, 0.0, 18.8f - (TIME-SCENE_3_START)*(TIME-SCENE_3_START));
    model_view *= Scale(6.0,3.0,1.0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUniform1i( uEnableSkybox, 1);
	glUniform1i(uEnableFade, 1);
    drawDecal(gl_textures[9]);
    model_view = mvstack.pop();


    double bookFade = 1.0f;
    glUniform1f(uFade, (float)bookFade);
    double bookWait = 75.0f;
    double bookTime = 5.0f;
    if (TIME > SCENE_3_START + bookWait && TIME < SCENE_3_START + bookWait + bookTime)
    {
        bookFade = sinBetween(SCENE_3_START + bookWait, SCENE_3_START + bookWait + bookTime,
                               1.0, 0, TIME);
        glUniform1f(uFade, (float)bookFade);
    }
    else if (TIME > SCENE_3_START + bookWait + bookTime)
    {
        bookFade = 0.0f;
        glUniform1f(uFade, (float)bookFade);
    }
	mvstack.push(model_view);
    model_view *= Translate(0.0f, -10.0f, 50.0f - 1.2*(TIME-SCENE_3_START));
    model_view *= Scale(20.0, 1.0, 60.0);
    model_view *= RotateX(-90.0);
    drawDecal(gl_textures[10]);

    glUniform1i( uEnableSkybox, 0);
	glUniform1i( uEnableFade, 0);
    glDisable(GL_BLEND);
	model_view = mvstack.pop();
}

double earth_rotation = 0.0f;
void draw_planets()
{
	mvstack.push(model_view);
	model_view *= Translate(0.0, -200.0, -60.0+20.0);
	model_view *= Scale(4.0);
	drawSphere(gl_textures[3]);//, gl_textures[4]);
	model_view = mvstack.pop();

	mvstack.push(model_view);
	model_view *= Translate(-100.0, -200.0, 5.0+20.0);
	model_view *= RotateY(1.3*TIME);
	model_view *= Scale(25.0);
	drawSphere(gl_textures[12]);
	model_view = mvstack.pop();

	mvstack.push(model_view);
	set_colour(1.0,1.0,1.0);
	model_view *= Translate(0.0f, -50.0f, (15.0+100.0f+20.0));
	model_view *= RotateX(TIME/4);
	model_view *= Scale(100.0);
	//glUniform1f(uEnableSkybox, 1);
	drawSphere(gl_textures[5]);//gl_textures[6]);
	//glUniform1f(uEnableSkybox, 0);
	model_view = mvstack.pop();
}

void draw_laser(double x, double z, double startY, double endY,
				double startTime, double endTime, int color, 
				int hit, double TIME)
{
	double laserTimeout = 5.0;
	if ( TIME < startTime || (TIME > endTime && hit) || (TIME > endTime + laserTimeout ) )
		return;
	double laserLen = 7.0;
	double hitSize = 4.0;
	double hitLen = 0.05;
	double yPos = startY-laserLen + (endY-startY)*(TIME-startTime)/(endTime-startTime);

    glUniform1f(uEnableSkybox, 1);
	mvstack.push(model_view);
	if ( color <= 0)
		set_colour(1.0, 0.0, 0.0);
	else
		set_colour(0.0, 1.0, 0.0);
	model_view *= Translate(x, yPos, z);
	model_view *= RotateX(90.0);
	model_view *= Scale(1.0/20, 1.0/20, laserLen);
	glUniform1f(uEnableSkybox, 1);
	drawCylinder();
	set_colour(1.0, 1.0, 1.0);
	glUniform1f(uEnableSkybox, 0);
	model_view = mvstack.pop();
	if ( TIME > endTime - hitLen && hit)
	{
		mvstack.push(model_view);
		model_view *= Translate(x, endY, z);
		model_view *= RotateX(90.0);
		model_view *= Scale(hitSize);
		set_colour(0.0f, 1.0f, 0.0f);
	    glEnable(GL_BLEND);
   		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glUniform1i( uEnableSkybox, 1);
		drawDecal(gl_textures[2]);
		glUniform1i( uEnableSkybox, 0);
		glDisable(GL_BLEND);
		model_view = mvstack.pop();
		set_colour(1.0, 1.0, 1.0);
	}
}

double lastCRX;
double lastCRY;
double lastCRZ;
void drawMan(double throwDone)
{
	
	double sub = .08;
	mvstack.push(model_view);
	set_colour(162.0/256.0, 55.0/256.0, 156/256.0);

	// body, head, and shoulders
	model_view *= RotateX(90.0);
	model_view *= Scale(1.0, 0.5, 1.0);
	drawCylinder(); // body
	model_view *= Scale(1.0, 2.0, 1.0);
	model_view *= RotateX(-90.0);
	model_view *= Translate(0.0, 1.0+0.25, 0.0);
	model_view *= RotateX(90.0);
	model_view *= Scale(1.0/3,1.0/4, 1.0/3 );
	drawCylinder(); //neck
	model_view *= Scale(3.0, 4.0, 3.0);
	model_view *= RotateX(-90.0);
	model_view *= Translate(0.0, 1.25, 0.0);
	drawSphere(); // head
	model_view *= Translate(-1, -1.5, 0.0);
	model_view *= Scale(0.25);
	drawSphere(); //right shoulder
	model_view *= Scale(4);

	mvstack.push(model_view);
	double lShoulderZ, lShoulderY, lElbowX, lElbowY;
	double rShoulderZ, rShoulderY, rElbowX, rElbowY;
    /* Namaste
	lShoulderZ = -80.0;
	lShoulderY = -20.0;
	lElbowX = 40.0;
	lElbowY = -110.0;
	rShoulderZ = lShoulderZ;
	rShoulderY = -lShoulderY;
	rElbowX = -lElbowX - 180.0;
	rElbowY = lElbowY;
	*/
	double windUpTime = 0.5;
	double throwTime = 0.2 + windUpTime;
	if ( throwDone == 0.0)
	{
    	lShoulderZ = -80.0;
    	lShoulderY = -20.0;
    	lElbowX = 40.0;
    	lElbowY = -120.0;
    	rShoulderZ = lShoulderZ;
    	rShoulderY = -lShoulderY;
    	rElbowX = -lElbowX - 180.0;
    	rElbowY = lElbowY;
	}
	else if ( throwDone > 0.0 && throwDone < windUpTime )
	{
		lShoulderZ = sinBetween(0.0, windUpTime, -80, -20, throwDone);
		lShoulderY = sinBetween(0.0, windUpTime, -20, 70, throwDone);
		lElbowX    = sinBetween(0.0, windUpTime, 40, 40, throwDone);
		lElbowY    = sinBetween(0.0, windUpTime, -120, 20, throwDone);

		rShoulderZ  = sinBetween(0.0, windUpTime, -80, 0, throwDone);
		rShoulderY  = sinBetween(0.0, windUpTime, -(-20), -(-120), throwDone);
		rElbowX     = sinBetween(0.0, windUpTime, -(40.0)-180.0, -(40.)-180.0, throwDone);
		rElbowY	   = sinBetween(0.0, windUpTime, -120, -20, throwDone);    
	}
	else if ( throwDone >= windUpTime && throwDone <= throwTime )
	{
		lShoulderZ = sinBetween(windUpTime, throwTime, -20, -60, throwDone);
        lShoulderY = sinBetween(windUpTime, throwTime, 70, -120, throwDone);
        lElbowX    = sinBetween(windUpTime, throwTime, 40, 40, throwDone);
        lElbowY    = sinBetween(windUpTime, throwTime, 20, -2.0, throwDone);

        rShoulderZ  = sinBetween(windUpTime, throwTime, 0, 160, throwDone);
        rShoulderY  = sinBetween(windUpTime, throwTime, -(-120), -(-200), throwDone);
        rElbowX     = sinBetween(windUpTime, throwTime, -(40.0)-180.0, -(0)-180.0, throwDone);
        rElbowY     = sinBetween(windUpTime, throwTime, -20, 0, throwDone);
	}
	else if (throwDone >= throwTime && throwDone <= 1.0)
	{
        lShoulderZ = sinBetween(throwTime, 1.0, -60, -20, throwDone);
        lShoulderY = sinBetween(throwTime, 1.0, -120, -10, throwDone);
        lElbowX    = sinBetween(throwTime, 1.0, 40, 40, throwDone);
        lElbowY    = sinBetween(throwTime, 1.0, -2.0, 20, throwDone);

        rShoulderZ  = sinBetween(throwTime, 1.0, 160, -20,throwDone);
        rShoulderY  = sinBetween(throwTime, 1.0, -(-200), -(-10), throwDone);
        rElbowX     = sinBetween(throwTime, 1.0, -(0)-180.0,-(40)-180.0, throwDone);
        rElbowY     = sinBetween(throwTime, 1.0, 0, 20, throwDone);
	}



	model_view *= Translate(2.0, 0.0, 0.0);
	model_view *= Scale(0.25);
	drawSphere(); //left shoulder
	model_view *= Scale(4.0);

    model_view *= RotateZ(lShoulderZ); // bend at left shoulder up/down
	model_view *= RotateY(lShoulderY); //in/out
	
	model_view *= Translate(0.55-sub, 0.0, 0.0);
	model_view *= RotateY(90);
	model_view *= Scale(0.15, 0.15, 0.3);
	drawCylinder(); // left bicep
	model_view *= Scale(1/0.15, 1/0.15, 1.0/0.3);
	model_view *= RotateY(-90);
	model_view *= Translate(0.55-sub, 0.0, 0.0);
	model_view *= Scale(0.25);
	drawSphere(); //left elbow
	model_view *= Scale(4.0);

	model_view *= RotateX(lElbowX);//bend at left elbow	up/down
	model_view *= RotateY(lElbowY);//in/out

	model_view *= Translate(0.75-sub, 0.0, 0.0);
	model_view *= RotateY(90);
    model_view *= Scale(0.15, 0.15, 0.5);
    drawCylinder(); //left forearm
    model_view *= Scale(1/0.15, 1/0.15, 1/0.5);
    model_view *= RotateY(-90);	
	model_view *= Translate(0.75-sub, 0.0, 0.0);
	model_view *= RotateY(90);
	model_view *= RotateX(90);
	model_view *= Scale(0.25, 0.25, 1.0/20);
	drawCylinder(); //left hand
	if ( throwDone < throwTime - throwTime/20 )
	{
		lastCRX = lElbowX;
		lastCRY = lShoulderY + lElbowY;
		lastCRZ = lShoulderZ;
		model_view *= Scale(4.0, 4.0, 20.0); //unscale
		model_view *= Translate(0.0, 0.5, 0.0);
		set_colour(0.8, 0.8, 0.8);
		drawCube(gl_textures[0], gl_textures[1]);
		set_colour(162.0/256.0, 55.0/256.0, 156/256.0);
	}	
	model_view = mvstack.pop();
	model_view *= RotateY(180.0);

    model_view *= RotateZ(rShoulderZ); // bend at right shoulder
    model_view *= RotateY(rShoulderY);

    model_view *= Translate(0.55-sub, 0.0, 0.0);
    model_view *= RotateY(90);
    model_view *= Scale(0.15, 0.15, 0.3);
    drawCylinder(); // right bicep
    model_view *= Scale(1/0.15, 1/0.15, 1.0/0.3);
    model_view *= RotateY(-90);
    model_view *= Translate(0.55-sub, 0.0, 0.0);
    model_view *= Scale(0.25);
    drawSphere(); //right elbow
    model_view *= Scale(4.0);

    model_view *= RotateX(rElbowX); // bend at right elbow
    model_view *= RotateY(rElbowY);

    model_view *= Translate(0.75-sub, 0.0, 0.0);
    model_view *= RotateY(90);
    model_view *= Scale(0.15, 0.15, 0.5);
    drawCylinder(); //right forearm
    model_view *= Scale(1/0.15, 1/0.15, 1/0.5);
    model_view *= RotateY(-90);
    model_view *= Translate(0.75-sub, 0.0, 0.0);
    model_view *= RotateY(90);
    model_view *= RotateX(90);
    model_view *= Scale(0.25, 0.25, 1.0/20);
    drawCylinder(); //right hand

	model_view = mvstack.pop();

	if( throwDone > throwTime - throwTime/20.0 )
	{
	    mvstack.push(model_view);
		double origX = 0.7;
		double origY = 0.5; 
		double origZ = 1.2;
		double throwST = throwTime - throwTime/20.0;
		double throwFT = 1.0f;
		double throwScale = (throwDone - throwST)/(throwFT-throwST);

		origX -= throwScale*(12.0);
		origZ += throwScale*(30.0);
		origY += (15.0)*(throwScale*1) - (9.8)*(throwScale)*(throwScale);
		double CRX = lastCRX  + throwScale*(560);
		double CRY = lastCRY  + throwScale*(60);
		double CRZ = lastCRZ  + throwScale*(75); 

		model_view *= Translate(origX, origY, origZ); 
        model_view *= RotateX (CRX);
        model_view *= RotateY (CRY);
        model_view *= RotateZ (CRZ);
		set_colour(0.8,0.8,0.8);
		drawCube(gl_textures[0], gl_textures[1]);
		set_colour(162.0/256.0, 55.0/256.0, 156/256.0);
		model_view = mvstack.pop();
	}

	set_colour(1.0,1.0,1.0);
}

void drawRebelEngineFlame()
{
    model_view *= Translate(0.0,0.0,1.0);
    model_view *= Scale(0.7, 0.7, 1/8.0);
    glUniform1i( uEnableSkybox, 1);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	set_colour(1.0,1.0,1.0);
	model_view *= RotateZ(10*TIME);
    drawCone(gl_textures[13]);
	model_view *= RotateZ(-10*TIME);
    set_colour(1.0,1.0,1.0);
    glUniform1i( uEnableSkybox, 0);
    glDisable(GL_BLEND);
    model_view *= Scale(1/0.7, 1/0.7, 8.0);
	model_view *= Translate(0.0,0.0,-1.0);
}

void drawEngineFlame()
{
	glUniform1i( uEnableSkybox, 1);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    set_colour(1.0,1.0,1.0);
	model_view *= Translate(0.0,0.0,3.0);
    model_view *= RotateZ(10*TIME);
    drawCone(gl_textures[13]);
    model_view *= RotateZ(-10*TIME);
	model_view *= Translate(0.0,0.0,-3.0);
    set_colour(1.0,1.0,1.0);
    glUniform1i( uEnableSkybox, 0);
    glDisable(GL_BLEND);

}

void drawImperials(double imperialY)
{
	mvstack.push(model_view);

	model_view *= Translate(imperialY, -15.0, 0.0);


	model_view *= RotateZ(90);
	model_view *= RotateY(-45);

	mvstack.push(model_view);

	model_view *= Scale(50);
	drawSPyramid(gl_textures[16]);//, gl_textures[6]);
	model_view *= Scale(1/50.0);
	model_view *= RotateY(-45);
    //mvstack.push(model_view);
	model_view *= RotateZ(-90);
	mvstack.push(model_view);
	model_view *= Translate(0.0, 0.0, 1.2);
	model_view *= Scale(1.0, 1.0, 2.0);
	drawCube( gl_textures[16]);  //gl_textures[6]);
	model_view *= Scale(1.0, 1.0, 1/2.0);	
	model_view *= Translate(0.0, 0.0, 1.0);
	model_view *= Scale(1.0, 3.0, 1.0);
	drawCube( gl_textures[16]); //, gl_textures[6]);
	model_view = mvstack.pop();
	model_view *= RotateY(90.0);
	model_view *= RotateZ(90.0);
	model_view *= Translate(0.0, 0.0, 0.0);

	model_view = mvstack.pop();
//	drawEngineFlame();
//  model_view *= RotateZ(-90.0);
//  model_view *= RotateY(-90.0);
//	model_view *= Translate(50.0, -0.0, 0);
//	model_view *= Scale(2.0/3.0);
//    drawEngineFlame();
//	model_view *= Scale(3.0/2.0);
//	model_view *= RotateZ(-90.0);
//    model_view *= RotateY(-90.0);
//    model_view *= Translate(4.0, 0.0, 0.0);
//    model_view *= Scale(2.0/3.0);
//    drawEngineFlame();
	//model_view *= Scale(3.0/2);


	model_view = mvstack.pop();
}

void drawRebels(double rebelY)
{
        mvstack.push(model_view);
		model_view *= Translate(rebelY, 0.0, 0.0);
        model_view *= RotateY(90.0);
        model_view *= Scale(1.0,1.0,4.0f);
        drawCylinder(gl_textures[15], gl_textures[6]);
        drawRebelEngineFlame();
        model_view *= Translate(2.0f, 0.0f, 0.0f);
        drawCylinder(gl_textures[15], gl_textures[6]);
        drawRebelEngineFlame();
        model_view *= Translate(-4.0f, 0.0, 0.0);
        drawCylinder(gl_textures[15], gl_textures[6]);
        drawRebelEngineFlame();
        model_view *= Translate(-1.0f, 1.8, 0.0);
        drawCylinder(gl_textures[15], gl_textures[6]);
        drawRebelEngineFlame();
        model_view *= Translate(2.0f,0.0,0.0);
        drawCylinder(gl_textures[15], gl_textures[6]);
        drawRebelEngineFlame();
        model_view *= Translate(2.0f,0.0,0.0);
        drawCylinder(gl_textures[15], gl_textures[6]);
        drawRebelEngineFlame();
        model_view *= Translate(2.0f,0.0,0.0);
        drawCylinder(gl_textures[15], gl_textures[6]);
        drawRebelEngineFlame();

        model_view *= Translate(0.0, -3.6, 0.0);
        drawCylinder(gl_textures[15], gl_textures[6]);
        drawRebelEngineFlame();
        model_view *= Translate(-2.0f,0.0,0.0);
        drawCylinder(gl_textures[15], gl_textures[6]);
        drawRebelEngineFlame();
        model_view *= Translate(-2.0f,0.0,0.0);
        drawCylinder(gl_textures[15], gl_textures[6]);
        drawRebelEngineFlame();
        model_view *= Translate(-2.0f,0.0,0.0);
        drawCylinder(gl_textures[15], gl_textures[6]);
        drawRebelEngineFlame();
        model_view *= Scale(1.0, 1.0, 1/4.0);
        model_view *= Translate(1.0+2.0, 1.8, -6.0);
        model_view *= RotateX(-90.0);
        model_view *= Scale(8.0, 8.0, 5.0);
        drawPyramid(gl_textures[15], gl_textures[6]);
        model_view *= Scale(1/8.0, 1/8.0, 1/5.0);
        model_view *= RotateX(90.0);
        model_view *= Translate(0.0, 0.0, -4.0-4);
        model_view *= RotateZ(90);
        model_view *= Scale(1.0, 1.0, 8.0);
        drawCylinder(gl_textures[15], gl_textures[6]);
        model_view *= Scale(1.0, 1.0, 1/8.0);
        model_view *= RotateZ(45.0);
        model_view *= Scale(2.5, 2.5, 7.0);
        drawCube(gl_textures[15], gl_textures[6]);
        model_view *= Scale(1/2.5, 1/2.5, 1/7.0);
        model_view *= RotateZ(-45.0);
        model_view *= Translate(0.0, 0.0, -4.0-4.0);
        model_view *= RotateX(90.0);
        model_view *= Scale(2.0, 2.0, 4.0);
        drawCylinder(gl_textures[15], gl_textures[6]);


        model_view = mvstack.pop();
}

// Display Function (Primary Changes)
void display(void)
{
	numFrames += 1.0;
	INST_FPS = 1.0/ (PTM.GetElapsedTime());
	if( Animate )
		AV_FPS = numFrames/TM.GetElapsedTime();	
	else
	{
		AV_FPS = INST_FPS;
	}
	PTM.Reset();
	printf("\rTIME:  %.2fs  AV.FPS:  %.2f  INST.FPS:  %.2f    ", TIME, AV_FPS, INST_FPS) ;	
    // Clear the screen with the background colour (set in myinit)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    model_view = mat4(1.0f);
    
	if ( SCENE_0_START <= TIME && TIME < SCENE_0_END ) //titlecard 1
	{
        glUniform4f(uLightPos,  15.0f, 15.0f, 30.0f, 0.0f);
        default_camera();
        place_camera();
        double fade;
        double eighth = (SCENE_0_END-SCENE_0_START)/8;
        if (TIME < SCENE_0_START+eighth)
        {
            fade = 1.0f;
            glUniform1f(uFade, (float)fade);
        }
        else if (TIME > SCENE_0_END - 3*eighth && TIME < SCENE_0_END-eighth )
        {
            fade = sinBetween(SCENE_0_END-3*eighth, SCENE_0_END-eighth, 1.0, 0.0, TIME);
            glUniform1f(uFade, (float)fade);
        }

        //double fade = sinBetween(SCENE_2_START, SCENE_2_END, 1.0, 0.0, TIME);
        glUniform1i(uEnableFade, 1);
        glUniform1i( uEnableSkybox, 1);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        model_view *= Scale(800.0/55, 600.0/55, 1.0);
		if (TIME == 0.0)	
        	drawDecal(gl_textures[14]);
		else
			drawDecal(gl_textures[7]);
        model_view *= Scale(55/800.0, 55/600.0, 1/1.0);
        glUniform1i(uEnableFade, 0);
        glUniform1i( uEnableSkybox, 0);
	}
	else if ( SCENE_1_START <= TIME && TIME < SCENE_1_END ) //titlecard 2
	{
	    glUniform4f(uLightPos,  15.0f, 15.0f, 30.0f, 0.0f);
		mvstack.push(model_view);

        double duration = 7.0;
        double spinTime = 3.0;
        double spinWait = 1.0;
        double startTime = SCENE_1_START + spinTime + spinWait ;
        double endTime = SCENE_1_START + spinTime +spinWait + duration;
        double percent = 0.0f;

        if ( TIME < startTime-spinWait)
        {
            double spinScale = (TIME - SCENE_1_START)/(startTime-spinWait - SCENE_1_START);

            ref.y = -5.0; ref.z = -7.0; ref.x = 0.0;
            eye.x = -10.0 * sin (spinScale*M_PI*2);
            eye.z =  -6 + 10.0 * cos (spinScale*M_PI*2);
            eye.y = 10.0 + -10*spinScale;

            model_view *= Angel::LookAt( eye, ref, up );
            glUniformMatrix4fv( uView, 1, GL_TRUE, model_view );

        }
        else
        {
            double spinScale = 1.0;
            ref.y = -5.0; ref.z = -7.0; ref.x = 0.0;
            eye.x = -10.0 * sin (spinScale*M_PI*2);
            eye.z =  -6 + 10.0 * cos (spinScale*M_PI*2);
            eye.y = 0.0f;

            model_view *= Angel::LookAt( eye, ref, up );
            glUniformMatrix4fv( uView, 1, GL_TRUE, model_view );
        }

        model_view *= Translate( 0.0, -6.0, -7.0 );

        if(TIME >= startTime && TIME <= endTime )
        {
            percent = (TIME - startTime)/(endTime - startTime);
        }
        else if ( TIME > endTime )
            percent = 1.0;

        drawMan(percent);

		if( TIME >= SCENE_1_START && TIME < SCENE_1_START + spinTime)
		{
			double fadeIStart = SCENE_1_START;
			double fadeIEnd = SCENE_1_START + 1.0;
			double fadeI = (TIME - fadeIStart) / (fadeIEnd - fadeIStart);
			if ( fadeI > 1.0)
                fadeI = 1.0;
            else if (fadeI < 0.0)
                fadeI = 0.0;
			fadeI = 1- fadeI;
			glUniform1f(uFade, (float)fadeI);

			model_view *= RotateX(90);
            model_view *= Scale(4.0, 4.0, 4.0);
            glUniform1i(uEnableFade, 1);
            glUniform1i( uEnableSkybox, 1);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			set_colour(0.0, 0.0, 0.0);
            drawCylinder();
			set_colour(1.0,1.0,1.0);
            glUniform1i(uEnableFade, 0);
            glUniform1i( uEnableSkybox, 0);

		}
		else if( TIME >= SCENE_1_START + spinTime )
		{
			double fadeOStart = SCENE_1_END-1.0;
			double fadeOEnd	  = SCENE_1_END;
			double fadeO = (TIME - fadeOStart) / (fadeOEnd - fadeOStart);
			if ( fadeO > 1.0)
				fadeO = 1.0;
			else if (fadeO < 0.0)
				fadeO = 0.0;

			double fadeStart = spinTime + SCENE_1_START;
			double fadeEnd = fadeStart + 1.0;
			double fade = (TIME - fadeStart) / (fadeEnd - fadeStart);
			if ( fade > 1.0)
				fade = 1.0;
			glUniform1f(uFade, (float)fade);
			model_view *= Translate( 0.2, 2.0, 3.5);
			model_view *= Scale(800.0/55.0, 600.0/55.0, 1.0);
    	   	glUniform1i(uEnableFade, 1);
    	    glUniform1i( uEnableSkybox, 1);
    	    glEnable(GL_BLEND);
    	    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    	    drawDecal(gl_textures[4]);

			model_view *= Scale(55.0/800, 55.0/600, 1.0);
			model_view *= Translate( 0.0, 0.0, 1.0);
			model_view *= Scale(800.0/55.0, 600.0/55.0, 1.0);
			glUniform1f(uFade, (float)fadeO);
			set_colour(0.0f, 0.0f, 0.0f);
			drawDecal();
			set_colour(1.0f, 1.0f, 1.0f);

    	    glUniform1i(uEnableFade, 0);
	        glUniform1i( uEnableSkybox, 0);

		}

		model_view = mvstack.pop();
	}
	else if (SCENE_2_START <= TIME && TIME < SCENE_2_END ) // along time ago
	{
	    glUniform4f(uLightPos,  15.0f, 15.0f, 30.0f, 0.0f);
		default_camera();
		place_camera();
		double fade;
		double eighth = (SCENE_2_END-SCENE_2_START)/8;
		if (TIME < SCENE_2_START+eighth)
		{
			fade = 0.0f;
			glUniform1f(uFade, (float)fade);
		}
		if (SCENE_2_START+eighth < TIME && TIME < SCENE_2_START + 3*eighth)
		{
			fade = sinBetween(SCENE_2_START+eighth, SCENE_2_START+3*eighth, 0.0, 1.0, TIME);
			glUniform1f(uFade, (float)fade);
		}
		else if (TIME > SCENE_2_END - 3*eighth && TIME < SCENE_2_END-eighth )
		{
			fade = sinBetween(SCENE_2_END-3*eighth, SCENE_2_END-eighth, 1.0, 0.0, TIME);
			glUniform1f(uFade, (float)fade);
		}

		//double fade = sinBetween(SCENE_2_START, SCENE_2_END, 1.0, 0.0, TIME);
		glUniform1i(uEnableFade, 1);
	    glUniform1i( uEnableSkybox, 1);
    	glEnable(GL_BLEND);
    	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		model_view *= Scale(10.0, 2.0, 1.0);
		drawDecal(gl_textures[11]);
		model_view *= Scale(1/10.0, 1/2.0, 1/1.0);
		glUniform1i(uEnableFade, 0);
		glUniform1i( uEnableSkybox, 0);

	}
	else if ( SCENE_3_START <= TIME && TIME < SCENE_3_END ) // title crawl & opening ships
	{
		glUniform4f(uLightPos,  0.0f, -30.0f, 0.0f, 0.0f);
		double panTime = 119.00;
		double laserTime = 130.0;
		double laserLen = 0.5;
		double laserOffset = 0.0;
		double rebelY = 10.0f;
		double imperialY = 50.0f;
		if ( TIME < panTime)
		{
			Camera.x = 0.0f; Camera.y= 0.0f; Camera.z = -20.0f;
	        Camera.Rx = 25.0; Camera.Ry = 0.0f; Camera. Rz = 0.0f;
	        place_camera();
    	    draw_stars();
        	draw_title_crawl();
		}
		else if ( TIME < SCENE_3_END)
		{
			rebelY = linearBetween(laserTime-5.0f, SCENE_3_END, 100.0f, -500.0f, TIME);
			imperialY = linearBetween(laserTime-5.0f, SCENE_3_END, 250.0f, -350.0f, TIME);
			Camera.x = 0.0f; Camera.y= 0.0f; Camera.z = -20.0f;
            glUniform4f(uLightPos,  600.0f, 600.0f, 000.0f, 0.0f);
			if ( TIME < 127.0 )
			{
				Camera.Rx = sinBetween(panTime, 127.00, 25.0, 90.0, TIME);
			}
			else 
			{
				Camera.Rx = 90.0;
			}

			if( TIME >140.0 && TIME <SCENE_3_END)
			{
				glUniform4f(uLightPos,  -600.0f, -600.0f, 0.0f, 0.0f);
				Camera.x = -10.0;
				Camera.y = 300.0;
				Camera.z = -15.0f;
				Camera.Rx = 270.0;
			}
			place_camera();
			draw_stars();
			draw_planets();
			model_view *= Translate(0.0, -4.0, 15.0);
			model_view *= RotateZ(90.0);
			model_view *= RotateX(90.0);
			drawRebels(rebelY);
			//model_view *= RotateY(45.0);
			drawImperials(imperialY);
			model_view *= RotateX(-90);
			model_view *= RotateZ(-90.0);
			model_view *= Translate(0.0, 4.0, -15.0);
			rebelY += 4.0;
			//drawImperialBastards(imperialY);
			for( int i = 0; i < 10; i ++)
			{
				draw_laser(-7,16,imperialY,rebelY,laserTime+laserOffset, laserTime+laserLen+laserOffset,1,0,TIME);
				draw_laser(-4,15,imperialY,rebelY,laserTime+laserLen+laserOffset, laserTime+laserLen*2+laserOffset,1,1,TIME);
				draw_laser(3,17,rebelY,imperialY,laserTime+laserLen*3+laserOffset, laserTime+laserLen*4+laserOffset,0,1,TIME);
				draw_laser(-5,16,imperialY,rebelY,laserTime+laserLen*4+laserOffset, laserTime+laserLen*5+laserOffset,1,1,TIME);
				draw_laser(0,17,rebelY, imperialY,laserTime+laserLen*6+laserOffset, laserTime+laserLen*7+laserOffset,0,0,TIME);
				draw_laser(1,16,rebelY, imperialY,laserTime+laserLen*7+laserOffset, laserTime+laserLen*8+laserOffset,0,1,TIME);
				draw_laser(3,15,imperialY,rebelY,laserTime+laserLen*9+laserOffset, laserTime+laserLen*10+laserOffset,1,1,TIME);
				laserOffset += laserLen*10;
			}
		}
	}
	else if ( SCENE_4_START <= TIME ) // forward engines hit
    {
		Animate = 1 - Animate;
		jumpTime(0.0);
		#ifndef __NOAUDIO__
        if( audioRunning )
        	kill(audioPID, SIGKILL);
        audioRunning = 0;
        #endif

	}
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
    printf("  a to toggle the animation.\n") ;
    printf("  m to toggle frame dumping.\n") ;
    printf("  q to quit.\n");
	printf("  Num to jump chapters.\n");
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

        //eye.x = 7*sin(TIME);
        //eye.z = 7*cos(TIME);
        
        
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
