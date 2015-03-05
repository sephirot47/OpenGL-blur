#define GL_GLEXT_PROTOTYPES

#include <iostream>
#include <SDL2/SDL.h>
#include <cstring>
#include <GL/gl.h>
#include <GL/glext.h>

#include "../include/glm/glm.hpp"
#include "../include/Shader.h"
#include "../include/ShaderProgram.h"

using namespace std;
using namespace glm;

const float triMesh[9] = {-1.0f, -1.0f, 0.0f,
                           0.0f,  1.0f, 0.0f,
                           1.0f, -1.0f, 0.0f};

const float screenMesh[18] = {-1.0f, -1.0f, 0.0f,
                               1.0f, -1.0f, 0.0f,
                               1.0f,  1.0f, 0.0f,
                               1.0f,  1.0f, 0.0f,
                              -1.0f,  1.0f, 0.0f,
                              -1.0f, -1.0f, 0.0f};



const int width = 640, height = 480;

GLuint vboId, vaoId, finalVboId, finalVaoId, frameBuffId, texId, depthBuffId,
       shaderProgramId, vShaderId, fShaderId;

Shader *vshader, *fshader, *finalVShader, *horShader, *verShader;
ShaderProgram *program, *horProgram, *verProgram;

unsigned char *image;

void Init()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);

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
    glBufferData(GL_ARRAY_BUFFER, sizeof(triMesh), triMesh, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //Creamos vao
    glEnableClientState(GL_VERTEX_ARRAY);
    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(glGetAttribLocation(program->GetId(), "position"));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glDisableClientState(GL_VERTEX_ARRAY);

    //Creamos framebuffer
    glGenFramebuffers(1, &frameBuffId);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffId);

    glGenTextures(1, &texId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

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
    glClear(GL_COLOR_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffId);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(vaoId);
    program->Use();

    mat4 transform(1.0f);
    vec3 axis = vec3(0.0, 1.0, 0.0), translate(0.0, 0.0, -3.0), scale(0.5);
    mat4 T = glm::translate(transform, translate);
    mat4 R = glm::rotate_slow(transform, rot, axis);
    mat4 S = glm::scale(transform, scale);
    transform = T * R * S;

    mat4 projection(1.0f);
    projection = perspective(45.0f * 3.1415f/180.0f, 1.0f, 0.1f, 100.0f);

    glUniform1f(glGetUniformLocation(program->GetId(), "time"), appTime);
    glUniformMatrix4fv(glGetUniformLocation(program->GetId(), "transform"), 1, GL_FALSE, value_ptr(transform));
    glUniformMatrix4fv(glGetUniformLocation(program->GetId(), "projection"), 1, GL_FALSE, value_ptr(projection));

    GLenum buffersEnum = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, &buffersEnum);

    glViewport(0, 0, width, height); //draws?
    glDrawArrays(GL_TRIANGLES, 0, 3);

    program->UnUse();
    glBindVertexArray(0);

    /// RENDER FINAL IMAGE /////////////////
    glBindVertexArray(finalVaoId);
    /// BLUR VERTICAL
    verProgram->Use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texId);

    glUniform1i(glGetUniformLocation(verProgram->GetId(), "renderedSceneTex"), 0);
    glUniform1f(glGetUniformLocation(verProgram->GetId(), "width"), width);
    glUniform1f(glGetUniformLocation(verProgram->GetId(), "height"), height);
    glUniform1f(glGetUniformLocation(verProgram->GetId(), "time"), appTime);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindTexture(GL_TEXTURE_2D, 0);
    verProgram->UnUse();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    ///BLUR HORIZONTAL
    horProgram->Use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texId);

    glUniform1i(glGetUniformLocation(horProgram->GetId(), "renderedSceneTex"), 0);
    glUniform1f(glGetUniformLocation(horProgram->GetId(), "width"), width);
    glUniform1f(glGetUniformLocation(horProgram->GetId(), "height"), height);
    glUniform1f(glGetUniformLocation(horProgram->GetId(), "time"), appTime);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindTexture(GL_TEXTURE_2D, 0);
    horProgram->UnUse();
    glBindVertexArray(0);
    //////////////////////////////////

    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    appTime += 0.03;
    rot += 0.09;
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
