//=================================================================================
// TheApp.hpp
// Author: Tyler George
// Date  : January 13, 2015
//=================================================================================

#pragma once

#include "Engine/Renderer/OpenGLRenderer.hpp"
#include "Engine/Renderer/AnimatedTexture.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Sound/SoundSystem.hpp"
#include "GameCode/GameCommon.hpp"
#include "GameCode/Game.hpp"
#include "Engine/Utilities/DeveloperConsole.hpp"

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
class TheApp 
{
public:

	///---------------------------------------------------------------------------------
	/// Constructor/Destructor
	///---------------------------------------------------------------------------------
	TheApp();
	~TheApp();

	///---------------------------------------------------------------------------------
	/// Initialization Functions
	///---------------------------------------------------------------------------------
	void Startup( void* windowHandle );
	void Shutdown(); 
	void Run();

	///---------------------------------------------------------------------------------
	/// Update Functions
	///---------------------------------------------------------------------------------
	void ProcessInput();
	void Update();
	
	///---------------------------------------------------------------------------------
	/// Render Functions
	///---------------------------------------------------------------------------------
	void Render();
    void RenderGame();
    void RenderDevConsole();
    void RenderLoadingScreen();


private:
	void*			m_windowHandle;
	OpenGLRenderer*	m_renderer;
	Vector2			m_displaySize;
	bool			m_isRunning;
	Game*			m_game;
	InputSystem*	m_inputSystem;
	//SoundSystem*	m_soundSystem;
	bool			m_debugModeEnabled;
    GraphicsDebugger* m_graphicsDebugger;
    FontRenderer* m_fontRenderer;

    DeveloperConsole* m_developerConsole;
    bool            m_developerConsoleEnabled;

    unsigned int m_texturedShaderID;
    Font* m_font;
    std::string m_fps;

    Texture* m_engineLoadingScreen;

    bool m_shouldTakeScreenshot;
};