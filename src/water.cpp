#include "water.h"
#include "Shader.h"
#include "glad.h"
#include <iostream>

Water::~Water()
{
	delete[] water_map;
	delete[] vertices;
}

void	Water::init(Landscape *land)
{
	this->land = land;
	map_size = land->map_size;
	water_map = new water_point[map_size * map_size];
    flow_map = new bool[map_size * map_size];
	for (int i = 0; i < map_size * map_size; ++i)
	{
		water_map[i].x = land->map[i].x;
		water_map[i].z = land->map[i].z;
        water_map[i].level = 0.0f;
        water_map[i].sum_level =  water_map[i].level +  land->map[i].y;
        water_map[i].rend_level =  0.0f;
        flow_map[i] = false;
	}
}

void	Water::vertex_buffer()
{
	vertices = new float[((map_size - 1) * (map_size - 1) * 3 * 2 * 3 + (map_size - 1) * 18 * 4)* sizeof(float)];

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, ((map_size - 1) * (map_size - 1) * 3 * 2 * 3 + (map_size - 1) * 18 * 4)* sizeof(float), nullptr, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void	Water::set_shader(const char* vPath, const char* fSPath)
{
	create_shader(&shader_id, vPath, fSPath);
}

void	Water::raise()
{
	for (int y = 0; y < map_size; ++y)
	{
		for (int x = 0; x < map_size; ++x)
		{
			if (x == 0 ||  y == 0 || x == map_size - 2 || y == map_size - 2)
				water_map[y * map_size + x].level += step;
		}
	}
	//std::cout << "water level" << water_map[0].level << std::endl;
}

void    Water::wave()
{
    if (!wave_start)
    {
        for (int x = 0; x < map_size * 10; ++x)
        {
            water_map[x].level = 0.8f;
            water_map[map_size + x].level = 0.78f;
            water_map[map_size * 2 + x].level = 0.76f;
        }
        wave_start = true;
    }
}

void    Water::rain()
{
    for (int j = 0; j < 6; ++j)
    {
        int i = rand() % ((map_size - 1) * (map_size - 1));
        float drop = 1 * step;
        water_map[i].level += drop;
        water_map[i + 1].level += drop;
        water_map[map_size + i].level += drop;
        water_map[map_size + i + 1].level += drop;
    }
}

void    Water::recalc(int i)
{
    water_map[i].sum_level =  water_map[i].level +  land->map[i].y;
    if (water_map[i].level > 0.0f)
        water_map[i].rend_level =  water_map[i].level +  land->map[i].y;
    else
        water_map[i].rend_level = land->map[i].y - step;
}

void	Water::flow()
{
    for (int i = 0; i < map_size * map_size; ++i)
    {
        flow_map[i] = false;
    }
//    for (int y = 3; y < map_size - 2; ++y)
//	{
//		for (int x = 3; x < map_size - 2; ++x)
//		{
//			std::cout << water_map[y * map_size + x].level << " ";
//		}
//		std::cout << "\n";
//	}
	for (int y = 0; y < map_size; ++y)
	{
		for (int x = 0; x < map_size; ++x)
		{
            recalc(y * map_size + x);

            if (water_map[y * map_size + x].level < 2 * step)
                continue ;

            if (x < map_size - 1  && water_map[y * map_size + x].level > 0.0f
                && water_map[y * map_size + x].sum_level - step  > water_map[y * map_size + x + 1].sum_level)
            {
//                if (water_map[y * map_size + x + 1].level == 0.0f)
//                    water_map[y * map_size + x + 1].level = land->map[y * map_size + x + 1].y;
                float wave_step = (water_map[y * map_size + x].sum_level - water_map[y * map_size + x + 1].sum_level) / 10.0f;
//                std::cout << wave_step << std::endl;

                water_map[y * map_size + x].level -= 1 * wave_step;
                water_map[y * map_size + x + 1].level += 1 * wave_step;
                recalc(y * map_size + x + 1);
                recalc(y * map_size + x);
            }
            if (x > 0 && water_map[y * map_size + x].level > 0.0f
                && water_map[y * map_size + x].sum_level - step > water_map[y * map_size + x - 1].sum_level)
            {
//                if (water_map[y * map_size + x - 1].level == 0.0f)
//                    water_map[y * map_size + x - 1].level = land->map[y * map_size + x - 1].y;
                float wave_step = (water_map[y * map_size + x].sum_level - water_map[y * map_size + x - 1].sum_level) / 10.0f;
//                std::cout << wave_step << std::endl;

                water_map[y * map_size + x].level -= 1 * wave_step;
                water_map[y * map_size + x - 1].level += 1 * wave_step;
                recalc(y * map_size + x - 1);
                recalc(y * map_size + x);
            }
            if (y < map_size - 1 && water_map[y * map_size + x].level > 0.0f
                && water_map[y * map_size + x].sum_level - step > water_map[(y + 1) * map_size + x].sum_level)
            {
//                if (water_map[(y + 1) * map_size + x].level == 0.0f)
//                    water_map[(y + 1) * map_size + x].level = land->map[(y + 1) * map_size + x].y;
                float wave_step = (water_map[y * map_size + x].sum_level - water_map[(y + 1) * map_size + x].sum_level) / 10.0f;
//                std::cout << wave_step << std::endl;

                water_map[y * map_size + x].level -= 1 * wave_step;
                water_map[(y + 1) * map_size + x].level += 1 * wave_step;
                recalc((y + 1) * map_size + x);
                recalc(y * map_size + x);
            }
            if (y > 0 && water_map[y * map_size + x].level > 0.0f
                && water_map[y * map_size + x].sum_level - step > water_map[(y - 1) * map_size + x].sum_level)
            {
//                if (water_map[(y - 1) * map_size + x].level == 0.0f)
//                    water_map[(y - 1) * map_size + x].level = land->map[(y - 1) * map_size + x].y;
                float wave_step = (water_map[y * map_size + x].sum_level - water_map[(y - 1) * map_size + x].sum_level) / 10.0f;
//                std::cout << wave_step << std::endl;

                water_map[y * map_size + x].level -= 1 * wave_step;
                water_map[(y - 1) * map_size + x].level += 1 * wave_step;
                recalc((y - 1) * map_size + x);
                recalc(y * map_size + x);
            }
		}
	}
}
