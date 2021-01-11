#include "sandbox.h"
#include <vector>

Sandbox::~Sandbox()
{
	//system("killall afplay");
}

void Sandbox::place_ent(Engine* eng)
{
	if (eng->landscape.load_map("map.mod1"))
		exit(1);
	eng->landscape.build_heights();
	eng->landscape.vertex_buffer();
	eng->landscape.set_shader("res/shaders/land_vertex.glsl", "res/shaders/land_fragment.glsl");
	eng->water.init(&eng->landscape);
	eng->water.vertex_buffer();
	eng->water.set_shader("res/shaders/water_vertex.glsl", "res/shaders/water_fragment.glsl");

	Model* lightMod = new Model();
	lightMod->load_obj("res/models/test.obj", true);
	lightMod->set_shader("res/shaders/light_vertex.glsl", "res/shaders/light_fragment.glsl");
	Entity* light = new Entity();
	light->set_model(lightMod);
	eng->add_entity(light);
	light->set_color(1.0f, 221.0f / 256.0f, 81.0f / 256.0f);
	glm::vec3 lightPositions(0.0f, 2.0f, 0.0f);
	light->move_to(lightPositions.x, lightPositions.y, lightPositions.z);
	light->scale(0.2f, 0.2f, 0.2f);
	eng->add_light_source(lightPositions, glm::vec3(1.0f, 221.0f / 256.0f, 81.0f / 256.0f));

	eng->rain.init();
	eng->rain.vertex_buffer();
	eng->rain.set_shader("res/shaders/water_vertex.glsl", "res/shaders/water_fragment.glsl");

	//system("afplay res/music/Day.mp3 -v 0.9 &");
}
