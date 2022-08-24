#include "SEnvironment.h"
#include <Game.h>
#include <Transform.h>
#include <Model.h>
#include <glew.h>
#include <glfw3.h>
#include <iostream>
#include <SkyBox.h>
#include <random>

SEnvironment::SEnvironment(Game* _game):
	Scene(_game)
{
}

bool SEnvironment::Init()
{
	nodes.reserve(MAX_NODES);
	actors.reserve(MAX_ACTORS);
	textures.reserve(MAX_TEXTURES);
	transform_manager.Init();
	model_manager.Init();
	pointlight_manager.Init();
	cubemap_manager.Init();

	editor.Initialize(game); 

	TextureParams params;

	params.width = game->window.width;
	params.height = game->window.height;
	params.internl_format = GL_RGB;
	params.format = GL_RGB;
	params.components_type = GL_UNSIGNED_BYTE;
	params.mag_filter = GL_LINEAR;
	params.min_filter = GL_LINEAR;
	params.wrap_s = GL_REPEAT;
	params.wrap_t = GL_REPEAT;

	environment_world_render_target.Generate_Framebuffer();
	environment_world_render_target.Generate_Texture_Attachment(Attachment::Color, params);

	params.internl_format = GL_DEPTH_COMPONENT; 
	params.format = GL_DEPTH_COMPONENT; 
	params.components_type = GL_FLOAT; 

	environment_world_render_target.Generate_Texture_Attachment(Attachment::Depth, params);

	if (!environment_world_render_target.Configure(0, 0))
	{
		std::cerr << "failed to configure reflected world render target\n";
		return false;
	}

	Initialize_Components();

	return true; 
}

void SEnvironment::Load_Data()
{
	SceneNode* teapot = game->node_manager.Create_Scene_Node(this, "teapot");
	model_manager.Add_Component(teapot->actor); 
	game->loader.Load(teapot, "Assets/teapot/teapot.obj");
	Transform* transform = transform_manager.Add_Component(teapot->actor);
	transform->angle_x = glm::radians(-90.0f);
	transform->scale = 0.05f;
	transform->translation.y = -0.7f;
	transform->translation.z = -3.0f;
	
	SceneNode* lightsource = game->node_manager.Create_Scene_Node(this, "lightsource");
	Transform* lightsource_transform = transform_manager.Add_Component(lightsource->actor);
	lightsource_transform->translation = glm::vec3(0.0f, 0.0f, -1.0f);
	lightsource_transform->scale = 0.01f;
	PointLight* point_light_cmp = pointlight_manager.Add_Component(lightsource->actor);
	point_light_cmp->light_intensity.x = 1.0f;
	point_light_cmp->light_intensity.y = 1.0f;
	point_light_cmp->light_intensity.z = 1.0f;

	SceneNode* environment = game->node_manager.Create_Scene_Node(this, "env");
	model_manager.Add_Component(environment->actor);
	game->loader.Load(environment, "Assets/cube/cube.obj");
	SkyBox* cube_map = cubemap_manager.Add_Component(environment->actor);

	cube_map->faces = { "Assets/cubemap/cubemap_posx.png", "Assets/cubemap/cubemap_negx.png", "Assets/cubemap/cubemap_posy.png",
	"Assets/cubemap/cubemap_negy.png", "Assets/cubemap/cubemap_posz.png", "Assets/cubemap/cubemap_negz.png" };

	cube_map->Initialize(); 

	editor.Setup(this); 
}

static void mouse_cursor_callback(GLFWwindow* window, double xpos, double ypos)
{
	Game* game = (Game*)glfwGetWindowUserPointer(window);

	Transform* transform = game->environment.nodes_map["teapot"]->actor->Get_Component<Transform>();

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		return;
	}

	const float EPSILON = 0.001f;

	static double x, y;

	float prev_x = x;
	float prev_y = y;

	glfwGetCursorPos(game->window.window_ptr, &x, &y);


	float d = game->timer.Get_Deltatime();

	float angular_x_speed = 2.0f;
	float angular_y_speed = 2.0f;

	if (y - prev_y > EPSILON)
	{
		transform->angle_x += 1.0f * d;
	}
	else if (prev_y - y > EPSILON)
	{
		transform->angle_x += -1.0f * d;
	}

	if (x - prev_x > EPSILON)
	{
		transform->angle_y += 1.0f * d;
	}
	else if (prev_x - x > EPSILON)
	{
		transform->angle_y += -1.0f * d;
	}
}

void SEnvironment::Update(float deltatime)
{
	Update_Components(deltatime);
	editor.Update(deltatime);
}

void SEnvironment::Update_Components(float deltatime)
{
	for (auto& transform : transform_manager.components)
	{
		transform.Update(deltatime);
	}

	for (auto& pointlight : pointlight_manager.components)
	{
		pointlight.Update(deltatime);
	}
}

void SEnvironment::Draw()
{
	float aspect = (float)game->window.width / (float)game->window.height;
	glm::mat4 perspective_proj = glm::perspective(glm::radians(90.0f), aspect, 0.1f, 100.0f);

	PointLight* point_light = nodes_map["lightsource"]->actor->Get_Component<PointLight>();
	SkyBox* env = nodes_map["env"]->actor->Get_Component<SkyBox>();
	Model* env_model = nodes_map["env"]->actor->Get_Component<Model>();

	environment_world_render_target.Bind();

	glViewport(0, 0, environment_world_render_target.color_buffers[0].params.width, environment_world_render_target.color_buffers[0].params.height);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.6f, 0.6f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (auto& node : nodes)
	{
		Transform* transform = node.actor->Get_Component<Transform>();
		Model* model = node.actor->Get_Component<Model>();

		if (&node == nodes_map["env"]) continue;

		if (!model || !transform) continue;

		glm::mat4 mvp = perspective_proj * transform->model_matrix;

		for (auto& mesh : model->meshes)
		{
			game->shaders_table["BlinnPhong"].Bind();

			game->shaders_table["BlinnPhong"].Set_Vec3_Uniform("point_light.light_position", point_light->light_position);
			game->shaders_table["BlinnPhong"].Set_Vec3_Uniform("point_light.light_intensity", point_light->light_intensity);

			game->shaders_table["BlinnPhong"].Set_Matrix4_Uniform("mvp", mvp);
			game->shaders_table["BlinnPhong"].Set_Matrix4_Uniform("model", transform->model_matrix);

			/*
			if (mesh.render_data.material.diffuse_map)
			{
				mesh.render_data.material.diffuse_map->Bind(0);
				game->shaders_table["BlinnPhong"].Set_Int_Uniform("material.diffuse_map", 0);
			}

			if (mesh.render_data.material.specular_map)
			{
				mesh.render_data.material.specular_map->Bind(1);
				game->shaders_table["BlinnPhong"].Set_Int_Uniform("material.specular_map", 1);
			}
			*/

			env->env_map.Bind(2);
			game->shaders_table["BlinnPhong"].Set_Int_Uniform("env", 2);

			game->shaders_table["BlinnPhong"].Set_Vec3_Uniform("material.ka", mesh.render_data.material.ka);
			game->shaders_table["BlinnPhong"].Set_Vec3_Uniform("material.kd", mesh.render_data.material.kd);
			game->shaders_table["BlinnPhong"].Set_Vec3_Uniform("material.ks", mesh.render_data.material.ks);
			game->shaders_table["BlinnPhong"].Set_Float_Uniform("material.shininess", mesh.render_data.material.shininess);

			mesh.vao.Bind();

			glDrawElements(GL_TRIANGLES, mesh.render_data.indices.size(), GL_UNSIGNED_INT, (void*)mesh.render_data.indices[0]);

			mesh.vao.Un_Bind();

			game->shaders_table["BlinnPhong"].Un_Bind();
		}
	}

	// Draw Background
	glDepthMask(GL_FALSE);
	game->shaders_table["envMap"].Bind();

	env->env_map.Bind(0);
	game->shaders_table["envMap"].Set_Int_Uniform("env", 0);

	game->shaders_table["envMap"].Set_Matrix4_Uniform("proj", perspective_proj);

	env_model->meshes.back().vao.Bind();

	glDrawElements(GL_TRIANGLES, env_model->meshes.back().render_data.indices.size(), GL_UNSIGNED_INT, (void*)env_model->meshes.back().render_data.indices[0]);

	env_model->meshes.back().vao.Un_Bind();
	game->shaders_table["envMap"].Un_Bind();

	glDepthMask(GL_TRUE);	
	environment_world_render_target.Unbind();
}