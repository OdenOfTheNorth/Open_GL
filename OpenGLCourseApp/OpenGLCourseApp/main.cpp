#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Mesh.h"
#include "Shader.h"
#include "Window.h"
#include "Camera.h"

const float toRadians = 3.14159265f / 180.f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader*> shaderList;
Camera camera;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

GLfloat moveSpeed = 5.0f;
GLfloat turnSpeed = 0.2f;

// Vertex Shader
static const char* vShader = "Shaders/shader.vert";

//fragment shader
static const char* fShader = "Shaders/shader.frag";

void CreateObject()
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

    Mesh* obj1 = new Mesh();
    obj1->CreateMesh(vertices, indices, 12, 12);
    meshList.push_back(obj1);

    Mesh* obj2 = new Mesh();
    obj2->CreateMesh(vertices, indices, 12, 12);
    meshList.push_back(obj2);
}

void CreateShaders() {
    Shader* shader1 = new Shader();
    shader1->CreateFromFiles(vShader, fShader);
    shaderList.push_back(shader1);
}

int main()
{
    mainWindow = Window();
    mainWindow.Initialise();

    CreateObject();
    CreateShaders();
    camera = Camera(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f), -90.f, 0.f, moveSpeed, turnSpeed);

    GLfloat aspectRation = mainWindow.GetBufferWidth() / mainWindow.GetBufferHeight();
    glm::mat4 projection = glm::perspective(45.f, aspectRation, 0.1f, 100.f);

    GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0;

    // Loop until winows closed
    while (!mainWindow.getShouldClose())
    {

        GLfloat now = glfwGetTime(); // SDL_GetPerfomaceCounter();
        deltaTime = now - lastTime; // (now - lastTime) * 1000 / SDL_GetPerfaomceFrequency(); multiplay by 1000 because we get miliseconds instead of seconds.
        lastTime = now;

        // Get + Handle user input events
        glfwPollEvents();

        camera.keyControl(mainWindow.getKeys(), deltaTime);
        camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

        // clear the window
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderList[0]->UseShader();
        uniformModel = shaderList[0]->GetModelLocation();
        uniformProjection = shaderList[0]->GetProjectionLocation();
        uniformView = shaderList[0]->GetViewLocation();

        glm::mat4 model(1.0f);

        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));
        model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
        meshList[0]->RenderMesh();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 1.0f, -2.5f));
        model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        meshList[1]->RenderMesh();

        glUseProgram(0);

        mainWindow.swapBuffers();
    }

    return 0;
}