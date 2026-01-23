struct Material {
    float4 ambient;
    float4 diffuse;
    float4 specular;
    float shininess;
};

struct Light {
    float4 ambient;
    float4 diffuse;
    float4 specular;
    float4 position;
};

//统一的 MatCB 定义，所有 DX11 着色器共享
cbuffer MatCB : register(b0) {
    matrix 		finallyMatrix;
    matrix 		worldMatrix;
    matrix 		worldNormalMatrix;
    matrix 		modelMatrix;
    //3x3矩阵
    matrix 		modelNormalMatrix;     //没用被使用到的uniform会被自动优化掉
    matrix 		projMatrix;
    int         g_uav_pick_flag_;
    int         g_uav_pos_x;
    int         g_uav_pos_y;
    int         gfill23232_nocare;
};

//当前的点是否满足显示，不满足返回false
bool vertIsVisble(float3 vert, float4 minBound, float4 maxBound) {
    if(vert.x < minBound.x
    || vert.x > maxBound.x) {
        return false;
    }
	if(vert.y < minBound.y
	|| vert.y > maxBound.y) {
		return false;
	}
	if(vert.z < minBound.z + 0.00005
		|| vert.z > maxBound.z + 0.0005) {
        return false;
    }
    return true;
}

//当前的点是否满足显示，不满足返回false
bool vertIsVisble2(float3 vert, float4 minBound, float4 maxBound) {
    if(vert.x < minBound.x
    || vert.x > maxBound.x) {
        return false;
    }
	if(vert.y < minBound.y
	|| vert.y > maxBound.y) {
		return false;
	}
	if(vert.z < minBound.z - 0.00005
		|| vert.z > maxBound.z + 0.00005) {
        return false;
    }
    return true;
}

//可以获取环境、漫反射、高光
void getPointLightColor2(const Light inLight, Material inMat, float3 inVertPos, float3 inVertNormal, float3 eyeDir, float3 lightDir, bool isOpenBlinn,
	out float3 ambient, out float3 diffuse, out float3 specular) {
    float3 norm = normalize(inVertNormal);
    //ambient
    ambient = (inLight.ambient.xyz * inMat.ambient.xyz);
    //diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    diffuse = inLight.diffuse.xyz * (diff * inMat.diffuse.xyz);

    float spec = 0.0;
    //openBlinnFlag光照，否则选择PHONG光照
    if(isOpenBlinn){
		//视线方向
        float3 halfwayDir = normalize(lightDir + eyeDir);
        spec = pow(max(dot(norm, halfwayDir), 0.0), 16);
    }else {
        float3 reflectDir = reflect(-lightDir, inVertNormal);
        float dotValue = dot(lightDir, reflectDir);
        spec = pow(max(dotValue, 0.0), inMat.shininess);
    }
    specular = inLight.specular.xyz * (spec * inMat.specular.xyz);
}

//获取点光源的颜色
float3 getPointLightColor(const Light inLight, Material inMat, float3 inVertPos, float3 inVertNormal, float3 eyeDir, float3 lightDir, bool isOpenBlinn) {
	float3 ambient  = float3(0, 0, 0);
	float3 diffuse  = float3(0, 0, 0);
	float3 specular = float3(0, 0, 0);
    getPointLightColor2(inLight, inMat, inVertPos, inVertNormal, eyeDir, lightDir, isOpenBlinn, ambient, diffuse, specular);
    float3 result = ambient + diffuse + specular;
    return result;
}
