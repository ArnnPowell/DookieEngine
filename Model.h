#pragma once
#ifndef _MODEL_H_
#define _MODEL_H_

// Includes
#include <d3d11.h>
#include <directxmath.h>
#include <fstream>
using namespace DirectX;
using namespace std;

#include "Texture.h"

class Model
{
private:
	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT2 uv;
		XMFLOAT3 normal;
	};

	struct ModelData
	{
		float x, y, z;		// Position
		float u, v;			// Texture Coords
		float nx, ny, nz;	// Normal x, y, z
	};

public:
	Model();
	Model(const Model&);
	~Model();
	
	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*, char*);
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

	bool LoadModel(char*);
	void ReleaseModel();

private:
	ID3D11Buffer * m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount;

	Texture* m_Texture;

	ModelData* m_model;
};

#endif