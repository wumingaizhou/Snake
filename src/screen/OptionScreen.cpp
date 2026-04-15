#include "screen/OptionScreen.h"

/*
 * 模块说明：
 * 该文件实现设置页面的选项布局、悬停反馈和配置切换。
 */

#include "Game.h"
#include <iostream>

using namespace sfSnake;

OptionScreen::OptionScreen()
    : optionName_(3),
      GridShowOptions_(2), GridColorOptions_(3), BGColorOptions_(3),
      GridShowfocused_(-1), GridColorfocused_(-1), BGColorfocused_(-1),
      grid_(), returnButton_()
{
    float characterSize = Game::GlobalVideoMode.size.x / 25.f;

    // 初始化三类设置项的标题文字。
    optionName_[0].settings(
        L"网格状态：",
        Game::GlobalFont,
        characterSize,
        Game::Color::Green,
        sf::Vector2f(Game::GlobalVideoMode.size.x / 5.0f, Game::GlobalVideoMode.size.y / 4.0f));

    optionName_[1].settings(
        L"网格颜色：",
        Game::GlobalFont,
        characterSize,
        Game::Color::Green,
        sf::Vector2f(Game::GlobalVideoMode.size.x / 5.0f, Game::GlobalVideoMode.size.y / 2.0f));

    optionName_[2].settings(
        L"背景颜色：",
        Game::GlobalFont,
        characterSize,
        Game::Color::Green,
        sf::Vector2f(Game::GlobalVideoMode.size.x / 5.0f, Game::GlobalVideoMode.size.y / 4.0f * 3.0f));

    // 网格显示开关。
    GridShowOptions_[0].settings(
        L"关闭",
        Game::GlobalFont,
        characterSize,
        Game::Color::NotSeleted,
        sf::Vector2f(Game::GlobalVideoMode.size.x / 15.0f * 7.0f, Game::GlobalVideoMode.size.y / 4.0f));
    GridShowOptions_[1].settings(
        L"开启",
        Game::GlobalFont,
        characterSize,
        Game::Color::NotSeleted,
        sf::Vector2f(Game::GlobalVideoMode.size.x / 15.0f * 11.0f, Game::GlobalVideoMode.size.y / 4.0f));

    // 网格颜色选项。
    GridColorOptions_[0].settings(
        L"白色",
        Game::GlobalFont,
        characterSize,
        Game::Color::NotSeleted,
        sf::Vector2f(Game::GlobalVideoMode.size.x / 5.0f * 2.0f, Game::GlobalVideoMode.size.y / 2.0f));
    GridColorOptions_[1].settings(
        L"黑色",
        Game::GlobalFont,
        characterSize,
        Game::Color::NotSeleted,
        sf::Vector2f(Game::GlobalVideoMode.size.x / 5.0f * 3.0f, Game::GlobalVideoMode.size.y / 2.0f));
    GridColorOptions_[2].settings(
        L"棕色",
        Game::GlobalFont,
        characterSize,
        Game::Color::NotSeleted,
        sf::Vector2f(Game::GlobalVideoMode.size.x / 5.0f * 4.0f, Game::GlobalVideoMode.size.y / 2.0f));

    // 背景颜色选项。
    BGColorOptions_[0].settings(
        L"白色",
        Game::GlobalFont,
        characterSize,
        Game::Color::NotSeleted,
        sf::Vector2f(Game::GlobalVideoMode.size.x / 5.0f * 2.0f, Game::GlobalVideoMode.size.y / 4.0f * 3.0f));
    BGColorOptions_[1].settings(
        L"黑色",
        Game::GlobalFont,
        characterSize,
        Game::Color::NotSeleted,
        sf::Vector2f(Game::GlobalVideoMode.size.x / 5.0f * 3.0f, Game::GlobalVideoMode.size.y / 4.0f * 3.0f));
    BGColorOptions_[2].settings(
        L"棕色",
        Game::GlobalFont,
        characterSize,
        Game::Color::NotSeleted,
        sf::Vector2f(Game::GlobalVideoMode.size.x / 5.0f * 4.0f, Game::GlobalVideoMode.size.y / 4.0f * 3.0f));

    returnButton_.update("assets/image/returnUI.png", 1 / 16.0f);
    returnButton_.setPosition(Game::GlobalVideoMode.size.x / 15.0f, Game::GlobalVideoMode.size.x / 15.0f);
}

void OptionScreen::handleInput(sf::RenderWindow &window)
{
    auto mousePosition = sf::Mouse::getPosition(window);
    // 每一帧都重新计算悬停项，未命中的项会保持 -1。
    GridShowfocused_ = -1;
    GridColorfocused_ = -1;
    BGColorfocused_ = -1;

    if (GridShowOptions_[0].contains(mousePosition))
    {
        GridShowfocused_ = 0;
        if (!Game::mouseButtonLocked && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
        {
            // 设置页中的切换属于即时生效，不需要离开页面后再统一保存。
            Game::GridVisibility = 0;
            return;
        }
    }
    if (GridShowOptions_[1].contains(mousePosition))
    {
        GridShowfocused_ = 1;
        if (!Game::mouseButtonLocked && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
        {
            Game::GridVisibility = 1;
            return;
        }
    }

    if (GridColorOptions_[0].contains(mousePosition))
    {
        GridColorfocused_ = 0;
        if (!Game::mouseButtonLocked && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
        {
            Game::GridColor = 0;
            return;
        }
    }
    if (GridColorOptions_[1].contains(mousePosition))
    {
        GridColorfocused_ = 1;
        if (!Game::mouseButtonLocked && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
        {
            Game::GridColor = 1;
            return;
        }
    }
    if (GridColorOptions_[2].contains(mousePosition))
    {
        GridColorfocused_ = 2;
        if (!Game::mouseButtonLocked && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
        {
            Game::GridColor = 2;
            return;
        }
    }

    if (BGColorOptions_[0].contains(mousePosition))
    {
        BGColorfocused_ = 0;
        if (!Game::mouseButtonLocked && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
        {
            Game::BackgroundColor = 0;
            return;
        }
    }
    if (BGColorOptions_[1].contains(mousePosition))
    {
        BGColorfocused_ = 1;
        if (!Game::mouseButtonLocked && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
        {
            Game::BackgroundColor = 1;
            return;
        }
    }
    if (BGColorOptions_[2].contains(mousePosition))
    {
        BGColorfocused_ = 2;
        if (!Game::mouseButtonLocked && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
        {
            Game::BackgroundColor = 2;
            return;
        }
    }

    returnButton_.focused(false);
    if (returnButton_.contain(mousePosition))
    {
        returnButton_.focused(true);
        if (!Game::mouseButtonLocked && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
        {
            Game::mouseButtonCDtime = sf::Time::Zero;
            Game::mouseButtonLocked = true;
            Game::MainScreen = Game::TmpScreen;
            Game::TmpScreen = nullptr;
            return;
        }
    }
}

void OptionScreen::update(sf::Time delta)
{
    // 先清空全部临时样式，再根据“当前选中项”和“当前悬停项”重新上色。
    for (auto &i : GridShowOptions_)
        i.clear();

    for (auto &i : GridColorOptions_)
        i.clear();

    for (auto &i : BGColorOptions_)
        i.clear();

    GridShowOptions_[Game::GridVisibility].seleted();
    if (GridShowfocused_ != -1)
        GridShowOptions_[GridShowfocused_].focused();

    GridColorOptions_[Game::GridColor].seleted();
    if (GridColorfocused_ != -1)
        GridColorOptions_[GridColorfocused_].focused();

    BGColorOptions_[Game::BackgroundColor].seleted();
    if (BGColorfocused_ != -1)
        BGColorOptions_[BGColorfocused_].focused();
}

void OptionScreen::render(sf::RenderWindow &window)
{
    // 网格预览直接使用当前全局配置，所见即所得。
    if (Game::GridVisibility)
        grid_.render(window);
    for (auto &i : optionName_)
        i.render(window);
    for (auto &i : GridShowOptions_)
        i.render(window);
    for (auto &i : GridColorOptions_)
        i.render(window);
    for (auto &i : BGColorOptions_)
        i.render(window);

    returnButton_.render(window);
}
