float4 Ads(in float4 ambientColor, in float4 diffuseColor, in float4 specularColor, in float specularPower, in float4 inDiffuse, in float3 inNormal, in float3 inLightDir, in float3 inViewDir) {
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