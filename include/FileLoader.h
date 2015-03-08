#ifndef FILELOADER_H
#define FILELOADER_H

#include <vector>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <fstream>
#include <sstream>

#include "stb_image.h"
#include "glm/glm.hpp"

using namespace std;
using namespace glm;


unsigned char* ReadTexture(const char *filepath, int &n, int &width, int &height)
{
    unsigned char* data = stbi_load(filepath, &width, &height, &n, 0);
    if(data == 0)
    {
        std::cout << "Error loading the texture '" << filepath << "', couldn't open/read the file." << std::endl;
        return data;
    }
    //size = width * height * n * sizeof(float);
    return data;
}

void GetOBJFormat(const char *filepath, bool &uvs, bool &normals, bool &triangles)
{
    FILE *f;
    f = fopen(filepath, "r");
    if(!f)
    {
        cout << "Error trying to open '" << filepath << "'";
        return;
    }

    fseek(f, -3, SEEK_END);
    int n = 1;

    char c, lastChar;
    while(ftell(f) > 0)
    {
        lastChar = fgetc(f);
        c = fgetc(f);
        if((lastChar == '\n' || lastChar == '\r') && c == 'f')
        {
            int foo;
            while(fgetc(f) == ' '); //Leemos espacios despues de 'f'
            fseek(f, -1, SEEK_CUR);
            foo = fscanf(f, "%d", &foo); //Leemos primer indice
            if(fgetc(f) == ' ') uvs = normals = false; //Solo un indice, sin barras
            else //Hay algo tal que asi:  5/*
            {
                uvs = (fgetc(f) != '/');
                if(!uvs) normals = true; //Es tal que asi 5//8
                if(uvs) //Es algo tal que asi 5/8*
                {
                    fseek(f, -1, SEEK_CUR);
                    foo = fscanf(f, "%d", &foo); //Leemos segundo indice
                    if(fgetc(f) == '/') //Es algo tal que asi 5/8/11
                    {
                        fseek(f, -1, SEEK_CUR);
                        foo = fscanf(f, "%d", &foo); //Leemos ultimo indice
                        normals = true;
                    }
                    else  normals = false;
                }
            }

            //Son triangulos o quads?
            lastChar = c;
            while(!feof(f) && (c = fgetc(f)) != '\n')
            {
                if(lastChar == ' ' && c != ' ') ++n;
                lastChar = c;
            }
            triangles = (n <= 3);
            break;
        }
        fseek(f, -3, SEEK_CUR);
    }
    fclose(f);
}

bool ReadOBJ(const char *filepath, vector<vec3> &vertexPos, vector<vec2> &vertexUvs, vector<vec3> &vertexNormals,
             bool &triangles)
{
    vector<vec3> disorderedVertexPos, disorderedVertexNormals;
    vector<vec2> disorderedVertexUvs;
    vector<unsigned int> vertexPosIndexes, vertexUvsIndexes, vertexNormIndexes;
    bool hasUvs, hasNormals;

    GetOBJFormat(filepath, hasUvs, hasNormals, triangles);

    ifstream f(filepath, ios::in);
    if(!f.is_open()) cout << "Error opening the mesh file" << endl;
    string line;
    string errormsg = "Error reading the mesh file";
    while(getline(f, line))
    {
        stringstream ss(line);
        string lineHeader;
        if(!(ss >> lineHeader)) continue;
        if(lineHeader == "v")
        {
            vec3 pos;
            ss >> pos.x, errormsg;
            ss >> pos.y, errormsg;
            ss >> pos.z, errormsg;
            disorderedVertexPos.push_back(pos);
        }
        else if(hasUvs && lineHeader == "vt") //Cargamos uvs
        {
            vec2 uv;
            ss >> uv.x, errormsg;
            ss >> uv.y, errormsg;
            disorderedVertexUvs.push_back(uv);
        }
        else if(hasNormals && lineHeader == "vn") //Cargamos normals
        {
            vec3 normal;
            ss >> normal.x, errormsg;
            ss >> normal.y, errormsg;
            ss >> normal.z, errormsg;
            disorderedVertexNormals.push_back(normal);
        }
        else if(lineHeader == "f")
        {
            int n = triangles ? 3 : 4;
            unsigned int index;
            char c;

            for (int i = 0; i < n; ++i)
            {
                ss >> index, errormsg;
                vertexPosIndexes.push_back(index);

                if(hasUvs)
                {
                    ss >> c, errormsg;  //Read the '/'
                    ss >> index, errormsg;
                    vertexUvsIndexes.push_back(index);

                    if (hasNormals)
                    {
                        ss >> c, errormsg;
                        ss >> index, errormsg;
                        vertexNormIndexes.push_back(index);
                    }
                }
                else
                {
                    if (hasNormals)
                    {
                        ss >> c, errormsg;
                        ss >> index, errormsg;
                        vertexNormIndexes.push_back(index);
                    }
                }
            }
        }
    }

    for(int i = 0; i < vertexPosIndexes.size(); ++i)
    {
        vertexPos.push_back(disorderedVertexPos[vertexPosIndexes[i]-1]);
    }

    vec2 defaultUvs(0.5f, 0.5f);
    for(int i = 0; i < vertexUvsIndexes.size(); ++i)
    {
        if(hasUvs) vertexUvs.push_back(disorderedVertexUvs[vertexUvsIndexes[i]-1]);
        else vertexUvs.push_back(defaultUvs);
    }

    vec3 defaultNormals(0.0f, 0.0f, 1.0f);
    for(int i = 0; i < vertexNormIndexes.size(); ++i)
    {
        if(hasNormals) vertexNormals.push_back(disorderedVertexNormals[vertexNormIndexes[i]-1]);
        else vertexNormals.push_back(defaultNormals);
    }

    return true;
}

#endif // FILELOADER_H
