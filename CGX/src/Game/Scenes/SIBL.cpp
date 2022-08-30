#include "SIBL.h"

#include <Game.h>
#include <iostream>
#include <glew.h>
#include <glfw3.h>
#include <random>

#include "../GLM/gtx/compatibility.hpp"

void Random_Points_In_Hemisphere(std::vector<glm::vec3>& samples, uint32_t N)
{
    const float PI = 3.14159265359f;

    std::random_device dev_x;
    std::mt19937 gen_x(dev_x());
    std::uniform_real_distribution<> dis_x(0.0f, 1.0f);

    std::random_device dev_y;
    std::mt19937 gen_y(dev_y());
    std::uniform_real_distribution<> dis_y(0.0f, 1.0f);

    for (uint32_t i = 0; i < N; ++i)
    {
        float cosTheta = 1.0f - dis_x(gen_x);
        float sinTheta = sqrt(1.0f - cosTheta * cosTheta);
        float phi = dis_y(gen_y) * 2.0f * PI;

        glm::vec3 sample = glm::vec3(cos(phi) * sinTheta, cosTheta, sin(phi) * sinTheta);

        samples.push_back(sample * 0.1f);
    }
}

SIBL::SIBL(Game* _game):
    Scene(_game)
{
}

bool SIBL::Init()
{
    nodes.reserve(MAX_NODES);
    nodes_ptrs.reserve(MAX_NODES);
    actors.reserve(MAX_ACTORS);
    textures.reserve(MAX_TEXTURES);
    transforms.Init();
    models.Init();    
    pointlights.Init(); 
    hdrskyboxes.Init(); 
    cameras.Init(); 

    editor.Initialize(game);

    TextureParams params;

    params.width = game->window.width;
    params.height = game->window.height;
    params.internl_format = GL_DEPTH_COMPONENT;
    params.format = GL_DEPTH_COMPONENT;
    params.components_type = GL_FLOAT;
    params.mag_filter = GL_NEAREST;
    params.min_filter = GL_NEAREST;
    params.wrap_s = GL_REPEAT;
    params.wrap_t = GL_REPEAT;

    Gbuffer.Generate_Framebuffer();
    Gbuffer.Generate_Texture_Attachment(Attachment::Depth, params);
    
    // Positions
    params.internl_format = GL_RGB16F;
    params.format = GL_RGB;
    params.components_type = GL_FLOAT;
    Gbuffer.Generate_Texture_Attachment(Attachment::Color, params);

    // Diffuse
    params.internl_format = GL_RGB;
    params.components_type = GL_UNSIGNED_BYTE; 
    Gbuffer.Generate_Texture_Attachment(Attachment::Color, params);

    // Normals
    params.internl_format = GL_RGB16F;
    params.components_type = GL_FLOAT;
    Gbuffer.Generate_Texture_Attachment(Attachment::Color, params);

    // Metalness
    params.internl_format = GL_R16F;
    params.format = GL_RED;
    params.components_type = GL_FLOAT;
    Gbuffer.Generate_Texture_Attachment(Attachment::Color, params);

    // Roughness
    Gbuffer.Generate_Texture_Attachment(Attachment::Color, params);

    // AO
    Gbuffer.Generate_Texture_Attachment(Attachment::Color, params);

    if (!Gbuffer.Configure(0, 5))
    {
        LOG_ERR("failed to configure Gbuffer\n");
        return false;
    }
    
    scene_refered_render_target.Generate_Framebuffer();

    params.internl_format = GL_RGBA16F;
    params.format = GL_RGBA;
    params.components_type = GL_FLOAT; 
    scene_refered_render_target.Generate_Texture_Attachment(Attachment::Color, params);

    if (!scene_refered_render_target.Configure(-1, 0))
    {
        LOG_ERR("failed to configure scene refered render target\n");
        return false;
    }

    display_refered_render_target.Generate_Framebuffer();

    params.internl_format = GL_RGBA;
    params.format = GL_RGBA;
    params.components_type = GL_UNSIGNED_BYTE;
    display_refered_render_target.Generate_Texture_Attachment(Attachment::Color, params);

    if (!display_refered_render_target.Configure(-1, 0))
    {
        LOG_ERR("failed to configure display refered render target\n");
        return false;
    }
    
    SSAO.Generate_Framebuffer();

    params.internl_format = GL_R16F;
    params.format = GL_RED;
    params.components_type = GL_FLOAT;
    SSAO.Generate_Texture_Attachment(Attachment::Color, params);

    if (!SSAO.Configure(-1, 0))
    {
        LOG_ERR("failed to initialize SSAO render target\n");
        return false;
    }

    blured_SSAO.Generate_Framebuffer();
    blured_SSAO.Generate_Texture_Attachment(Attachment::Color, params);

    if (!blured_SSAO.Configure(-1, 0))
    {
        LOG_ERR("failed to initialize SSAO render target\n");
        return false;
    }

    params.width = 512;
    params.height = 512;
    params.internl_format = GL_RGB16F;
    params.format = GL_RGB;
    params.components_type = GL_FLOAT;
    params.mag_filter = GL_LINEAR;
    params.min_filter = GL_LINEAR_MIPMAP_LINEAR;
    params.wrap_s = GL_CLAMP_TO_EDGE;
    params.wrap_t = GL_CLAMP_TO_EDGE;
    params.wrap_r = GL_CLAMP_TO_EDGE;
    params.type = TextureType::HDR;
    params.generate_mips = true;

    cube_map.Generate_Framebuffer();
    cube_map.Generate_Texture_Attachment(Attachment::ColorCube, params);

    if (!cube_map.Configure(-1, 0))
    {
        LOG_ERR("failed to initialize cube_map render target\n");
        return false;
    }

    params.width = 64;
    params.height = 64;
    params.min_filter = GL_LINEAR;
    params.generate_mips = false;

    irradiance_map.Generate_Framebuffer();
    irradiance_map.Generate_Texture_Attachment(Attachment::ColorCube, params);

    std::cout << glGetError() << '\n';

    if (!irradiance_map.Configure(-1, 0))
    {
        LOG_ERR("failed to initialize irradiance_map render target\n");
        return false;
    }

    params.width = 128; 
    params.height = 128; 
    params.mag_filter = GL_LINEAR;
    params.min_filter = GL_LINEAR_MIPMAP_LINEAR;
    params.generate_mips = true;

    pre_filtered_env_map.Generate_Framebuffer();
    pre_filtered_env_map.Generate_Texture_Attachment(Attachment::ColorCube, params);

    if (!pre_filtered_env_map.Configure(-1, 0))
    {
        LOG_ERR("failed to initialize pre_filtered_env_map render target\n");
        return false;
    }

    params.internl_format = GL_RG16F;
    params.format = GL_RG; 
    params.components_type = GL_FLOAT; 
    params.mag_filter = GL_LINEAR; 
    params.min_filter = GL_LINEAR;
    params.generate_mips = false; 

    integral_LUT.Generate_Framebuffer(); 
    integral_LUT.Generate_Texture_Attachment(Attachment::Color, params); 

    if (!integral_LUT.Configure(-1, 0)) 
    {
        LOG_ERR("failed to initialize integral_LUT render target\n");
        return false;
    }

    return true;
}


void SIBL::Load_Data()
{
    SceneNode* armor = game->node_manager.Create_Scene_Node(this, "armor");

    Model* sponza_model = models.Add_Component(armor->actor);
    game->loader.Load(armor, "Assets/armor/scene.gltf");

    Transform* sponza_transform = transforms.Add_Component(armor->actor);
    sponza_transform->scale = 0.01f;

    //////////////////////////////////////////////////////////////////////////////
    SceneNode* cube = game->node_manager.Create_Scene_Node(this, "cube");
    Transform* cube_trasnform = transforms.Add_Component(cube->actor);
    models.Add_Component(cube->actor);
    game->loader.Load(cube, "Assets/cube/cube.obj");
    HDRSkyBox* sky_box = hdrskyboxes.Add_Component(cube->actor);

    cube_trasnform->translation.z = 0.0f;
    sky_box->Initialize("Assets/HDR Env/preller_drive/preller_drive_4k.hdr");
    
    ///////////////////////////////////////////////////////////////////////////////
    SceneNode* camera = game->node_manager.Create_Scene_Node(this, "camera"); 
    Transform* camera_transform = transforms.Add_Component(camera->actor);
    scene_camera = cameras.Add_Component(camera->actor);
    camera_transform->translation.z = -0.2f;
    scene_camera->Initialize(); 

    //////////////////////////////////////////////////////////////////////////////

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
    
    //////////////////////////////////////////////////////////////////////////////
    
    editor.Setup(this); 

    //////////////////////////////////////////////////////////////////////////////

    Pre_SSAO_Data();
}

void SIBL::Update(float deltatime)
{
    Update_Components(deltatime);
    Update_Shaders(); 
    Scene_Logic(deltatime);
    editor.Update(deltatime);
}

void SIBL::Update_Components(float deltatime)
{
    for (auto& transform : transforms.components)
    {
        transform.Update(deltatime);
    }

    for (auto& camera : cameras.components)
    {
        camera.Update(deltatime);
    }

    for (auto& pointlight : pointlights.components)
    {
        pointlight.Update(deltatime);
    }
}

void SIBL::Update_Shaders()
{
    game->shaders_table["pbrShading"].Re_Compile();
    game->shaders_table["DrawCubeMap"].Re_Compile();
    game->shaders_table["EnvToCube"].Re_Compile();
}


void SIBL::Scene_Logic(float deltatime)
{
    Transform* camera_transform = scene_camera->owner->Get_Component<Transform>();

    float speed = 1.0f; 

    int state_w = glfwGetKey(game->window.window_ptr, GLFW_KEY_W);
    if (state_w == GLFW_PRESS)
    {
       camera_transform->translation += scene_camera->forward * speed * deltatime;
    }

    int state_s = glfwGetKey(game->window.window_ptr, GLFW_KEY_S);
    if (state_s == GLFW_PRESS)
    {
        camera_transform->translation += scene_camera->forward * -speed * deltatime;
    }
    
    int state_a = glfwGetKey(game->window.window_ptr, GLFW_KEY_A);
    if (state_a == GLFW_PRESS)
    {
        camera_transform->translation += scene_camera->right * -speed * deltatime;
    }

    int state_d = glfwGetKey(game->window.window_ptr, GLFW_KEY_D);
    if (state_d == GLFW_PRESS)
    {
       camera_transform->translation += scene_camera->right * speed * deltatime;
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

void SIBL::Draw()
{
    static bool irradiance_map = false; 
    static bool pre_filtered_env_map = false; 
    static bool integral_LUT = false; 

    Generate_GBuffer(); 
    Calculate_SSAO();
    Blur_SSAO();
    HDREnv_To_CubeMap();

    if (!irradiance_map)
    {
        Generate_Irradiance_Map();
        irradiance_map = true; 
    }

    if (!pre_filtered_env_map) 
    {
        Generate_Pre_Filtered_Env_Map();
        pre_filtered_env_map = true;
    }

    if (!integral_LUT) 
    {
        Generate_Integral_LUT(); 
        integral_LUT = false; 
    }

    Draw_Scene_Refered_Image();
    Draw_Display_Refered_Image();
}

void SIBL::Clear()
{
    Gbuffer.Clear();
    scene_refered_render_target.Clear();
    display_refered_render_target.Clear();

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

void SIBL::Generate_GBuffer()
{
    glViewport(0, 0, game->window.width, game->window.height);
    Gbuffer.Bind();
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto& node : nodes)
    {
        if (node.tag == "quad" || node.tag == "sphere" || node.tag == "cube") continue;

        Transform* transform = node.actor->Get_Component<Transform>();
        Model* model = node.actor->Get_Component<Model>();

        if (!model || !transform) continue;

        game->shaders_table["gbuffer"].Bind();

        for (auto& mesh : model->meshes)
        {
            game->shaders_table["gbuffer"].Set_Matrix4_Uniform("model", transform->model_matrix);
            game->shaders_table["gbuffer"].Set_Matrix4_Uniform("view", scene_camera->view);
            game->shaders_table["gbuffer"].Set_Matrix4_Uniform("proj", game->window.prespective_proj);

            mesh.render_data.pbr_material.diffuse_map->Bind(0);
            game->shaders_table["gbuffer"].Set_Int_Uniform("pbr_material.diffuse_map", 0);

            mesh.render_data.pbr_material.normal_map->Bind(1);
            game->shaders_table["gbuffer"].Set_Int_Uniform("pbr_material.normal_map", 1);

            mesh.render_data.pbr_material.metallicRoughness_map->Bind(2);
            game->shaders_table["gbuffer"].Set_Int_Uniform("pbr_material.metallicRoughness_map", 2);

            mesh.vao.Bind();

            glDrawElements(GL_TRIANGLES, mesh.render_data.indices.size(), GL_UNSIGNED_INT, (void*)mesh.render_data.indices[0]);

            mesh.vao.Un_Bind();
        }
    }

    game->shaders_table["gbuffer"].Un_Bind();
    Gbuffer.Unbind();
}

void SIBL::Draw_Scene_Refered_Image()
{
    glViewport(0, 0, game->window.width, game->window.height);
    scene_refered_render_target.Bind();
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);

    Model* quad_model = nodes_map["quad"]->actor->Get_Component<Model>();

    game->shaders_table["pbrShading"].Bind();

    Gbuffer.color_buffers[0].Bind(0);
    game->shaders_table["pbrShading"].Set_Int_Uniform("position_map", 0);

    Gbuffer.color_buffers[1].Bind(1);
    game->shaders_table["pbrShading"].Set_Int_Uniform("diffuse_map", 1);

    Gbuffer.color_buffers[2].Bind(2);    
    game->shaders_table["pbrShading"].Set_Int_Uniform("normal_map", 2);

    Gbuffer.color_buffers[3].Bind(3);
    game->shaders_table["pbrShading"].Set_Int_Uniform("roughness_map", 3);

    Gbuffer.color_buffers[4].Bind(4);
    game->shaders_table["pbrShading"].Set_Int_Uniform("metalness_map", 4);

    blured_SSAO.color_buffers[0].Bind(5);
    game->shaders_table["pbrShading"].Set_Int_Uniform("ao_map", 5);

    irradiance_map.color_buffers[0].Bind_Cube_Map(6);
    game->shaders_table["pbrShading"].Set_Int_Uniform("irradiance_map", 6);

    pre_filtered_env_map.color_buffers[0].Bind_Cube_Map(7);
    game->shaders_table["pbrShading"].Set_Int_Uniform("pre_filtered_cnv_map", 7);

    integral_LUT.color_buffers[0].Bind(8);
    game->shaders_table["pbrShading"].Set_Int_Uniform("integral_LUT", 8);

    quad_model->meshes[0].vao.Bind();

    glDrawElements(GL_TRIANGLES, quad_model->meshes.back().render_data.indices.size(),
        GL_UNSIGNED_INT, (void*)quad_model->meshes.back().render_data.indices[0]);

    quad_model->meshes[0].vao.Un_Bind();

    game->shaders_table["pbrShading"].Un_Bind();

    scene_refered_render_target.Unbind();
}

void SIBL::Draw_Display_Refered_Image()
{
    display_refered_render_target.Bind();
    glViewport(0, 0, game->window.width, game->window.height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    game->shaders_table["toneMapping"].Bind();

    Model* model = nodes_map["quad"]->actor->Get_Component<Model>();

    scene_refered_render_target.color_buffers[0].Bind(0);
    game->shaders_table["toneMapping"].Set_Int_Uniform("scene_refered_image", 0);

    model->meshes[0].vao.Bind();

    glDrawElements(GL_TRIANGLES, model->meshes.back().render_data.indices.size(),
        GL_UNSIGNED_INT, (void*)model->meshes.back().render_data.indices[0]);

    model->meshes[0].vao.Un_Bind();

    game->shaders_table["toneMapping"].Un_Bind();

    Draw_Cube_Map();

    display_refered_render_target.Unbind();
}

void SIBL::Pre_SSAO_Data()
{
    const int N = 256;
    samples.reserve(N);

    Random_Points_In_Hemisphere(samples, N);

    for (int i = 0; i < N; ++i)
    {
        float scale = std::max((float)i / (float)N, 0.0001f);
        samples[i] *= scale;
    }
}

void SIBL::Calculate_SSAO()
{
    SSAO.Bind();
    glViewport(0, 0, game->window.width, game->window.height);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    game->shaders_table["SSAO"].Bind();

    Gbuffer.color_buffers[0].Bind(0);
    game->shaders_table["SSAO"].Set_Int_Uniform("position_map", 0);

    Gbuffer.color_buffers[2].Bind(1);
    game->shaders_table["SSAO"].Set_Int_Uniform("normal_map", 1);

    for (int i = 0; i < samples.size(); ++i)
    {
        game->shaders_table["SSAO"].Set_Vec3_Uniform("samples[" + std::to_string(i) + "]", samples[i]);
    }

    game->shaders_table["SSAO"].Set_Matrix4_Uniform("projection", scene_camera->prespective_proj);
    game->shaders_table["SSAO"].Set_Float_Uniform("radius", 1.0f);

    Model* model = nodes_map["quad"]->actor->Get_Component<Model>();

    model->meshes[0].vao.Bind();

    glDrawElements(GL_TRIANGLES, model->meshes.back().render_data.indices.size(),
        GL_UNSIGNED_INT, (void*)model->meshes.back().render_data.indices[0]);

    model->meshes[0].vao.Un_Bind();

    game->shaders_table["SSAO"].Un_Bind();

    SSAO.Unbind();
}

void SIBL::Blur_SSAO()
{
    blured_SSAO.Bind();
    glViewport(0, 0, game->window.width, game->window.height);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    game->shaders_table["blurSSAO"].Bind();

    SSAO.color_buffers[0].Bind(0);
    game->shaders_table["blurSSAO"].Set_Int_Uniform("ssao", 0);

    Model* model = nodes_map["quad"]->actor->Get_Component<Model>();

    model->meshes[0].vao.Bind();

    glDrawElements(GL_TRIANGLES, model->meshes.back().render_data.indices.size(),
        GL_UNSIGNED_INT, (void*)model->meshes.back().render_data.indices[0]);

    model->meshes[0].vao.Un_Bind();

    game->shaders_table["blurSSAO"].Un_Bind();

    blured_SSAO.Unbind();
}

void SIBL::HDREnv_To_CubeMap()
{
    cube_map.Bind();
    glViewport(0, 0, cube_map.color_buffers[0].params.width, cube_map.color_buffers[0].params.height);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    game->shaders_table["EnvToCube"].Bind();

    Transform* cube_transform = nodes_map["cube"]->actor->Get_Component<Transform>();
    Model* cube_model = nodes_map["cube"]->actor->Get_Component<Model>();
    HDRSkyBox* sky_box = nodes_map["cube"]->actor->Get_Component<HDRSkyBox>();

    glm::mat4 proj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 100.0f); 

    std::vector<glm::mat4> views;
    views.reserve(6);

    glm::mat4 pos_x = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    glm::mat4 neg_x = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    glm::mat4 pos_y = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 neg_y = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    glm::mat4 pos_z = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    glm::mat4 neg_z = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));

    views.push_back(std::move(pos_x));
    views.push_back(std::move(neg_x));
    views.push_back(std::move(pos_y));
    views.push_back(std::move(neg_y));
    views.push_back(std::move(pos_z));
    views.push_back(std::move(neg_z));

    game->shaders_table["EnvToCube"].Set_Matrix4_Uniform("model", cube_transform->model_matrix);
    game->shaders_table["EnvToCube"].Set_Matrix4_Uniform("proj", proj);

    for (int i = 0; i < 6; ++i)
    {
        game->shaders_table["EnvToCube"].Set_Matrix4_Uniform("views[" + std::to_string(i) + "]", views[i]);
    }

    sky_box->hdr_env_map->Bind(0);
    game->shaders_table["EnvToCube"].Set_Int_Uniform("env_map", 0);

    cube_model->meshes[0].vao.Bind();

    glDrawElements(GL_TRIANGLES, cube_model->meshes.back().render_data.indices.size(),
        GL_UNSIGNED_INT, (void*)cube_model->meshes.back().render_data.indices[0]);

    cube_model->meshes[0].vao.Un_Bind();
    
    glGenerateTextureMipmap(cube_map.color_buffers[0].id);

    game->shaders_table["EnvToCube"].Un_Bind();

    cube_map.Unbind();
}

void SIBL::Draw_Cube_Map()
{
    game->shaders_table["DrawCubeMap"].Bind();

    Transform* cube_transform = nodes_map["cube"]->actor->Get_Component<Transform>();
    Model* cube_model = nodes_map["cube"]->actor->Get_Component<Model>();
    HDRSkyBox* sky_box = nodes_map["cube"]->actor->Get_Component<HDRSkyBox>();

    game->shaders_table["DrawCubeMap"].Set_Matrix4_Uniform("view_rotation", scene_camera->view_rotation);
    game->shaders_table["DrawCubeMap"].Set_Matrix4_Uniform("proj", scene_camera->prespective_proj);
    game->shaders_table["DrawCubeMap"].Set_Vec2_Uniform("window_dim", glm::vec2((float)game->window.width, (float)game->window.height));

    cube_map.color_buffers[0].Bind_Cube_Map(0);
    game->shaders_table["DrawCubeMap"].Set_Int_Uniform("cube_map", 0);

    Gbuffer.depth_buffers[0].Bind(1);
    game->shaders_table["DrawCubeMap"].Set_Int_Uniform("depth", 1);

    cube_model->meshes[0].vao.Bind();

    glDrawElements(GL_TRIANGLES, cube_model->meshes.back().render_data.indices.size(),
        GL_UNSIGNED_INT, (void*)cube_model->meshes.back().render_data.indices[0]);

    cube_model->meshes[0].vao.Un_Bind();

    game->shaders_table["DrawCubeMap"].Un_Bind();
}

void SIBL::Generate_Irradiance_Map()
{
    glViewport(0, 0, irradiance_map.color_buffers[0].params.width, irradiance_map.color_buffers[0].params.height); 
    irradiance_map.Bind();
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);

    Transform* cube_transform = nodes_map["cube"]->actor->Get_Component<Transform>();
    Model* cube_model = nodes_map["cube"]->actor->Get_Component<Model>();

    glm::mat4 proj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 100.0f);

    std::vector<glm::mat4> views;
    views.reserve(6);

    glm::mat4 pos_x = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    glm::mat4 neg_x = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    glm::mat4 pos_y = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 neg_y = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    glm::mat4 pos_z = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    glm::mat4 neg_z = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));

    views.push_back(std::move(pos_x));
    views.push_back(std::move(neg_x));
    views.push_back(std::move(pos_y));
    views.push_back(std::move(neg_y));
    views.push_back(std::move(pos_z));
    views.push_back(std::move(neg_z));

    game->shaders_table["GenerateIrradianceMap"].Bind(); 

    for (int i = 0; i < 6; ++i) 
    {
        game->shaders_table["GenerateIrradianceMap"].Set_Matrix4_Uniform("views[" + std::to_string(i) + "]", views[i]);
    }

    game->shaders_table["GenerateIrradianceMap"].Set_Matrix4_Uniform("proj", proj);

    cube_map.color_buffers[0].Bind_Cube_Map(0);
    game->shaders_table["GenerateIrradianceMap"].Set_Int_Uniform("cube_map", 0);

    cube_model->meshes[0].vao.Bind();

    glDrawElements(GL_TRIANGLES, cube_model->meshes.back().render_data.indices.size(),
        GL_UNSIGNED_INT, (void*)cube_model->meshes.back().render_data.indices[0]);

    cube_model->meshes[0].vao.Un_Bind();

    game->shaders_table["GenerateIrradianceMap"].Un_Bind(); 
    irradiance_map.Unbind();
}

void SIBL::Generate_Pre_Filtered_Env_Map()
{
    glViewport(0, 0, pre_filtered_env_map.color_buffers[0].params.width, pre_filtered_env_map.color_buffers[0].params.height);
    pre_filtered_env_map.Bind();
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);

    Transform* cube_transform = nodes_map["cube"]->actor->Get_Component<Transform>();
    Model* cube_model = nodes_map["cube"]->actor->Get_Component<Model>();

    glm::mat4 proj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 100.0f);

    std::vector<glm::mat4> views;
    views.reserve(6);

    glm::mat4 pos_x = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    glm::mat4 neg_x = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    glm::mat4 pos_y = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 neg_y = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    glm::mat4 pos_z = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    glm::mat4 neg_z = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));

    views.push_back(std::move(pos_x));
    views.push_back(std::move(neg_x));
    views.push_back(std::move(pos_y));
    views.push_back(std::move(neg_y));
    views.push_back(std::move(pos_z));
    views.push_back(std::move(neg_z));

    game->shaders_table["GeneratePreFilteredEnvMap"].Bind();


    for (int i = 0; i < 6; ++i)
    {
        game->shaders_table["GeneratePreFilteredEnvMap"].Set_Matrix4_Uniform("views[" + std::to_string(i) + "]", views[i]);
    }

    game->shaders_table["GeneratePreFilteredEnvMap"].Set_Matrix4_Uniform("proj", proj);

    cube_map.color_buffers[0].Bind_Cube_Map(0);
    game->shaders_table["GeneratePreFilteredEnvMap"].Set_Int_Uniform("cube_map", 0);

    const int MAX_MIP_LEVEL = 5; 
    for (int i = 0; i < MAX_MIP_LEVEL; ++i)
    {
        unsigned int mipWidth = pre_filtered_env_map.color_buffers[0].params.width * std::pow(0.5, i);
        unsigned int mipHeight = pre_filtered_env_map.color_buffers[0].params.height * std::pow(0.5, i);
        
        glViewport(0, 0, mipWidth, mipHeight);
        
        float roughness = (float)i / (float)(MAX_MIP_LEVEL - 1);
        
        game->shaders_table["GeneratePreFilteredEnvMap"].Set_Float_Uniform("roughness", roughness);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, pre_filtered_env_map.color_buffers[0].id, i);
        
        cube_model->meshes[0].vao.Bind();

        glDrawElements(GL_TRIANGLES, cube_model->meshes.back().render_data.indices.size(),
            GL_UNSIGNED_INT, (void*)cube_model->meshes.back().render_data.indices[0]);

        cube_model->meshes[0].vao.Un_Bind();
    }


    game->shaders_table["GeneratePreFilteredEnvMap"].Un_Bind();

    pre_filtered_env_map.Unbind();
}

void SIBL::Generate_Integral_LUT()
{
    glViewport(0, 0, integral_LUT.color_buffers[0].params.width, integral_LUT.color_buffers[0].params.height);
    integral_LUT.Bind();
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);

    Model* quad_model = nodes_map["quad"]->actor->Get_Component<Model>();
    
    game->shaders_table["IntegralLUT"].Bind(); 

    quad_model->meshes[0].vao.Bind();

    glDrawElements(GL_TRIANGLES, quad_model->meshes.back().render_data.indices.size(),
        GL_UNSIGNED_INT, (void*)quad_model->meshes.back().render_data.indices[0]);

    quad_model->meshes[0].vao.Un_Bind();

    game->shaders_table["IntegralLUT"].Un_Bind(); 

    integral_LUT.Unbind();
}