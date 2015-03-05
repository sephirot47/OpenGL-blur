#include "../include/ShaderProgram.h"

ShaderProgram::ShaderProgram()
{
    program = glCreateProgram();
}

ShaderProgram::~ShaderProgram()
{
    if(program == 0) return;
    glDeleteProgram(program);
}

void ShaderProgram::AttachShader(const Shader &shader) const
{
    glAttachShader(program, shader.GetId());
}

void ShaderProgram::Link() const
{
    glLinkProgram(program);

    // Check the program
    int infoLogLength = 0, result = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &result);
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
    if(infoLogLength < 1) infoLogLength = 1;
    std::vector<char> errormsg(infoLogLength);
    glGetProgramInfoLog(program, infoLogLength, NULL, &errormsg[0]);
    fprintf(stdout, "%s\n", &errormsg[0]);
}

void ShaderProgram::Use() const
{
    glUseProgram(program);
}

void ShaderProgram::UnUse() const
{
    glUseProgram(0);
}

GLuint ShaderProgram::GetId() const
{
    return program;
}

