//FIXA IORDNING ALLA STRUCTS KORREKT SÅ DU INTE FÅR DE JÄVLA BITSHIFTSEN IGEN!!!!!!!!!!!!!!!!!!!!!
SamplerState m_sample : register(s0);

struct domain_output
{
	float4 m_position  : SV_POSITION;
	float3 world_position : WORLD_POS;
	float3 m_normal : NORMAL;
	float2 texels : TEXCOORD;
};

struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3]	: SV_TessFactor;
	float InsideTessFactor	: SV_InsideTessFactor;
};

struct domain_input
{
	float4 m_position: SV_POSITION;
	float3 m_normal: NORMAL;
	float2 texels : TEXCOORD;
};

cbuffer ConstantBuffer : register(b0)
{
	float4x4 WVP;
	float4x4 world;
}


#define control_points 3

[domain("tri")]
domain_output main(
	HS_CONSTANT_DATA_OUTPUT input,
	float3 UVW : SV_DomainLocation,
	const OutputPatch<domain_input, control_points> tri)
{
	domain_output output = (domain_output)0;
	float3 final_position = UVW.x * tri[0].m_position + UVW.y * tri[1].m_position + UVW.z * tri[2].m_position;
	output.world_position = mul(float4(final_position, 1), world).xyz;
	output.m_position = mul(float4(output.world_position, 1), WVP);

	float3 final_normal = UVW.x * tri[0].m_normal + UVW.y * tri[1].m_normal + UVW.z * tri[2].m_normal;
	output.m_normal = mul(float4(final_normal, 0), world).xyz;
	
	float2 final_texels = UVW.x * tri[0].texels + UVW.y * tri[1].texels + UVW.z * tri[2].texels;
	output.texels = final_texels;

	output.m_normal = normalize(mul(float4(output.m_normal, 1.0f), world));

	return output;
}
/*
float4 PS(DS_OUT input) : SV_TARGET
{
	float3 lightPos = float3(0, -5.5f, 0);
	float shade = dot(-normalize(input.wPos - lightPos), normalize(input.wNorm));
	float3 color = g_ColorTexture.Sample(TextureSample, input.texels).xyz; //Importera den där jävla texturen innan du kör

	return float4(color * shade, 1); 
}*/