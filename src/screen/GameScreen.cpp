#include <SFML/Graphics.hpp>

#include <random>
#include <memory>
#include <iostream>

#include "screen/GameScreen.h"
#include "screen/GameOverScreen.h"
#include "screen/PauseScreen.h"
#include "screen/HelpScreen.h"

using namespace sfSnake;

GameScreen::GameScreen()
    : snake_(), grid_(), pauseButton_(), score_(Game::GlobalFont)
{
    pauseButton_.update("assets/image/pauseUI.png", 1 / 16.0f);
    pauseButton_.setPosition(
        Game::GlobalVideoMode.size.x / 15.0f * 14.0f,
        Game::GlobalVideoMode.size.x / 15.0f);

    score_.setFont(Game::GlobalFont);
    score_.setString(sf::String(L"分数:") + std::to_string(snake_.getScore()));
    score_.setCharacterSize(Game::GlobalVideoMode.size.x / 25.0f);
    score_.setFillColor(Game::Color::Yellow);
    setOriginMiddle(score_);
    score_.setPosition(
        {Game::GlobalVideoMode.size.x / 2.0f,
         Game::GlobalVideoMode.size.x * 0.05f});
}

void GameScreen::handleInput(sf::RenderWindow &window)
{
    snake_.handleInput(window);

    auto mousePosition = sf::Mouse::getPosition(window);

    pauseButton_.focused(false);

    if (pauseButton_.contain(mousePosition))
    {
        pauseButton_.focused(true);
        if (
            !Game::mouseButtonLocked &&
            sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
        {
            Game::mouseButtonCDtime = sf::Time::Zero;
            Game::mouseButtonLocked = true;
            Game::TmpGameScreen = Game::MainScreen;
            Game::MainScreen = std::make_shared<PauseScreen>();
            return;
        }
    }
}

void GameScreen::update(sf::Time delta)
{
    while (fruit_.size() < 5)
        generateFruit();

    snake_.update(delta);
    snake_.checkFruitCollisions(fruit_);

    if (snake_.hitSelf())
    {
        Game::MainScreen = std::make_shared<GameOverScreen>(snake_.getScore());
        return;
    }

    score_.setString(sf::String(L"分数:\t") + std::to_string(snake_.getScore()));
}

void GameScreen::render(sf::RenderWindow &window)
{
    if (Game::GridVisibility)
        grid_.render(window);
    snake_.render(window);
    for (auto fruit : fruit_)
        fruit.render(window);
    pauseButton_.render(window);
    window.draw(score_);
}

void GameScreen::generateFruit()
{
    static std::default_random_engine engine(time(NULL));
    static std::default_random_engine colorEngine(time(NULL));

    static std::uniform_int_distribution<int> xPos(
        Game::GlobalVideoMode.size.x / 15.0f,
        Game::GlobalVideoMode.size.x -
            Game::GlobalVideoMode.size.x / 10.0f);

    static std::uniform_int_distribution<int> yPos(
        Game::GlobalVideoMode.size.x / 10.0f,
        Game::GlobalVideoMode.size.y -
            Game::GlobalVideoMode.size.x / 15.0f);

    static std::uniform_int_distribution<int> fruitColor(0, 7);

    switch (fruitColor(colorEngine))
    {
    case 0: // black
        fruit_.push_back(Fruit(
            sf::Vector2f(xPos(engine), yPos(engine)),
            Game::Color::Fruit[0],
            0));
        break;
    case 1: // brown
        fruit_.push_back(Fruit(
            sf::Vector2f(xPos(engine), yPos(engine)),
            Game::Color::Fruit[1],
            0));
        break;
    case 2:
    case 3: // red
        fruit_.push_back(Fruit(
            sf::Vector2f(xPos(engine), yPos(engine)),
            Game::Color::Fruit[2],
            3));
        break;
    case 4:
    case 5: // blue
        fruit_.push_back(Fruit(
            sf::Vector2f(xPos(engine), yPos(engine)),
            Game::Color::Fruit[3],
            2));
        break;
    case 6:
    case 7: // green
        fruit_.push_back(Fruit(
            sf::Vector2f(xPos(engine), yPos(engine)),
            Game::Color::Fruit[4],
            1));
        break;
    default:
        break;
    }
}
