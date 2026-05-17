#version 330 core
#define MAX_SPOT_LIGHTS 4


in vec3 vColor;
in vec3 vNormal;      // 頂點的法向量 (N)
in vec3 vLight;       // 頂點指向光源的 Light(L) 向量
in vec3 vView;        // 頂點指向 view 的 view(V) 向量
in vec3 v3Pos;

uniform int  uShadingMode;     // 1=頂點色, 2=物件色, 3=Per-Pixel
uniform vec4 ui4Color;         // 模型顏色
uniform sampler2D uTexture; // 模型貼圖
in vec2 vTexCoord;


struct LightSource {
    vec3 position;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float constant;
    float linear;
    float quadratic;

    vec3 direction;
    float cutOff;
    float outerCutOff;
    float exponent;

    bool enabled;
};

struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};

uniform LightSource uLight;
uniform LightSource uLight1;
uniform LightSource spotLights[MAX_SPOT_LIGHTS];
uniform Material uMaterial;

out vec4 FragColor;
vec3 N;
vec3 V;
vec3 L;
vec3 R;

vec4 computeLight(LightSource light, vec4 baseColor, bool isSpot) {

    if (!light.enabled) return vec4(0.0);

     N = normalize(vNormal);
     V = normalize(vView);
     L = normalize(light.position - v3Pos);
     R = reflect(-L, N);
    
    float dist = length(light.position - v3Pos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * dist * dist);

    // Ambient
    vec4 ambient = light.ambient * baseColor;

    // --- Quantized Diffuse ---
    float diff = max(dot(N, normalize(L)), 0.0);
    float levels = 4.0;
    float qdiff = floor(diff * levels) / levels;
    vec4 diffuse = light.diffuse * qdiff * baseColor;

    // --- Sharp Specular ---
    float specAngle = max(dot(R, V), 0.0);
    float spec = (specAngle > 0.9) ? 1.0 : 0.0;
    vec4 specular = light.specular * spec * uMaterial.specular;

    if (isSpot) {
        float theta = dot(normalize(-L), normalize(light.direction));
        float epsilon = max(light.cutOff - light.outerCutOff, 0.001);
        float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
        attenuation *= (light.exponent == 1.0) ? intensity : pow(intensity, light.exponent);
    }
    vec4 color = (ambient + diffuse + specular) * attenuation;
    return color;
}

void main() {

    // 頂點顏色
    if (uShadingMode == 1) {
        FragColor = vec4(vColor, 1.0);
        return;
    }

    // 模型顏色
    if (uShadingMode == 2) {
        FragColor = uMaterial.diffuse;
        return;
    }

    // 貼圖無光照
    if (uShadingMode == 4) {
        FragColor = texture(uTexture, vTexCoord);
        return;
    }

    // 光照
    vec4 baseColor = (uShadingMode == 5) ? texture(uTexture, vTexCoord) : uMaterial.diffuse;

    // 計算點光源
    vec4 finalColor = computeLight(uLight, baseColor, false);
    finalColor += computeLight(uLight1, baseColor, false);

    // 聚光燈
    for (int i = 0; i < MAX_SPOT_LIGHTS; ++i) {
        finalColor += computeLight(spotLights[i], baseColor, true);
    }

    finalColor.a = baseColor.a;
    float edge = abs(dot(N,V));
    if( edge < 0.2 ){ finalColor = vec4(0.0, 0.0, 0.0, 1.0); };
    FragColor = finalColor;
}
