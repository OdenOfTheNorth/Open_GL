#pragma once

#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Window
{
public:
	Window();
	Window(GLuint windowWidth, GLuint windowHeight);

	int Initialise();

	GLfloat GetBufferWidth()	{	return bufferWidth;		}
	GLfloat GetBufferHeight()	{	return bufferHeight;	}

	bool getShouldClose() { return glfwWindowShouldClose(mainWindow); }

	void swapBuffers() { glfwSwapBuffers(mainWindow); }

	~Window();

private:
	GLFWwindow* mainWindow;

	GLint width, height;
	GLint bufferWidth, bufferHeight;
};

