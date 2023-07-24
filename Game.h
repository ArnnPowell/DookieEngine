#pragma once
#ifndef _GAME_H_
#define _GAME_H_

// Includes
#include <Windows.h>

// Class Includes
#include "Input.h"
#include "GraphicsEngine.h"

class Game
{
public:
	Game();
	Game(const Game&);
	~Game();

	bool Initialize();
	void Shutdown();
	void Run();
	
	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Loop();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

	LPCWSTR m_GraphicsEngineName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	Input* m_Input;
	GraphicsEngine* m_GraphicsEngine;
};

// Function Prototypes
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Globals
static Game* ApplicationHandle = 0;

#endif