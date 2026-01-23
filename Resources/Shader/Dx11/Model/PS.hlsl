#include "commonData.hlsl"


float4 PS(GeoOut geoOut) : SV_TARGET {
    float3 vertNormal = mul((float3x3)worldNormalMatrix, geoOut.vertNormal).xyz;
    float3 lightDir = normalize(g_normal_light_.position.xyz - (geoOut.vert));  
    float3 eyeDir = normalize(float3(0, 0, 2000) - (geoOut.vert));
    //归一化 
    float3 normalizeNormal = normalize(vertNormal);
    //法线点乘视线 (法线和视线平行的时候该值就是1 如果呈90°那么值为0)
    float NdE = dot(normalizeNormal, eyeDir); 
    float3 resultColor = float3(0,0,0);
	resultColor = getPointLightColor(g_normal_light_, g_normal_material_, geoOut.vert, vertNormal, eyeDir, lightDir, true);
	//开启透明度以后给他一个透明值
	float4 fragColor = float4(resultColor, 1);
    return fragColor;
}