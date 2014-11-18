#version 430
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aCoord;

layout(location = 0) uniform mat4 mvpMatrix;

out vec3 vNormal;
out vec2 vCoord;
void main(void) {
    gl_Position = mvpMatrix * vec4(aPosition, 1.0);
    vNormal = aNormal;
    vCoord = aCoord;
}