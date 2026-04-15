#pragma once

/*
 * 模块说明：
 * Fruit 表示游戏中的水果对象，既支持普通水果，也支持限时出现的奖励水果。
 */

#include <SFML/Graphics.hpp>

namespace sfSnake
{
    enum class FruitKind
    {
        Normal,
        Bonus
    };

    struct Fruit
    {
        // 水果的可视化图形。
        sf::CircleShape shape_;
        // 吃到该水果后增长的分数。
        int score_;
        // 水果类型，用于区分普通与奖励效果。
        FruitKind kind_;
        // 用于控制奖励水果闪烁显示。
        bool visible_;

        Fruit(
            sf::Vector2f position,
            sf::Color color,
            int score,
            FruitKind kind = FruitKind::Normal,
            float radiusScale = 1.0f);

        // 返回水果圆心位置。
        sf::Vector2f getPosition() const;
        // 返回水果半径，供碰撞检测使用。
        float getRadius() const;
        // 设置水果当前是否可见。
        void setVisible(bool visible);

        // 将水果绘制到窗口中。
        void render(sf::RenderWindow &window) const;
    };
}
