/////////////
// Globals //
/////////////
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

//////////////////
// Type Defines //
//////////////////
struct VertexInput
{
	float4 position : POSITION;
	float4 color : COLOR;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

///////////////////
// Vertex Shader //
///////////////////
PixelInput ColorVertexShader(VertexInput input)
{
	PixelInput output;

	// Change position to be 4 units for matrix math
	input.position.w = 1.0f;

	// Calculate world, view, projection matrices
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Store the input color for pixel shader
	output.color = input.color;

	return output;
}