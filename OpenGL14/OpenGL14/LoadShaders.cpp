#define GLEW_STATIC
#define FREEGLUT_STATIC
#pragma comment (lib, "freeglut_static.lib")
#pragma comment (lib, "libglew32.lib")
//#pragma comment (lib, "libpng16.lib")
#include <stdio.h>
#include <stdlib.h>
#include <GL\glew.h>
#include <GL\freeglut.h>
//#include <iostream>
#include <fstream>

#include "LoadShaders.h"

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