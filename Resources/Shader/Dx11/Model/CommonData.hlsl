#include "../LightHelper.hlsl"

struct VertexIn {
    float3 vertex    : POSITION;
};

struct VertexOut {
    float4 position : SV_POSITION;
    float3 vert    	: POSITION;
};

struct GeoOut {
    float4 		position : SV_POSITION;
    float3 		vert : POSITION;
    float3 		vertNormal : NORMAL;
	uint		vIdx : SV_PrimitiveID;
	//float3      edgeCoordinates: POSITION2;
};

cbuffer ColorCB : register(b1) {
       //光照和物体材质
    Material    g_normal_material_;
    Light       g_normal_light_;
    //指定法向的材质
    Material    g_assign_normal_material_;
};

cbuffer ParameterCB : register(b2) {
    //模型显示的最大最小限制 XYZ的 比如z最小为0 z最大为10 那么只显示z为0-10的z高度
    float4      g_xyz_visible_max_loc_;
    float4      g_xyz_visible_min_loc_;
    //如果三角形的法向和指定法向一样，那么渲染为指定颜色
    float4      g_assig_normal_;
    //透明值
    float       g_transparent_value_;
    //菲涅尔系数
    float       g_fresnel_value_;
    //最大菲尼尔值(不要让菲涅尔太亮)
    float       g_max_fresnel_value_;
    //开关
    bool        g_open_fresnel_flag_;
    //开启冯灯光效果
    bool        g_open_blinn_flag_;
    //开启透明
    bool        g_open_transparent_render_flag_;
    //是否开启光影计算
    bool        g_open_light_cal_flag_;
    float       g_brightness_;
    bool        g_open_assign_vertex_height_;
    float       g_assgin_vertex_height_;
};
