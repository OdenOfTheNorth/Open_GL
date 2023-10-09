#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "CommonValues.h"

#include "Mesh.h"
#include "Shader.h"
#include "Window.h"
#include "Camera.h"

#include "Texture.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"

#include "Model.h"

#include "SkyBox.h"

const float toRadians = 3.14159265f / 180.f;

GLuint  uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
uniformShininess = 0, uniformSpecularIntensity = 0, uniformOmniLightPos = 0, uniformFarPlane = 0;

Window mainWindow;

std::vector<Mesh*> meshList;

std::vector<Shader> shaderList;
Shader directionalShadowShader;
Shader omniShadowShader;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture quadTexture;

Material shinyMaterial;
Material dullMaterial;
Material fighterMaterial;

std::vector<Model*> fighters;

DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

SkyBox skyBox;

unsigned int pointLightCount = 0;
unsigned int spotLightCount = 0;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

GLfloat moveSpeed = 5.0f;
GLfloat turnSpeed = 0.2f;

const int space = 3;

float currentTime = 0.0f;

// Vertex Shader
static const char* vShader = "Shaders/shader.vert";

//fragment shader
static const char* fShader = "Shaders/shader.frag";

const int fightersCount = 10;
const int gridX = 10;
const int gridY = 10;

void CalcAvarageNormals(unsigned int * indices,     unsigned int indiceCount,  GLfloat* vertices, 
                        unsigned int verticeCount,  unsigned int vLength,       unsigned int normalOffset)
{
    for (size_t i = 0; i < (int)indiceCount; i += 3) 
    {
        // Set in * vlength to acces the vertices position values
        unsigned int in0 = indices[i] * vLength;
        unsigned int in1 = indices[i + 1] * vLength;
        unsigned int in2 = indices[i + 2] * vLength;

        glm::vec3 v1(   vertices[in1]       -   vertices[in0],      // X
                        vertices[in1 + 1]   -   vertices[in0 + 1],  // Y
                        vertices[in1 + 2]   -   vertices[in0 + 2]   // Z
        );
        glm::vec3 v2(   vertices[in2]       -   vertices[in0],      // X
                        vertices[in2 + 1]   -   vertices[in0 + 1],  // Y
                        vertices[in2 + 2]   -   vertices[in0 + 2]   // Z
        );

        glm::vec3 normal = glm::cross(v1, v2);
        normal = glm::normalize(normal);

        // Add offset to acces the vertices Normals
        in0 += normalOffset;
        in1 += normalOffset;
        in2 += normalOffset;

        vertices[in0] += normal.x;
        vertices[in0 + 1] += normal.y;
        vertices[in0 + 2] += normal.z;

        vertices[in1] += normal.x;
        vertices[in1 + 1] += normal.y;
        vertices[in1 + 2] += normal.z;

        vertices[in2] += normal.x;
        vertices[in2 + 1] += normal.y;
        vertices[in2 + 2] += normal.z;

    }

    for (size_t i = 0; i < verticeCount / vLength; i++) 
    {
        unsigned int nOffset = i * vLength + normalOffset;
        glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
        vec = glm::normalize(vec);
        vertices[nOffset] = vec.x;
        vertices[nOffset + 1] = vec.y;
        vertices[nOffset + 2] = vec.z;
    }
}

void CreateObject()
{
    unsigned int indices[] = {
        0, 3, 1,
        1, 3, 2,
        2, 3, 0,
        0, 1, 2,
    };

    GLfloat vertices[] = {
    //   x      y      z         u     v         nX   nY   nZ
        -1.f,  -1.f,  -0.6f,     0.0f, 0.0f,     0.f, 0.f, 0.f,
         0.f,  -1.f,   1.0f,     0.5f, 0.0f,     0.f, 0.f, 0.f,
         1.f,  -1.f,  -0.6f,     1.0f, 0.0f,     0.f, 0.f, 0.f,
         0.0f,  1.f,   0.0f,     0.5f, 1.0f,     0.f, 0.f, 0.f
    };

    unsigned int floorIndices[] = {
        0, 2, 1,
        1, 2, 3
    };

    GLfloat floorVertices[] = {
        //   x        y      z          u        v         nX   nY   nZ
            -10.f,   -0.f,  -10.0f,     0.0f,    0.0f,     0.f, -1.f, 0.f,
             10.f,   -0.f,  -10.0f,     10.f,    0.0f,     0.f, -1.f, 0.f,
            -10.f,   -0.f,   10.6f,     1.0f,    10.0f,    0.f, -1.f, 0.f,
             10.0f,   0.f,   10.0f,     10.0f,   10.0f,    0.f, -1.f, 0.f
    };

    CalcAvarageNormals(indices, 12, vertices, 32, 8, 5);

    const int grid = gridX * gridY;

    for (size_t i = 0; i < grid; i++) {
        Mesh* obj = new Mesh();
        obj->CreateMesh(vertices, indices, 32, 12);
        meshList.push_back(obj);
    }

    Mesh* plane = new Mesh();
    plane->CreateMesh(floorVertices, floorIndices, 32, 6);
    meshList.push_back(plane);
}

void CreateFighters() {
  
    for (size_t i = 0; i < fightersCount; i++) {
        Model* fighter = new Model();
        fighter->LoadModel("Models/uh60.obj");
        fighters.push_back(fighter);
    }   
}

void CreateShaders() {
    Shader* shader1 = new Shader();
    shader1->CreateFromFiles(vShader, fShader);
    shaderList.push_back(*shader1);

    directionalShadowShader = Shader();
    directionalShadowShader.CreateFromFiles("Shaders/directional_shadow_map.vert", "Shaders/directional_shadow_map.frag");

    omniShadowShader = Shader();
    omniShadowShader.CreateFromFiles("Shaders/omni_directional_shadow_map.vert", "Shaders/omni_directional_shadow_map.geom", "Shaders/omni_directional_shadow_map.frag");
}

void RenderScene()
{
    glm::mat4 model(1.0f);

    const float angle = 360 / fightersCount;

    for (size_t i = 0; i < fightersCount; i++) {
        
        //currentTime +
        //cos(currentTime + i)
        model = glm::mat4(1.0f);
        model = glm::rotate(model, currentTime + (angle * i * toRadians),glm::vec3(0.0, -1.0, 0));
        //model = glm::translate(model, glm::vec3(0, 2, 0));
        model = glm::translate(model, glm::vec3(-10.0, cos(currentTime + i), 0));
        model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0, 0.0, 0.0));
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        //dullMaterial.useMaterial(uniformSpecularIntensity, uniformShinines);
        fighters[i]->RenderModel();
    }

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
    model = glm::scale(model, glm::vec3(3.0f, 1.0f, 3.0f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    dirtTexture.UseTexture();
    dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
    meshList[gridX * gridY]->RenderMesh();

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -10.0f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    brickTexture.UseTexture();
    dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
    meshList[0]->RenderMesh();
}

void DirectionalShadowMapPass(DirectionalLight* light)
{
	directionalShadowShader.UseShader();

	glViewport(0, 0, light->GetShadowMap()->GetShadowWidth(), light->GetShadowMap()->GetShadowHeight());

	light->GetShadowMap()->Write();
	glClear(GL_DEPTH_BUFFER_BIT);

	uniformModel = directionalShadowShader.GetModelLocation();
    glm::mat4 lTransform = light->CalculateLightTransform();
	directionalShadowShader.SetDirectionalLightTransform(&lTransform);

	directionalShadowShader.Validate();
	RenderScene();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OmniShadowMapPass(PointLight* light) 
{
    glViewport(0, 0, light->GetShadowMap()->GetShadowWidth(), light->GetShadowMap()->GetShadowHeight());

    omniShadowShader.UseShader();
    uniformModel = omniShadowShader.GetModelLocation();
    uniformOmniLightPos = omniShadowShader.GetOmniLightPosLocation();
    uniformFarPlane = omniShadowShader.GetFarPlaneLocation();

    light->GetShadowMap()->Write();

    glClear(GL_DEPTH_BUFFER_BIT);

    glUniform3f(uniformOmniLightPos, light->GetPosition().x, light->GetPosition().y, light->GetPosition().z);
    glUniform1f(uniformFarPlane, light->GetFarPlane());
    omniShadowShader.SetOmniLightMatrices(light->CalculateLightTransform());

    omniShadowShader.Validate();
    RenderScene();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderPass(glm::mat4 projectionMatrix, glm::mat4 viewMatrix)
{
    glViewport(0, 0, 1920, 1080);

    // Clear the window
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    skyBox.DrawSkybox(viewMatrix, projectionMatrix);

    shaderList[0].UseShader();

    uniformModel = shaderList[0].GetModelLocation();
    uniformProjection = shaderList[0].GetProjectionLocation();
    uniformView = shaderList[0].GetViewLocation();
    uniformEyePosition = shaderList[0].GetEyePositionLocation();
    uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
    uniformShininess = shaderList[0].GetShininessLocation();

    glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

    shaderList[0].SetDirectionalLight(&mainLight);
    shaderList[0].SetPointLights(pointLights, pointLightCount, 3, 0);
    shaderList[0].SetSpotLights(spotLights, spotLightCount, 3 + pointLightCount, pointLightCount);
    glm::mat4 lTransform = mainLight.CalculateLightTransform();
    shaderList[0].SetDirectionalLightTransform(&lTransform);

    mainLight.GetShadowMap()->Read(GL_TEXTURE2);


    shaderList[0].SetTexture(1);
    shaderList[0].SetDirectionalShadowMap(2);

    glm::vec3 lowerLight = camera.getCameraPosition();
    lowerLight.y -= 0.3f;
    spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

    shaderList[0].Validate();
    RenderScene();
}


int main()
{
    mainWindow = Window(1920, 1080);
    mainWindow.Initialise();

    CreateObject();
    CreateFighters();
    CreateShaders();

    camera = Camera(glm::vec3(0.f, 0.f, 2.f), glm::vec3(0.f, 1.f, 0.f), -90.f, 0.f, moveSpeed, turnSpeed);

    brickTexture = Texture("Textures/brick.png");
    brickTexture.LoadTextureA();

    dirtTexture = Texture("Textures/dirt.png");
    dirtTexture.LoadTextureA();

    quadTexture = Texture("Textures/plain.png");
    quadTexture.LoadTextureA();

    shinyMaterial = Material(0.3f, 1);
    dullMaterial = Material(0.3f, 1);

    mainLight = DirectionalLight(   2048, 2048,
                                    0.1f, 0.1f, 0.1f,
                                    0.1f, 0.1f,
                                    0.0f, -15.0f, -10.0f);

    pointLights[0] = PointLight(1024, 1024,
                                0.1f, 100.0f,
                                0.0f, 0.0f, 1.0f,
                                0.0f, 0.4f,
                                2.0f, 2.0f, 0.0f,
                                0.3f, 0.01f, 0.01f);
    pointLightCount++;
    
    pointLights[1] = PointLight(1024, 1024,
                                0.1f, 100.0f,
                                0.0f, 1.0f, 0.0f,
                                0.0f, 0.4f,
                                -2.0f, 2.0f, 0.0f,
                                0.3f, 0.01f, 0.01f);
    pointLightCount++;

    spotLights[0] = SpotLight(  1024, 1024,
                                0.1f, 100.0f,
                                1.0f, 1.0f, 1.0f,
                                0.0f, 2.0f,
                                0.0f, 0.0f, 0.0f,
                                0.0f, -1.0f, 0.0f,
                                1.0f, 0.0f, 0.0f,
                                20.0f);
    spotLightCount++;

    spotLights[1] = SpotLight(  1024, 1024,
                                0.1f, 100.0f,
                                1.0f, 1.0f, 1.0f,
                                0.0f, 1.0f,
                                0.0f, -1.5f, 0.0f,
                                -100.0f, -1.0f, 0.0f,
                                1.0f, 0.0f, 0.0f,
                                20.0f);
    spotLightCount++;

    std::vector<std::string> skyBoxFaces;
    skyBoxFaces.push_back("Textures/SkyBox/cupertin-lake_rt.tga");
    skyBoxFaces.push_back("Textures/SkyBox/cupertin-lake_lf.tga");
    skyBoxFaces.push_back("Textures/SkyBox/cupertin-lake_up.tga");
    skyBoxFaces.push_back("Textures/SkyBox/cupertin-lake_dn.tga");
    skyBoxFaces.push_back("Textures/SkyBox/cupertin-lake_bk.tga");
    skyBoxFaces.push_back("Textures/SkyBox/cupertin-lake_ft.tga");

    skyBox = SkyBox(skyBoxFaces);

    GLfloat aspectRation = mainWindow.GetBufferWidth() / mainWindow.GetBufferHeight();
    glm::mat4 projection = glm::perspective(glm::radians(60.0f), aspectRation, 0.1f, 100.f);

    // Loop until winows closed
    while (!mainWindow.getShouldClose())
    {

        GLfloat now = glfwGetTime(); // SDL_GetPerfomaceCounter();
        deltaTime = now - lastTime; // (now - lastTime) * 1000 / SDL_GetPerfaomceFrequency(); multiplay by 1000 because we get miliseconds instead of seconds.
        lastTime = now;

        currentTime += deltaTime;

        // Get + Handle user input events
        glfwPollEvents();

        camera.keyControl(mainWindow.getKeys(), deltaTime);
        camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());     

        camera.RotateAroundPoint(glm::vec3(0.0, 2.0, 0.0), glm::vec3(5.0, 3.0, 5.0), currentTime * 0.1);

        DirectionalShadowMapPass(&mainLight);
        
        for (size_t i = 0; i < pointLightCount; i++) 
        {
            OmniShadowMapPass(&pointLights[i]);
        }

        for (size_t i = 0; i < spotLightCount; i++) 
        {
            OmniShadowMapPass(&spotLights[i]);
        }

        RenderPass(projection, camera.calculateViewMatrix());

        glUseProgram(0);

        mainWindow.swapBuffers();
    }

    return 0;
}