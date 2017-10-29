struct VS_INPUT
{
	float4		position	: POSITION;
	float3		normal		: NORMAL;
	float3		tangent		: TANGENT;
	float2		texcoord	: TEXCOORD0;
};

float4 main(VS_INPUT input) : SV_POSITION
{
	return input.position;
}