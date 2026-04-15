#include <SFML/Graphics.hpp>

/*
 * 模块说明：
 * 该文件实现关于页面的文本展示和返回逻辑。
 */

#include <memory>
#include <iostream>

#include "screen/GameScreen.h"
#include "screen/AboutScreen.h"
#include "screen/OptionScreen.h"
#include "Game.h"

using namespace sfSnake;

AboutScreen::AboutScreen()
    : text_(Game::GlobalFont)
{
    text_.setFont(Game::GlobalFont);
    text_.setString(
        sf::String(L"软件工程结构化设计设计作业\n\n") +
        sf::String(L"原作者：精益君(JingYiJun3104@outlook.com)\n\n") +
        sf::String(L"The MIT License (MIT) Copyright © 2022 <JingYiJun>"));
    text_.setCharacterSize(Game::GlobalVideoMode.size.x / 35.0f);
    text_.setFillColor(Game::Color::Green);
    setOriginMiddle(text_);
    text_.setPosition(
        {Game::GlobalVideoMode.size.x / 2.0f,
         Game::GlobalVideoMode.size.y / 5.0f * 3.0f});

    returnButton_.update("assets/image/returnUI.png", 1 / 16.0f);
    returnButton_.setPosition(
        Game::GlobalVideoMode.size.x / 15.0f,
        Game::GlobalVideoMode.size.x / 15.0f);
}

void AboutScreen::handleInput(sf::RenderWindow &window)
{
    const auto mousePosition = sf::Mouse::getPosition(window);

    returnButton_.focused(false);
    if (returnButton_.contain(mousePosition))
    {
        returnButton_.focused(true);
        if (!Game::mouseButtonLocked && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
        {
            // 回到进入关于页之前的界面。
            Game::mouseButtonCDtime = sf::Time::Zero;
            Game::mouseButtonLocked = true;
            Game::MainScreen = Game::TmpScreen;
            Game::TmpScreen = nullptr;
            return;
        }
    }
}

void AboutScreen::update(sf::Time delta)
{
    Game::GlobalTitle.update(delta);
}

void AboutScreen::render(sf::RenderWindow &window)
{
    Game::GlobalTitle.render(window);
    returnButton_.render(window);
    window.draw(text_);
}
