#include "Renderer.h"
#include <Game.h>
#include <glew.h>
#include <glfw3.h>

Renderer::Renderer(Game* _game):
	game(_game)
{

}

void Renderer::Send_Data_To_GPU(const Scene* scene)
{
	for (auto node : scene->nodes)
	{
		Model* model = node.actor->Get_Component<Model>();

		if (!model) continue;

		for (auto& mesh : model->meshes) 
		{
			mesh.vao.Send_Data_To_GPU(&mesh.render_data);
		}
	}
}

void Renderer::Draw()
{
	//game->environment.Draw(); 
	//game->shadow.Draw(); 
	//game->tessellation.Draw(); 
	//game->deferred.Draw(); 
	game->rays.Draw();
	//game->ibl.Draw();
	//game->voxelization.Draw();
	game->final.Draw();

	//game->ibl.editor.Draw();
	//game->shadow.editor.Draw();
	//game->environment.editor.Draw(); 
	game->rays.editor.Draw(); 

	glfwSwapBuffers(game->window.window_ptr);
}
