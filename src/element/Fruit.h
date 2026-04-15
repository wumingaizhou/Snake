#pragma once

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
        sf::CircleShape shape_;
        int score_;
        FruitKind kind_;
        bool visible_;

        Fruit(
            sf::Vector2f position,
            sf::Color color,
            int score,
            FruitKind kind = FruitKind::Normal,
            float radiusScale = 1.0f);

        sf::Vector2f getPosition() const;
        float getRadius() const;
        void setVisible(bool visible);

        void render(sf::RenderWindow &window) const;
    };
}
