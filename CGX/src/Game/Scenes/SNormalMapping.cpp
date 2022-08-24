#include "SNormalMapping.h"
#include <Game.h>
#include <glew.h>
#include <glfw3.h>
#include <iostream>

SNormalMapping::SNormalMapping(Game* _game):
	Scene(_game)
{
}

bool SNormalMapping::Init()
{
	nodes.reserve(MAX_NODES);
	actors.reserve(MAX_ACTORS);
	textures.reserve(MAX_TEXTURES);
	transforms.Init(); 
	models.Init(); 
	pointlights.Init(); 

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

	render_target.Generate_Framebuffer(); 
	render_target.Generate_Texture_Attachment(Attachment::Color, params);

	params.internl_format = GL_DEPTH_COMPONENT;
	params.format = GL_DEPTH_COMPONENT;
	params.components_type = GL_FLOAT;
	render_target.Generate_Texture_Attachment(Attachment::Depth, params);
	
	if (!render_target.Configure(0, 0)) 
	{
		LOG_ERR("failed to configure STesselation render target\n"); 
		return false; 
	}

	depth_render_target.Generate_Framebuffer(); 

	params.width = 1024; 
	params.height = 1024; 
	params.wrap_s = GL_CLAMP_TO_EDGE;
	params.wrap_t = GL_CLAMP_TO_EDGE;
	params.wrap_r = GL_CLAMP_TO_EDGE;
	depth_render_target.Generate_Texture_Attachment(Attachment::DepthCube, params);

	if (!depth_render_target.Configure(0, -1)) 
	{
		LOG_ERR("failed to configure shadow maps render target\n"); 
		return false; 
	}

	return true;
}

void SNormalMapping::Load_Data()
{
	SceneNode* sphere = game->node_manager.Create_Scene_Node(this, "sphere");
	Transform* sphere_transform = transforms.Add_Component(sphere->actor);
	sphere_transform->translation = glm::vec3(0.0f, -0.199999f, -1.7f);
	sphere_transform->scale = 0.01f;
	PointLight* point_light_cmp = pointlights.Add_Component(sphere->actor);

	SceneNode* plane = game->node_manager.Create_Scene_Node(this, "plane");
	Transform* plane_transform = transforms.Add_Component(plane->actor);
	plane_transform->translation = glm::vec3(0.0f, -0.7f, -4.0f);
	plane_transform->scale = 2.0f;

	Model* model = models.Add_Component(plane->actor);

	model->meshes.push_back(Mesh());
	model->meshes.back().render_data.positions.reserve(4);
	model->meshes.back().render_data.uvs.reserve(4);
	model->meshes.back().render_data.indices.reserve(6);

	model->meshes.back().render_data.positions.push_back(glm::vec3(1.0f, 1.0f, 0.0f));
	model->meshes.back().render_data.positions.push_back(glm::vec3(1.0f, -1.0f, 0.0f));
	model->meshes.back().render_data.positions.push_back(glm::vec3(-1.0f, -1.0f, 0.0f));
	model->meshes.back().render_data.positions.push_back(glm::vec3(-1.0f, 1.0f, 0.0f));

	model->meshes.back().render_data.uvs.push_back(glm::vec2(1.0f, 1.0f));
	model->meshes.back().render_data.uvs.push_back(glm::vec2(1.0f, 0.0f));
	model->meshes.back().render_data.uvs.push_back(glm::vec2(0.0f, 0.0f));
	model->meshes.back().render_data.uvs.push_back(glm::vec2(0.0f, 1.0f));

	model->meshes.back().render_data.indices.push_back(0);
	model->meshes.back().render_data.indices.push_back(1);
	model->meshes.back().render_data.indices.push_back(3);
	model->meshes.back().render_data.indices.push_back(1);
	model->meshes.back().render_data.indices.push_back(2);
	model->meshes.back().render_data.indices.push_back(3);

	TextureParams params; 
	params.mag_filter = GL_LINEAR; 
	params.min_filter = GL_LINEAR; 
	params.wrap_s = GL_REPEAT;  
	params.wrap_t = GL_REPEAT;
	params.generate_mips = false; 

	model->meshes.back().render_data.material.normal_map = game->loader.Load_Texture(this, "Assets/plane/teapot_normal.png", params);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Game* game = (Game*)glfwGetWindowUserPointer(window);

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) 
	{
		game->normal_mapping.draw_wireframes = !game->normal_mapping.draw_wireframes; 
	}
}

static void mouse_cursor_callback(GLFWwindow* window, double xpos, double ypos)
{
	Game* game = (Game*)glfwGetWindowUserPointer(window);

	Transform* transform = game->normal_mapping.nodes_map["plane"]->actor->Get_Component<Transform>();

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

void SNormalMapping::Update(float deltatime)
{
	Update_Components(deltatime);

	glfwSetKeyCallback(game->window.window_ptr, key_callback);
	glfwSetCursorPosCallback(game->window.window_ptr, mouse_cursor_callback);

	SceneNode* light_source = nodes_map["plane"];

	if (!light_source)
	{
		return;
	}

	Transform* transform = light_source->actor->Get_Component<Transform>();

	float speed = 1.0f;

	if (glfwGetKey(game->window.window_ptr, GLFW_KEY_W) == GLFW_PRESS)
	{
		transform->translation.z += -speed * deltatime;
	}

	if (glfwGetKey(game->window.window_ptr, GLFW_KEY_S) == GLFW_PRESS)
	{
		transform->translation.z += speed * deltatime;
	}

	if (glfwGetKey(game->window.window_ptr, GLFW_KEY_A) == GLFW_PRESS)
	{
		transform->translation.x += -speed * deltatime;
	}

	if (glfwGetKey(game->window.window_ptr, GLFW_KEY_D) == GLFW_PRESS)
	{
		transform->translation.x += speed * deltatime;
	}

	if (glfwGetKey(game->window.window_ptr, GLFW_KEY_UP) == GLFW_PRESS)
	{
		transform->translation.y += speed * deltatime;
	}

	if (glfwGetKey(game->window.window_ptr, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		transform->translation.y += -speed * deltatime;
	}

	Generate_Depth_Maps(); 
}

void SNormalMapping::Update_Components(float deltatime)
{
	for (auto& transform : transforms.components) 
	{
		transform.Update(deltatime);
	}

	for (auto& pointlight : pointlights.components) 
	{
		pointlight.Update(deltatime); 
	}
}

void SNormalMapping::Draw()
{
	float aspect = (float)game->window.width / (float)game->window.height;
	glm::mat4 perspective_proj = glm::perspective(glm::radians(90.0f), aspect, 0.1f, 100.0f);
	PointLight* point_light = nodes_map["sphere"]->actor->Get_Component<PointLight>();

	render_target.Bind();

	glViewport(0, 0, render_target.color_buffers[0].params.width, render_target.color_buffers[0].params.height);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Normal mapping
	for (auto& node : nodes)
	{
		Transform* transform = node.actor->Get_Component<Transform>();
		Model* model = node.actor->Get_Component<Model>();

		if (!model || !transform) continue;

		glm::mat4 mvp = perspective_proj * transform->model_matrix;

		for (auto& mesh : model->meshes)
		{
			game->shaders_table["shadow2"].Bind();

			game->shaders_table["shadow2"].Set_Vec3_Uniform("point_light.light_position", point_light->light_position);
			game->shaders_table["shadow2"].Set_Vec3_Uniform("point_light.light_intensity", point_light->light_intensity);

			game->shaders_table["shadow2"].Set_Matrix4_Uniform("mvp", mvp);
			game->shaders_table["shadow2"].Set_Matrix4_Uniform("model", transform->model_matrix);


			if (mesh.render_data.material.diffuse_map)
			{
				mesh.render_data.material.diffuse_map->Bind(0);
				game->shaders_table["shadow2"].Set_Int_Uniform("material.diffuse_map", 0);
			}

			if (mesh.render_data.material.specular_map)
			{
				mesh.render_data.material.specular_map->Bind(1);
				game->shaders_table["shadow2"].Set_Int_Uniform("material.specular_map", 1);
			}

			if (mesh.render_data.material.normal_map)
			{
				mesh.render_data.material.normal_map->Bind(2);
				game->shaders_table["shadow2"].Set_Int_Uniform("material.normal_map", 2);
			}

			depth_render_target.depth_buffers[0].Bind_Cube_Map(3);
			game->shaders_table["shadow2"].Set_Int_Uniform("depth_map", 3);

			game->shaders_table["shadow2"].Set_Vec3_Uniform("material.ka", mesh.render_data.material.ka);
			game->shaders_table["shadow2"].Set_Vec3_Uniform("material.kd", mesh.render_data.material.kd);
			game->shaders_table["shadow2"].Set_Vec3_Uniform("material.ks", mesh.render_data.material.ks);
			game->shaders_table["shadow2"].Set_Float_Uniform("material.shininess", mesh.render_data.material.shininess);

			mesh.vao.Bind();

			glDrawElements(GL_TRIANGLES, mesh.render_data.indices.size(), GL_UNSIGNED_INT, (void*)mesh.render_data.indices[0]);

			mesh.vao.Un_Bind();

			game->shaders_table["shadow2"].Un_Bind();
		}
	}
	
	// Draw wireframes
	if (draw_wireframes) 
	{
		for (auto& node : nodes)
		{
			Transform* transform = node.actor->Get_Component<Transform>();
			Model* model = node.actor->Get_Component<Model>();

			if (!model || !transform) continue;

			glm::mat4 mvp = perspective_proj * transform->model_matrix;

			for (auto& mesh : model->meshes)
			{
				game->shaders_table["wireFrame"].Bind();

				game->shaders_table["wireFrame"].Set_Matrix4_Uniform("mvp", mvp);

				mesh.vao.Bind();

				glDrawElements(GL_TRIANGLES, mesh.render_data.indices.size(), GL_UNSIGNED_INT, (void*)mesh.render_data.indices[0]);

				mesh.vao.Un_Bind();

				game->shaders_table["wireFrame"].Un_Bind();
			}
		}
	}
	
	render_target.Unbind();
}

void SNormalMapping::Clear()
{
	render_target.Clear();
	depth_render_target.Clear(); 

	for (auto& model : models.components)
	{
		model.Clear();
	}

	while (!nodes.empty())
	{
		nodes_map[nodes.back().tag] = nullptr;
		game->node_manager.Remove_Scene_Node(this, &nodes.back());
	}

	nodes_map.clear();
	dead_scene_nodes.clear();

	for (auto& texture : textures)
	{
		texture.Clear();
	}

	textures.clear();
	textures_map.clear();
}

void SNormalMapping::Generate_Depth_Maps()
{

	PointLight* pointlight = nodes_map["sphere"]->actor->Get_Component<PointLight>();
	float light_aspect = (float)depth_render_target.depth_buffers[0].params.width / (float)depth_render_target.depth_buffers[0].params.height;

	glm::mat4 translation = glm::translate(glm::mat4(1.0f), -pointlight->light_position);
	glm::mat4 light_perspective_proj = glm::perspective(glm::radians(90.0f), light_aspect, 0.1f, 25.0f);


	std::vector<glm::mat4> shadow_transforms;
	shadow_transforms.reserve(6);

	// +X Face
	shadow_transforms.push_back(light_perspective_proj *
		glm::lookAt(pointlight->light_position, pointlight->light_position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));

	// -X Face
	shadow_transforms.push_back(light_perspective_proj *
		glm::lookAt(pointlight->light_position, pointlight->light_position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));

	// +Y Face
	shadow_transforms.push_back(light_perspective_proj *
		glm::lookAt(pointlight->light_position, pointlight->light_position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));

	// -Y Face
	shadow_transforms.push_back(light_perspective_proj *
		glm::lookAt(pointlight->light_position, pointlight->light_position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));

	// +Z Face
	shadow_transforms.push_back(light_perspective_proj *
		glm::lookAt(pointlight->light_position, pointlight->light_position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));

	// -Z Face
	shadow_transforms.push_back(light_perspective_proj *
		glm::lookAt(pointlight->light_position, pointlight->light_position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

	depth_render_target.Bind();
	glViewport(0, 0, depth_render_target.depth_buffers[0].params.width, depth_render_target.depth_buffers[0].params.height);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);

	for (auto& node : nodes)
	{
		Transform* transform = node.actor->Get_Component<Transform>();
		Model* model = node.actor->Get_Component<Model>();

		if (!model || !transform) continue;

		for (auto& mesh : model->meshes)
		{
			game->shaders_table["DepthMap"].Bind();

			game->shaders_table["DepthMap"].Set_Matrix4_Uniform("model", transform->model_matrix);

			for (int i = 0; i < shadow_transforms.size(); ++i)
			{
				game->shaders_table["DepthMap"].Set_Matrix4_Uniform("shadowMatrices[" + std::to_string(i) + "]", shadow_transforms[i]);
			}

			game->shaders_table["DepthMap"].Set_Vec3_Uniform("light_pos", pointlight->light_position);

			mesh.vao.Bind();

			glDrawElements(GL_TRIANGLES, mesh.render_data.indices.size(), GL_UNSIGNED_INT, (void*)mesh.render_data.indices[0]);

			mesh.vao.Un_Bind();

			game->shaders_table["DepthMap"].Un_Bind();
		}
	}

	depth_render_target.Unbind();
}

