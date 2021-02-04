#ifndef SHADER_HPP
#define SHADER_HPP

#include "common.h"



std::string readSourceFile(const char *path);

void CompileShader(std::string program_code, GLuint shader_id);

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);

#endif
