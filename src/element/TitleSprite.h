#pragma once

/*
 * 模块说明：
 * TitleSprite 负责主标题图片的加载、轻微摆动动画和渲染。
 */

#include <SFML/Graphics.hpp>

namespace sfSnake
{
    class TitleSprite
    {
    private:
        // 标题图像资源。
        sf::Texture titleTexture_;
        // 标题精灵。
        sf::Sprite titleSprite_;
        // 标记当前是向左还是向右摆动。
        bool rotateDirection_;

    public:
        TitleSprite();

        // 更新标题的小幅摇摆动画。
        void update(sf::Time delta);
        // 绘制标题。
        void render(sf::RenderWindow &window);
    };
}
