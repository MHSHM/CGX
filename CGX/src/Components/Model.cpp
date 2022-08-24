#include <Model.h>

Model::Model(Actor* _owner):
	Component(_owner)
{
	type = "Model";
}

void Model::Update(float deltatime)
{

}

void Model::Clear()
{
	for (auto& mesh : meshes) 
	{
		mesh.vao.Clear(); 
		mesh.render_data.positions.clear(); 
		mesh.render_data.normals.clear(); 
		mesh.render_data.uvs.clear(); 
		mesh.render_data.indices.clear(); 
	}
}
