#include "SPBR.h"
#include <Game.h>
#include <glew.h>
#include <glfw3.h>
#include <iostream>
#include <random>
#include <algorithm>

#include "../GLM/gtx/compatibility.hpp"

SPBR::SPBR(Game* _game):
	Scene(_game)
{
}

bool SPBR::Init()
{
	nodes.reserve(MAX_NODES);
    nodes_ptrs.reserve(MAX_NODES);
	actors.reserve(MAX_ACTORS);
	textures.reserve(MAX_TEXTURES);
    transforms.Init(); 
    models.Init();
    pointlights.Init();
    quadlights.Init(); 

    editor.Initialize(game);


    TextureParams params;

    params.width = game->window.width;
    params.height = game->window.height;
    params.internl_format = GL_DEPTH_COMPONENT;
    params.format = GL_DEPTH_COMPONENT;
    params.components_type = GL_FLOAT;
    params.mag_filter = GL_LINEAR;
    params.min_filter = GL_LINEAR;
    params.wrap_s = GL_REPEAT;
    params.wrap_t = GL_REPEAT;

    Gbuffer.Generate_Framebuffer();
    Gbuffer.Generate_Texture_Attachment(Attachment::Depth, params);

    // Positions
    params.internl_format = GL_RGB16F;
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
    params.internl_format = GL_RED;
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

    return true;
}

void SPBR::Load_Data()
{
    ///////////////////////////////////////////////////////////////////
    SceneNode* sponza = game->node_manager.Create_Scene_Node(this, "sponza");
    
    Model* sponza_model = models.Add_Component(sponza->actor);
    game->loader.Load(sponza, "Assets/sponza/Sponza.gltf");

    Transform* sponza_transform = transforms.Add_Component(sponza->actor);
    sponza_transform->scale = 0.01f;
    sponza_transform->translation.y = -1.0f;
    sponza_transform->translation.z = -5.0f;

    sponza_transform->angle_y = glm::radians(-90.0f);

    //////////////////////////////////////////////////////////////////

    SceneNode* light_sources = game->node_manager.Create_Scene_Node(this, "light_sources_container"); 
    const int N = 3;

    float offset_z = -7.0f;
    float scale = 0.004f; 
    glm::vec3 translation = glm::vec3(0.0f, 2.0f, -1.1f); 

    for (int i = 0; i < N; ++i) 
    {
        SceneNode* quad_light = game->node_manager.Create_Scene_Node(this, "quad_light_" + std::to_string(i));

        QuadLight* quad_light_source = quadlights.Add_Component(quad_light->actor);

        Transform* transform = transforms.Add_Component(quad_light->actor);

        transform->scale = scale;
        transform->translation = translation + glm::vec3(0.0f, 0.0f, offset_z * i);
        transform->angle_x = glm::radians(-90.0f);

        Model* quad_light_model = models.Add_Component(quad_light->actor);
        game->loader.Load(quad_light, "Assets/quad/quad.obj");

        light_sources->Add_Child(quad_light);
    }

    light_sources->children[0]->actor->Get_Component<QuadLight>()->intensity = glm::vec3(80.0f, 10.0f, 80.0f);
    light_sources->children[1]->actor->Get_Component<QuadLight>()->intensity = glm::vec3(30.0f, 70.0f, 80.0f);
    light_sources->children[2]->actor->Get_Component<QuadLight>()->intensity = glm::vec3(100.0f, 25.0f, 10.0f);

    //////////////////////////////////////////////////////////////////

    SceneNode* quad = game->node_manager.Create_Scene_Node(this, "quad");
    Transform* quad_transform = transforms.Add_Component(quad->actor);
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

    //////////////////////////////////////////////////////////////////

    TextureParams params; 
    params.mag_filter = GL_LINEAR; 
    params.min_filter = GL_LINEAR;
    params.wrap_s = GL_REPEAT; 
    params.wrap_t = GL_REPEAT; 
    params.generate_mips = false; 

    ltc_1 = game->loader.Load_Texture(this, "Assets/ltc_1.png", params);
    ltc_2 = game->loader.Load_Texture(this, "Assets/ltc_2.png", params);

    //////////////////////////////////////////////////////////////////

    editor.Setup(this);

    //////////////////////////////////////////////////////////////////

    Pre_SSAO_Data();
}

void SPBR::Generate_GBuffer()
{
    glViewport(0, 0, game->window.width, game->window.height);
    Gbuffer.Bind();
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto& node : nodes)
    {
        if (node.tag == "quad" || node.tag == "sphere" || node.tag == "quad_light") continue;

        Transform* transform = node.actor->Get_Component<Transform>();
        Model* model = node.actor->Get_Component<Model>();

        if (!model || !transform) continue;

        game->shaders_table["gbuffer"].Bind();

        for (auto& mesh : model->meshes) 
        {
            game->shaders_table["gbuffer"].Set_Matrix4_Uniform("mvp", game->window.prespective_proj * transform->model_matrix);
            game->shaders_table["gbuffer"].Set_Matrix4_Uniform("model", transform->model_matrix);

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

void SPBR::Draw_Scene_Refered_Image()
{
    glViewport(0, 0, game->window.width, game->window.height);
    scene_refered_render_target.Bind();
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    std::vector<SceneNode*>& light_sources = nodes_map["light_sources_container"]->children;
    Model* quad_model = nodes_map["quad"]->actor->Get_Component<Model>();

    game->shaders_table["LTC_M"].Bind();

    Gbuffer.color_buffers[0].Bind(0); 
    game->shaders_table["LTC_M"].Set_Int_Uniform("position_map", 0); 

    Gbuffer.color_buffers[1].Bind(1); 
    game->shaders_table["LTC_M"].Set_Int_Uniform("diffuse_map", 1); 

    Gbuffer.color_buffers[2].Bind(2); 
    game->shaders_table["LTC_M"].Set_Int_Uniform("normal_map", 2); 

    Gbuffer.color_buffers[3].Bind(3); 
    game->shaders_table["LTC_M"].Set_Int_Uniform("roughness_map", 3); 

    Gbuffer.color_buffers[4].Bind(4); 
    game->shaders_table["LTC_M"].Set_Int_Uniform("metalness_map", 4); 

    blured_SSAO.color_buffers[0].Bind(5); 
    game->shaders_table["LTC_M"].Set_Int_Uniform("ao_map", 5); 

    ltc_1->Bind(6); 
    game->shaders_table["LTC_M"].Set_Int_Uniform("ltc_1", 6);

    ltc_2->Bind(7); 
    game->shaders_table["LTC_M"].Set_Int_Uniform("ltc_2", 7);
    
    
    for (int i = 0; i < light_sources.size(); ++i) 
    {
        QuadLight* light_source = light_sources[i]->actor->Get_Component<QuadLight>();

        for (int j = 0; j < 4; ++j)
        {
            // quadlight[i].vertices[j]
            game->shaders_table["LTC_M"].Set_Vec3_Uniform("quadlight[" + std::to_string(i) + "].vertices[" + std::to_string(j) + "]",
                glm::vec3(light_source->vertices[j].x, light_source->vertices[j].y, light_source->vertices[j].z));
        }

        game->shaders_table["LTC_M"].Set_Vec3_Uniform("quadlight[" + std::to_string(i) + "].intensity", light_source->intensity);

    }

    game->shaders_table["LTC_M"].Set_Int_Uniform("active_quad_lights", light_sources.size());

    quad_model->meshes[0].vao.Bind();

    glDrawElements(GL_TRIANGLES, quad_model->meshes.back().render_data.indices.size(),
        GL_UNSIGNED_INT, (void*)quad_model->meshes.back().render_data.indices[0]);

    quad_model->meshes[0].vao.Un_Bind();

    game->shaders_table["LTC_M"].Un_Bind();

    scene_refered_render_target.Unbind();
}

void SPBR::Calculate_Scene_Exposure()
{
}

void SPBR::Draw_Display_Refered_Image()
{
    display_refered_render_target.Bind(); 
    glViewport(0, 0, game->window.width, game->window.height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    game->shaders_table["toneMapping"].Bind();

    Model* model = nodes_map["quad"]->actor->Get_Component<Model>();

    game->pbr.scene_refered_render_target.color_buffers[0].Bind(0);
    game->shaders_table["toneMapping"].Set_Int_Uniform("scene_refered_image", 0);
        
    model->meshes[0].vao.Bind();

    glDrawElements(GL_TRIANGLES, model->meshes.back().render_data.indices.size(),
        GL_UNSIGNED_INT, (void*)model->meshes.back().render_data.indices[0]);

    model->meshes[0].vao.Un_Bind();

    game->shaders_table["toneMapping"].Un_Bind();

    // Render the light sources
    SceneNode* light_sources_container = nodes_map["light_sources_container"]; 
    
    game->shaders_table["lightSource"].Bind();

    for (int i = 0; i < light_sources_container->children.size(); ++i)
    {
        SceneNode* quad_light = light_sources_container->children[i];

        game->shaders_table["lightSource"].Set_Matrix4_Uniform("mvp",
            game->window.prespective_proj * quad_light->actor->Get_Component<Transform>()->model_matrix);

        game->shaders_table["lightSource"].Set_Vec3_Uniform("light_intensity", quad_light->actor->Get_Component<QuadLight>()->intensity);

        quad_light->actor->Get_Component<Model>()->meshes.back().vao.Bind();

        glDrawElements(GL_TRIANGLES, quad_light->actor->Get_Component<Model>()->meshes.back().render_data.indices.size(),
            GL_UNSIGNED_INT, (void*)quad_light->actor->Get_Component<Model>()->meshes.back().render_data.indices[0]);

        quad_light->actor->Get_Component<Model>()->meshes.back().vao.Un_Bind();
    }

    game->shaders_table["lightSource"].Un_Bind();

    display_refered_render_target.Unbind(); 
}

void SPBR::Pre_SSAO_Data()
{
    const int N = 256;
    samples.reserve(N);

    Random_Points_In_Hemisphere(samples, N);

    for (int i = 0; i < N; ++i)
    {
        float scale = (float)i / (float)N;
        scale = glm::lerp(0.1f, 1.0f, scale * scale);
        samples[i] *= scale;
    }
}

void SPBR::Calculate_SSAO()
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

    game->shaders_table["SSAO"].Set_Matrix4_Uniform("projection", game->window.prespective_proj);
    game->shaders_table["SSAO"].Set_Float_Uniform("radius", 0.5f);

    Model* model = nodes_map["quad"]->actor->Get_Component<Model>();

    model->meshes[0].vao.Bind();

    glDrawElements(GL_TRIANGLES, model->meshes.back().render_data.indices.size(),
        GL_UNSIGNED_INT, (void*)model->meshes.back().render_data.indices[0]);

    model->meshes[0].vao.Un_Bind();
    
    game->shaders_table["SSAO"].Un_Bind(); 
    
    SSAO.Unbind();
}

void SPBR::Blur_SSAO()
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

void SPBR::Update(float deltatime)
{
	Update_Components(deltatime);
    Update_Shaders(); 
    Scene_Logic(deltatime); 
    editor.Update(deltatime); 
}

void SPBR::Update_Components(float deltatime)
{
    for (auto& transform : transforms.components) 
    {
        transform.Update(deltatime); 
    }

    for (auto& pointlight : pointlights.components) 
    {
        pointlight.Update(deltatime); 
    }

    for (auto& quadlight : quadlights.components) 
    {
        quadlight.Update(deltatime);
    }
}

void SPBR::Update_Shaders()
{
    game->shaders_table["LTC_M"].Re_Compile();
}

void SPBR::Scene_Logic(float deltatime)
{
}

void SPBR::Draw()
{
    Generate_GBuffer();
    Calculate_SSAO();
    Blur_SSAO();
    Draw_Scene_Refered_Image();
    Calculate_Scene_Exposure();
    Draw_Display_Refered_Image(); 
}

void SPBR::Clear()
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


void Random_Points_In_Hemisphere(std::vector<glm::vec3>& samples, uint32_t N)
{
    const float PI = 3.14159265359f;

    std::random_device dev_x;
    std::mt19937 gen_x(dev_x());
    std::uniform_real_distribution<> dis_x(0.0f, 1.0f);

    std::random_device dev_y;
    std::mt19937 gen_y(dev_y());
    std::uniform_real_distribution<> dis_y(0.0f, 1.0f);

    auto lerp = [](float a, float b, float f)
    {
        return a + f * (b - a);
    };

    for (uint32_t i = 0; i < N; ++i)
    {
        float cosTheta = 1.0f - dis_x(gen_x);
        float sinTheta = sqrt(1.0f - cosTheta * cosTheta);
        float phi = dis_y(gen_y) * 2.0f * PI;

        glm::vec3 sample = glm::vec3(cos(phi) * sinTheta, cosTheta, sin(phi) * sinTheta);

        samples.push_back(sample * 0.1f);
    }
}