#include <SShadow.h>
#include <Game.h>
#include <glew.h>
#include <glfw3.h>
#include <iostream>
#include <LDRTexture.h>
#include <gtc/matrix_transform.hpp>
#include <gtx/transform.hpp>


SShadow::SShadow(Game* _game) :
	Scene(_game)
{
}

bool SShadow::Init()
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

	params.width = 1024;
	params.height = 1024;
	params.internl_format = GL_DEPTH_COMPONENT;
	params.format = GL_DEPTH_COMPONENT;
	params.components_type = GL_FLOAT;
	params.mag_filter = GL_LINEAR;
	params.min_filter = GL_LINEAR;
	params.wrap_s = GL_CLAMP_TO_EDGE;
	params.wrap_t = GL_CLAMP_TO_EDGE;
	params.wrap_r = GL_CLAMP_TO_EDGE;
	params.generate_mips = false; 

	depth_maps_buffer.Generate_Framebuffer();
	depth_maps_buffer.Generate_Texture_Attachment(Attachment::DepthCube, params);

	if (!depth_maps_buffer.Configure(0, -1))
	{
		LOG_ERR("failed to configure depth maps buffer\n");
		return false;
	}
	
	shadow_render_target.Generate_Framebuffer();

	params.width = game->window.width; 
	params.height = game->window.height; 
	params.internl_format = GL_RGB; 
	params.format = GL_RGB; 
	params.components_type = GL_UNSIGNED_BYTE; 
	params.wrap_s = GL_REPEAT;
	params.wrap_t = GL_REPEAT;
	shadow_render_target.Generate_Texture_Attachment(Attachment::Color, params);

	params.internl_format = GL_DEPTH_COMPONENT;
	params.format = GL_DEPTH_COMPONENT;
	params.components_type = GL_FLOAT;
	shadow_render_target.Generate_Texture_Attachment(Attachment::Depth, params);

	if (!shadow_render_target.Configure(0, 0))
	{
		LOG_ERR("failed to configure intermediate render target\n"); 
		return false;
	}

	return true; 

}

void SShadow::Load_Data()
{
	SceneNode* teapot = game->node_manager.Create_Scene_Node(this, "teapot");
	model_manager.Add_Component(teapot->actor);
	game->loader.Load(teapot, "Assets/teapot/teapot.obj");
	Transform* transform = transform_manager.Add_Component(teapot->actor);
	transform->angle_x = glm::radians(-90.0f);
	transform->scale = 0.02f;
	transform->translation.y = 0.0f;
	transform->translation.z = -2.0f;

	SceneNode* sphere = game->node_manager.Create_Scene_Node(this, "sphere");
	Transform* sphere_transform = transform_manager.Add_Component(sphere->actor);
	sphere_transform->translation = glm::vec3(0.0f, 1.0f, 1.0f);
	sphere_transform->scale = 0.01f;
	PointLight* point_light_cmp = pointlight_manager.Add_Component(sphere->actor);

	SceneNode* plane = game->node_manager.Create_Scene_Node(this, "plane");
	model_manager.Add_Component(plane->actor);
	game->loader.Load(plane, "Assets/plane/plane.obj");
	Transform* plane_transform = transform_manager.Add_Component(plane->actor);
	plane_transform->translation = glm::vec3(0.0f, -2.0f, -4.0f);
	plane_transform->scale = 0.05f;

	editor.Setup(this);
}

void SShadow::Update(float deltatime)
{
	Update_Components(deltatime);
	editor.Update(deltatime);
}

void SShadow::Update_Components(float deltatime)
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

void SShadow::Draw()
{
	Generate_Depth_Maps();
	Draw_Scene_With_Shadows(); 
}

void SShadow::Clear()
{
	shadow_render_target.Clear();
	depth_maps_buffer.Clear(); 

	for (auto& model : model_manager.components)
	{
		model.Clear();
	}

	for (auto& cube_map : cubemap_manager.components) 
	{
		cube_map.Clear(); 
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

void SShadow::Generate_Depth_Maps()
{
	PointLight* pointlight = nodes_map["sphere"]->actor->Get_Component<PointLight>();
	float light_aspect = (float)depth_maps_buffer.depth_buffers[0].params.width / (float)depth_maps_buffer.depth_buffers[0].params.height;

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
	
	depth_maps_buffer.Bind();
	glViewport(0, 0, depth_maps_buffer.depth_buffers[0].params.width, depth_maps_buffer.depth_buffers[0].params.height);
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

	depth_maps_buffer.Unbind();
}

void SShadow::Draw_Scene_With_Shadows()
{
	float aspect = (float)game->window.width / (float)game->window.height;
	glm::mat4 perspective_proj = glm::perspective(glm::radians(90.0f), aspect, 0.1f, 100.0f);
	PointLight* point_light = nodes_map["sphere"]->actor->Get_Component<PointLight>();

	shadow_render_target.Bind();

	glViewport(0, 0, shadow_render_target.color_buffers[0].params.width, shadow_render_target.color_buffers[0].params.height);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (auto& node : nodes)
	{
		Transform* transform = node.actor->Get_Component<Transform>();
		Model* model = node.actor->Get_Component<Model>();

		if (!model || !transform) continue;

		glm::mat4 mvp = perspective_proj * transform->model_matrix;

		for (auto& mesh : model->meshes)
		{
			game->shaders_table["shadow"].Bind();

			game->shaders_table["shadow"].Set_Vec3_Uniform("point_light.light_position", point_light->light_position);
			game->shaders_table["shadow"].Set_Vec3_Uniform("point_light.light_intensity", point_light->light_intensity);

			game->shaders_table["shadow"].Set_Matrix4_Uniform("mvp", mvp);
			game->shaders_table["shadow"].Set_Matrix4_Uniform("model", transform->model_matrix);

			depth_maps_buffer.depth_buffers[0].Bind_Cube_Map(3);
			game->shaders_table["shadow"].Set_Int_Uniform("depth_map", 3);

			game->shaders_table["shadow"].Set_Vec3_Uniform("material.ka", mesh.render_data.material.ka);
			game->shaders_table["shadow"].Set_Vec3_Uniform("material.kd", mesh.render_data.material.kd);
			game->shaders_table["shadow"].Set_Vec3_Uniform("material.ks", mesh.render_data.material.ks);
			game->shaders_table["shadow"].Set_Float_Uniform("material.shininess", mesh.render_data.material.shininess);

			mesh.vao.Bind();

			glDrawElements(GL_TRIANGLES, mesh.render_data.indices.size(), GL_UNSIGNED_INT, (void*)mesh.render_data.indices[0]);

			mesh.vao.Un_Bind();

			game->shaders_table["shadow"].Un_Bind();
		}
	}

	shadow_render_target.Unbind();
}
