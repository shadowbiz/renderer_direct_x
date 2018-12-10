cbuffer MatrixBuffer : register(b0) {
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer CameraBuffer : register(b1) {
    float3 cameraPosition;
    float padding;
};

cbuffer LightBuffer : register(b2) {
	float4	ambientColor;
    float4	diffuseColor;
    float4	specularColor;
	float3	lightPosition;
	float	specularPower;
};

struct VertexInputType {
    float4 position			: POSITION;
    float2 tex				: TEXCOORD0;
    float3 normal			: NORMAL;
	float3 tangent			: TANGENT;
	float3 binormal			: BINORMAL;
};

struct PixelInputType {
    float4 position			: SV_POSITION;
    float2 tex				: TEXCOORD0;
    float3 normal			: NORMAL;
	float3 tangent			: TANGENT;
	float3 binormal			: BINORMAL;
	float3 worldPosition	: TEXCOORD1;
	float3 viewDirection	: TEXCOORD2;
};

PixelInputType LightVertexShader(VertexInputType input) {
    PixelInputType output;
    input.position.w = 1.0f;

	float4 tmpPos = mul(input.position, worldMatrix);
	output.worldPosition = tmpPos.xyz;

	output.position = mul(tmpPos, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    output.viewDirection = normalize(cameraPosition - output.worldPosition.xyz);

	output.tex = input.tex;
    output.normal = normalize(mul(input.normal, (float3x3)worldMatrix));
	output.tangent = normalize(mul(input.tangent, (float3x3)worldMatrix));
	output.binormal = normalize(mul(input.binormal, (float3x3)worldMatrix));

    return output;
}