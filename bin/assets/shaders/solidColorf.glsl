#version 430

layout(location = 1) uniform vec3 uColor;
void main(void) {
	gl_FragColor = vec4(uColor, 1.0);//opacity?nah
}