/*
* @file Models.h
* @author Marek Cernak
* @date 2017/2/12
*/

#pragma once
#include <GLFW/glfw3.h>
#include <vector>
#include <map>
#include "Vertex.h"

namespace Core
{
	struct Model
	{
		unsigned int vao;
		std::vector<unsigned int> vbos;

		Model() {}
	};

	class Models
	{
	public:
		~Models();
		void CreateModel(const std::string &modelName, const std::vector<Vertex> &model, const std::vector<unsigned int> &indices);
		void DeleteModel(const std::string &modelName);
		unsigned int GetModel(const std::string &modelName);
        void UpdateModel(const std::string &modelName, const std::vector<Vertex> &model);
	private:
		std::map<std::string, Model> m_modelList;//keep our models
	};
}