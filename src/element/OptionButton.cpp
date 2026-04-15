#include "element/OptionButton.h"

/*
 * 模块说明：
 * 该文件实现文字按钮的状态样式切换与绘制。
 */

#include "Game.h"
#include <iostream>

using namespace sfSnake;

OptionButton::OptionButton()
    : text_(Game::GlobalFont)
{
}

void OptionButton::settings(
    sf::String content,
    sf::Font &font,
    float characterSize,
    sf::Color color,
    sf::Vector2f position)
{
    // 这里不在构造函数里固定内容，而是让同一个组件类可以复用于多种设置项。
    text_.setFont(font);
    text_.setString(content);
    text_.setCharacterSize(characterSize);
    text_.setFillColor(color);
    setOriginMiddle(text_);
    text_.setPosition(position);

    originColor_ = color;
}

void OptionButton::clear()
{
    // 恢复到默认文字颜色和普通样式。
    text_.setStyle(sf::Text::Regular);
    text_.setFillColor(originColor_);
}

void OptionButton::focused()
{
    // 鼠标悬停时只加下划线，不改变已选中颜色。
    text_.setStyle(sf::Text::Underlined);
}

void OptionButton::seleted()
{
    // 已选中项使用醒目的黄色提示当前配置。
    text_.setFillColor(Game::Color::Yellow);
}

void OptionButton::render(sf::RenderWindow &window)
{
    window.draw(text_);
}
