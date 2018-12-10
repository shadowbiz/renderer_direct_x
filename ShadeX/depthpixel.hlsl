struct PixelInputType {
    float4 position			: SV_POSITION;
    float depth				: TEXTURE0;
};

float4 DepthPixelShader(PixelInputType input) : SV_TARGET {
	return float4(input.depth.xxx, 1.0);
}