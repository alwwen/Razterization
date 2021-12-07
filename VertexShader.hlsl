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

VertexShaderOutput main(VertexShaderInput input)
{
	VertexShaderOutput output;
	output.position = float4(input.position, 1.0f);
	//output.colour = input.colour;
	output.normal = input.normal;
	output.uv = input.uv;
	return output;
}