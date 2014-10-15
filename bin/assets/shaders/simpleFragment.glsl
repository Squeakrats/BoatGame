#version 420

in vec3 vNormal;
void main(void) {
	gl_FragColor = vec4(abs(vNormal), 1.0);
}