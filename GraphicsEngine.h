#pragma once
#ifndef _GRAPHICSENGINE_H_
#define _GRAPHICSENGINE_H_

// Includes
#include "D3D.h"

// Globals
const bool FULL_SCREEN = true;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.3f;

class GraphicsEngine
{
public:
	GraphicsEngine();
	GraphicsEngine(const GraphicsEngine&);
	~GraphicsEngine();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();

private:
	bool Render();

private:
	// Pointer to D3D object
	D3D* m_d3d;
};

#endif