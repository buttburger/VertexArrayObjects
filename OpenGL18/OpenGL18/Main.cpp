#define GLEW_STATIC
#define FREEGLUT_STATIC
#define _LIB
#define FREEGLUT_LIB_PRAGMAS 0
#pragma comment(lib, "libpng16.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "freeglut_static.lib")
#pragma comment(lib, "glfw3.lib")
#include <stdio.h>
#include <stdlib.h>
#include <glew.h>
#include <freeglut.h>
#include <glfw3.h>

#include <glm\glm.hpp>
#include <glm\vec3.hpp>
#include <glm\vec4.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include <png.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>

#include "objloader.h"
#include "Camera.h"
#include "object.h"
#include "load_shaders.h"
#include "load_textures.h"
#include "object.h"
using namespace std;

int refreshMS = 1000/60; //60fps
GLuint prog_hdlr;
const int SCREEN_WIDTH = 800, SCREEN_HEIGHT = 600;

GLuint Texture;
GLuint vertexbuffer;
GLuint colorbuffer;
GLuint uvbuffer;
GLuint normalbuffer;
GLuint elementbuffer;

GLuint LightID;

GLuint test = 3;
GLuint test1 = 6;
unsigned int index, color, tx;
int offset = 0;
int swapshader = 0;

int wkey=0, skey=0, akey=0, dkey=0; //WASD KEY
float lastx, lasty, xrot = 0, yrot = 0, zrot = 0;

int modelx, modely, modelz; //MOVE LOADED OBJECT
float targetx = 0, targety = 1, targetz = 0;
float camx = 0, camy = 5, camz = 0;
glm::vec3 eye = glm::vec3(camx, camy, camz);

int xpos = 0, ypos = 0;
float horizontalAngle = 0.0f;
// Initial vertical angle : none
float verticalAngle = 0.0f;
float speed = 3.0f; // 3 units / second
float mouseSpeed = 0.005f;

std::vector<glm::vec3> vectorvertices;
std::vector<glm::vec2> vectoruvs;
std::vector<glm::vec3> vectornormals; // Won't be used at the moment.
int verticevectorsize;

void display()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	GLuint uniformModel = glGetUniformLocation(prog_hdlr, "model");
	GLuint uniformView = glGetUniformLocation(prog_hdlr, "view");
	GLuint uniformProjection = glGetUniformLocation(prog_hdlr, "projection");
	GLuint uniformTexture = glGetUniformLocation(prog_hdlr, "myTexture");
	//printf("Uniform Test %d - %d - %d - %d\n", uniformModel, uniformView, uniformProjection, uniformTexture);
	glm::mat4 projection = glm::perspective(glm::radians(60.0f), (float)SCREEN_WIDTH/(float)SCREEN_HEIGHT, 0.1f, 1000.0f);
	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

	glutWarpPointer(SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
	//SetCursorPos(SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
	horizontalAngle += mouseSpeed*float(SCREEN_WIDTH/2 - xpos);
	verticalAngle   += mouseSpeed*float(SCREEN_HEIGHT/2 - ypos);
	//printf("hAngle: %f xPos: %d yPos: %d\n", horizontalAngle, xpos, ypos);
	
	glm::vec3 target = //glm::vec3(targetx, targety, targetz);
	glm::vec3(
	cos(verticalAngle) * sin(horizontalAngle), 
	sin(verticalAngle),
	cos(verticalAngle) * cos(horizontalAngle));

	// Right vector
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f/2.0f),
		0,
		cos(horizontalAngle - 3.14f/2.0f));

	glm::vec3 up = glm::cross(right, target);
	//printf("UP: %f - %f - %f\n", up[0], up[1], up[2]);
	if(wkey == 1)eye += target*0.75f*speed; //Move forward
	if(skey == 1)eye -= target*0.75f*speed; //Move backward
	if(dkey == 1)eye += right* 0.75f*speed; //Strafe right
	if(akey == 1)eye -= right* 0.75f*speed; //Strafe left

	//float hAngleTEST = sin(horizontalAngle - 3.14f/2.0f);
	//printf("hAngleTEST: %f\n", hAngleTEST);
	//glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f); //up = glm::cross(right, target);

	glm::mat4 view = LookAt(eye, eye+target, up);
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(modelx, modely, modelz));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

	glm::vec3 lightPos = glm::vec3(4,4,4);
	glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);
	glUniform1i(glGetUniformLocation(prog_hdlr, "myTexture"), 0);

	glEnableClientState(GL_VERTEX_ARRAY);
	//glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	//vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	//colors
	/*
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (GLvoid*)(3 * sizeof(GL_FLOAT)));
	*/
	//textures
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	//Normals
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glVertexAttribPointer(
		2,        // attribute
		3,        // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0,        // stride
		(void*)0  // array buffer offset
	);
	//Indices
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)0);
	glDrawArrays(GL_TRIANGLES, 0, vectorvertices.size() * sizeof(glm::vec3));

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glBindTexture(GL_TEXTURE_2D, 0);
	glutSwapBuffers();
}
void keyDown(unsigned char key, int x, int y)
{
	if(key == 27)exit(0);
	if(key == 'w')wkey = 1;
		if(key == 's')skey = 1;
			if(key == 'a')akey = 1;
				if(key == 'd')dkey = 1;
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
void initGL()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	
	xpos = SCREEN_WIDTH/2, ypos = SCREEN_HEIGHT/2;
	
	glEnable(GL_CULL_FACE);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //WIREFRAME MODE
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //FILL OBJECT DRAW

	glewInit();
	if(GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader && GL_EXT_geometry_shader4)
	{
		printf("Ready for GLSL - vertex, fragment, and geometry units\n");
		loadShaders(prog_hdlr, "shaders/vert_shader.glsl", "shaders/frag_shader.glsl");
		loadShaders(prog_hdlr, "shaders/vert_test.glsl", "shaders/frag_test.glsl");

		bool res = loadOBJ("cube.obj", vectorvertices, vectoruvs, vectornormals);

		//vertex
		glGenBuffers(1, &vertexbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, vectorvertices.size() * sizeof(glm::vec3), &vectorvertices[0], GL_STATIC_DRAW);
		//color
		/*
		glGenBuffers(1, &colorbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(ftest), ftest, GL_STATIC_DRAW);
		*/
		glGenBuffers(1, &uvbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glBufferData(GL_ARRAY_BUFFER, vectoruvs.size() * sizeof(glm::vec2), &vectoruvs[0], GL_STATIC_DRAW);
		//normals
		glGenBuffers(1, &normalbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glBufferData(GL_ARRAY_BUFFER, vectornormals.size() * sizeof(glm::vec3), &vectornormals[0], GL_STATIC_DRAW);
		//indices
		/*
		glGenBuffers(1, &elementbuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indarr), indarr, GL_STATIC_DRAW);
		*/

		LightID = glGetUniformLocation(prog_hdlr, "LightPosition_worldspace");

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
void CallBackPassiveFunc(int x, int y) //glutPassiveMotionFunc
{
	//BLANK
}
void CallBackMouseFunc(int button, int state, int x, int y) //glutMouseFunc
{
	printf("BUTTON: %d STATE: %d X: %d Y: %d\n", button, state, x, y);
}
void CallBackMotionFunc(int x, int y) //glutMotionFunc
{
	xpos = x, ypos = y;
	printf("DRAG! X: %d Y: %d\n", xpos, ypos);
}
int main(int argc, char**argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB|GLUT_DEPTH|GLUT_DOUBLE);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutCreateWindow("GLSL tutorial");
	initGL();
	glutDisplayFunc(display);
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);
	glutKeyboardFunc(keyDown);
	glutKeyboardUpFunc(keyUp);
	//glutSpecialFunc(SpecialFunc);
	glutMouseFunc(CallBackMouseFunc);
	glutMotionFunc(CallBackMotionFunc);
	glutPassiveMotionFunc(CallBackPassiveFunc);
	glutTimerFunc(0, timer, 0);
	glutMainLoop();
	return 0;
}