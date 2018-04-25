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
//#include <vector>
#include <glew.h>
#include <freeglut.h>
#include <glfw3.h>
// Include GLFW
#include <glfw3.h>
// Include GLM
#include <glm\glm.hpp>
#include <glm\vec3.hpp>
#include <glm\vec4.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include <png.h>
#include <time.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>

#include "load_shaders.h"
//#include <common/texture.hpp>
#include "controls.h"
#include "objloader.h"
using namespace glm;

GLFWwindow*window;
GLuint prog_hdlr;
int MAXFPS = 1000/30;
int SCREEN_WIDTH = 800, SCREEN_HEIGHT = 600;

double alastTime;
double acurrentTime;
float adeltaTime;

int frameCount = 0;

void wait(double seconds)
{
	clock_t endwait;
	endwait = clock () + seconds * CLOCKS_PER_SEC;
	while(clock() < endwait){}
	
}
int main()
{
	glfwInit();
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//Open a window and create its OpenGL context
	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tutorial 07 - Model Loading", NULL, NULL);
	if(window == NULL)
	{
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwSetWindowPos(window, 11, 30);
	glfwMakeContextCurrent(window);
	//Initialize GLEW Needed for core profile
	glewExperimental = true;
	if(glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE); //Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //Hide the mouse and enable unlimited mouvement
	//Set the mouse at the center of the screen
	glfwPollEvents();
	glfwSetCursorPos(window, SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
	
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); //Accept fragment if it closer to the camera than the former one
	glEnable(GL_CULL_FACE); //Cull triangles which normal is not towards the camera

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	//Create and compile our GLSL program from the shaders
	GLuint programID; //= LoadShaders("TransformVertexShader.vertexshader", "TextureFragmentShader.fragmentshader");
	loadShaders(programID, "TransformVertexShader.vertexshader", "TextureFragmentShader.fragmentshader");
	//Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	//Load the texture
	GLuint Texture = 1;//loadDDS("uvmap.DDS");
	//Get a handle for our "myTextureSampler" uniform
	GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");

	//Read our .obj file
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals; // Won't be used at the moment.
	bool res = loadOBJ("cube.obj", vertices, uvs, normals);

	//Load it into a VBO
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
	//Textures
	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

	glfwSwapInterval(0); //FRAMERATE 0 = UNLOCK 1 = LIMIT TO 60FPS


	static double limitFPS = 1.0 / 30.0;
	double lastTime = glfwGetTime(), timer = lastTime;
	double deltaTime = 0, nowTime = 0;
	int frames = 0 , updates = 0;

	double test;

	for(;;)
	{
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		glUseProgram(programID); //Use our shader

		/*
		nowTime = glfwGetTime();
        deltaTime += (nowTime - lastTime) / limitFPS;
        lastTime = nowTime;
        while(deltaTime >= 1.0)
		{
            //update(); //Update function
            updates++;
            deltaTime--;
        }
		*/

		acurrentTime = glfwGetTime();
		adeltaTime = float(acurrentTime - alastTime);
		//printf("DELTA: %f\n", adeltaTime);
		//printf("currTime: %f\n", acurrentTime);
		computeMatricesFromInputs();//adeltaTime);
		
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();
		glm::mat4 ModelMatrix = glm::mat4(1.0);
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		//Send our transformation to the currently bound shader in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		//Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glUniform1i(TextureID, 0); //Set our "myTextureSampler" sampler to use Texture Unit 0
		//1st attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		//2nd attribute buffer : UVs
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
		//printf("SIZETEST: %d\n", vertices.size());
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		glfwSwapBuffers(window); //Swap buffers
		glfwPollEvents();

		alastTime = acurrentTime;
		//printf("alastTime: %f\n", alastTime);
		frames++;

		acurrentTime = glfwGetTime();
		test = (acurrentTime - alastTime);
		printf("TEST: %f\n", test);
		wait(limitFPS - test);

		//Reset after one second
		if(glfwGetTime() - timer > 1.0)
		{
			timer++;
			//std::cout << "FPS: " << frames << " Updates:" << updates << std::endl;
			updates = 0, frames = 0;
		}
		if((glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && glfwWindowShouldClose(window) == 0))break;
	}
	//while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);
	//Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteProgram(programID);
	glDeleteTextures(1, &Texture);
	glDeleteVertexArrays(1, &VertexArrayID);
	glfwTerminate(); //Close OpenGL window and terminate GLFW
	return 0;
}