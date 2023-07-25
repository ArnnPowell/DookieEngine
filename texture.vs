// Globals
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

// Type Defines
struct VertexInput
{
	float4 position : POSITION;
	float2 uv : TEXCOORD0;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
};

// Vertex Shader
PixelInput TextureVertexShader(VertexInput input)
{
	PixelInput output;

	// Change position to vector for matrix math
	input.position.w = 1.0f;

	// Views - world, view, projection
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Texture coordinates for pixel shader
	output.uv = input.uv;

	return output;
}