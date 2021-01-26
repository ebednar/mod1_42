#include "engine.h"
#include "glad.h"
#include <iostream>
#include "glm/glm.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

double Engine::delta_time = 0.0f;
Engine::~Engine()
{
	int length = models.size();
	for (int i = 0; i < length; ++i)
	{
		delete models[i]->vertices;
		delete models[i];
	}
	length = scene.ents.size();
	for (int i = 0; i < length; ++i)
		delete scene.ents[i];
	std::cout << "Engine off" << std::endl;
}

void Engine::init_engine(int width, int height)
{
	if (!glfwInit())
		exit(EXIT_FAILURE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(width, height, "Mod1", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGL())
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwSetKeyCallback(window, key_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetWindowUserPointer(window, &controls);
	glViewport(0, 0, width, height);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	controls.yaw = cam.yaw;
	controls.pitch = cam.pitch;
	rend.init();
	std::vector<std::string> faces;
	faces.push_back("res/cubemaps/right.jpg");
	faces.push_back("res/cubemaps/left.jpg");
	faces.push_back("res/cubemaps/top.jpg");
	faces.push_back("res/cubemaps/bottom.jpg");
	faces.push_back("res/cubemaps/front.jpg");
	faces.push_back("res/cubemaps/back.jpg");
	skybox.init(faces);
	skybox.set_shader("res/shaders/skybox_vert.glsl", "res/shaders/skybox_frag.glsl");

    IMGUI_CHECKVERSION();
	ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

}

void Engine::run_engine()
{
	old_time = glfwGetTime();
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);

		delta_time = glfwGetTime() - old_time;
		timer += delta_time;
		fps++;
		if (timer >= 1.0)
		{
			timer -= 1.0;
			std::cout << "fps - " << fps << std::endl;
			fps = 0;
		}
		old_time = glfwGetTime();

        ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

		cam.speed = 8.0f * delta_time;
		if (controls.keys[GLFW_KEY_W])
			cam.pos = cam.pos + cam.speed * cam.front;
		if (controls.keys[GLFW_KEY_S])
			cam.pos = cam.pos - cam.speed * cam.front;
		if (controls.keys[GLFW_KEY_A])
			cam.pos = cam.pos - cam.speed * normalize(cross(cam.front, cam.up));
		if (controls.keys[GLFW_KEY_D])
			cam.pos = cam.pos + cam.speed * normalize(cross(cam.front, cam.up));
		cam.yaw = controls.yaw;
		cam.pitch = controls.pitch;

		if (water.w_state == water_state::raise)
		{
			water.raise();
		}
		else if (water.w_state == water_state::wave)
		{
			water.wave();
		}
		else if (water.w_state == water_state::rain)
		{
			water.rain();
			rain.rainfall();
		}

		water.flow();
		water.flow();
		water.flow();
		water.flow();
		water.flow();

		cam.update_free();
		rend.draw_skybox(&skybox, &cam);
		rend.draw_scene(&animator, &scene, &cam);
        rend.draw_landscape(&scene, &landscape, &cam);
        rend.draw_water(&water, &cam);
        if (water.w_state == water_state::rain)
        {
            rend.draw_rain(&rain, &cam);
        }
		//rend.draw_pbr(&scene, &cam);

        ImGui::Begin("mod1");
        ImGui::Text("Use buttons to change scenario"); // Display some text (you can use a format strings too)
        if (ImGui::Button("Raise"))
        {
            water.reset();
            water.w_state = water_state::raise;
        }
       	if (ImGui::Button("Wave"))
        {
            water.reset();
            water.w_state = water_state::wave;
        }
       	if (ImGui::Button("Rain"))
        {
            water.reset();
            water.w_state = water_state::rain;
        }
       	ImGui::SliderFloat("step", &water.step, 0.002f, 0.006f);
       	ImGui::SliderFloat("water tension", &water.tension, 0.0001f, 0.006f);
       	ImGui::SliderInt("rain power", &water.rain_pow, 2, 20);
        ImGui::End();
        ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
		if(close_eng)
			glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
}

void Engine::add_model(Model *mod)
{
	models.push_back(mod);
}

void Engine::add_entity(Entity *ent)
{
	scene.add_entity(ent);
}

void Engine::set_player(Entity *ent)
{
	player = ent;
	rend.player = ent;
}

void Engine::add_light_source(glm::vec3 l_pos, glm::vec3 color)
{
	scene.add_light_source(l_pos, color);
}

