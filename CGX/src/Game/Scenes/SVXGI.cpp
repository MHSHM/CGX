#include "SVXGI.h"

#include <Game.h>
#include <glew.h>
#include <glfw3.h>
#include <iostream>

SVXGI::SVXGI(Game* _game):
    Scene(_game)
{
}

bool SVXGI::Init()
{
    nodes.reserve(MAX_NODES);
    nodes_ptrs.reserve(MAX_NODES);
    actors.reserve(MAX_ACTORS);
    textures.reserve(MAX_TEXTURES);
    transforms.Init();
    models.Init();
    cameras.Init();

    editor.Initialize(game); 

    glGenTextures(1, &voxel_grid);
    glBindTexture(GL_TEXTURE_3D, voxel_grid);

    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, width, height, depth, 0, GL_RGBA, GL_FLOAT, 0);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    return true;
}

void SVXGI::Load_Data()
{
    SceneNode* sponza = game->node_manager.Create_Scene_Node(this, "sponza");

    Model* sponza_model = models.Add_Component(sponza->actor);
    game->loader.Load(sponza, "Assets/sponza/Sponza.gltf");

    Transform* sponza_transform = transforms.Add_Component(sponza->actor);
    sponza_transform->scale = 0.01f;

    ///////////////////////////////////////////////////////////////////////////////
    
    SceneNode* camera = game->node_manager.Create_Scene_Node(this, "camera");
    Transform* camera_transform = transforms.Add_Component(camera->actor);
    cameras.Add_Component(camera->actor);
    camera_transform->translation.z = -0.2f;

    ////////////////////////////////////////////////////////////////////////////////
    
    SceneNode* cube = game->node_manager.Create_Scene_Node(this, "cube");
    Transform* cube_trasnform = transforms.Add_Component(cube->actor);
    models.Add_Component(cube->actor);
    game->loader.Load(cube, "Assets/cube/cube.obj");

    cube_trasnform->translation.z = 0.0f;

    ///////////////////////////////////////////////////////////////////////////////

    SceneNode* quad = game->node_manager.Create_Scene_Node(this, "quad");
    Transform* quad_transform = transforms.Add_Component(quad->actor);
    quad_transform->translation = glm::vec3(0.0f);
    Model* quad_model = models.Add_Component(quad->actor);

    quad_model->meshes.push_back(Mesh());
    quad_model->meshes.back().render_data.positions.reserve(4);
    quad_model->meshes.back().render_data.uvs.reserve(4);
    quad_model->meshes.back().render_data.indices.reserve(6);

    quad_model->meshes.back().render_data.positions.push_back(glm::vec3(1.0f, 1.0f, 0.0f));
    quad_model->meshes.back().render_data.positions.push_back(glm::vec3(1.0f, -1.0f, 0.0f));
    quad_model->meshes.back().render_data.positions.push_back(glm::vec3(-1.0f, -1.0f, 0.0f));
    quad_model->meshes.back().render_data.positions.push_back(glm::vec3(-1.0f, 1.0f, 0.0f));

    quad_model->meshes.back().render_data.uvs.push_back(glm::vec2(1.0f, 1.0f));
    quad_model->meshes.back().render_data.uvs.push_back(glm::vec2(1.0f, 0.0f));
    quad_model->meshes.back().render_data.uvs.push_back(glm::vec2(0.0f, 0.0f));
    quad_model->meshes.back().render_data.uvs.push_back(glm::vec2(0.0f, 1.0f));

    quad_model->meshes.back().render_data.indices.push_back(0);
    quad_model->meshes.back().render_data.indices.push_back(1);
    quad_model->meshes.back().render_data.indices.push_back(3);
    quad_model->meshes.back().render_data.indices.push_back(1);
    quad_model->meshes.back().render_data.indices.push_back(2);
    quad_model->meshes.back().render_data.indices.push_back(3);

    quad_model->meshes.back().Init(quad_model);


    editor.Setup(this);
}

void SVXGI::Update(float deltatime)
{
    Update_Components(deltatime);
    Update_Shaders();
    Scene_Logic(deltatime);
    editor.Update(deltatime);
}

void SVXGI::Update_Components(float deltatime)
{
    for (auto& transform : transforms.components)
    {
        transform.Update(deltatime);
    }

    for (auto& camera : cameras.components)
    {
        camera.Update(deltatime);
    }
}

void SVXGI::Update_Shaders()
{
}

void SVXGI::Scene_Logic(float deltatime)
{

    SceneNode* camera = nodes_map["camera"];
    Camera* camera_cmp = camera->actor->Get_Component<Camera>();
    Transform* camera_transform = camera->actor->Get_Component<Transform>();

    float speed = 1.0f;

    int state_w = glfwGetKey(game->window.window_ptr, GLFW_KEY_W);
    if (state_w == GLFW_PRESS)
    {
        camera_transform->translation += camera_cmp->forward * speed * deltatime;
    }

    int state_s = glfwGetKey(game->window.window_ptr, GLFW_KEY_S);
    if (state_s == GLFW_PRESS)
    {
        camera_transform->translation += camera_cmp->forward * -speed * deltatime;
    }

    int state_a = glfwGetKey(game->window.window_ptr, GLFW_KEY_A);
    if (state_a == GLFW_PRESS)
    {
        camera_transform->translation += camera_cmp->right * -speed * deltatime;
    }

    int state_d = glfwGetKey(game->window.window_ptr, GLFW_KEY_D);
    if (state_d == GLFW_PRESS)
    {
        camera_transform->translation += camera_cmp->right * speed * deltatime;
    }

    int state_up = glfwGetKey(game->window.window_ptr, GLFW_KEY_UP);
    if (state_up == GLFW_PRESS)
    {
        camera_transform->translation.y += speed * deltatime;
    }

    int state_down = glfwGetKey(game->window.window_ptr, GLFW_KEY_DOWN);
    if (state_down == GLFW_PRESS)
    {
        camera_transform->translation.y += -speed * deltatime;
    }
}

void SVXGI::Draw()
{
    Voxelize();
}

void SVXGI::Clear()
{
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

    editor.ShutDown();
}

void SVXGI::Voxelize()
{
    glm::vec4 clear_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    GLint previousBoundTextureID;
    glGetIntegerv(GL_TEXTURE_BINDING_3D, &previousBoundTextureID);
    glBindTexture(GL_TEXTURE_3D, voxel_grid);
    glClearTexImage(voxel_grid, 0, GL_RGBA, GL_FLOAT, (void*)&clear_color);
    glBindTexture(GL_TEXTURE_3D, previousBoundTextureID);
    glViewport(0, 0, width, height);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    game->shaders_table["Voxelization"].Bind(); 

    //glm::mat4 ortho = glm::ortho(-0.5f * width, 0.5f * width, -0.5f * height, 0.5f * height, -0.5f * width, 0.5f * width);
    glm::mat4 ortho = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 10.0f);
    
    glm::mat4 viewX = glm::lookAt(glm::vec3(1.0, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 viewY = glm::lookAt(glm::vec3(0.0f, 1.0, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    glm::mat4 viewZ = glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    game->shaders_table["Voxelization"].Set_Matrix4_Uniform("orthoX", ortho * viewX);
    game->shaders_table["Voxelization"].Set_Matrix4_Uniform("orthoY", ortho * viewY);
    game->shaders_table["Voxelization"].Set_Matrix4_Uniform("orthoZ", ortho * viewZ);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, voxel_grid);
    game->shaders_table["Voxelization"].Set_Int_Uniform("voxel_grid", 0);
    glBindImageTexture(0, voxel_grid, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);

    for (auto& node : nodes) 
    {
        if (node.tag == "quad" || node.tag == "cube") continue;

        game->shaders_table["Voxelization"].Set_Matrix4_Uniform("model", node.actor->Get_Component<Transform>()->model_matrix);

        Model* model = node.actor->Get_Component<Model>(); 

        if (!model) continue;

        for (auto& mesh : model->meshes)
        {
            PBRMaterial& material = mesh.render_data.pbr_material; 

            material.diffuse_map->Bind(0);
            game->shaders_table["Voxelization"].Set_Int_Uniform("diffuse", 0);

            mesh.vao.Bind();
            glDrawElements(GL_TRIANGLES, mesh.render_data.indices.size(), GL_UNSIGNED_INT, (void*)mesh.render_data.indices[0]);
            mesh.vao.Un_Bind();
        }
    }

    glGenerateMipmap(GL_TEXTURE_3D);
    game->shaders_table["Voxelization"].Un_Bind();
}

void SVXGI::Visualize_Voxels()
{

}

void SVXGI::Cone_Tracing()
{
}
