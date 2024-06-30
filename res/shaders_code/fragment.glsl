#version 450

layout(binding = 0) uniform sampler2D fontTexture;

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

//sizeof texSampler - 1
const uint FontId = 4;
const vec3 fontColor = vec3(1.0, 1.0, 1.0);

void main() {
    outColor = texture(fontTexture, fragTexCoord);
    if (outColor.r < 0.1) discard;
    outColor = vec4(vec3(outColor.r) * fontColor, 1.0f);
    
    if (outColor.a < 0.01) discard;
    return;
}