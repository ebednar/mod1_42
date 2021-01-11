#include "render.h"
#include "glad.h"
#include <iostream>
#include "glm/glm.hpp"
#include "engine.h"

void Render::init()
{
    projection = perspective(60.0f * M_PI / 180.0f, 640.0f / 480.0f, 0.1f, 100.0f);
    animation_key = "idle";
}

void Render::draw_scene(Animator* animator, Scene* scene, Camera* cam)
{
    //std::cout << "draw scene" << std::endl;
    int length = scene->ents.size();

    for (int i = 0; i < length; ++i) {
        Entity* ent = scene->ents[i];
        Model* mod = ent->mod;
        glUseProgram(mod->shader_id);
        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, mod->texture);
        glBindVertexArray(mod->vao);


        mat4 model = mat4(1.0f);
        model = translate(model, ent->position);
        model = rotate(model, ent->angle);
        model = scale(model, ent->e_scale);

        unsigned int model_loc = glGetUniformLocation(mod->shader_id, "u_M");
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, model.mat);
        unsigned int view_loc = glGetUniformLocation(mod->shader_id, "u_V");
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, cam->view.mat);
        unsigned int proj_loc = glGetUniformLocation(mod->shader_id, "u_P");
        glUniformMatrix4fv(proj_loc, 1, GL_FALSE, projection.mat);

        glUniform1i(glGetUniformLocation(mod->shader_id, "lightNumb"), 1);
        glUniform3f(glGetUniformLocation(mod->shader_id, "lightPos"), scene->point_lights[0].position.x, scene->point_lights[0].position.y, scene->point_lights[0].position.z);
        glUniform3f(glGetUniformLocation(mod->shader_id, "viewPos"), cam->pos.x, cam->pos.y, cam->pos.z);
        glUniform3f(glGetUniformLocation(mod->shader_id, "material.diffuse"), ent->color.x, ent->color.y, ent->color.z);
        glUniform3f(glGetUniformLocation(mod->shader_id, "material.specular"), 0.4f, 0.4f, 0.4f);
        glUniform1f(glGetUniformLocation(mod->shader_id, "material.shininess"), 16.0f);
        glUniform3f(glGetUniformLocation(mod->shader_id, "light.ambient"), 0.3f, 0.3f, 0.3f);
        glUniform3f(glGetUniformLocation(mod->shader_id, "light.diffuse"), 0.8f, 0.8f, 0.8f);
        glUniform3f(glGetUniformLocation(mod->shader_id, "light.specular"), 0.5f, 0.5f, 0.5f);
        glUniform1f(glGetUniformLocation(mod->shader_id, "light.constant"), 1.0f);
        glUniform1f(glGetUniformLocation(mod->shader_id, "light.linear"), 0.045f);
        glUniform1f(glGetUniformLocation(mod->shader_id, "light.quadratic"), 0.0075f);
        glDrawArrays(GL_TRIANGLES, 0, mod->ind_number);
    }
}

void	Render::draw_skybox(Skybox* skybox, Camera* cam)
{
    glDepthMask(GL_FALSE);
    glUseProgram(skybox->shader_id);
    mat3 tmp = mat3(cam->view);
    mat4 view = mat4(tmp);
    unsigned int view_loc = glGetUniformLocation(skybox->shader_id, "u_V");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, view.mat);
    unsigned int proj_loc = glGetUniformLocation(skybox->shader_id, "u_P");
    glUniformMatrix4fv(proj_loc, 1, GL_FALSE, projection.mat);
    glBindVertexArray(skybox->vao);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->texture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthMask(GL_TRUE);
}

void    Render::draw_landscape(Scene* scene, Landscape* landscape, Camera* cam)
{
    glUseProgram(landscape->shader_id);
    unsigned int view_loc = glGetUniformLocation(landscape->shader_id, "u_V");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, cam->view.mat);
    unsigned int proj_loc = glGetUniformLocation(landscape->shader_id, "u_P");
    glUniformMatrix4fv(proj_loc, 1, GL_FALSE, projection.mat);
    glUniform1i(glGetUniformLocation(landscape->shader_id, "lightNumb"), 3);
    glUniform3f(glGetUniformLocation(landscape->shader_id, "lightPos"), scene->point_lights[0].position.x, scene->point_lights[0].position.y, scene->point_lights[0].position.z);
    glUniform3f(glGetUniformLocation(landscape->shader_id, "viewPos"), cam->pos.x, cam->pos.y, cam->pos.z);
    glUniform3f(glGetUniformLocation(landscape->shader_id, "material.specular"), 0.4f, 0.4f, 0.4f);
    glUniform1f(glGetUniformLocation(landscape->shader_id, "material.shininess"), 16.0f);
    glUniform3f(glGetUniformLocation(landscape->shader_id, "light.ambient"), 0.3f, 0.3f, 0.3f);
    glUniform3f(glGetUniformLocation(landscape->shader_id, "light.diffuse"), scene->point_lights[0].color.x, scene->point_lights[0].color.y, scene->point_lights[0].color.z);
    glUniform3f(glGetUniformLocation(landscape->shader_id, "light.specular"), 0.5f, 0.5f, 0.5f);
    glUniform1f(glGetUniformLocation(landscape->shader_id, "light.constant"), 1.0f);
    glUniform1f(glGetUniformLocation(landscape->shader_id, "light.linear"), 0.045f);
    glUniform1f(glGetUniformLocation(landscape->shader_id, "light.quadratic"), 0.0075f);

    glBindVertexArray(landscape->vao);
    glDrawArrays(GL_TRIANGLES, 0, (landscape->map_size - 1) * (landscape->map_size - 1) * 3 * 2);
}

void    Render::draw_water(Water* water, Camera* cam)
{
    int k = 0;
    for (int x = 0; x < water->map_size - 1; ++x)
    {
        water->vertices[k + 0] = water->water_map[x].x;
        water->vertices[k + 1] = 0.0f;
        water->vertices[k + 2] = water->water_map[x].z;
        water->vertices[k + 3] = water->water_map[x + 1].x;
        water->vertices[k + 4] = 0.0f;
        water->vertices[k + 5] = water->water_map[x + 1].z;
        water->vertices[k + 6] = water->water_map[x + 1].x;
        water->vertices[k + 7] = water->water_map[x + 1].sum_level;
        water->vertices[k + 8] = water->water_map[x + 1].z;
        water->vertices[k + 9] = water->water_map[x].x;
        water->vertices[k + 10] = 0.0f;
        water->vertices[k + 11] = water->water_map[x].z;
        water->vertices[k + 12] = water->water_map[x].x;
        water->vertices[k + 13] = water->water_map[x].sum_level;
        water->vertices[k + 14] = water->water_map[x].z;
        water->vertices[k + 15] = water->water_map[x + 1].x;
        water->vertices[k + 16] = water->water_map[x + 1].sum_level;
        water->vertices[k + 17] = water->water_map[x + 1].z;
        k += 18;

        water->vertices[k + 0] = water->water_map[water->map_size * (water->map_size - 1) + x].x;
        water->vertices[k + 1] = 0.0f;
        water->vertices[k + 2] = water->water_map[water->map_size * (water->map_size - 1) + x].z;
        water->vertices[k + 3] = water->water_map[water->map_size * (water->map_size - 1) + x + 1].x;
        water->vertices[k + 4] = 0.0f;
        water->vertices[k + 5] = water->water_map[water->map_size * (water->map_size - 1) + x + 1].z;
        water->vertices[k + 6] = water->water_map[water->map_size * (water->map_size - 1) + x + 1].x;
        water->vertices[k + 7] = water->water_map[water->map_size * (water->map_size - 1) + x + 1].sum_level;
        water->vertices[k + 8] = water->water_map[water->map_size * (water->map_size - 1) + x + 1].z;
        water->vertices[k + 9] = water->water_map[water->map_size * (water->map_size - 1) + x].x;
        water->vertices[k + 10] = 0.0f;
        water->vertices[k + 11] = water->water_map[water->map_size * (water->map_size - 1) + x].z;
        water->vertices[k + 12] = water->water_map[water->map_size * (water->map_size - 1) + x].x;
        water->vertices[k + 13] = water->water_map[water->map_size * (water->map_size - 1) + x].sum_level;
        water->vertices[k + 14] = water->water_map[water->map_size * (water->map_size - 1) + x].z;
        water->vertices[k + 15] = water->water_map[water->map_size * (water->map_size - 1) + x + 1].x;
        water->vertices[k + 16] = water->water_map[water->map_size * (water->map_size - 1) + x + 1].sum_level;
        water->vertices[k + 17] = water->water_map[water->map_size * (water->map_size - 1) + x + 1].z;
        k += 18;

        water->vertices[k + 0] = water->water_map[water->map_size * x].x;
        water->vertices[k + 1] = 0.0f;
        water->vertices[k + 2] = water->water_map[water->map_size * x].z;
        water->vertices[k + 3] = water->water_map[water->map_size * (x + 1)].x;
        water->vertices[k + 4] = 0.0f;
        water->vertices[k + 5] = water->water_map[water->map_size * (x + 1)].z;
        water->vertices[k + 6] = water->water_map[water->map_size * (x + 1)].x;
        water->vertices[k + 7] = water->water_map[water->map_size * (x + 1)].sum_level;
        water->vertices[k + 8] = water->water_map[water->map_size * (x + 1)].z;
        water->vertices[k + 9] = water->water_map[water->map_size * x].x;
        water->vertices[k + 10] = 0.0f;
        water->vertices[k + 11] = water->water_map[water->map_size * x].z;
        water->vertices[k + 12] = water->water_map[water->map_size * x].x;
        water->vertices[k + 13] = water->water_map[water->map_size * x].sum_level;
        water->vertices[k + 14] = water->water_map[water->map_size * x].z;
        water->vertices[k + 15] = water->water_map[water->map_size * (x + 1)].x;
        water->vertices[k + 16] = water->water_map[water->map_size * (x + 1)].sum_level;
        water->vertices[k + 17] = water->water_map[water->map_size * (x + 1)].z;
        k += 18;

        water->vertices[k + 0] = water->water_map[water->map_size * x + (water->map_size - 1)].x;
        water->vertices[k + 1] = 0.0f;
        water->vertices[k + 2] = water->water_map[water->map_size * x + (water->map_size - 1)].z;
        water->vertices[k + 3] = water->water_map[water->map_size * (x + 1) + (water->map_size - 1)].x;
        water->vertices[k + 4] = 0.0f;
        water->vertices[k + 5] = water->water_map[water->map_size * (x + 1) + (water->map_size - 1)].z;
        water->vertices[k + 6] = water->water_map[water->map_size * (x + 1) + (water->map_size - 1)].x;
        water->vertices[k + 7] = water->water_map[water->map_size * (x + 1) + (water->map_size - 1)].sum_level;
        water->vertices[k + 8] = water->water_map[water->map_size * (x + 1) + (water->map_size - 1)].z;
        water->vertices[k + 9] = water->water_map[water->map_size * x + (water->map_size - 1)].x;
        water->vertices[k + 10] = 0.0f;
        water->vertices[k + 11] = water->water_map[water->map_size * x + (water->map_size - 1)].z;
        water->vertices[k + 12] = water->water_map[water->map_size * x + (water->map_size - 1)].x;
        water->vertices[k + 13] = water->water_map[water->map_size * x + (water->map_size - 1)].sum_level;
        water->vertices[k + 14] = water->water_map[water->map_size * x + (water->map_size - 1)].z;
        water->vertices[k + 15] = water->water_map[water->map_size * (x + 1) + (water->map_size - 1)].x;
        water->vertices[k + 16] = water->water_map[water->map_size * (x + 1) + (water->map_size - 1)].sum_level;
        water->vertices[k + 17] = water->water_map[water->map_size * (x + 1) + (water->map_size - 1)].z;
        k += 18;
    }

    for (int y = 0; y < water->map_size - 1; ++y)
    {
        for (int x = 0; x < water->map_size - 1; ++x)
        {
            water->vertices[k + 0] = water->water_map[y * water->map_size + x].x;
            water->vertices[k + 1] = water->water_map[y * water->map_size + x].rend_level;
            water->vertices[k + 2] = water->water_map[y * water->map_size + x].z;
            water->vertices[k + 3] = water->water_map[y * water->map_size + x + 1].x;
            water->vertices[k + 4] = water->water_map[y * water->map_size + x + 1].rend_level;
            water->vertices[k + 5] = water->water_map[y * water->map_size + x + 1].z;
            water->vertices[k + 6] = water->water_map[(y + 1) * water->map_size + x + 1].x;
            water->vertices[k + 7] = water->water_map[(y + 1) * water->map_size + x + 1].rend_level;
            water->vertices[k + 8] = water->water_map[(y + 1) * water->map_size + x + 1].z;
            water->vertices[k + 9] = water->water_map[y * water->map_size + x].x;
            water->vertices[k + 10] = water->water_map[y * water->map_size + x].rend_level;
            water->vertices[k + 11] = water->water_map[y * water->map_size + x].z;
            water->vertices[k + 12] = water->water_map[(y + 1) * water->map_size + x].x;
            water->vertices[k + 13] = water->water_map[(y + 1) * water->map_size + x].rend_level;
            water->vertices[k + 14] = water->water_map[(y + 1) * water->map_size + x].z;
            water->vertices[k + 15] = water->water_map[(y + 1) * water->map_size + x + 1].x;
            water->vertices[k + 16] = water->water_map[(y + 1) * water->map_size + x + 1].rend_level;
            water->vertices[k + 17] = water->water_map[(y + 1) * water->map_size + x + 1].z;
            k += 18;
        }

    }
    glUseProgram(water->shader_id);
    unsigned int view_loc = glGetUniformLocation(water->shader_id, "u_V");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, cam->view.mat);
    unsigned int proj_loc = glGetUniformLocation(water->shader_id, "u_P");
    glUniformMatrix4fv(proj_loc, 1, GL_FALSE, projection.mat);
    glBindVertexArray(water->vao);
    glBindBuffer(GL_ARRAY_BUFFER, water->vbo);
    glBufferData(GL_ARRAY_BUFFER, ((water->map_size - 1) * (water->map_size - 1) * 3 * 2 * 3 + (water->map_size - 1) * 18 * 4) * sizeof(float), water->vertices, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, (water->map_size - 1) * (water->map_size - 1) * 3 * 2 + (water->map_size - 1) * 18 * 4);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}