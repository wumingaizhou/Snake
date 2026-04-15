#pragma once

/*
 * 模块说明：
 * Button 封装了一个基于贴图的圆形按钮，负责加载图片、定位、
 * 焦点高亮以及鼠标命中检测。
 */

#include <SFML/Graphics.hpp>
#include "Game.h"

namespace sfSnake
{
    class Button
    {
    private:
        // 按钮贴图资源。
        sf::Texture texture_;
        // 用于绘制按钮的精灵。
        sf::Sprite sprite_;
        // 记录按钮当前是否处于高亮状态。
        bool focused_;

    public:
        Button();
        // 根据指定图片更新按钮外观，并按窗口大小自动缩放。
        void update(std::string filename, float scale = 1 / 8.0f);
        // 设置按钮中心点位置。
        void setPosition(float x, float y);

        // 修改按钮是否高亮。
        void focused(bool status);
        // 获取按钮高亮状态。
        bool focused() const;

        template <typename T>
        bool contain(sf::Vector2<T>) const;

        // 将按钮绘制到窗口中。
        void render(sf::RenderWindow &window) const;
    };

    template <typename T>
    bool Button::contain(sf::Vector2<T> point) const
    {
        // 按钮资源近似为圆形，因此使用“点到中心距离”判断是否命中。
        return dis(point, sprite_.getPosition()) < sprite_.getGlobalBounds().size.x / 2.0f;
    }
}
