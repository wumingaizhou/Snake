#include "element/Button.h"
#include "Game.h"

#include <iostream>

using namespace sfSnake;

Button::Button()
    : sprite_(texture_), focused_{false}
{
}

void Button::update(std::string filename, float scale)
{
    if (!texture_.loadFromFile(filename))
    {
        std::cerr << "Failed to load button texture: " << filename << '\n';
        return;
    }
    texture_.setSmooth(true);
    sprite_.setTexture(texture_, true);
    sf::FloatRect bounds = setOriginMiddle(sprite_);
    if (bounds.size.x > 0.f)
    {
        const float buttonScale = Game::GlobalVideoMode.size.x / bounds.size.x * scale;
        sprite_.setScale({buttonScale, buttonScale});
    }
}

void Button::setPosition(float x, float y)
{
    sprite_.setPosition({x, y});
}

void Button::focused(bool status)
{
    focused_ = status;
    if (status)
        sprite_.setColor(sf::Color::Green);
    else
        sprite_.setColor(sf::Color::White);
}

bool Button::focused() const
{
    return focused_;
}

void Button::render(sf::RenderWindow &window) const
{
    window.draw(sprite_);
}
