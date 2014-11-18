#version 430
layout(location = 0) in vec3 aPosition;

layout(location = 0) uniform mat4 mvpMatrix;
void main(void) {
	gl_Position = mvpMatrix * vec4(aPosition, 1.0);
}