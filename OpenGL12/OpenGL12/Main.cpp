#define GLEW_STATIC
#define FREEGLUT_STATIC
#pragma comment (lib, "freeglut_static.lib")
#pragma comment (lib, "libglew32.lib")
//#pragma comment (lib, "libpng16.lib")
#include <stdio.h>
#include <stdlib.h>
#include <GL\glew.h>
#include <GL\freeglut.h>
//#include <png.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
using namespace std;

int refreshMS = 15;

GLuint  prog_hdlr, program;
GLint location_attribute_0, location_viewport;
const int SCREEN_WIDTH  = 800, SCREEN_HEIGHT = 600;
const float light0_position[4] = {1,1,1,0};

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glUseProgram(prog_hdlr);
	
	//glEnableVertexAttribArray(0);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	//glDisableVertexAttribArray(0);

	glutSwapBuffers();
}
void process_keys(unsigned char key, int x, int y)
{
	if(key == 27)exit(0);
}
void reshape(GLsizei w, GLsizei h)
{
	if(h == 0) h = 1;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, 1.0*(GLfloat)w/(GLfloat)h, 0.1f, 50.0f);
	glMatrixMode(GL_MODELVIEW);
}
void printInfoLog(GLuint obj)
{
	int log_size = 0;
	int bytes_written = 0;
	glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &log_size);
	if(!log_size)return;
	char*infoLog = new char[log_size];
	glGetProgramInfoLog(obj, log_size, &bytes_written, infoLog);
	printf("%s\n", infoLog);
	delete[]infoLog;
}
bool read_n_compile_shader(const char *filename, GLuint &hdlr, GLenum shaderType)
{
	std::ifstream is(filename, std::ios::in|std::ios::binary|std::ios::ate);
	if(!is.is_open())
	{
		printf("Unable to open file %s\n", filename);
		return false;
	}
	long size = (long)is.tellg();
	char*buffer = new char[size+1];
	is.seekg(0, std::ios::beg);
	is.read (buffer, size);
	is.close();
	buffer[size] = 0;
	hdlr = glCreateShader(shaderType);
	glShaderSource(hdlr, 1, (const GLchar**)&buffer, NULL);
	glCompileShader(hdlr);
	printf("Loading info for: %s\n", filename);
	printInfoLog(hdlr);
	delete[]buffer;
	return true;
}
void setShaders(GLuint &prog_hdlr, const char*vsfile, const char*fsfile)
{
	GLuint vert_hdlr, frag_hdlr;
	read_n_compile_shader(vsfile, vert_hdlr, GL_VERTEX_SHADER);
	read_n_compile_shader(fsfile, frag_hdlr, GL_FRAGMENT_SHADER);
	prog_hdlr = glCreateProgram();
	glAttachShader(prog_hdlr, frag_hdlr);
	glAttachShader(prog_hdlr, vert_hdlr);
	glLinkProgram(prog_hdlr);
	printf("Loading linked program: ");
	printInfoLog(prog_hdlr);
}
void timer(int value)
{
	glutPostRedisplay(); //Post re-paint request to activate display()
	glutTimerFunc(refreshMS, timer, 0); //next timer call milliseconds later
}

void initGL()
{
	glClearColor(0.7f, 0.7f, 0.3f, 1.0f);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	glewInit();
	if(GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader && GL_EXT_geometry_shader4)
	{
		printf("Ready for GLSL - vertex, fragment, and geometry units\n");
		setShaders(prog_hdlr, "shaders/vert_shader.glsl", "shaders/frag_shader.glsl");
		location_attribute_0 = glGetAttribLocation(prog_hdlr, "R"); //radius
		location_viewport = glGetUniformLocation(prog_hdlr, "viewport"); //viewport
		//
		static const GLfloat f[] =
		{
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			0.0f, 0.5f, 0.0f
		};
		//
		glGenBuffers(1, &program);
		glBindBuffer(GL_ARRAY_BUFFER, program);
		//
		glBufferData(GL_ARRAY_BUFFER, sizeof(f), f, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	}
	else
	{
		printf("No GLSL support\n");
	}
}

int main(int argc, char**argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB|GLUT_DEPTH|GLUT_DOUBLE);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutCreateWindow("GLSL tutorial");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(process_keys);
	//glutIdleFunc(display);
	initGL();
	glutTimerFunc(0, timer, 0);
	glutMainLoop();
	return 0;
}
