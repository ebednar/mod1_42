#include "landscape.h"
#include "Shader.h"
#include "glad.h"
#include <fstream>
#include <iostream>

Landscape::Landscape()
{
	map_size = 200;
	map = new point[map_size * map_size];
	for (int z = 0; z < map_size; ++z)
	{
		for (int x = 0; x < map_size; ++x)
		{
			map[z * map_size + x].x = x / 20.0f;
			map[z * map_size + x].z = z / 20.0f;
		}
	}
}

Landscape::~Landscape()
{
	delete[] map;
}

int	Landscape::load_map(std::string path)
{
	std::ifstream	file;

	file.open(path);
	if (not file.is_open())
	{
		std::cout << "map file not found" << std::endl;
		return 1;
	}
	std::string line;
	while (std::getline(file, line))
	{
		std::vector<std::string> sub_lines;
		int count = 0;
		for (int i = 0; i < line.size(); ++i)
		{
			if (line[i] == '(')
			{
				count++;
				for (int j = i; j < line.size(); ++j)
				{
					if (line[j] == ')')
					{
						sub_lines.push_back(line.substr(i, j - i + 1));
						break;
					}
				}
			}
		}
		//std::cout << "line - " <<  count << " " << sub_lines[0] << std::endl;
		for (int i = 0; i < count; ++i)
		{
			glm::vec3 vert;
			if (sscanf(sub_lines[i].c_str(), "(%f,%f,%f)", &vert.x, &vert.y, &vert.z) != 3)
			{
				std::cout << "map file corrupted" << std::endl;
				return 1;
			}
			vert.x /= 2000;
			vert.y /= 2000;
			vert.z /= 5000;
			displacements.push_back(vert);
		}
	}
	return 0;
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
				float height = displacements[i].z / (dist + 1);
				//float height = displacements[i].z;
				if (map[z * map_size + x].y < height)
					map[z * map_size + x].y = height;
			}
		}
	}

	// print map
	//for (int y = 0; y < map_size; ++y)
	//{
	//	for (int x = 0; x < map_size; ++x)
	//	{
	//		std::cout << map[y * map_size + x].y << " ";
	//	}
	//	std::cout << "\n";
	//}
}

void	Landscape::vertex_buffer()
{
	float* vertices = new float[(map_size - 1) * (map_size - 1) * 3 * 2 * 3];
	float* normales = new float[(map_size - 1) * (map_size - 1) * 3 * 2 * 3];
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

			glm::vec3 tmp_n1 = glm::normalize(glm::vec3(map[y * map_size + x + 1].x - map[y * map_size + x].x, map[y * map_size + x + 1].y - map[y * map_size + x].y, map[y * map_size + x + 1].z - map[y * map_size + x].z));
			glm::vec3 tmp_n2 = glm::normalize(glm::vec3(map[(y + 1) * map_size + x + 1].x - map[y * map_size + x].x, map[(y + 1) * map_size + x + 1].y - map[y * map_size + x].y, map[(y + 1) * map_size + x + 1].z - map[y * map_size + x].z));
			glm::vec3 cross = glm::normalize(glm::cross(tmp_n2, tmp_n1));
			normales[k + 0] = cross.x;
			normales[k + 1] = cross.y;
			normales[k + 2] = cross.z;
			normales[k + 3] = cross.x;
			normales[k + 4] = cross.y;
			normales[k + 5] = cross.z;
			normales[k + 6] = cross.x;
			normales[k + 7] = cross.y;
			normales[k + 8] = cross.z;

			tmp_n1 = glm::normalize(glm::vec3(map[(y + 1) * map_size + x].x - map[y * map_size + x].x, map[(y + 1) * map_size + x].y - map[y * map_size + x].y, map[(y + 1) * map_size + x].z - map[y * map_size + x].z));
			tmp_n2 = glm::normalize(glm::vec3(map[(y + 1) * map_size + x + 1].x - map[y * map_size + x].x, map[(y + 1) * map_size + x + 1].y - map[y * map_size + x].y, map[(y + 1) * map_size + x + 1].z - map[y * map_size + x].z));
			cross = glm::normalize(glm::cross(tmp_n1, tmp_n2));
			normales[k + 9] = cross.x;
			normales[k + 10] = cross.y;
			normales[k + 11] = cross.z;
			normales[k + 12] = cross.x;
			normales[k + 13] = cross.y;
			normales[k + 14] = cross.z;
			normales[k + 15] = cross.x;
			normales[k + 16] = cross.y;
			normales[k + 17] = cross.z;

			//std::cout << cross.x << " " << cross.y << " " << cross.z << " " << std::endl;

			k += 18;
		}
	}

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &vbo_norm);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, (map_size - 1) * (map_size - 1) * 3 * 2 * 3 * sizeof(float), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_norm);
	glBufferData(GL_ARRAY_BUFFER, (map_size - 1) * (map_size - 1) * 3 * 2 * 3 * sizeof(float), normales, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	delete[] vertices;
	delete[] normales;
}

void Landscape::set_shader(const char* vPath, const char* fSPath)
{
	create_shader(&shader_id, vPath, fSPath);
}