/*
* @file ShaderLoader.cpp
* @author Marek Cernak
* @date 2017/2/12
*/

#include "ShaderLoader.h"
#include <glad/glad.h>
#include <fstream>
#include <vector>
#include <gtc/type_ptr.hpp>

namespace Core
{
	ShaderLoader::ShaderLoader() {}
	ShaderLoader::~ShaderLoader() {}


	GLuint ShaderLoader::CreateProgram(const std::string& vertexShaderFilename, const std::string& fragmentShaderFilename)
	{

		//read the shader files and save the code
		std::string vertex_shader_code = ReadShader(vertexShaderFilename);
		std::string fragment_shader_code = ReadShader(fragmentShaderFilename);

		GLuint vertex_shader = CreateShader(GL_VERTEX_SHADER, vertex_shader_code, "vertex shader");
		GLuint fragment_shader = CreateShader(GL_FRAGMENT_SHADER, fragment_shader_code, "fragment shader");

		int link_result = 0;
		//create the program handle, attatch the shaders and link it
		GLuint program = glCreateProgram();
		glAttachShader(program, vertex_shader);
		glAttachShader(program, fragment_shader);

		glLinkProgram(program);
		glGetProgramiv(program, GL_LINK_STATUS, &link_result);
		//check for link errors
		if (link_result == GL_FALSE)
		{

			int info_log_length = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_length);
			std::vector<char> program_log(info_log_length);
			glGetProgramInfoLog(program, info_log_length, NULL, &program_log[0]);
			std::cout << "Shader Loader : LINK ERROR" << std::endl << &program_log[0] << std::endl;
			return 0;
		}
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);
		return program;
	}


	std::string ShaderLoader::ReadShader(const std::string& filename)
	{

		std::string shaderCode;
		std::ifstream file(filename.c_str(), std::ios::in);

		if (!file.good())
		{
			std::cout << "Can't read file " << filename.c_str() << std::endl;
			std::terminate();
		}

		file.seekg(0, std::ios::end);
		shaderCode.resize((unsigned int)file.tellg());
		file.seekg(0, std::ios::beg);
		file.read(&shaderCode[0], shaderCode.size());
		file.close();
		return shaderCode;
	}


	GLuint ShaderLoader::CreateShader(GLenum shaderType, const std::string& source, const std::string& shaderName)
	{

		int compile_result = 0;

		GLuint shader = glCreateShader(shaderType);
		const char *shader_code_ptr = source.c_str();
		const int shader_code_size = source.size();

		glShaderSource(shader, 1, &shader_code_ptr, &shader_code_size);
		glCompileShader(shader);
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_result);

		//check for errors
		if (compile_result == GL_FALSE)
		{

			int info_log_length = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);
			std::vector<char> shader_log(info_log_length);
			glGetShaderInfoLog(shader, info_log_length, NULL, &shader_log[0]);
			std::cout << "ERROR compiling shader: " << shaderName.c_str() << std::endl << &shader_log[0] << std::endl;
			return 0;
		}
		return shader;
	}

	void ShaderLoader::setBool(unsigned int programId, const std::string &name, bool value)
	{
		glUniform1i(glGetUniformLocation(programId, name.c_str()), (int)value);
	}
	void ShaderLoader::setInt(unsigned int programId, const std::string &name, int value)
	{
		glUniform1i(glGetUniformLocation(programId, name.c_str()), value);
	}
	void ShaderLoader::setFloat(unsigned int programId, const std::string &name, float value)
	{
		glUniform1f(glGetUniformLocation(programId, name.c_str()), value);
	}

    void ShaderLoader::setVec2(unsigned int programId, const std::string &name, glm::vec2 vec)
    {
        glUniform2fv(glGetUniformLocation(programId, name.c_str()), 1, glm::value_ptr(vec));
    }
	void ShaderLoader::setMat4(unsigned int programId, const std::string &name, glm::mat4 mat)
	{
		unsigned int loc = glGetUniformLocation(programId, name.c_str());
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
	}
}