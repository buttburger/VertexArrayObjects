#define GLEW_STATIC
#define FREEGLUT_STATIC
#define _LIB
#define FREEGLUT_LIB_PRAGMAS 0
#pragma comment(lib, "libpng16.lib")
#pragma comment (lib, "libglew32.lib")
#pragma comment(lib, "libglew32.lib")
#pragma comment(lib, "freeglut_static.lib")
#include <stdio.h>
#include <stdlib.h>
#include <glew.h>
#include <freeglut.h>
#include <glm\glm.hpp>
#include <glm\vec3.hpp>
#include <glm\vec4.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <png.h>

#include "Camera.h"
#include "object.h"
#include "load_shaders.h"
#include "load_textures.h"
//#include "object.h"
using namespace std;

int refreshMS = 1000/60; //60fps
GLuint prog_hdlr, program;
const int SCREEN_WIDTH = 800, SCREEN_HEIGHT = 600;

float f[] =
{
	-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		 1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f,

		 /*
		 1.000000, -1.000000, -1.000000,
		 1.000000, -1.000000, 1.000000,
		 -1.000000, -1.000000, 1.000000,
		 -1.000000, -1.000000, -1.000000,
		 1.000000, 1.000000, -0.999999,
		 0.999999, 1.000000, 1.000001,
		 -1.000000, 1.000000, 1.000000,
		 -1.000000, 1.000000, -1.000000,
		 */
};
float texarrays[] =
{
	0.000059f, 1.0f-0.000004f, 
		0.000103f, 1.0f-0.336048f, 
		0.335973f, 1.0f-0.335903f, 
		1.000023f, 1.0f-0.000013f, 
		0.667979f, 1.0f-0.335851f, 
		0.999958f, 1.0f-0.336064f, 
		0.667979f, 1.0f-0.335851f, 
		0.336024f, 1.0f-0.671877f, 
		0.667969f, 1.0f-0.671889f, 
		1.000023f, 1.0f-0.000013f, 
		0.668104f, 1.0f-0.000013f, 
		0.667979f, 1.0f-0.335851f, 
		0.000059f, 1.0f-0.000004f, 
		0.335973f, 1.0f-0.335903f, 
		0.336098f, 1.0f-0.000071f, 
		0.667979f, 1.0f-0.335851f, 
		0.335973f, 1.0f-0.335903f, 
		0.336024f, 1.0f-0.671877f, 
		1.000004f, 1.0f-0.671847f, 
		0.999958f, 1.0f-0.336064f, 
		0.667979f, 1.0f-0.335851f, 
		0.668104f, 1.0f-0.000013f, 
		0.335973f, 1.0f-0.335903f, 
		0.667979f, 1.0f-0.335851f, 
		0.335973f, 1.0f-0.335903f, 
		0.668104f, 1.0f-0.000013f, 
		0.336098f, 1.0f-0.000071f, 
		0.000103f, 1.0f-0.336048f, 
		0.000004f, 1.0f-0.671870f, 
		0.336024f, 1.0f-0.671877f, 
		0.000103f, 1.0f-0.336048f, 
		0.336024f, 1.0f-0.671877f, 
		0.335973f, 1.0f-0.335903f, 
		0.667969f, 1.0f-0.671889f, 
		1.000004f, 1.0f-0.671847f, 
		0.667979f, 1.0f-0.335851f,

		/*
		0.624375, 0.500624,
		0.375625, 0.749375,
		0.375624, 0.500625,
		0.375624, 0.003126,
		0.624374, 0.251874,
		0.375625, 0.251875,
		0.873126, 0.998126,
		0.624375, 0.749375,
		0.873126, 0.749375,
		0.375625, 0.998126,
		0.126874, 0.749375,
		0.126874, 0.998126,
		0.624373, 0.003126,
		0.624375, 0.998126,
		*/
};
float normals[] =
{
	0.0000, 0.0000, -1.0000,
	0.0000, 0.0000, 1.0000,
	1.0000, 0.0000, -0.0000,
	-0.0000, -1.0000, -0.0000,
	-1.0000, 0.0000, -0.0000,
	0.0000, 1.0000, 0.0000,
};
unsigned indarr[] =
{
	1,3,0,
	7,5,4,
	4,1,0,
	5,2,1,
	2,7,3,
	0,7,4,
	1,2,3,
	7,6,5,
	4,5,1,
	5,6,2,
	2,6,7,
	0,3,7,
};

unsigned int index, color, tx;
int offset = 0;
int swapshader = 0;


int modelx, modely, modelz;
int wkey=0, skey=0, akey=0, dkey=0;

float lastx, lasty;
float xrot = 0, yrot = 0, zrot = 0;
float camx = 0, camy = -3, camz = 0;
float targetx=0, targety=0, targetz=0;

GLuint test = 3;
GLuint test1 = 6;
GLuint Texture;

GLuint vertexbuffer;
GLuint uvbuffer;

//float xtest,ztest;

void display()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	/*
	if(wkey == 1)
	{
		float xrotrad, yrotrad, zrotrad;
		xrotrad = (xrot/180*3.141592654f);
		yrotrad = (yrot/180*3.141592654f);
		zrotrad = (zrot/180*3.141592654f);
		camx -= float(sin(zrotrad));
		camy += float(cos(zrotrad));
		//camz -= float(cos(xrotrad));
	}
	if(skey == 1)
	{
		float xrotrad, yrotrad, zrotrad;
		xrotrad = (xrot/180*3.141592654f);
		yrotrad = (yrot/180*3.141592654f);
		zrotrad = (zrot/180*3.141592654f);
		camx += float(sin(zrotrad));
		camy -= float(cos(zrotrad));
		//camz += float(cos(xrotrad));
	}
	if(akey == 1)
	{
		float zrotrad;
		zrotrad = (zrot/180*3.141592654f);
		camx -= float(cos(zrotrad))*(float)0.75;
		camy -= float(sin(zrotrad))*(float)0.75;
	}
	if(dkey == 1)
	{
		float zrotrad;
		zrotrad = (zrot/180*3.141592654f);
		camx += float(cos(zrotrad))*(float)0.75;
		camy += float(sin(zrotrad))*(float)0.75;
	}
	*/

	GLuint uniformModel = glGetUniformLocation(prog_hdlr, "model");
	GLuint uniformView = glGetUniformLocation(prog_hdlr, "view");
	GLuint uniformProjection = glGetUniformLocation(prog_hdlr, "projection");
	GLuint uniformTexture = glGetUniformLocation(prog_hdlr, "myTexture");

	//printf("Uniform Test %d - %d - %d - %d\n", uniformModel, uniformView, uniformProjection, uniformTexture);

	glm::mat4 projection = glm::perspective(glm::radians(60.0f), 1.0f, 0.1f, 100.0f);
	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
	glm::vec3 eye = glm::vec3(camx, camy - 1, camz);	
	glm::vec3 target = glm::vec3(camx + targetx, camy + targety, camz + targetz);
	glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);
	//glm::mat4 view = glm::lookAt(eye, target, up);
	glm::mat4 view = LookAt(eye, target, up);
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(view));
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(modelx, modely, modelz));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);
	glUniform1i(glGetUniformLocation(prog_hdlr, "myTexture"), 0);

	// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			2,                                // size : U+V => 2
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 12*3); // 12*3 indices starting at 0 -> 12 triangles

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
	glutSwapBuffers();
}
void keyDown(unsigned char key, int x, int y)
{
	if(key == 'w')wkey = 1;
	if(key == 's')skey = 1;
	if(key == 'a')akey = 1;
	if(key == 'd')dkey = 1;

	if(key == 27)exit(0);

	if(key == 'i')modelx++;
	if(key == 'j')modelx--;

	if(key == 'o')modely++;
	if(key == 'k')modely--;

	if(key == 'p')modelz++;
	if(key == 'l')modelz--;

	if(key == 't')camx++;
	if(key == 'f')camx--;

	if(key == 'y')camy++;
	if(key == 'g')camy--;

	if(key == 'u')camz++;
	if(key == 'h')camz--;

	if(key == 'c')targetx++;
	if(key == 'x')targetx--;

	if(key == 'b')targety++;
	if(key == 'v')targety--;

	if(key == 'm')targetz++;
	if(key == 'n')targetz--;

	if(key == '1' && swapshader != 1)
	{
		setShaders(test, 1, 2);
		swapshader = 1;
	}
	if(key == '2' && swapshader != 2)
	{
		setShaders(test1, 4, 5);
		swapshader = 2;
	}

}
void keyUp(unsigned char key, int x, int y)
{
	//printf("KeyUp %d\n", key);
	if(key == 'w')wkey = 0;
	if(key == 's')skey = 0;
	if(key == 'a')akey = 0;
	if(key == 'd')dkey = 0;
}
void timer(int value)
{
	glutPostRedisplay();
	glutTimerFunc(refreshMS, timer, 0);
}
void mouseMovement(int x, int y)
{
	int diffx = x-(int)lastx;
	int diffy = y-(int)lasty;
	lastx = (float)x;
	lasty = (float)y;
	xrot -= (float)diffy; //UP AND DOWN
	//yrot += (float)diffx; //TILT LEFT AND RIGHT
	zrot -= (float)diffx; //LEFT AND RIGHT

	//ztest =	cos(xrot/180*3.141592654f);
	//xtest = sin(zrot/180*3.141592654f);
	//printf("xrot: %f zrot: %f\n", xtest, ztest);
}
void initGL()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	/*
	GLfloat LightModelAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);
	GLfloat LightAmbient[] = {0.25f, 0.25f, 0.25f, 1.0f};
	glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);
	GLfloat LightDiffuse[] = {0.75f, 0.75f, 0.75f, 1.0f};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);
	GLfloat light0_position[4] = {1,1,1,0};
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	
	GLfloat MaterialAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);
	GLfloat MaterialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);
	
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	*/
	
	glEnable(GL_CULL_FACE);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //WIREFRAME MODE
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //FILL OBJECT DRAW

	glewInit();
	if(GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader && GL_EXT_geometry_shader4)
	{
		printf("Ready for GLSL - vertex, fragment, and geometry units\n");
		loadShaders(prog_hdlr, "shaders/vert_test.glsl", "shaders/frag_test.glsl");
		loadShaders(prog_hdlr, "shaders/vert_shader.glsl", "shaders/frag_shader.glsl");
		
		glGenBuffers(1, &vertexbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(f), f, GL_STATIC_DRAW);

		glGenBuffers(1, &uvbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(texarrays), texarrays, GL_STATIC_DRAW);

		glEnable(GL_TEXTURE_2D);
		int width, height;
		bool hasAlpha;
		char filename[] = "texture.png", filename1[] = "texture1.png";
		//num_textures[0]=LoadBMP("texture.bmp");
		//num_textures[1] = LoadBMP("texture1.bmp");
		//printf("TEXTURE: %d - %d\n", num_textures[0], num_textures[1]);

		//num_textures[0] = loadPngImage(filename, width, height, hasAlpha, &textureImage);
		//num_textures[1] = loadPngImage(filename1, width, height, hasAlpha, &textureImage);
		//printf("TEXTURE: %d - %d\n", num_textures[0], num_textures[1]);

		Texture = loadPngImage(filename, width, height, hasAlpha, &textureImage);
		printf("LOAD TEXTURE %d\n", Texture);
	}
}
int main(int argc, char**argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB|GLUT_DEPTH|GLUT_DOUBLE);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutCreateWindow("GLSL tutorial");
	initGL();
	glutDisplayFunc(display); //glutIdleFunc(display);
	//glutPassiveMotionFunc(mouseMovement);
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);
	glutKeyboardFunc(keyDown);
	glutKeyboardUpFunc(keyUp);
	glutTimerFunc(0, timer, 0);
	glutMainLoop();
	return 0;
}
/*
//glutReshapeFunc(reshape);
void reshape(GLsizei w, GLsizei h)
{
	if(h == 0)h = 1;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, 1.0*(GLfloat)w/(GLfloat)h, 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
}
*/