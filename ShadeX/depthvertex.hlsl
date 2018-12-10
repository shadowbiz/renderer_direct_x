cbuffer MatrixBuffer : register(b0) {
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct VertexInputType {
    float4 position			: POSITION;
};

struct PixelInputType {
    float4 position			: SV_POSITION;
};

PixelInputType DepthVertexShader(VertexInputType input) {
    PixelInputType output;
    input.position.w = 1.0f;
	output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    //output.depth = output.position.z / output.position.w;
	
    return output;
}