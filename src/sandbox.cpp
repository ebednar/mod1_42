#include "sandbox.h"

void Sandbox::place_ent(Engine* eng)
{
    eng->landscape.load_map("map.mod1");
    eng->landscape.build_heights();
    eng->landscape.vertex_buffer();
    eng->landscape.set_shader("src/res/shaders/land_vertex.glsl", "src/res/shaders/land_fragment.glsl");
    eng->water.init(&eng->landscape);
    eng->water.vertex_buffer();
    eng->water.set_shader("src/res/shaders/water_vertex.glsl", "src/res/shaders/water_fragment.glsl");
}