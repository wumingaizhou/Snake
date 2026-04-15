#include "element/Button.h"
#include "Game.h"

/*
 * 模块说明：
 * 该文件实现图片按钮的资源加载、状态切换和渲染逻辑。
 */

#include <iostream>

using namespace sfSnake;

Button::Button()
    : sprite_(texture_), focused_{false}
{
}

void Button::update(std::string filename, float scale)
{
    // 贴图加载失败时保留原有状态，并输出错误信息便于定位资源问题。
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
        // 按窗口宽度换算按钮缩放，保证不同分辨率下大小比较一致。
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
    // 高亮时改成绿色，和项目中的交互色保持一致。
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
    // 按钮本身不处理层级，谁调用 render 谁决定显示顺序。
    window.draw(sprite_);
}
