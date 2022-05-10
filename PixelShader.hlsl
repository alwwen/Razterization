struct PixelShaderInput
{
	float4 position : SV_POSITION;
	//float3 colour : COLOUR;
	float3 world_position : WORLD_POS;
	float3 world_normal : NORMAL_WORLD;
	float2 texels : TEXCOORD;
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
	float4 m_normal = float4(normalize(input.world_normal), 1.0f);
	float4 m_distance = length(position - input.world_position);
	float4 beforeLight = (0.0f, 255.0f, 0.0f, 1.0f);
	float4 vectorToLight = normalize(position - float4(input.world_position, 1.0f));
	float4 m_ambient = beforeLight * ambient;

	float m_att = 1.0f - (m_distance/att.r);
	m_att = m_att * m_att * m_att * m_att * m_att;

	float4 m_diffuse = beforeLight * diffuse * max(dot(m_normal, -vectorToLight), 0) * m_att * att.g;

	float4 reflection = reflect(vectorToLight, float4(input.world_normal, 1.0f));

	float4 m_specular = beforeLight * pow(max(dot(reflection, -vectorToLight), 0), 128);
	
	
	float4 finalLight = m_ambient + m_diffuse + m_specular;


	return finalLight;

};
