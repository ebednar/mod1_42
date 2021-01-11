#include "rain.h"
#include "Shader.h"
#include "glad.h"
#include <iostream>

Rain::~Rain()
{
	delete[] vertices;
}

void	Rain::init()
{
    drops_number = 20;
    drops = new drop[drops_number];
    int k = 0;
    for (int i = 0; i < drops_number; ++i)
    {
        float rx = rand() % 10;
        float ry = 2.0f + rand() % 3;
        float rz = rand() % 10;
        drops[i].x = rx;
        drops[i].y = ry;
        drops[i].z = rz;
    }
}

void	Rain::vertex_buffer()
{
    vertices = new float[drops_number * 6];

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, drops_number * 6, nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void	Rain::set_shader(const char* vPath, const char* fSPath)
{
    create_shader(&shader_id, vPath, fSPath);
}

void    Rain::rainfall()
{
    for (int i = 0; i < drops_number; ++i)
    {
        drops[i].y -= 0.08;
        if (drops[i].y <= 0)
        {
            float rx = rand() % 10;
            float ry = 2.0f + rand() % 3;
            float rz = rand() % 10;
            drops[i].x = rx;
            drops[i].y = ry;
            drops[i].z = rz;
        }
    }
}