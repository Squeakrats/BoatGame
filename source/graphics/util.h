#pragma once

#include <iostream>
#include "GL\glew.h"
#include <string>
#include <fstream>

std::string loadFile(std::string path);

unsigned int createShader(GLenum type, std::string fPath);
unsigned int createProgram(std::string vPath, std::string fPath);

