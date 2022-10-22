#include <SFinal.h>
#include <Game.h>
#include <glew.h>
#include <iostream>

SFinal::SFinal(Game* _game):
	Scene(_game)
{
}

bool SFinal::Init()
{
	nodes.reserve(MAX_NODES); 
	actors.reserve(MAX_ACTORS); 
	transform_manager.Init(); 
	model_manager.Init(); 

	return true;
}

void SFinal::Load_Data()
{
	SceneNode* square = game->node_manager.Create_Scene_Node(this, "square");
	Transform* transform = transform_manager.Add_Component(square->actor);
	Model* model = model_manager.Add_Component(square->actor);
	
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
}

void SFinal::Update(float deltatime)
{
	Update_Components(deltatime);
}

void SFinal::Update_Components(float deltatime)
{
	for (auto& transform : transform_manager.components) 
	{
		transform.Update(deltatime); 
	}

	for (auto& model : model_manager.components) 
	{
		model.Update(deltatime); 
	}
}

void SFinal::Draw()
{
	default_render_target.Bind();
	glViewport(0, 0, game->window.width, game->window.height);
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	game->shaders_table["basic"].Bind();

	Model* model = nodes.back().actor->Get_Component<Model>();

	game->rays.render_target.color_buffers[0].Bind(0);
	game->shaders_table["basic"].Set_Int_Uniform("scene", 0);

	model->meshes.back().vao.Bind();

	glDrawElements(GL_TRIANGLES, model->meshes.back().render_data.indices.size(), GL_UNSIGNED_INT, (void*)model->meshes.back().render_data.indices[0]);
	
	model->meshes.back().vao.Un_Bind();

	game->shaders_table["basic"].Un_Bind();
}
