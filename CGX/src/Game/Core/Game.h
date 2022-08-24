#pragma once

#define LOG_ERR(MSG) std::cerr << '[' << __FILE__ << ']' << '[' << __LINE__ << ']' << ' ' << MSG;

#define MAX_SCENES 10

#include <Window.h>
#include <Timer.h>
#include "../Threading/ThreadPool.h"
#include "../Editor/Editor.h"
#include <Component.h>
#include <Shader.h>
#include <thread>
#include <SShadow.h>
#include <Loader.h>
#include <Renderer.h>
#include <Framebuffer.h>
#include <SceneNodeManager.h>
#include <SFinal.h>
#include <SEnvironment.h>
#include <STessellation.h>
#include <SDeferred.h>
#include <SIBL.h>
#include <SRayTracing.h>

class Game
{
public:
	Game();
	~Game();

public:
	bool Init();
	void Pre_Init(); 
	void Run_Game();
	void Shutdown();
	void Process_Input();
	void Update();
	void Generate_Output();
	void Load_Shaders();
	
public:

	// Loader
	Loader loader;

	// Renderer
	Renderer renderer;

	// Node Manager
	SceneNodeManager node_manager; 

	// Times
	Timer timer;

	// Window
	Window window;
	
	// Thread Pool
	ThreadPool pool;

	// Should i close
	bool is_game_running;

	std::unordered_map<std::string, Shader> shaders_table;
	
	// scenes
	SFinal final;
	SEnvironment environment;
	SShadow shadow;
	STessellation tessellation;
	SDeferred deferred;
	SIBL ibl;
	SRayTracing rays; 

	std::vector<class Scene*> scenes;
};

