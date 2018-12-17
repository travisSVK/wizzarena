/*
* @file Player.cpp
* @author Marek Cernak
* @date 2018/12/4
*/

#include "Player.h"
#include "../core/Models.h"
#include "../core/TextureLoader.h"
#include "../core/ShaderLoader.h"

#include <gtx/transform.hpp>
#include <glad/glad.h>
#include <algorithm>

namespace Game
{
    Player::Player(const std::shared_ptr<Core::Models> &modelManager, Core::TextureLoader &textureLoader) : 
        m_speed(sqrt(pow(1920, 2) + pow(1080, 2))), m_modelManager(modelManager), m_playerCenter(glm::vec2(0.045f, 0.0f)), m_playerDirection(glm::vec2(0.0f, 1.0f))
    {
        m_playerModel = {
        Core::Vertex(glm::vec3(0.045f, 0.09f, -1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)),
        Core::Vertex(glm::vec3(0.045f, -0.09f, -1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)),
        Core::Vertex(glm::vec3(-0.045f, -0.09f, -1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)),
        Core::Vertex(glm::vec3(-0.045f, 0.09f, -1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f))
        };
        std::vector<unsigned int> indices = {
            0, 1, 3,
            1, 2, 3
        };

        modelManager->CreateModel("player", m_playerModel, indices);

        m_playerTexture = textureLoader.LoadTexture("textures/Player1-Equiped-96x96.png", true);
    }

    void Player::Render(unsigned int program)
    {
        glUseProgram(program);
        if (m_playerNewPosition.x != 0)
        {
            if (m_playerNewPosition.x > m_speedX)
            {
                m_playerNewPosition.x -= m_speedX;
                std::for_each(m_playerModel.begin(), m_playerModel.end(), [&](auto &vertex)
                {
                    vertex.position.x += m_speedX;
                });
            }
            else if(m_playerNewPosition.x < -m_speedX)
            {
                m_playerNewPosition.x += m_speedX;
                std::for_each(m_playerModel.begin(), m_playerModel.end(), [&](auto &vertex)
                {
                    vertex.position.x -= m_speedX;
                });
            }
            else if(m_playerNewPosition.x >=0)
            {
                std::for_each(m_playerModel.begin(), m_playerModel.end(), [&](auto &vertex)
                {
                    vertex.position.x += m_playerNewPosition.x;
                });
                m_playerNewPosition.x = 0;
            }
            else
            {
                std::for_each(m_playerModel.begin(), m_playerModel.end(), [&](auto &vertex)
                {
                    vertex.position.x -= m_playerNewPosition.x;
                });
                m_playerNewPosition.x = 0;
            }
        }
        if (m_playerNewPosition.y != 0)
        {
            if (m_playerNewPosition.y > m_speedY)
            {
                m_playerNewPosition.y -= m_speedY;
                std::for_each(m_playerModel.begin(), m_playerModel.end(), [&](auto &vertex)
                {
                    vertex.position.y += m_speedY;
                });
            }
            else if (m_playerNewPosition.y < -m_speedY)
            {
                m_playerNewPosition.y += m_speedY;
                std::for_each(m_playerModel.begin(), m_playerModel.end(), [&](auto &vertex)
                {
                    vertex.position.y -= m_speedY;
                });
            }
            else if (m_playerNewPosition.y > 0)
            {
                std::for_each(m_playerModel.begin(), m_playerModel.end(), [&](auto &vertex)
                {
                    vertex.position.y += m_playerNewPosition.y;
                });
                m_playerNewPosition.y = 0;
            }
            else
            {
                std::for_each(m_playerModel.begin(), m_playerModel.end(), [&](auto &vertex)
                {
                    vertex.position.y -= m_playerNewPosition.y;
                });
                m_playerNewPosition.y = 0;
            }
        }
        /*float fovy = glm::radians(45.0f);
        float aspectRatio = float(1920) / float(1080);
        float nearPlane = 0.01f;
        float farPlane = 300.0f;
        glm::mat4 projectionMatrix = glm::perspective(fovy, aspectRatio, nearPlane, farPlane);
        Core::ShaderLoader::setMat4(program, "projection", projectionMatrix);*/
        m_modelManager->UpdateModel("player", m_playerModel);
        UpdateCenter();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_playerTexture);
        glBindVertexArray(m_modelManager->GetModel("player"));
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    void Player::UpdatePosition(const float &x, const float &y)
    {
        float newX = x / 1920;
        float newY = y / 1080;

        newX = newX * 2 - 1;
        newY = newY * 2 - 1;

        float translateX = newX - m_playerCenter.x;
        float translateY = newY - m_playerCenter.y;

        // absolute values to simplify the computation before rendering
        float translateXAbs = abs(x - ((m_playerCenter.x + 1) / 2 * 1920));
        float translateYAbs = abs(y - ((m_playerCenter.y + 1) / 2 * 1080));

        float trianLenght = sqrt(pow(translateXAbs, 2) + pow(translateYAbs, 2));
        float speedProportion = m_speed / trianLenght;
        m_speedX = translateXAbs / 1920 * 2 / 2000 * speedProportion;
        m_speedY = translateYAbs / 1080 * 2 / 2000 * speedProportion;

        //std::cout << sqrt(pow(translateXAbs * speedProportion, 2) + pow(translateYAbs * speedProportion, 2)) << std::endl;
        m_playerNewPosition = glm::vec2(translateX, translateY);

        glm::vec3 translateToOrigin(m_playerCenter - glm::vec2(0.0f, 0.0f), 0.0f);
        // rotate points
        glm::vec2 translateNormalized = glm::normalize(glm::vec2(translateX, translateY));

        // skuska
        float cross = translateNormalized.x * m_playerDirection.y - translateNormalized.y - m_playerDirection.x;
        float sinAngle = cross / (sqrt((pow(m_playerDirection.x, 2) + pow(translateNormalized.x, 2)) * (pow(m_playerDirection.y, 2) + pow(translateNormalized.y, 2))));
        float anglebla = asin(sinAngle);

        // how to compute angle between two vectors with direction defined
        // https://stackoverflow.com/questions/14066933/direct-way-of-computing-clockwise-angle-between-2-vectors
        float cosAngle = glm::dot(m_playerDirection, translateNormalized);
        float determinant = (m_playerDirection.x * translateNormalized.y) - (m_playerDirection.y * translateNormalized.x);
        float arcustangent = atan2f(determinant, cosAngle);
        float angle = acos(cosAngle);
        glm::mat4 R(1.0f);
        R = glm::rotate(R, arcustangent, glm::vec3(0.0f, 0.0f, 1.0f));
        for (auto &vertex : m_playerModel)
        {
            vertex.position.x -= translateToOrigin.x;
            vertex.position.y -= translateToOrigin.y;
            vertex.position.x *= 1920;
            vertex.position.y *= 1080;
            vertex.position = R * glm::vec4(vertex.position, 1.0f);
            vertex.position.x /= 1920;
            vertex.position.y /= 1080;
            vertex.position.x += translateToOrigin.x;
            vertex.position.y += translateToOrigin.y;
        }
        m_playerDirection = translateNormalized;
    }

    void Player::UpdateCenter()
    {
        float max = std::max<float>(m_playerModel[0].position.x, m_playerModel[1].position.x);
        max = std::max<float>(max, m_playerModel[2].position.x);
        max = std::max<float>(max, m_playerModel[3].position.x);
        float min = std::min<float>(m_playerModel[0].position.x, m_playerModel[1].position.x);
        min = std::min<float>(min, m_playerModel[2].position.x);
        min = std::min<float>(min, m_playerModel[3].position.x);
        float diff = (max - min) / 2;
        m_playerCenter.x = abs(diff) + min;

        max = std::max<float>(m_playerModel[0].position.y, m_playerModel[1].position.y);
        max = std::max<float>(max, m_playerModel[2].position.y);
        max = std::max<float>(max, m_playerModel[3].position.y);
        min = std::min<float>(m_playerModel[0].position.y, m_playerModel[1].position.y);
        min = std::min<float>(min, m_playerModel[2].position.y);
        min = std::min<float>(min, m_playerModel[3].position.y);
        diff = (max - min) / 2;
        m_playerCenter.y = abs(diff) + min;
    }
}