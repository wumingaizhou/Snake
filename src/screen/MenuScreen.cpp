#include <SFML/Graphics.hpp>

/*
 * 模块说明：
 * 该文件实现主菜单界面的布局、交互和排行榜显示。
 */

#include <iostream>
#include <memory>
#include <string>

#include "Game.h"
#include "HighScoreStorage.h"
#include "screen/GameScreen.h"
#include "screen/MenuScreen.h"
#include "screen/OptionScreen.h"
#include "screen/HelpScreen.h"
#include "screen/AboutScreen.h"

using namespace sfSnake;

MenuScreen::MenuScreen()
    : button_(3),
      helpButton_(),
      aboutButton_(),
      highScoresText_(Game::GlobalFont),
      topScores_(loadTopScores()),
      highScoreRefreshElapsed_(sf::Time::Zero)
{
    // 全局字体在菜单页首次加载，供后续各个界面复用。
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

    highScoresText_.setFont(Game::GlobalFont);
    highScoresText_.setCharacterSize(Game::GlobalVideoMode.size.x / 32.0f);
    highScoresText_.setFillColor(Game::Color::Yellow);
    highScoresText_.setOutlineThickness(Game::GlobalVideoMode.size.x / 500.0f);
    highScoresText_.setOutlineColor(sf::Color(0x1c2833dd));
    // 启动时先构建一次排行榜文本，避免界面初次显示为空。
    refreshHighScores();
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
            // 进入设置页前暂存当前页面，方便返回时回到菜单。
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
                // 首次开始游戏前，先展示帮助页，再进入真正的游戏界面。
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

    // 排行榜按秒刷新，既能及时看到新纪录，也不会造成无意义的频繁 I/O。
    highScoreRefreshElapsed_ += delta;
    if (highScoreRefreshElapsed_ >= sf::seconds(1.f))
    {
        highScoreRefreshElapsed_ = sf::Time::Zero;
        topScores_ = loadTopScores();
        refreshHighScores();
    }
}

void MenuScreen::render(sf::RenderWindow &window)
{
    Game::GlobalTitle.render(window);
    window.draw(highScoresText_);
    for (auto &button : button_)
        button.render(window);
    helpButton_.render(window);
    aboutButton_.render(window);
}

void MenuScreen::refreshHighScores()
{
    // 右上角始终固定显示前三名，不足的位置用 -- 补齐。
    std::wstring text = L"历史前三\n";
    for (std::size_t index = 0; index < 3; ++index)
    {
        text += std::to_wstring(index + 1);
        text += L". ";
        if (index < topScores_.size())
            text += std::to_wstring(topScores_[index]);
        else
            text += L"--";

        if (index + 1 < 3)
            text += L"\n";
    }

    highScoresText_.setString(text);
    const sf::FloatRect bounds = highScoresText_.getLocalBounds();
    highScoresText_.setOrigin(
        {bounds.position.x + bounds.size.x, bounds.position.y});
    highScoresText_.setPosition(
        {Game::GlobalVideoMode.size.x - Game::GlobalVideoMode.size.x / 20.0f,
         Game::GlobalVideoMode.size.y / 15.0f});
}
