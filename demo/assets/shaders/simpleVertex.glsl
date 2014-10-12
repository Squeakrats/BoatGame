#version 420
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
uniform mat4 mvpMatrix;

out vec3 vNormal;
void main(void) {
    gl_Position = mvpMatrix * vec4(aPosition, 1.0);
    vNormal = aNormal;
}