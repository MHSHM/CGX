#include "STessellation.h"
#include <Game.h>
#include <glew.h>
#include <glfw3.h>
#include <iostream>

STessellation::STessellation(Game* _game):
    Scene(_game)
{
}

bool STessellation::Init()
{
    nodes.reserve(MAX_NODES); 
    actors.reserve(MAX_ACTORS); 
    textures.reserve(MAX_TEXTURES); 
    transforms.Init(); 
    models.Init();

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
        LOG_ERR("Failed to configure render target\n"); 
        return false; 
    }

    return true;
}

void STessellation::Load_Data()
{

	SceneNode* plane = game->node_manager.Create_Scene_Node(this, "plane");
	Transform* transform = transforms.Add_Component(plane->actor);
	Model* model = models.Add_Component(plane->actor); 

	transform->translation = glm::vec3(0.0f, 0.0f, -2.0f);
	transform->scale = 1.0f;
	
	TextureParams params;
	params.mag_filter = GL_LINEAR;
	params.min_filter = GL_LINEAR;
	params.wrap_s = GL_REPEAT;
	params.wrap_t = GL_REPEAT;
	params.generate_mips = false;

	model->meshes.push_back(Mesh()); 
	model->meshes.back().render_data.material.normal_map = game->loader.Load_Texture(this, "Assets/teapot_normal.png", params);
	model->meshes.back().render_data.material.height_map = game->loader.Load_Texture(this, "Assets/teapot_disp.png", params);

	/*
	    v3      v2
		* * * * *
		*       * 
		*       *
		* * * * *
		v0      v1
	*/

	float positions[] = {
		 // v0 
		 -0.5f,  -0.5f, 0.0f,
		 // v1
		 0.5f, -0.5f, 0.0f,
		 // v2
		 0.5f,  0.5f, 0.0f,
		 // v3
		 -0.5f, 0.5f, 0.0f,
	};

	float uvs[] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
	};

	glPatchParameteri(GL_PATCH_VERTICES, 4); 

	glGenVertexArrays(1, &vertex_array_id);
	glBindVertexArray(vertex_array_id);
	
	glGenBuffers(1, &positions_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, positions_buffer);
	glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(float), positions, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

	glGenBuffers(1, &uvs_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvs_buffer);
	glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), uvs, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
}


static void mouse_cursor_callback(GLFWwindow* window, double xpos, double ypos)
{
	Game* game = (Game*)glfwGetWindowUserPointer(window);

	Transform* transform = game->tessellation.nodes_map["plane"]->actor->Get_Component<Transform>();

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

void STessellation::Update(float deltatime)
{
	Update_Components(deltatime); 

	glfwSetCursorPosCallback(game->window.window_ptr, mouse_cursor_callback);

	SceneNode* plane = nodes_map["plane"];

	if (!plane)
	{
		return;
	}

	Transform* transform = plane->actor->Get_Component<Transform>();

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

}

void STessellation::Update_Components(float deltatime)
{
	for (auto& transform : transforms.components) 
	{
		transform.Update(deltatime);
	}
}

void STessellation::Draw()
{
	float aspect = (float)game->window.width / (float)game->window.height;
	glm::mat4 perspective_proj = glm::perspective(glm::radians(90.0f), aspect, 0.1f, 100.0f);
	glm::mat4 model_matrix = nodes_map["plane"]->actor->Get_Component<Transform>()->model_matrix;
	LDRTexture* height_map = nodes_map["plane"]->actor->Get_Component<Model>()->meshes.back().render_data.material.height_map;
	LDRTexture* normal_map = nodes_map["plane"]->actor->Get_Component<Model>()->meshes.back().render_data.material.normal_map;

	glViewport(0, 0, render_target.color_buffers[0].params.width, render_target.color_buffers[0].params.height);
	render_target.Bind(); 
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	game->shaders_table["tessellation"].Bind();

	game->shaders_table["tessellation"].Set_Matrix4_Uniform("mvp", perspective_proj * model_matrix);

	height_map->Bind(0); 
	game->shaders_table["tessellation"].Set_Int_Uniform("height_map", 0); 

	normal_map->Bind(1); 
	game->shaders_table["tessellation"].Set_Int_Uniform("normal_map", 1); 

	glBindVertexArray(vertex_array_id); 
	glDrawArrays(GL_PATCHES, 0, 4);
	glBindVertexArray(0); 

	game->shaders_table["tessellation"].Un_Bind();
	render_target.Unbind(); 
}

void STessellation::Clear()
{
	render_target.Clear(); 

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

