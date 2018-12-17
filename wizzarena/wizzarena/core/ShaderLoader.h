/*
* @file ShaderLoader.h
* @author Marek Cernak
* @date 2017/2/12
*/
#pragma once

#include <iostream>
#include <GLFW/glfw3.h>
#include <glm.hpp>

namespace Core
{
	class ShaderLoader
	{
	public:
		ShaderLoader();
		~ShaderLoader();

		GLuint CreateProgram(const std::string& VertexShaderFilename, const std::string& FragmentShaderFilename);
		// utility uniform functions
		static void setBool(unsigned int programId, const std::string &name, bool value);
		static void setInt(unsigned int programId, const std::string &name, int value);
		static void setFloat(unsigned int programId, const std::string &name, float value);
		static void setMat4(unsigned int programId, const std::string &name, glm::mat4 mat);
        static void setVec2(unsigned int programId, const std::string &name, glm::vec2 vec);

	private:
		std::string ReadShader(const std::string& filename);
		GLuint CreateShader(GLenum shaderType, const std::string& source, const std::string& shaderName);
	};
}