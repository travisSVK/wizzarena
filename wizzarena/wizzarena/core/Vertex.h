/*
* @file Vertex.h
* @author Marek Cernak
* @date 2017/2/12
*/

#pragma once
#include <glm.hpp>

namespace Core
{
    struct Vertex
    {
        glm::vec3 position;
        glm::vec4 color;
        glm::vec2 texture;

        Vertex(const glm::vec3 &pos, const glm::vec4 &col, const glm::vec2 &tex)
        {
            position = pos;
            color = col;
            texture = tex;
        }
    };
}