struct VertexShaderInput
{
	float3 position : POSITION;
	//float3 colour : COLOUR;
	float3 normal : NORMAL;
	float2 uv : UV;
};

struct VertexShaderOutput
{
	float4 position : SV_POSITION;
	//float3 colour : COLOUR;
	float3 normal : NORMAL;
	float2 uv : UV;

};

cbuffer ConstantBuffer : register(b0)
{
	float4x4 WVP;
	float4x4 world;
}

VertexShaderOutput main(VertexShaderInput input)
{
	VertexShaderOutput output;
	output.position = mul(float4(input.position, 1.0f), WVP);
	//output.colour = input.colour;
	output.normal = input.normal;
	output.uv = input.uv;
	return output;
}