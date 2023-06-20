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

	bool* getKeys() { return keys; }
	GLfloat getXChange();
	GLfloat getYChange();

	~Window();

private:
	GLFWwindow* mainWindow;

	GLint width, height;
	GLint bufferWidth, bufferHeight;

	//keys
	bool keys[1024];

	//Mouse
	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;

	bool mousedFirstMoved;

	void createCallbacks();
	static void handleKeys(GLFWwindow* window, int key, int code, int action, int mode);

	static void handleMouse(GLFWwindow* window, double xPos, double yPos);
};

