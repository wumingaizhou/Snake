#include <SFML/Graphics.hpp>
#include <cmath>

/*
 * 模块说明：
 * 该文件实现水果对象的构造、可见性控制和渲染效果。
 */

#include "Fruit.h"
#include "Game.h"

using namespace sfSnake;

Fruit::Fruit(
    sf::Vector2f position,
    sf::Color color,
    int score,
    FruitKind kind,
    float radiusScale)
    : score_(score), kind_(kind), visible_(true)
{
    // 水果大小按窗口宽度换算，保证不同分辨率下仍有相似视觉比例。
    shape_.setRadius(Game::GlobalVideoMode.size.x / 256.0f * radiusScale);
    setOriginMiddle(shape_);
    shape_.setPosition(position);
    shape_.setFillColor(color);

    if (kind_ == FruitKind::Bonus)
    {
        // 奖励水果额外绘制描边，便于和普通水果区分。
        shape_.setOutlineThickness(Game::GlobalVideoMode.size.x / 400.0f);
        shape_.setOutlineColor(sf::Color(0xf1c40fff));
    }
}

sf::Vector2f Fruit::getPosition() const
{
    return shape_.getPosition();
}

float Fruit::getRadius() const
{
    return shape_.getRadius();
}

void Fruit::setVisible(bool visible)
{
    visible_ = visible;
}

void Fruit::render(sf::RenderWindow &window) const
{
    if (!visible_)
        return;

    window.draw(shape_);

    if (kind_ == FruitKind::Bonus)
    {
        // 奖励水果外围的脉冲圆环只在渲染时临时生成，用来强化“限时奖励”提示。
        static sf::Clock pulseClock;
        const float phase = pulseClock.getElapsedTime().asSeconds() * 6.0f;
        const float pulse = 0.5f + 0.5f * std::sin(phase);

        sf::CircleShape outerRing(shape_.getRadius() * (1.35f + pulse * 0.2f));
        setOriginMiddle(outerRing);
        outerRing.setPosition(shape_.getPosition());
        outerRing.setFillColor(sf::Color::Transparent);
        outerRing.setOutlineThickness(Game::GlobalVideoMode.size.x / 500.0f);
        outerRing.setOutlineColor(sf::Color(
            241,
            196,
            15,
            static_cast<std::uint8_t>(140 + pulse * 90)));
        window.draw(outerRing);
    }
}
