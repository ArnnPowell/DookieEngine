//Globals
cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer CameraBuffer
{
    float3 cameraPosition;
    float padding;
}

// Type Defines -  added Normal for lighting
struct VertexInput
{
    float4 position : POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
    float3 viewDirection : TEXCOORD1;
};

PixelInput LightVertexShader(VertexInput input)
{
    PixelInput output;
    float4 worldPosition;

    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // Store the texture coordinates for the pixel shader.
    output.uv = input.uv;

    // Calculate the normal vector against the world matrix only.
    output.normal = mul(input.normal, (float3x3)worldMatrix);
	
    // Normalize the normal vector.
    output.normal = normalize(output.normal);

    // Added for specular lighting
    // Calculate world position
    worldPosition = mul(input.position, worldMatrix);

    // Determine the viewing direction - cameraPosition and vertex position
    output.viewDirection = cameraPosition.xyz - worldPosition.xyz;

    // Normalize viewing direction vector
    output.viewDirection = normalize(output.viewDirection);

    return output;
}