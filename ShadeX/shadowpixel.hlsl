Texture2D shaderTexture				: register(t0);
Texture2D<float> depthMapTexture	: register(t1);

SamplerState SampleTypeWrap			: register(s0);
SamplerState SampleTypeClamp		: register(s1);

cbuffer LightBuffer : register(b0) {
	float4	ambientColor;
    float4	diffuseColor;
    float4	specularColor;
	float3	lightDirection;
	float	specularPower;
	float4	shadowTextureSize;
};

struct PixelInputType {
    float4 position				: SV_POSITION;
    float2 tex					: TEXCOORD0;
    float3 normal				: NORMAL;
    float4 shadowPosition		: TEXCOORD1;
	float3 worldPosition		: TEXCOORD2;
	float3 cameraPosition		: TEXCOORD3;
};

static const float	g_depthOffset = 0.9995;
static const float2 g_uvOffset = shadowTextureSize.zw;
static const float	g_gamma = 2.2;

float4 ColorGamma(in float4 color, in float gamma) {
	return float4(saturate(pow(color.xyz, gamma)),1);
}

float ShadowValueUv(in float2 shadowUv, in float lightDepthValue) {
	float depthValue = depthMapTexture.Sample(SampleTypeClamp, shadowUv).r;
	if(lightDepthValue < depthValue) {
		return 1.0;
	}
	return 0.0;
}

float2 TexOffset(float u, float v) {
    return float2(u,v) * g_uvOffset;
}

float CalculateShadowValue(float4 shadowPosition) {
	float sum = 0.0;
	if (shadowPosition.x < -1.0f || shadowPosition.x > 1.0f || shadowPosition.y < -1.0f || shadowPosition.y > 1.0f || shadowPosition.z <  0.0f || shadowPosition.z > 1.0f ) { 
		return 1;
	} else {
		shadowPosition.xy		= (shadowPosition.xy / shadowPosition.w) / 2.0f + 0.5f;
		shadowPosition.y		= 1 - shadowPosition.y;
		shadowPosition.z		= (shadowPosition.z / shadowPosition.w) * g_depthOffset;
		float x, y;
		for (y = -1.5; y <= 1.5; y += 1.0) {
			for (x = -1.5; x <= 1.5; x += 1.0) {
				sum += ShadowValueUv(shadowPosition.xy + TexOffset(x,y), shadowPosition.z);
			}
		}
		sum /= 16.0;
	}
	return sum;
}

float2 ComputeDirectionalLight(float3 vWorldPos, float3 vWorldNormal, float3 vDirLightDir, float3 vCamPosition) {
    float2 Result			= float2(0, 0);
    Result.x				= saturate(dot(normalize(vWorldNormal), normalize(-vDirLightDir)));
    float3 vPointToCamera	= normalize(vCamPosition - vWorldPos);
    float3 vHalfAngle		= normalize(vPointToCamera - vDirLightDir);
    Result.y				= pow(saturate(dot(vHalfAngle, normalize(vWorldNormal))), specularPower);
    return Result;
}

float4 MainPS(PixelInputType input) : SV_TARGET {
	float sum				= CalculateShadowValue(input.shadowPosition);
	float4 textureColor		= ColorGamma(shaderTexture.Sample(SampleTypeWrap, input.tex), g_gamma);
	float2 directionalLight = ComputeDirectionalLight(input.worldPosition, input.normal, lightDirection, input.cameraPosition);
	//float4 color			= ambientColor + (specularColor * directionalLight.y + diffuseColor * directionalLight.x) * sum;
	float4 color			= ambientColor + (diffuseColor * directionalLight.x) * sum;
	color *= textureColor;
	color = ColorGamma(color, 1/g_gamma);
	return color;
}