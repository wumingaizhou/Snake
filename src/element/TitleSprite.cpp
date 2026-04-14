#include <iostream>

#include "element/TitleSprite.h"
#include "Game.h"

using namespace sfSnake;

TitleSprite::TitleSprite()
    : titleTexture_(), titleSprite_(titleTexture_), rotateDirection_(true)
{
    if (titleTexture_.loadFromFile("assets/image/logo.png"))
    {
        titleTexture_.setSmooth(true);
        titleSprite_.setTexture(titleTexture_, true);
    }
    else
    {
        std::cerr << "Failed to load title texture: assets/image/logo.png\n";
    }

    sf::FloatRect titleSpriteBounds = setOriginMiddle(titleSprite_);
    if (titleSpriteBounds.size.x > 0.f)
    {
        const float titleScale = Game::GlobalVideoMode.size.x / titleSpriteBounds.size.x * 0.5f;
        titleSprite_.setScale({titleScale, titleScale});
    }
    titleSprite_.setPosition(
        {Game::GlobalVideoMode.size.x / 2.0f,
         Game::GlobalVideoMode.size.y / 4.0f});
}

void TitleSprite::update(sf::Time delta)
{
    if (rotateDirection_)
    {
        titleSprite_.rotate(sf::degrees(delta.asSeconds()));
        const float rotation = titleSprite_.getRotation().asDegrees();
        if (rotation >= 10.0f && rotation <= 11.0f)
            rotateDirection_ = false;
    }
    else
    {
        titleSprite_.rotate(sf::degrees(-delta.asSeconds()));
        const float rotation = titleSprite_.getRotation().asDegrees();
        if (rotation >= 349.0f && rotation <= 350.0f)
            rotateDirection_ = true;
    }
}

void TitleSprite::render(sf::RenderWindow &window)
{
    window.draw(titleSprite_);
}
