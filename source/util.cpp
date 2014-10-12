#include "util.h"

std::string loadFile(std::string path) {
	std::ifstream file(path);
	if (!file.is_open()){
		std::cout << "failed to open the file :( " << path << std::endl;
		while (true);
	}

	std::string accum;
	std::string contents;

	while (getline(file, accum)){
		contents += accum + "\n";
	}

	file.close();

	return contents;

}

unsigned int createShader(GLenum type, std::string fPath) {
	std::string source = loadFile(fPath);
	unsigned int shader = glCreateShader(type);
	const char* cStr = source.c_str();
	const int len = source.length();
	glShaderSource(shader, 1, &cStr, &len);
	glCompileShader(shader);

	int compiled = -1;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (compiled != GL_TRUE) {
		char log[2048];
		glGetShaderInfoLog(shader, 2048, nullptr, log);
		std::cout << ((type == GL_VERTEX_SHADER) ? "GL_VERTEX_SHADER" : "GL_FRAGMENT_SHADER") << std::endl;
		std::cout << source << std::endl;
		std::cout << log << std::endl;
		while (true);
	}

	return shader;
}

unsigned int createProgram(std::string vPath, std::string fPath) {

	unsigned int program = glCreateProgram();
	unsigned int vShader = createShader(GL_VERTEX_SHADER, vPath);
	unsigned int fShader = createShader(GL_FRAGMENT_SHADER, fPath);


	glAttachShader(program, vShader);
	glAttachShader(program, fShader);
	glLinkProgram(program);

	int linked = -1;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (linked != GL_TRUE) {
		std::cout << "failed to link :(" << std::endl;
		while (true);
	}

	glDetachShader(program, vShader);
	glDetachShader(program, fShader);
	glDeleteShader(vShader);
	glDeleteShader(fShader);

	return program;
}

