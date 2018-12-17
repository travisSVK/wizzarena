/*
* @file Models.cpp
* @author Marek Cernak
* @date 2017/2/12
*/

#include "Models.h"
#include <glad/glad.h>

namespace Core
{

	Models::~Models()
	{

		std::map<std::string, Model>::iterator it;
		for (it = m_modelList.begin(); it != m_modelList.end(); ++it)
		{
			//delete VAO and VBOs (if many)
			unsigned int* p = &it->second.vao;
			glDeleteVertexArrays(1, p);
			glDeleteBuffers((GLsizei)it->second.vbos.size(), &it->second.vbos[0]);
			it->second.vbos.clear();
		}
        m_modelList.clear();
	}

	void Models::CreateModel(const std::string& modelName, const std::vector<Vertex> &model, const std::vector<unsigned int> &indices)
	{
		unsigned int vao;
		unsigned int vbo;
		unsigned int ebo;

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * model.size(), &model[0], GL_STATIC_DRAW);
		
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Vertex::position)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Vertex::color)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Vertex::texture)));

        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);
		//assign element buffer object to tell specify the indices of triangles in object
		/*glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * indices.size(), &indices[0], GL_STATIC_DRAW);*/
		

		Model myModel;                            //is allocated on Stack
		myModel.vao = vao;                        //add vao
		myModel.vbos.push_back(vbo);              //add vbo
        m_modelList[modelName] = myModel;  //add to std::map

	}

    void Models::UpdateModel(const std::string &modelName, const std::vector<Vertex> &model)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_modelList[modelName].vbos[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * model.size(), &model[0]);
    }

	void Models::DeleteModel(const std::string& modelName)
	{
		Model model = m_modelList[modelName];
		unsigned int p = model.vao;
		glDeleteVertexArrays(1, &p);
		glDeleteBuffers((GLsizei)(model.vbos.size()), &model.vbos[0]);
		model.vbos.clear();
        m_modelList.erase(modelName);

	}

	unsigned int Models::GetModel(const std::string& modelName)
	{
		return m_modelList[modelName].vao;
	}
}