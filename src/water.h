#pragma once
#include "landscape.h"

struct water_point
{
	float	x;
	float	z;
	float	level;
	float   sum_level;
	float   rend_level;
	water_point()
	{
		x = 0.0f;
		level = 0.0f;
		sum_level = 0.0f;
		z = 0.0f;
	};
};

class Water
{
public:
	water_point*			water_map;
	Landscape*				land;
	float*					vertices;
	bool*					flow_map;
	unsigned int			vbo;
	unsigned int			vao;
	int						shader_id;
	int						map_size;
	float					step = 0.002f;
	bool                    wave_start = false;
public:
	~Water();
	void	init(Landscape* land);
	void	vertex_buffer();
	void	set_shader(const char* vPath, const char* fSPath);
	void	raise();
	void	wave();
	void	rain();
	void	flow();
private:
    void recalc(int i);
};