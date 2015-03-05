#ifndef SHADER_H
#define SHADER_H

#define GL_GLEXT_PROTOTYPES

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>


enum ShaderType
{
    VertexShader = GL_VERTEX_SHADER,
    FragmentShader = GL_FRAGMENT_SHADER
};

class Shader
{
private:

    Shader(const Shader& s) {};
    Shader& operator=(const Shader &s) { return *this; }
    GLuint shader;

public:

    Shader();
    ~Shader();

    bool Create(const std::string filepath, const ShaderType type);
    GLuint GetId() const { return shader; }
};

#endif // SHADER_H
