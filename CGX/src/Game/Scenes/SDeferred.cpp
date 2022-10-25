#include "SDeferred.h"
#include <Game.h>
#include <iostream>
#include <glew.h>
#include <glfw3.h>


SDeferred::SDeferred(Game* _game):
	Scene(_game)
{

}

bool SDeferred::Init()
{
    nodes.reserve(MAX_NODES);
    actors.reserve(MAX_ACTORS);
    textures.reserve(MAX_TEXTURES);
    transforms.Init();
    models.Init();
    pointlights.Init(); 

    TextureParams params; 

    params.width  = game->window.width; 
    params.height = game->window.height; 
    params.internl_format = GL_DEPTH_COMPONENT; 
    params.format = GL_DEPTH_COMPONENT; 
    params.components_type = GL_FLOAT; 
    params.mag_filter = GL_LINEAR; 
    params.min_filter = GL_LINEAR;
    params.wrap_s = GL_REPEAT; 
    params.wrap_t = GL_REPEAT; 

    GBuffer.Generate_Framebuffer();
    GBuffer.Generate_Texture_Attachment(Attachment::Depth, params);

    // Positions
    params.internl_format = GL_RGBA16F;
    params.format = GL_RGBA;
    GBuffer.Generate_Texture_Attachment(Attachment::Color, params);

    // Normals
    GBuffer.Generate_Texture_Attachment(Attachment::Color, params);

    // Diffuse
    params.internl_format = GL_RGBA; 
    params.format = GL_RGBA; 
    params.components_type = GL_UNSIGNED_BYTE; 
    GBuffer.Generate_Texture_Attachment(Attachment::Color, params);

    // Specular
    GBuffer.Generate_Texture_Attachment(Attachment::Color, params);

    if (!GBuffer.Configure(0, 3))
    {
        LOG_ERR("failed to configure GBuffer\n"); 
        return false; 
    }

    params.internl_format = GL_DEPTH_COMPONENT;
    params.format = GL_DEPTH_COMPONENT;
    params.components_type = GL_FLOAT; 

    render_target.Generate_Framebuffer();
    render_target.Generate_Texture_Attachment(Attachment::Depth, params);

    params.internl_format = GL_RGB; 
    params.format = GL_RGB; 
    params.components_type = GL_UNSIGNED_BYTE; 
    render_target.Generate_Texture_Attachment(Attachment::Color, params);

    if (!render_target.Configure(0, 0)) 
    {
        LOG_ERR("failed to initialize render target\n"); 
        return false; 
    }

}

void SDeferred::Load_Data()
{
    SceneNode* sphere = game->node_manager.Create_Scene_Node(this, "sphere");
    Transform* sphere_transform = transforms.Add_Component(sphere->actor);
    sphere_transform->translation = glm::vec3(0.0f, 1.0f, -2.0f);
    sphere_transform->scale = 0.01f;
    PointLight* point_light_cmp = pointlights.Add_Component(sphere->actor);

    SceneNode* backpack = game->node_manager.Create_Scene_Node(this, "backpack");
    Model* model = models.Add_Component(backpack->actor);
    game->loader.Load(backpack, "Assets/backpack/backpack.obj");

    TextureParams params;
    params.mag_filter = GL_LINEAR;
    params.min_filter = GL_LINEAR_MIPMAP_LINEAR;
    params.wrap_s = GL_REPEAT;
    params.wrap_t = GL_REPEAT;
    params.generate_mips = true;

    model->meshes[0].render_data.material.diffuse_map = game->loader.Load_Texture(this, "Assets/backpack/diffuse.png", params, false);

    params.generate_mips = false; 
    model->meshes[0].render_data.material.normal_map = game->loader.Load_Texture(this, "Assets/backpack/normal.png", params, true);
    model->meshes[0].render_data.material.specular_map = game->loader.Load_Texture(this, "Assets/backpack/specular.png", params, false);

    Transform* transform = transforms.Add_Component(backpack->actor); 
    transform->translation.z = -4.0f;
    transform->scale = 1.0f; 

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
}


static void mouse_cursor_callback(GLFWwindow* window, double xpos, double ypos)
{
    Game* game = (Game*)glfwGetWindowUserPointer(window);

    Transform* transform = game->deferred.nodes_map["backpack"]->actor->Get_Component<Transform>();

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

void SDeferred::Update(float deltatime)
{
    Update_Components(deltatime);

    glfwSetCursorPosCallback(game->window.window_ptr, mouse_cursor_callback);

    SceneNode* light_source = nodes_map["sphere"];

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

    std::cout << transform->translation.x << ' ' << transform->translation.y << ' ' << transform->translation.z << '\n';
}

void SDeferred::Update_Components(float deltatime)
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

void SDeferred::Draw()
{
    Draw_First_Pass();
    Draw_Second_Pass(); 
}

void SDeferred::Draw_First_Pass()
{
    float aspect = (float)game->window.width / (float)game->window.height;
    glm::mat4 perspective_proj = glm::perspective(glm::radians(90.0f), aspect, 0.1f, 100.0f);

    glViewport(0, 0, game->window.width, game->window.height);
    GBuffer.Bind();
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto& model : models.components)
    {
        if (model.owner->scene_node->tag == "quad")
        {
            continue;
        }

        glm::mat4 mvp = perspective_proj * model.owner->Get_Component<Transform>()->model_matrix;

        game->shaders_table["deferred"].Bind();

        game->shaders_table["deferred"].Set_Matrix4_Uniform("mvp", mvp);
        game->shaders_table["deferred"].Set_Matrix4_Uniform("model", model.owner->Get_Component<Transform>()->model_matrix);

        model.meshes[0].render_data.material.normal_map->Bind(0);
        game->shaders_table["deferred"].Set_Int_Uniform("normal_map", 0);

        model.meshes[0].render_data.material.diffuse_map->Bind(1);
        game->shaders_table["deferred"].Set_Int_Uniform("diffuse_map", 1);

        model.meshes[0].render_data.material.specular_map->Bind(2);
        game->shaders_table["deferred"].Set_Int_Uniform("specular_map", 2);

        for (auto& mesh : model.meshes)
        {
            mesh.vao.Bind();

            glDrawElements(GL_TRIANGLES, mesh.render_data.indices.size(), GL_UNSIGNED_INT, (void*)mesh.render_data.indices[0]);

            mesh.vao.Un_Bind();
        }
    }

    game->shaders_table["deferred"].Un_Bind();
    GBuffer.Unbind();
}

void SDeferred::Draw_Second_Pass()
{
    glViewport(0, 0, game->window.width, game->window.height);
    render_target.Bind();
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);

    PointLight* pointlight = nodes_map["sphere"]->actor->Get_Component<PointLight>(); 
    Model* quad_model = nodes_map["quad"]->actor->Get_Component<Model>(); 

    game->shaders_table["deferred2"].Bind(); 

    GBuffer.color_buffers[0].Bind(0); 
    game->shaders_table["deferred2"].Set_Int_Uniform("position_map", 0); 

    GBuffer.color_buffers[1].Bind(1); 
    game->shaders_table["deferred2"].Set_Int_Uniform("normal_map", 1); 

    GBuffer.color_buffers[2].Bind(2); 
    game->shaders_table["deferred2"].Set_Int_Uniform("diffuse_map", 2); 

    GBuffer.color_buffers[3].Bind(3); 
    game->shaders_table["deferred2"].Set_Int_Uniform("specular_map", 3); 

    game->shaders_table["deferred2"].Set_Vec3_Uniform("pointlight.light_position", pointlight->light_position); 
    game->shaders_table["deferred2"].Set_Vec3_Uniform("pointlight.light_intensity", pointlight->light_intensity);

    quad_model->meshes[0].vao.Bind();

    glDrawElements(GL_TRIANGLES, quad_model->meshes.back().render_data.indices.size(),
        GL_UNSIGNED_INT, (void*)quad_model->meshes.back().render_data.indices[0]);
    
    quad_model->meshes[0].vao.Un_Bind();

    game->shaders_table["deferred2"].Un_Bind();

    render_target.Unbind(); 
}


void SDeferred::Clear()
{
    GBuffer.Clear();

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