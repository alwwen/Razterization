struct vertex_input
{
	float3 m_position: POSITION;
	float3 m_normal: NORMAL;
	float2 m_texels : TEXCOORD;
};

struct vertex_output
{
	float4 m_position : SV_POSITION;
	float3 m_normal: NORMAL;
	float2 m_texels : TEXCOORD;
};

cbuffer ConstantBuffer : register(b0)
{
	float4x4 WVP;
	float4x4 world;
}


vertex_output main(vertex_input input)
{
	vertex_output output;
	output.m_position = float4(input.m_position, 1.0f);
	output.m_normal = input.m_normal;
	output.m_texels = input.m_texels;
	return output;
}