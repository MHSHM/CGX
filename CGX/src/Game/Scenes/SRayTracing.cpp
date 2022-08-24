#include "SRayTracing.h"
#include <Game.h>
#include <glew.h>
#include <glfw3.h>
#include <iostream>
#include <random>

SRayTracing::SRayTracing(Game* _game):
	Scene(_game)
{
}

bool SRayTracing::Init()
{
    nodes.reserve(MAX_NODES);
    nodes_ptrs.reserve(MAX_NODES);
    actors.reserve(MAX_ACTORS);
    textures.reserve(MAX_TEXTURES);
    transforms.Init();
    models.Init();
    cameras.Init(); 

    editor.Initialize(game);

    TextureParams params;
    
    params.width = game->window.width;
    params.height = game->window.height;
    params.internl_format = GL_RGBA16F;
    params.format = GL_RGBA;
    params.components_type = GL_FLOAT;
    params.mag_filter = GL_NEAREST;
    params.min_filter = GL_NEAREST;
    params.wrap_s = GL_REPEAT;
    params.wrap_t = GL_REPEAT;

    image2D.Generate(params);

    params.width = game->window.width;
    params.height = game->window.height;
    params.internl_format = GL_RGBA;
    params.format = GL_RGBA;
    params.components_type = GL_UNSIGNED_BYTE;
    params.mag_filter = GL_LINEAR;
    params.min_filter = GL_LINEAR;
    params.wrap_s = GL_REPEAT;
    params.wrap_t = GL_REPEAT;

    render_target.Generate_Framebuffer();
    render_target.Generate_Texture_Attachment(Attachment::Color, params);
    
    if (!render_target.Configure(-1, 0)) 
    {
        LOG_ERR("failed to initialize render target\n"); 
        return false; 
    }

	return true;
}

void SRayTracing::Initialize_Components()
{
    for (auto& camera : cameras.components) 
    {
        camera.Initialize(); 
    }
}

void SRayTracing::Load_Data()
{
    SceneNode* camera = game->node_manager.Create_Scene_Node(this, "camera");
    Transform* camera_transform = transforms.Add_Component(camera->actor);
    cameras.Add_Component(camera->actor);
    camera_transform->translation = glm::vec3(0.0f, 0.0f, 2.0f);

    SceneNode* cube = game->node_manager.Create_Scene_Node(this, "cube");
    Transform* cube_transform = transforms.Add_Component(cube->actor);
    models.Add_Component(cube->actor);
    cube_transform->translation = glm::vec3(0.0f, 0.0f, 0.0f);
    game->loader.Load(cube, "Assets/cube/cube.obj");

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

void SRayTracing::Update(float deltatime)
{
    Update_Components(deltatime);
    Update_Shaders();
    Scene_Logic(deltatime);
    editor.Update(deltatime);
}

void SRayTracing::Update_Components(float deltatime)
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

void SRayTracing::Update_Shaders()
{

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_G && action == GLFW_PRESS)
        generate = true;
}


void SRayTracing::Scene_Logic(float deltatime)
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

    glfwSetKeyCallback(game->window.window_ptr, key_callback);

}

void SRayTracing::Draw()
{
    static bool init_buffers = false; 
    if (!init_buffers) 
    {
        Initialize_Buffers(); 
        init_buffers = true; 
    }
    Generate_Image();
    Draw_Image_On_Screen();
}

void SRayTracing::Clear()
{
}

void SRayTracing::Initialize_Buffers()
{
    Model* cube = nodes_map["cube"]->actor->Get_Component<Model>();
    std::vector<glm::vec3>& positions  = cube->meshes[0].render_data.positions;
    std::vector<unsigned int>& indices = cube->meshes[0].render_data.indices;

    glGenBuffers(1, &positions_buffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, positions_buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec3) * positions.size(), positions.data(), GL_STATIC_READ);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, positions_buffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenBuffers(1, &indices_buffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, indices_buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_READ);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, indices_buffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void SRayTracing::Generate_Image()
{
    SceneNode* camera = nodes_map["camera"];
    Camera* camera_cmp = camera->actor->Get_Component<Camera>();
    Transform* camera_transform = camera->actor->Get_Component<Transform>();

    game->shaders_table["RayTracing"].Bind();

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, positions_buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, positions_buffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, indices_buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, indices_buffer);

    image2D.Bind_As_Image(0, GL_WRITE_ONLY, GL_RGBA16F);
    game->shaders_table["RayTracing"].Set_Int_Uniform("output_image", 0);

    game->shaders_table["RayTracing"].Set_Float_Uniform("FOV", glm::radians(camera_cmp->FOV));
    //game->shaders_table["RayTracing"].Set_Vec3_Uniform("cam_pos", camera_transform->translation);
    game->shaders_table["RayTracing"].Set_Matrix4_Uniform("camera_to_world", glm::inverse(camera_cmp->view));

    const int THREADS_X = 8;
    const int THREADS_Y = 4;

    const int GROUP_X = (game->window.width / THREADS_X) + 1;
    const int GROUP_Y = (game->window.height / THREADS_Y) + 1;

    glDispatchCompute(GROUP_X, GROUP_Y, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    game->shaders_table["RayTracing"].Un_Bind();
}

void SRayTracing::Draw_Image_On_Screen()
{

    glViewport(0, 0, game->window.width, game->window.height);
    render_target.Bind();
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);

    Model* quad_model = nodes_map["quad"]->actor->Get_Component<Model>();

    game->shaders_table["DrawQuad"].Bind();

    image2D.Bind(0);
    game->shaders_table["DrawQuad"].Set_Int_Uniform("image", 0);

    quad_model->meshes[0].vao.Bind();

    glDrawElements(GL_TRIANGLES, quad_model->meshes.back().render_data.indices.size(),
        GL_UNSIGNED_INT, (void*)quad_model->meshes.back().render_data.indices[0]);

    quad_model->meshes[0].vao.Un_Bind();

    game->shaders_table["DrawQuad"].Un_Bind();

    render_target.Unbind();
}
