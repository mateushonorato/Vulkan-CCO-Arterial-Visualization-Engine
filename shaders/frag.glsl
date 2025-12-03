#version 450
layout(location = 0) in vec4 inColor;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

layout(location = 0) out vec4 outFragColor;

layout(set = 0, binding = 1) uniform Lighting {
    vec3 lightDir;
    vec3 lightColor;
} lighting;

void main() {
    // Simple Lambertian baseline (upgradeable to Gouraud/Phong in TP2)
    float ndotl = max(dot(normalize(inNormal), normalize(lighting.lightDir)), 0.0);
    vec3 shaded = inColor.rgb * (0.2 + 0.8 * ndotl * lighting.lightColor);
    outFragColor = vec4(shaded, inColor.a);
}
