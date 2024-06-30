#version 450

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec2 fragTexCoord;

void main() {
    vec2 screen_space = vec2(inPosition.x / 400.0 - 1.0, inPosition.y / 400.0 - 1.0);
    gl_Position = vec4(screen_space, 0.0, 1.0);
    fragTexCoord = inTexCoord;
    
    return;
}