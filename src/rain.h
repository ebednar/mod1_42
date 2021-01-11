#pragma once

struct drop
{
	float	x;
	float	y;
	float	z;
};

class Rain
{
public:
	float*			vertices;
	drop			*drops;
	unsigned int	vbo;
	unsigned int	vao;
	int				shader_id;
	int				drops_number;
public:
	~Rain();
	void	init();
	void	vertex_buffer();
	void	set_shader(const char* vPath, const char* fSPath);
	void	rainfall();
};