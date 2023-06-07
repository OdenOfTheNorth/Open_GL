#include <stdio.h>
#include <string.h>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Window Size
const GLint WIDTH = 800, HEIGHT = 600;
const float toRadians = 3.14159265f / 180.f;

GLuint VAO, VBO, IBO, Shader, uniformModel, uniformProjection;

bool direction = true;
float triOffset = 0.0f;
float triMaxOffset = 0.7f;
float triIncrement = 0.005f;

float currentAngle = 0;

bool sizeDirection = true;
float curSize = 0;
float minSize = 0;
float maxSize = 0.5;

// Vertex Shader
static const char* vShader = "                                                  \n\
#version 330                                                                    \n\
                                                                                \n\
layout (location = 0) in vec3 pos;                                              \n\
                                                                                \n\
out vec4 vCol;                                                                  \n\
uniform mat4 model;                                                             \n\
uniform mat4 projection;                                                        \n\
                                                                                \n\
void main(){                                                                    \n\
    gl_Position = projection * model * vec4(pos, 1.0);                          \n\
    vCol = vec4(clamp(pos, 0.0, 1.0), 1.0);                                     \n\
}                                                                               \n\
";

//fragment shader
static const char* fShader = "                                                  \n\
#version 330                                                                    \n\
                                                                                \n\
in vec4 vCol;                                                                   \n\
                                                                                \n\
out vec4 color;                                                                 \n\
                                                                                \n\
void main(){                                                                    \n\
    color = vCol;                                                               \n\
}                                                                               \n\
";


void CreateTriangle()
{
    unsigned int indices[] = {
        0, 3, 1,
        1, 3, 2,
        2, 3, 0,
        0, 1, 2,
    };

    GLfloat vertices[] = {
        -1.f, -1.f, 0.0f,
        0.f,  -1.f,  1.0f,
        1.f,  -1.f,  0.0f,
        0.0f,  1.f,  0.0f,
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

void addShader(GLuint theProgram, const char* shaderCode, GLenum shaderType) {
    GLuint theShader = glCreateShader(shaderType);

    const GLchar* theCode[1];
    theCode[0] = shaderCode;

    GLint codeLength[1];
    codeLength[0] = strlen(shaderCode);

    glShaderSource(theShader, 1, theCode, codeLength);
    glCompileShader(theShader);

    GLint result = 0;
    GLchar eLog[1024] = { 0 };

    glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);

    if (!result) {
        glGetProgramInfoLog(theShader, sizeof(eLog), NULL, eLog);
        printf("Error Compiling the %d shader: %s'\n", shaderType, eLog);
        return;
    }

    glAttachShader(theProgram, theShader);
}

void compileShaders() {
    Shader = glCreateProgram();

    if (!Shader) {
        printf("Error creating shader program");
        return;
    }

    addShader(Shader, vShader, GL_VERTEX_SHADER);
    addShader(Shader, fShader, GL_FRAGMENT_SHADER);


    GLint result = 0;
    GLchar eLog[1024] = { 0 };

    glLinkProgram(Shader);
    glGetProgramiv(Shader, GL_LINK_STATUS, &result);

    if (!result) {
        glGetProgramInfoLog(Shader, sizeof(eLog), NULL, eLog);
        printf("Error Linking program: '%s'\n", eLog);
        return;
    }

    glValidateProgram(Shader);
    glGetProgramiv(Shader, GL_VALIDATE_STATUS, &result);

    if (!result) {
        glGetProgramInfoLog(Shader, sizeof(eLog), NULL, eLog);
        printf("Error Valedating program: '%s'\n", eLog);
        return;
    }

    uniformModel = glGetUniformLocation(Shader, "model");
    uniformProjection = glGetUniformLocation(Shader, "projection");
}

int main()
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

    GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Test Window", NULL, NULL);
    if (!mainWindow) {
        printf("glfw Window Create failed");
        glfwTerminate();
        return 1;
    }

    // Get Buffer size information
    int  bufferWidth, bufferHeight;
    glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

    //set contect for GLEW to use
    glfwMakeContextCurrent(mainWindow);

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

    CreateTriangle();
    compileShaders();

    GLfloat aspectRation = (GLfloat)bufferWidth / (GLfloat)bufferHeight;
    glm::mat4 projection = glm::perspective(90.f, aspectRation, 0.1f, 100.f);

    // Loop until winows closed
    while (!glfwWindowShouldClose(mainWindow))
    {
        // Get + Handle user input events
        glfwPollEvents();

        if (direction) {
            triOffset += triIncrement;
        }
        else {
            triOffset -= triIncrement;
        }

        if (abs(triOffset) >= triMaxOffset) {
            direction = !direction;
        }

        currentAngle += 0.5f;

        if (currentAngle > 360.f) {
            currentAngle -= 360.f;
        }

        if (sizeDirection) {
            curSize += 0.001;
        }
        else {
            curSize -= 0.001;
        }

        if (curSize > maxSize || curSize < minSize) {
            sizeDirection = !sizeDirection;
        }

        // clear the window
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(Shader);

        glm::mat4 model(1.0f);

        //model = glm::translate(model, glm::vec3(triOffset, 0.f, 0.f));
        model = glm::translate(model, glm::vec3(0.0f, triOffset, -2.f));
        //model = glm::rotate(model, currentAngle * toRadians, glm::vec3(0.f, 1.f, 0.f));
        //model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));


        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));



        glBindVertexArray(VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        glUseProgram(0);

        glfwSwapBuffers(mainWindow);
    }

    return 0;
}

