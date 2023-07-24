#pragma once
#ifndef _MODEL_H_
#define _MODEL_H_

// Includes
#include <d3d11.h>
#include <directxmath.h>
using namespace DirectX;

class Model
{
private:
	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

public:
	Model();
	Model(const Model&);
	~Model();
	
	bool Initialize(ID3D11Device*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

private:
	ID3D11Buffer * m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount;
};

#endif