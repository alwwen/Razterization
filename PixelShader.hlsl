
Texture2D m_texture;
SamplerState m_samplerState;
struct PixelShaderInput
{
	float4 position : SV_POSITION;
	//float3 colour : COLOUR;
	float3 normal : NORMAL;
	float2 uv : UV;
	float3 worldPosition : POSITION;
};


struct Lighting
{

	float3 att;
	float3 attPadding;

	float3 position;
	float3 positionPadding;

	float3 cameraPosition;
	float3 cameraPositionPadding;

	float3 colour;
	float3 colourPadding;

	float3 ambient;
	float3 ambientPadding;

	float3 diffuse;
	float3 diffusePadding;

};


cbuffer LightConstantBuffer : register(b1)
{
	Lighting light;
}

float4 main(PixelShaderInput input) : SV_TARGET //Render Target
{
	float3 finalLight = {0.0f,0.0f,0.0f};
	float3 vectorToLight = light.position - input.worldPosition;
	float distance = length(vectorToLight);
	float3 posToCamera = normalize(light.position - light.cameraPosition);


	float3 ambient = light.ambient * light.diffuse;

	float3 amountOfLight = dot(vectorToLight/distance, light.ambient);

	float3 diffuse = light.diffuse * light.colour * max(dot(normal, vectorToLight), 0);
	
	float3 specular = light.specular

	return m_texture.Sample(m_samplerState, input.uv);
};
