#include <iostream>

/*
 * 模块说明：
 * 该文件实现标题图片的资源加载、缩放定位和摆动动画。
 */

#include "element/TitleSprite.h"
#include "Game.h"

using namespace sfSnake;

TitleSprite::TitleSprite()
    : titleTexture_(), titleSprite_(titleTexture_), rotateDirection_(true)
{
    // 标题贴图丢失时仅提示错误，避免资源问题直接导致程序崩溃。
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
        // 标题宽度大约占窗口宽度的一半，方便在菜单页居中展示。
        const float titleScale = Game::GlobalVideoMode.size.x / titleSpriteBounds.size.x * 0.5f;
        titleSprite_.setScale({titleScale, titleScale});
    }
    titleSprite_.setPosition(
        {Game::GlobalVideoMode.size.x / 2.0f,
         Game::GlobalVideoMode.size.y / 4.0f});
}

void TitleSprite::update(sf::Time delta)
{
    // 让标题在一个很小的角度范围内来回摆动，增加动态感。
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
