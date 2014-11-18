#version 430

in vec3 vNormal;
in vec2 vCoord;
//ASSUMED ONE FOR NOW. 

layout(binding = 0) uniform sampler2D uTexture;
void main(void) {
	//gl_FragColor = vec4(abs(vNormal), 1.0);
	gl_FragColor = texture2D(uTexture, vCoord);
	//gl_FragColor = vec4(vCoord.x, 0.0, 0.0, 1.0);
}