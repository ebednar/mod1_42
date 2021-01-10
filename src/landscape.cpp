#include "landscape.h"
#include "Shader.h"
#include "glad.h"
#include <fstream>
#include <iostream>

Landscape::Landscape()
{
	map_size = 300;
	map = new point[map_size * map_size];
	for (int z = 0; z < map_size; ++z)
	{
		for (int x = 0; x < map_size; ++x)
		{
			map[z * map_size + x].x = x / 30.0f;
			map[z * map_size + x].z = z / 30.0f;
		}
	}
}

Landscape::~Landscape()
{
	delete[] map;
}

void	Landscape::load_map(std::string path)
{
	std::ifstream	file;

	file.open(path);
	if (not file.is_open())
	{
		std::cout << "map file not found" << std::endl;
		return;
	}
	std::string line;
	while (std::getline(file, line))
	{
		glm::vec3 vert;
		if (sscanf(line.c_str(), "%f %f %f", &vert.x, &vert.y, &vert.z) != 3)
		{
			std::cout << "map file corrupted" << std::endl;
			return;
		}
		displacements.push_back(vert);
	}
}

void	Landscape::build_heights()
{
	int length = displacements.size();

	for (int z = 2; z < map_size - 1; ++z)
	{
		for (int x = 2; x < map_size - 1; ++x)
		{
			for (int i = 0; i < length; ++i)
			{
				float dist = (map[z * map_size + x].x - displacements[i].x) * (map[z * map_size + x].x - displacements[i].x) +
					(map[z * map_size + x].z - displacements[i].y) * (map[z * map_size + x].z - displacements[i].y);
//				float height = displacements[i].z / (dist + 1);
				float height = displacements[i].z;
				if (map[z * map_size + x].y < height)
					map[z * map_size + x].y = height;
			}
		}
	}

	// print map
//	for (int y = 0; y < map_size; ++y)
//	{
//		for (int x = 0; x < map_size; ++x)
//		{
//			std::cout << map[y * map_size + x].x << " ";
//		}
//		std::cout << "\n";
//	}
}

void	Landscape::vertex_buffer()
{
	float *vertices = new float[(map_size - 1) * (map_size - 1) * 3 * 2 * 3];
	int k = 0;
	for (int y = 0; y < map_size - 1; ++y)
	{
		for (int x = 0; x < map_size - 1; ++x)
		{
			
			vertices[k + 0] = map[y * map_size + x].x;
			vertices[k + 1] = map[y * map_size + x].y;
			vertices[k + 2] = map[y * map_size + x].z;
			vertices[k + 3] = map[y * map_size + x + 1].x;
			vertices[k + 4] = map[y * map_size + x + 1].y;
			vertices[k + 5] = map[y * map_size + x + 1].z;
			vertices[k + 6] = map[(y + 1) * map_size + x + 1].x;
			vertices[k + 7] = map[(y + 1) * map_size + x + 1].y;
			vertices[k + 8] = map[(y + 1) * map_size + x + 1].z;
			vertices[k + 9] = map[y * map_size + x].x;
			vertices[k + 10] = map[y * map_size + x].y;
			vertices[k + 11] = map[y * map_size + x].z;
			vertices[k + 12] = map[(y + 1) * map_size + x].x;
			vertices[k + 13] = map[(y + 1) * map_size + x].y;
			vertices[k + 14] = map[(y + 1) * map_size + x].z;
			vertices[k + 15] = map[(y + 1) * map_size + x + 1].x;
			vertices[k + 16] = map[(y + 1) * map_size + x + 1].y;
			vertices[k + 17] = map[(y + 1) * map_size + x + 1].z;
			k += 18;
		}
	}

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, (map_size - 1) * (map_size - 1) * 3 * 2 * 3 * sizeof(float), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	delete[] vertices;
}

void Landscape::set_shader(const char* vPath, const char* fSPath)
{
	create_shader(&shader_id, vPath, fSPath);
}
