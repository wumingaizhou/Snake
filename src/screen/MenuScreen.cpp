#include <SFML/Graphics.hpp>

#include <memory>
#include <iostream>

#include "Game.h"
#include "screen/GameScreen.h"
#include "screen/MenuScreen.h"
#include "screen/OptionScreen.h"
#include "screen/HelpScreen.h"
#include "screen/AboutScreen.h"

using namespace sfSnake;

MenuScreen::MenuScreen()
    : button_(3)
{
    if (!Game::GlobalFont.openFromFile("assets/fonts/SourceHanSansSC-Bold.otf"))
        std::cerr << "Failed to load font: assets/fonts/SourceHanSansSC-Bold.otf\n";

    button_[0].update("assets/image/optionUI.png");
    button_[1].update("assets/image/startUI.png");
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

void MenuScreen::handleInput(sf::RenderWindow &window)
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
            Game::mouseButtonLocked = true;
            Game::mouseButtonCDtime = sf::Time::Zero;
            if (!Game::ifShowedHelp)
            {
                Game::TmpScreen = std::make_shared<GameScreen>();
                Game::MainScreen = std::make_shared<HelpScreen>();
            }
            else
            {
                Game::MainScreen = std::make_shared<GameScreen>();
            }
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

void MenuScreen::update(sf::Time delta)
{
    Game::GlobalTitle.update(delta);
}

void MenuScreen::render(sf::RenderWindow &window)
{
    Game::GlobalTitle.render(window);
    for (auto &button : button_)
        button.render(window);
    helpButton_.render(window);
    aboutButton_.render(window);
}
