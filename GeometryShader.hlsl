struct geometry_input
{
	float4 m_position  : SV_POSITION;
	float3 world_position : WORLD_POS;
	float3 m_normal : NORMAL;
	float2 texels : TEXCOORD;
};

struct geometry_output
{
	float4 m_position  : SV_POSITION;
	float3 world_position : WORLD_POS;
	float3 m_normal : NORMAL;
	float2 texels : TEXCOORD;
};

[maxvertexcount(3)]
void main(
	triangle geometry_input input[3] : SV_POSITION, 
	inout TriangleStream< geometry_output > output
)
{
	//New normals for all the new vertices created with the tessellation
	
	
	
	float3 position1 = input[0].m_position;
	float3 position2 = input[1].m_position;
	float3 position3 = input[2].m_position;
	//Kollar om normalerna för trianglarna kommer peka mot eller ifrån oss för att avgöra om det
	//är värt att räkna om det

	float m_dot = dot(normalize(cross(position2 - position1, position3 - position1)), -position1);

	float3 w_position1 = input[0].world_position;
	float3 w_position2 = input[1].world_position;
	float3 w_position3 = input[2].world_position;
	
	if (m_dot >= 0.0f)
	{
		for (int i = 0; i < 3; i++)
		{
			geometry_output v = input[1];
			v.m_normal = normalize(cross(w_position2 - w_position1, w_position3 - w_position1));
			output.Append(v);
		}
	}
}