cbuffer MatrixBuffer : register(b0) {
    matrix	worldMatrix;
    matrix	viewMatrix;
    matrix	projectionMatrix;
	matrix	lightVPMatrix;
};

cbuffer CameraBuffer : register(b1) {
    float3	cameraPosition;
    float	padding;
};

struct VertexInputType {
    float4 position		: POSITION;
    float2 tex			: TEXCOORD0;
    float3 normal		: NORMAL;
	float3 tangent		: TANGENT;
	float3 binormal		: BINORMAL;
};

struct PixelInputType {
    float4 position				: SV_POSITION;
    float2 tex					: TEXCOORD0;
    float3 normal				: NORMAL;
    float4 shadowPosition		: TEXCOORD1;
	float3 worldPosition		: TEXCOORD2;
	float3 cameraPosition		: TEXCOORD3;
};

PixelInputType ShadowVertexShader(VertexInputType input) {
    PixelInputType output;
    input.position.w = 1.0f;

	float4 tmpPos = mul(input.position, worldMatrix);
	output.worldPosition = tmpPos.xyz;

	output.position = mul(tmpPos, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

	output.shadowPosition = mul(tmpPos, lightVPMatrix);
	output.cameraPosition = mul(float4(cameraPosition,1), worldMatrix).xyz;

	output.tex = input.tex;
    output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);



    return output;
}