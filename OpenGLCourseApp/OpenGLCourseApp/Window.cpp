#include "Window.h"

Window::Window()
{
	width = 800;
	height = 600;

    xChange = 0.0f;
    yChange = 0.0f;

    for (size_t i = 0; i < 1024; i++) {
        keys[i] = 0;
    }    
}

Window::Window(GLuint windowWidth, GLuint windowHeight)
{
	width = windowWidth;
	height = windowHeight;
}

int Window::Initialise()
{
    // Initialise GLFW
    if (!glfwInit()) {
        printf("GLFW initialasation failed");
        glfwTerminate();
        return 1;
    }

    // Setup GLFW windows properties
    // OpenGL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //Core profile = no backwards compatibility
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //Allow Forward compatibility
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    mainWindow = glfwCreateWindow(width, height, "Test Window", NULL, NULL);
    if (!mainWindow) {
        printf("glfw Window Create failed");
        glfwTerminate();
        return 1;
    }

    // Get Buffer size information
    glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

    //set contect for GLEW to use
    glfwMakeContextCurrent(mainWindow);

    //Handle key and mouse Input
    createCallbacks();
    glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //Allow moder extensin feuters
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK) {
        printf("GLEW initialasation failed");
        glfwDestroyWindow(mainWindow);
        glfwTerminate();
        return 1;
    }

    glEnable(GL_DEPTH_TEST);

    // setUpp viewport size
    glViewport(0, 0, bufferWidth, bufferHeight);

    glfwSetWindowUserPointer(mainWindow, this);

	return 0;
}

void Window::createCallbacks()
{
    glfwSetKeyCallback(mainWindow, handleKeys);
    glfwSetCursorPosCallback(mainWindow, handleMouse);
}

GLfloat Window::getXChange()
{
    GLfloat theChange = xChange;
    xChange = 0.0f;
    return theChange;
}

GLfloat Window::getYChange()
{
    GLfloat theChange = yChange;
    yChange = 0.0f;
    return theChange;
}

void Window::handleKeys(GLFWwindow* window, int key, int code, int action, int mode)
{
    Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

    if (key == GLFW_KEY_ESCAPE && GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            theWindow->keys[key] = true;
            //printf("pressed: %d\n", key);
        }
        else if (action == GLFW_RELEASE) {
            theWindow->keys[key] = false;
            //printf("realeased: %d\n", key);
        }
    }
}

void Window::handleMouse(GLFWwindow* window, double xPos, double yPos)
{
    Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

    if (theWindow->mousedFirstMoved) {
        theWindow->lastX = xPos;
        theWindow->lastY = yPos;
        theWindow->mousedFirstMoved = false;
    }


    theWindow->xChange = xPos - theWindow->lastX;
    //(yPos - theWindow->lastY) is inverted controls
    theWindow->yChange = theWindow->lastY - yPos;

    theWindow->lastX = xPos;
    theWindow->lastY = yPos;

    //printf("x: %.6f, y: %.6f\n", theWindow->xChange, theWindow->yChange);
}



Window::~Window()
{
    glfwDestroyWindow(mainWindow);
    glfwTerminate();
}


