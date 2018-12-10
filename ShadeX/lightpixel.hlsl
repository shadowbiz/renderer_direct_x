Texture2D shaderTexture		: register(t0);
SamplerState SampleType		: register(s0);

cbuffer LightBuffer			: register(b0) {
	float4	ambientColor;
    float4	diffuseColor;
    float4	specularColor;
	float3	lightPosition;
	float	specularPower;
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

float4 Ads(in float4 inDiffuse, in float3 inNormal, in float3 inLightDir, in float3 inViewDir) {
	float4 color = inDiffuse;
	float3 n = normalize(inNormal);
	float3 l = normalize(inLightDir);
	float3 r = normalize(reflect(-l, n));
	float sDotN = saturate(dot(l, n));

	float4 ambient = ambientColor;
	float4 diffuse = diffuseColor * sDotN;
	float4 spec = float4(0,0,0,0);
	
	if (sDotN > 0.0f) {
		spec = specularColor * pow(max(dot(r,inViewDir), 0.0f), specularPower);
	}
		
	return saturate(color * (ambient + diffuse + spec));
}

float4 LightPixelShader(PixelInputType input) : SV_TARGET {
	float4 textureColor		= shaderTexture.Sample(SampleType, input.tex);
	float3 lightDir			= lightPosition - input.worldPosition;
	float4 color			= Ads(textureColor, input.normal, lightDir, input.viewDirection);
	color = pow(color, 2.2);
	return color;
}