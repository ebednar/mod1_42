#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>

struct point
{
	float	x;
	float	y;
	float	z;
	point()
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	};
};

class Landscape
{
public:
	std::vector<glm::vec3>	displacements;
	point*					map;
	int						map_size;
	unsigned int			vbo;
	unsigned int			vbo_norm;
	unsigned int			vao;
	int						shader_id;
public:
	Landscape();
	~Landscape();
	int		load_map(std::string path);
	void	build_heights();
	void	vertex_buffer();
	void	set_shader(const char* vPath, const char* fSPath);
private:
};