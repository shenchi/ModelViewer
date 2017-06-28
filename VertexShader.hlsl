cbuffer InstanceConstants : register (b0)
{
	matrix		matWorld;
};

cbuffer FrameConstants : register (b1)
{
	matrix		matView;
	matrix		matProj;
};

struct Input
{
	float3	position : POSITION;
	float3	normal : NORMAL;
	float3	tangent : TANGENT;
	float3	texcoord : TEXCOORD;
};

struct V2F
{
	float4	position : SV_POSITION;
};

V2F main(Input input)
{
	V2F output;

	matrix matMVP = mul(mul(matWorld, matView), matProj);

	output.position = mul(float4(input.position, 1), matMVP);

	return output;
}