#include <SFML/Graphics.hpp>

/*
 * 模块说明：
 * 该文件实现结算界面的展示和交互逻辑。
 */

#include <iostream>
#include <memory>

#include "Game.h"
#include "HighScoreStorage.h"
#include "screen/GameScreen.h"
#include "screen/GameOverScreen.h"
#include "screen/HelpScreen.h"
#include "screen/AboutScreen.h"

using namespace sfSnake;

GameOverScreen::GameOverScreen(std::size_t score)
    : text_(Game::GlobalFont), button_(3), score_(score)
{
    // 进入结算页时立即记录本局得分，供菜单页排行榜显示。
    recordScore(score_);

    text_.setFont(Game::GlobalFont);
    text_.setString(sf::String(L"最终分数：") + std::to_string(score));
    text_.setFillColor(Game::Color::Yellow);
    text_.setCharacterSize(Game::GlobalVideoMode.size.x / 15);
    setOriginMiddle(text_);
    text_.setPosition(
        {Game::GlobalVideoMode.size.x / 2.0f,
         Game::GlobalVideoMode.size.y / 4.0f});

    button_[0].update("assets/image/optionUI.png");
    button_[1].update("assets/image/restartUI.png");
    button_[2].update("assets/image/exitUI.png");

    button_[0].setPosition(Game::GlobalVideoMode.size.x / 3.0f, Game::GlobalVideoMode.size.y / 5.0f * 3.0f);
    button_[1].setPosition(Game::GlobalVideoMode.size.x / 2.0f, Game::GlobalVideoMode.size.y / 5.0f * 3.0f);
    button_[2].setPosition(Game::GlobalVideoMode.size.x / 3.0f * 2.0f, Game::GlobalVideoMode.size.y / 5.0f * 3.0f);

    helpButton_.settings(
        L"帮助",
        Game::GlobalFont,
        Game::GlobalVideoMode.size.x / 20.0f,
        Game::Color::Green,
        sf::Vector2f(Game::GlobalVideoMode.size.x / 5.0f * 2.0f, Game::GlobalVideoMode.size.y / 5.0f * 4.0f));

    aboutButton_.settings(
        L"关于",
        Game::GlobalFont,
        Game::GlobalVideoMode.size.x / 20.0f,
        Game::Color::Green,
        sf::Vector2f(Game::GlobalVideoMode.size.x / 5.0f * 3.0f, Game::GlobalVideoMode.size.y / 5.0f * 4.0f));
}

void GameOverScreen::handleInput(sf::RenderWindow &window)
{
    static sf::Vector2i mousePosition;
    mousePosition = sf::Mouse::getPosition(window);

    for (auto &i : button_)
        i.focused(false);
    helpButton_.clear();
    aboutButton_.clear();

    if (button_[0].contain(mousePosition))
    {
        button_[0].focused(true);
        if (!Game::mouseButtonLocked && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
        {
            Game::mouseButtonCDtime = sf::Time::Zero;
            Game::mouseButtonLocked = true;
            Game::TmpScreen = Game::MainScreen;
            Game::MainScreen = std::make_shared<OptionScreen>();
            return;
        }
    }

    if (button_[1].contain(mousePosition))
    {
        button_[1].focused(true);
        if (!Game::mouseButtonLocked && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
        {
            // 直接重新开一局，不需要回菜单。
            Game::mouseButtonCDtime = Game::mouseButtonClock.restart();
            Game::mouseButtonLocked = true;
            Game::MainScreen = std::make_shared<GameScreen>();
            return;
        }
    }

    if (button_[2].contain(mousePosition))
    {
        button_[2].focused(true);
        if (!Game::mouseButtonLocked && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
        {
            window.close();
            return;
        }
    }

    if (helpButton_.contains(mousePosition))
    {
        helpButton_.focused();
        if (!Game::mouseButtonLocked && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
        {
            helpButton_.seleted();
            Game::mouseButtonCDtime = sf::Time::Zero;
            Game::mouseButtonLocked = true;
            Game::TmpScreen = Game::MainScreen;
            Game::MainScreen = std::make_shared<HelpScreen>();
            return;
        }
    }

    if (aboutButton_.contains(mousePosition))
    {
        aboutButton_.focused();
        if (!Game::mouseButtonLocked && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
        {
            aboutButton_.seleted();
            Game::mouseButtonCDtime = sf::Time::Zero;
            Game::mouseButtonLocked = true;
            Game::TmpScreen = Game::MainScreen;
            Game::MainScreen = std::make_shared<AboutScreen>();
            return;
        }
    }
}

void GameOverScreen::update(sf::Time delta)
{
    // 结算页是静态页面，目前不需要逐帧更新逻辑。
}

void GameOverScreen::render(sf::RenderWindow &window)
{
    window.draw(text_);
    for (auto &button : button_)
        button.render(window);
    helpButton_.render(window);
    aboutButton_.render(window);
}
