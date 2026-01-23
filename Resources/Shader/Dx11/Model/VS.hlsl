#include "commonData.hlsl"

VertexOut VS(VertexIn vin) {
    VertexOut vout;
    vout.vert = mul(modelMatrix, float4(vin.vertex, 1.0f)).xyz;
    vout.position = mul(finallyMatrix,float4(vin.vertex, 1.0f));
    return vout;
}