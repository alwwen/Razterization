
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




cbuffer LightConstantBuffer : register(b1)
{
	float4 att;

	float4 position;

	float4 cameraPosition;

	float4 colour;

	float4 ambient;

	float4 diffuse;
}

float4 main(PixelShaderInput input) : SV_TARGET //Render Target
{
	float4 m_normal = float4(normalize(input.normal), 1.0f);
	float4 m_distance = length(position - input.worldPosition);
	float4 beforeLight = m_texture.Sample(m_samplerState, input.uv);
	float4 vectorToLight = normalize(position - float4(input.worldPosition, 1.0f));
	float4 m_ambient = beforeLight * ambient;

	float m_att = 1.0f - (m_distance/att.r);
	m_att = m_att * m_att * m_att * m_att * m_att;

	float4 m_diffuse = beforeLight * diffuse * max(dot(m_normal, -vectorToLight), 0) * m_att * att.g;

	float4 reflection = reflect(vectorToLight, float4(input.normal, 1.0f));

	float4 m_specular = beforeLight * pow(max(dot(reflection, -vectorToLight), 0), 128);
	
	
	float4 finalLight = m_ambient + m_diffuse + m_specular;


	return finalLight;




};
