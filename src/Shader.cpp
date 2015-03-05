#include "../include/Shader.h"

Shader::Shader()
{
    shader = 0;
}

Shader::~Shader()
{
    if(shader == 0) return;
    glDeleteShader(shader);
}

bool Shader::Create(const std::string filepath, const ShaderType type)
{
    shader = glCreateShader(type);

    FILE *f = fopen(filepath.c_str(), "r");
    if(f == NULL) { std::cout << "Error opening the shader file " << filepath << std::endl; return false; }

    fseek(f, 0, SEEK_END);
    int srcCodeLength = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *srcCodeC = new char[srcCodeLength+1];
    int readres = fread(srcCodeC, sizeof(char), srcCodeLength, f);
    srcCodeC[srcCodeLength] = '\0';

    shader = glCreateShader(type);
    glShaderSource(shader, 1, &srcCodeC, &srcCodeLength);
    delete[] srcCodeC;

    glCompileShader(shader);

    int compilationResult;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compilationResult);

    if(compilationResult == GL_FALSE)
    {
        char shaderErrorLog[1000];
        glGetShaderInfoLog(shader, 1000, 0, shaderErrorLog);
        std::cout << "'" << filepath << "'' contains errors! It can't be compiled:" << std::endl << shaderErrorLog << std::endl;
        glDeleteShader(shader);
        return false;
    }
    return true;
}
