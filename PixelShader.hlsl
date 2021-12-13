
Texture2D m_texture;
SamplerState m_samplerState;
struct PixelShaderInput
{
	float4 position : SV_POSITION;
	//float3 colour : COLOUR;
	float3 normal : NORMAL;
	float2 uv : UV;
};

float4 main(PixelShaderInput input) : SV_TARGET //Render Target
{
	return m_texture.Sample(m_samplerState, input.uv);
};