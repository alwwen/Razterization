
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

	float4 att;
	float4 attPadding;

	float4 position;
	float4 positionPadding;

	float4 cameraPosition;
	float4 cameraPositionPadding;

	float4 colour;
	float4 colourPadding;

	float4 ambient;
	float4 ambientPadding;

	float4 diffuse;
	float4 diffusePadding;

};


cbuffer LightConstantBuffer : register(b1)
{
	Lighting light;
}

float4 main(PixelShaderInput input) : SV_TARGET //Render Target
{
	/*float4 finalLight = m_texture.Sample(m_samplerState, input.uv);
	float3 vectorToLight = light.position - input.worldPosition;
	float distance = length(vectorToLight);
	float3 posToCamera = normalize(light.cameraPosition - light.position);
	float3 reflection = normalize(reflect(-vectorToLight, input.normal));


	float4 ambient = light.ambient * light.colour;

	float3 amountOfLight = dot(vectorToLight/distance, light.ambient);

	float4 diffuse = light.colour * max(dot(input.normal, vectorToLight), 0);

	float4 specular = light.colour * pow(max(dot(reflection, posToCamera), 0), 1024.0f);

	return float4((finalLight * (ambient + diffuse * light.att) + specular * light.att), 1.0f); */
	
	float4 finalLight;
	float4 diffuse = m_texture.Sample(m_samplerState, input.uv);
	float3 vectorToLight = normalize(input.worldPosition - light.position);
	float4 diffuseI = dot(input.normal, -vectorToLight);
	float m_distance = distance(input.worldPosition, light.position);
	float att = 1 / m_distance;
	finalLight = diffuse; 
	if (0 == light.position.x && 0 == light.position.y && 0 == light.position.z)
		return float4(1,1,1,1);
	return light.position;


};
