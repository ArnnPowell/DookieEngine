#pragma once
#ifndef _MODEL_H_
#define _MODEL_H_

// Includes
#include <d3d11.h>
#include <directxmath.h>
using namespace DirectX;

#include "Texture.h"

class Model
{
private:
	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT2 uv;
	};

public:
	Model();
	Model(const Model&);
	~Model();
	
	bool Initialize(ID3D11Device*, ID3D11DeviceContext* deviceContext, char* textureFilename);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();

	ID3D11ShaderResourceView* GetTexture();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);
	void ReleaseTexture();

private:
	ID3D11Buffer * m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount;

	Texture* m_Texture;
};

#endif