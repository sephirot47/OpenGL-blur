#define GL_GLEXT_PROTOTYPES

#include <iostream>
#include <SDL2/SDL.h>
#include <cstring>
#include <GL/gl.h>
#include <GL/glext.h>

#include "../include/glm/glm.hpp"
#include "../include/FileLoader.h"
#include "../include/Shader.h"
#include "../include/ShaderProgram.h"

using namespace std;
using namespace glm;

const float screenMesh[18] = {-1.0f, -1.0f, 0.0f,
                               1.0f, -1.0f, 0.0f,
                               1.0f,  1.0f, 0.0f,
                               1.0f,  1.0f, 0.0f,
                              -1.0f,  1.0f, 0.0f,
                              -1.0f, -1.0f, 0.0f};



const int width = 840, height = 680;

GLuint vboId, vaoId, finalVboId, finalVaoId, frameBuffId, texId, depthBuffId,
       shaderProgramId, vShaderId, fShaderId, realTexId;

Shader *vshader, *fshader, *finalVShader, *horShader, *verShader;
ShaderProgram *program, *horProgram, *verProgram;

vector<vec3> vertexPos, vertexNormals;
vector<vec2> vertexUv;
bool triangles;

void Init()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glEnable(GL_DEPTH_TEST);

    ReadOBJ("luigi.obj", vertexPos, vertexUv, vertexNormals, triangles);

    //Creamos textura gordo
    unsigned char *texData;
    glGenTextures(1, &realTexId);
    glBindTexture(GL_TEXTURE_2D, realTexId);
    int tWidth, tHeight, n;
    texData = ReadTexture("luigiD.jpg", n, tWidth, tHeight);
    glTexImage2D(GL_TEXTURE_2D, 0, n == 3 ? GL_RGB : GL_RGBA, tWidth, tHeight, 0,
                 n == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, texData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    //Creamos shaders
    vshader = new Shader(); if( !vshader->Create("vshader", VertexShader) ) std::cout << "FUUUU" << std::endl;
    fshader = new Shader(); if( !fshader->Create("fshader", FragmentShader) ) std::cout << "FUUUU" << std::endl;
    program = new ShaderProgram();
    program->AttachShader(*vshader);
    program->AttachShader(*fshader);
    program->Link();

    //Creamos vbo
    glGenBuffers(1, &vboId);
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    int size = vertexPos.size() * sizeof(vec3) + vertexUv.size() * sizeof(vec2);
    char *joinedData = new char[size];
    memcpy(joinedData, &vertexPos[0], vertexPos.size() * sizeof(vec3));
    memcpy((char*)(joinedData + vertexPos.size() * sizeof(vec3)), &vertexUv[0], vertexUv.size() * sizeof(vec2));
    glBufferData(GL_ARRAY_BUFFER, size, joinedData, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //Creamos vao
    glEnableClientState(GL_VERTEX_ARRAY);
    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glVertexAttribPointer(glGetAttribLocation(program->GetId(), "position"), 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(glGetAttribLocation(program->GetId(), "position"));
    glVertexAttribPointer(glGetAttribLocation(program->GetId(), "inuv"), 2, GL_FLOAT, GL_FALSE, 0, (void*)(vertexPos.size() * sizeof(vec3)));
    glEnableVertexAttribArray(glGetAttribLocation(program->GetId(), "inuv"));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glDisableClientState(GL_VERTEX_ARRAY);

    //Creamos framebuffer
    glGenFramebuffers(1, &frameBuffId);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffId);

    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenRenderbuffers(1, &depthBuffId);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffId);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffId);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texId, 0);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //Creamos shaders finales
    finalVShader = new Shader(); if( !finalVShader->Create("finalVShader", VertexShader) ) std::cout << "FUUUU" << std::endl;
    horShader = new Shader(); if( !horShader->Create("blurHorizontalFShader", FragmentShader) ) std::cout << "FUUUU" << std::endl;
    horProgram = new ShaderProgram();
    horProgram->AttachShader(*finalVShader);
    horProgram->AttachShader(*horShader);
    horProgram->Link();

    verShader = new Shader(); if( !verShader->Create("blurVerticalFShader", FragmentShader) ) std::cout << "FUUUU" << std::endl;
    verProgram = new ShaderProgram();
    verProgram->AttachShader(*finalVShader);
    verProgram->AttachShader(*verShader);
    verProgram->Link();

    //Creamos vbo final
    glGenBuffers(1, &finalVboId);
    glBindBuffer(GL_ARRAY_BUFFER, finalVboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screenMesh), screenMesh, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //Creamos vao final
    glEnableClientState(GL_VERTEX_ARRAY);
    glGenVertexArrays(1, &finalVaoId);
    glBindVertexArray(finalVaoId);
    glBindBuffer(GL_ARRAY_BUFFER, finalVboId);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(glGetAttribLocation(horProgram->GetId(), "position"));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glDisableClientState(GL_VERTEX_ARRAY);

}

float appTime = 0.0, rot = 0.0;

void RenderScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffId);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindVertexArray(vaoId);
    program->Use();
    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, realTexId);

    mat4 transform(1.0f);
    vec3 axis = vec3(0.0, 1.0, 0.0), translate(0.0, -1.0, -3.0), scale(0.01);
    mat4 T = glm::translate(transform, translate);
    mat4 R = glm::rotate_slow(transform, rot, axis);
    mat4 S = glm::scale(transform, scale);
    transform = T * R * S;

    mat4 projection(1.0f);
    projection = perspective(45.0f * 3.1415f/180.0f, 1.0f, 0.1f, 100.0f);

    glUniform1f(glGetUniformLocation(program->GetId(), "time"), appTime);
    glUniform1i(glGetUniformLocation(program->GetId(), "tex"), 0);
    glUniformMatrix4fv(glGetUniformLocation(program->GetId(), "transform"), 1, GL_FALSE, value_ptr(transform));
    glUniformMatrix4fv(glGetUniformLocation(program->GetId(), "projection"), 1, GL_FALSE, value_ptr(projection));

    glDrawArrays(triangles ? GL_TRIANGLES : GL_QUADS, 0, vertexPos.size());

    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, 0);
    program->UnUse();
    glBindVertexArray(0);

    /// BLUR VERTICAL
    glBindVertexArray(finalVaoId);
    verProgram->Use();
    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, texId);

    glUniform1i(glGetUniformLocation(verProgram->GetId(), "renderedSceneTex"), 0);
    glUniform1f(glGetUniformLocation(verProgram->GetId(), "width"), width);
    glUniform1f(glGetUniformLocation(verProgram->GetId(), "height"), height);
    glUniform1f(glGetUniformLocation(verProgram->GetId(), "time"), appTime);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, 0);
    verProgram->UnUse();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    ///BLUR HORIZONTAL E IMAGEN FINAL
    horProgram->Use();
    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, texId);

    glUniform1i(glGetUniformLocation(horProgram->GetId(), "renderedSceneTex"), 0);
    glUniform1f(glGetUniformLocation(horProgram->GetId(), "width"), width);
    glUniform1f(glGetUniformLocation(horProgram->GetId(), "height"), height);
    glUniform1f(glGetUniformLocation(horProgram->GetId(), "time"), appTime);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, 0);
    horProgram->UnUse();
    glBindVertexArray(0);
    //////////////////////////////////

    appTime += 0.03;
    rot += 0.05;
}


int main(int argc, char* argv[])
{
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* win;
    win = SDL_CreateWindow("Pruebas OpenGL", 100, 100, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    SDL_GLContext context;
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    context = SDL_GL_CreateContext(win);

    Init();
    bool running = true;
    while(running)
    {
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
                running = false;
        }

        RenderScene();
        SDL_GL_SwapWindow(win);
        SDL_Delay(25);
    }

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return 0;
}
