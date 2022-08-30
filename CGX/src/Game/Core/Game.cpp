#include <Game.h>

#define GLEW_STATIC

#include <iostream>
#include <sstream>
#include <fstream>
#include <glew.h>
#include <glfw3.h>

Game::Game() :
	is_game_running(true),
	window(1395, 815),
	shadow(this),
	final(this),
	loader(this),
	renderer(this),
	environment(this),
	tessellation(this),
	deferred(this),
	ibl(this),
	rays(this)
{
	
}

bool Game::Init()
{
	if (!glfwInit())
	{
		LOG_ERR("failed to Initialize GLFW!\n"); 
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	
	if (!window.Initialize()) 
	{
		LOG_ERR("Failed to initialize window\n"); 
		return false; 
	};

	glfwSetWindowUserPointer(window.window_ptr, (void*)this);

	GLenum err = glewInit();
	std::cout << glGetError() << '\n';
	if (GLEW_OK != err)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		return false;
	}

	pool.Initialize(MAX_THREADS);

	/*
	if (!environment.Init()) 
	{
		LOG_ERR("failed to initialize environment scene\n");
		return false; 
	}

	environment.Load_Data();
	*/

	/*
	if (!shadow.Init()) 
	{
		LOG_ERR("failed to initialize shadow scene\n"); 
		return false;
	}
	
	shadow.Load_Data(); 
	*/

	/*
	if (!tessellation.Init()) 
	{
		LOG_ERR("failed to initialize tesselation scene\n");
		return false;
	}
	
	
	tessellation.Load_Data(); 
	*/

	/*
	if (!deferred.Init()) 
	{
		LOG_ERR("failed to initialize deferred scene\n"); 
		return false; 
	}

	deferred.Load_Data(); 
	*/
	
	if (!ibl.Init()) 
	{
		LOG_ERR("failied to initialize ibl scene\n");
		return false;
	}

	ibl.Load_Data();
	
	/*
	if (!rays.Init())
	{
		LOG_ERR("failied to initialize vxgi scene\n");
		return false;
	}

	rays.Load_Data();
	rays.Initialize_Components();
	*/

	if (!final.Init()) 
	{
		LOG_ERR("failed to initialize final scene\n"); 
		return false; 
	}

	final.Load_Data();

	return true;
}

void Game::Pre_Init()
{
	//renderer.Send_Data_To_GPU(&environment);
	//renderer.Send_Data_To_GPU(&tessellation);
	//renderer.Send_Data_To_GPU(&deferred);
	renderer.Send_Data_To_GPU(&ibl);
	//renderer.Send_Data_To_GPU(&rays); 
	//renderer.Send_Data_To_GPU(&shadow); 
	renderer.Send_Data_To_GPU(&final);

	Load_Shaders();
}

void Game::Run_Game()
{
	while (is_game_running)
	{
		Process_Input();
		Update();
		Generate_Output();
	}
}

void Game::Shutdown()
{
	pool.Shutdown(); 

	for (auto shader : shaders_table) 
	{
		shader.second.Clear();
	}

	shaders_table.clear();

	glfwTerminate();
}

void Game::Process_Input()
{
	glfwPollEvents();

	is_game_running = !glfwWindowShouldClose(window.window_ptr);
}


void Game::Update()
{	
	float deltatime = timer.Get_Deltatime();
	//shadow.Update(deltatime); 
	//environment.Update(deltatime);
	//tessellation.Update(deltatime);
	//deferred.Update(deltatime); 
	//pbr.Update(deltatime);
	ibl.Update(deltatime); 
	//rays.Update(deltatime);
	final.Update(deltatime);
}

void Game::Generate_Output()
{
	renderer.Draw();
}

void Game::Load_Shaders()
{
	shaders_table["basic"] = Shader(); 
	shaders_table["basic"].Create_Shader_Program("Shaders/basic.vert",
		"Shaders/basic.frag", "basic");

	shaders_table["DrawQuad"] = Shader(); 
	shaders_table["DrawQuad"].Create_Shader_Program("Shaders/DrawQuadVert.glsl",
		"Shaders/DrawQuadFrag.glsl", "DrawQuad");

	shaders_table["BlinnPhong"] = Shader();
	shaders_table["BlinnPhong"].Create_Shader_Program("Shaders/BlinnPhong.vert",
		"Shaders/BlinnPhong.frag", "BlinnPhong");

	shaders_table["envMap"] = Shader();
	shaders_table["envMap"].Create_Shader_Program("Shaders/EnvMapping/envMap.vert",
		"Shaders/EnvMapping/envMap.frag", "envMap");

	shaders_table["shadow"] = Shader();
	shaders_table["shadow"].Create_Shader_Program("Shaders/ShadowMpping/Shadow.vert",
		"Shaders/ShadowMpping/Shadow.frag", "shadow");

	shaders_table["wireFrame"] = Shader();
	shaders_table["wireFrame"].Create_Shader_Program("Shaders/Geometry/WireFrame.vert",
		"Shaders/Geometry/WireFrame.frag", "Shaders/Geometry/WireFrame.geo", "wireFrame");

	shaders_table["DepthMap"] = Shader();
	shaders_table["DepthMap"].Create_Shader_Program("Shaders/ShadowMpping/DepthMap.vert",
		"Shaders/ShadowMpping/DepthMap.frag",
		"Shaders/ShadowMpping/DepthMap.geom", "DepthMap");

	shaders_table["tessellation"] = Shader(); 
	shaders_table["tessellation"].Create_Shader_Program("Shaders/Tessellation/Tessellation.vert",
		"Shaders/Tessellation/Tessellation.frag",
		"Shaders/Tessellation/Tessellation.tcs",
		"Shaders/Tessellation/Tessellation.tes"
		, "Shaders/Tessellation/Tessellation.geo", "tessellation");

	shaders_table["deferred"] = Shader();
	shaders_table["deferred"].Create_Shader_Program("Shaders/Deferred/Deferred.vert",
		"Shaders/Deferred/Deferred.frag", "deferred");

	shaders_table["deferred2"] = Shader();
	shaders_table["deferred2"].Create_Shader_Program("Shaders/Deferred/Deferred2.vert",
		"Shaders/Deferred/Deferred2.frag", "deferred2");

	shaders_table["gbuffer"] = Shader();
	shaders_table["gbuffer"].Create_Shader_Program("Shaders/PBR/GBuffer.vert",
		"Shaders/PBR/GBuffer.frag", "gbuffer");

	shaders_table["pbrShading"] = Shader();
	shaders_table["pbrShading"].Create_Shader_Program("Shaders/PBR/Shading.vert",
		"Shaders/PBR/Shading.frag", "pbrShading");

	shaders_table["toneMapping"] = Shader();
	shaders_table["toneMapping"].Create_Shader_Program("Shaders/PBR/ToneMapping.vert",
		"Shaders/PBR/ToneMapping.frag", "toneMapping");

	shaders_table["lightSource"] = Shader();
	shaders_table["lightSource"].Create_Shader_Program("Shaders/LightSource.vert",
		"Shaders/LightSource.frag", "lightSource");

	shaders_table["SSAO"] = Shader();
	shaders_table["SSAO"].Create_Shader_Program("Shaders/SSAO/SSAO.vert",
		"Shaders/SSAO/SSAO.frag", "SSAO");

	shaders_table["blurSSAO"] = Shader();
	shaders_table["blurSSAO"].Create_Shader_Program("Shaders/SSAO/BlurSSAO.vert",
		"Shaders/SSAO/BlurSSAO.frag", "blurSSAO");

	shaders_table["EnvToCube"] = Shader();
	shaders_table["EnvToCube"].Create_Shader_Program("Shaders/PBR/IBL/EnvToCubeMap.vert",
		"Shaders/PBR/IBL/EnvToCubeMap.frag", "Shaders/PBR/IBL/EnvToCubeMap.geom", "EnvToCube");

	shaders_table["DrawCubeMap"] = Shader();
	shaders_table["DrawCubeMap"].Create_Shader_Program("Shaders/PBR/IBL/DrawCubeMap.vert",
		"Shaders/PBR/IBL/DrawCubeMap.frag", "DrawCubeMap");

	shaders_table["GenerateIrradianceMap"] = Shader();
	shaders_table["GenerateIrradianceMap"].Create_Shader_Program("Shaders/PBR/IBL/GenerateIrradianceMap.vert",
		"Shaders/PBR/IBL/GenerateIrradianceMap.frag", "Shaders/PBR/IBL/GenerateIrradianceMap.geom", "GenerateIrradianceMap");

	shaders_table["GeneratePreFilteredEnvMap"] = Shader();
	shaders_table["GeneratePreFilteredEnvMap"].Create_Shader_Program("Shaders/PBR/IBL/GeneratePreFilteredEnvMap.vert",
		"Shaders/PBR/IBL/GeneratePreFilteredEnvMap.frag", "Shaders/PBR/IBL/GeneratePreFilteredEnvMap.geom", "GeneratePreFilteredEnvMap");

	shaders_table["IntegralLUT"] = Shader();
	shaders_table["IntegralLUT"].Create_Shader_Program("Shaders/PBR/IBL/IntegralLUT.vert",
		"Shaders/PBR/IBL/IntegralLUT.frag", "DrawCubeMap");

	shaders_table["RayTracing"] = Shader();
	shaders_table["RayTracing"].Create_Shader_Program("Shaders/Raytracing/RayTracingCompute.glsl", "RayTracing");
}


Game::~Game()
{
}
