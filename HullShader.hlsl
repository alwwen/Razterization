//FIXA IORDNING ALLA STRUCTS KORREKT SÅ DU INTE FÅR DE JÄVLA BITSHIFTSEN IGEN!!!!!!!!!!!!!!!!!!!!!

/*struct VS_CONTROL_POINT_OUTPUT
{
	float3 vPosition : WORLDPOS;
}; */

//struct HS_CONTROL_POINT_OUTPUT
//{
//	float3 vPosition : WORLDPOS; 
//};

struct hull_input
{
	float4 m_position: SV_POSITION;
	float3 m_normal: NORMAL;
	float2 m_texels : TEXCOORD;
};

struct hull_output
{
	float4 m_position : SV_POSITION;
	float3 m_normal : NORMAL;
	float2 m_texels : TEXCOORD;
};


struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3]	: SV_TessFactor;
	float InsideTessFactor	: SV_InsideTessFactor;
};

#define control_points 3
#define tess_factor 3.0f

HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(
	InputPatch<hull_input, control_points>ip,
	uint PatchID : SV_PrimitiveID)
{
	HS_CONSTANT_DATA_OUTPUT output;
	for (int i = 0; i < 3; i++)
	{
		output.EdgeTessFactor[i] = tess_factor;
	}
	output.InsideTessFactor = tess_factor;

	return output;
}

[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("CalcHSPatchConstants")]
hull_output main( 
	InputPatch<hull_input, control_points> input, 
	uint i : SV_OutputControlPointID,
	uint PatchID : SV_PrimitiveID )
{
	hull_output output;
	output.m_position = input[i].m_position;
	output.m_normal = input[i].m_normal;
	output.m_texels = input[i].m_texels;
	return output;
	
}
