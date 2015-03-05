#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#define GL_GLEXT_PROTOTYPES

#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include "../include/Shader.h"

class ShaderProgram
{
private:

    GLuint program;

public:

    ShaderProgram();
    ~ShaderProgram();

    void AttachShader(const Shader &shader) const;
    void Link() const;
    void Use() const;
    void UnUse() const;
    GLuint GetId() const;
};

#endif // SHADERPROGRAM_H
