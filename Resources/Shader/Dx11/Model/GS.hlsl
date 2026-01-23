#include "commonData.hlsl"

//最多点限制
[maxvertexcount(3)]
void GS(triangle VertexOut gs_in[3], 
	in uint vIdx : SV_PrimitiveID,
	inout TriangleStream<GeoOut> geoOut) {
    GeoOut geo;
    geo.vert = gs_in[0].vert;
	geo.vIdx = vIdx;
    float3 a = gs_in[1].vert - gs_in[0].vert;
    float3 b = gs_in[2].vert - gs_in[1].vert;
    //计算法向
    geo.vertNormal = normalize(cross(a, b));           
    geo.position = gs_in[0].position;
    geoOut.Append(geo);
	//第二个点
    geo.vert = gs_in[1].vert;
    geo.position = gs_in[1].position;
	geoOut.Append(geo);
	//第三个点
    geo.vert = gs_in[2].vert;
    geo.position = gs_in[2].position;
    geoOut.Append(geo);
}