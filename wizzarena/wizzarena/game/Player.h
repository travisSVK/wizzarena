/*
* @file Player.h
* @author Marek Cernak
* @date 2018/12/4
*/

#include <memory>
#include <glm.hpp>
#include <vector>
#include "../core/Vertex.h"

namespace Core
{
    class Models;
    class TextureLoader;
}

namespace Game
{
    class Player 
    {
    public:
        Player(const std::shared_ptr<Core::Models> &modelManager, Core::TextureLoader &textureLoader);
        void Render(unsigned int program);
        void UpdatePosition(const float &x, const float &y);

    private:
        void UpdateCenter();

    private:
        unsigned int m_playerTexture;
        std::vector<Core::Vertex> m_playerModel;
        glm::vec2 m_playerNewPosition;
        std::shared_ptr<Core::Models> m_modelManager;
        float m_speed;
        float m_speedX;
        float m_speedY;
        glm::vec2 m_playerCenter;
        glm::vec2 m_playerDirection;
    };
}