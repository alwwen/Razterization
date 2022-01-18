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
	float3 worldPosition : POSITION;
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
	output.normal = mul(float4(input.normal, 0.0f), world);
	output.uv = input.uv;
	output.worldPosition = mul(float4(input.position, 1.0f), world);
	return output;
}