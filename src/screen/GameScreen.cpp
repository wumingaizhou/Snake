#include <SFML/Graphics.hpp>

#include <algorithm>
#include <memory>
#include <random>

#include "screen/GameScreen.h"
#include "screen/GameOverScreen.h"
#include "screen/PauseScreen.h"
#include "screen/HelpScreen.h"

using namespace sfSnake;

namespace
{
constexpr std::size_t NormalFruitTargetCount = 5;
constexpr int BonusFruitScore = 6;
constexpr float BonusFruitRadiusScale = 1.8f;
const sf::Time BonusFruitLifetime = sf::seconds(8.f);
const sf::Time BonusFruitBlinkThreshold = sf::seconds(2.f);
} // namespace

GameScreen::GameScreen()
    : snake_(),
      fruit_(),
      bonusFruit_(std::nullopt),
      grid_(),
      pauseButton_(),
      score_(Game::GlobalFont),
      randomEngine_(std::random_device{}()),
      bonusFruitLifetime_(sf::Time::Zero),
      nextBonusFruitSpawn_(sf::Time::Zero)
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

    scheduleNextBonusFruit();
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
    while (fruit_.size() < NormalFruitTargetCount)
        generateFruit();

    updateBonusFruit(delta);
    snake_.update(delta);

    auto normalFruit = std::find_if(
        fruit_.begin(),
        fruit_.end(),
        [this](const Fruit &fruit)
        { return snake_.canEatFruit(fruit); });
    if (normalFruit != fruit_.end())
    {
        snake_.eatFruit(*normalFruit);
        fruit_.erase(normalFruit);
    }

    if (bonusFruit_ && snake_.canEatFruit(*bonusFruit_))
    {
        snake_.eatFruit(*bonusFruit_);
        bonusFruit_.reset();
        bonusFruitLifetime_ = sf::Time::Zero;
        scheduleNextBonusFruit();
    }

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
    for (const auto &fruit : fruit_)
        fruit.render(window);
    if (bonusFruit_)
        bonusFruit_->render(window);
    pauseButton_.render(window);
    window.draw(score_);
}

void GameScreen::generateFruit()
{
    static std::uniform_int_distribution<int> fruitColor(0, 7);

    const float radius = Game::GlobalVideoMode.size.x / 256.0f;
    const sf::Vector2f position = generateFruitPosition(radius);

    switch (fruitColor(randomEngine_))
    {
    case 0: // black
        fruit_.push_back(Fruit(
            position,
            Game::Color::Fruit[0],
            0));
        break;
    case 1: // brown
        fruit_.push_back(Fruit(
            position,
            Game::Color::Fruit[1],
            0));
        break;
    case 2:
    case 3: // red
        fruit_.push_back(Fruit(
            position,
            Game::Color::Fruit[2],
            3));
        break;
    case 4:
    case 5: // blue
        fruit_.push_back(Fruit(
            position,
            Game::Color::Fruit[3],
            2));
        break;
    case 6:
    case 7: // green
        fruit_.push_back(Fruit(
            position,
            Game::Color::Fruit[4],
            1));
        break;
    default:
        break;
    }
}

Fruit GameScreen::createBonusFruit()
{
    const float radius =
        Game::GlobalVideoMode.size.x / 256.0f * BonusFruitRadiusScale;
    return Fruit(
        generateFruitPosition(radius),
        sf::Color(0xff4d4fff),
        BonusFruitScore,
        FruitKind::Bonus,
        BonusFruitRadiusScale);
}

sf::Vector2f GameScreen::generateFruitPosition(float radius)
{
    std::uniform_real_distribution<float> xPos(
        Game::GlobalVideoMode.size.x / 15.0f + radius,
        Game::GlobalVideoMode.size.x -
            Game::GlobalVideoMode.size.x / 10.0f - radius);
    std::uniform_real_distribution<float> yPos(
        Game::GlobalVideoMode.size.x / 10.0f + radius,
        Game::GlobalVideoMode.size.y -
            Game::GlobalVideoMode.size.x / 15.0f - radius);

    sf::Vector2f position;
    for (int attempt = 0; attempt < 64; ++attempt)
    {
        position = sf::Vector2f(xPos(randomEngine_), yPos(randomEngine_));
        if (isPositionValid(position, radius))
            return position;
    }

    return position;
}

bool GameScreen::isPositionValid(const sf::Vector2f &position, float radius) const
{
    const float minHeadDistance = std::max(
        snake_.getHeadRadius() * 6.0f,
        radius * 8.0f);
    if (dis(position, snake_.getHeadPosition()) < minHeadDistance)
        return false;

    for (const auto &fruit : fruit_)
    {
        if (dis(position, fruit.getPosition()) <
            radius + fruit.getRadius() + Game::GlobalVideoMode.size.x / 80.0f)
            return false;
    }

    if (bonusFruit_ &&
        dis(position, bonusFruit_->getPosition()) <
            radius + bonusFruit_->getRadius() + Game::GlobalVideoMode.size.x / 80.0f)
        return false;

    return true;
}

void GameScreen::scheduleNextBonusFruit()
{
    std::uniform_real_distribution<float> intervalSeconds(8.f, 12.f);
    nextBonusFruitSpawn_ = sf::seconds(intervalSeconds(randomEngine_));
}

void GameScreen::updateBonusFruit(sf::Time delta)
{
    if (bonusFruit_)
    {
        bonusFruitLifetime_ -= delta;
        if (bonusFruitLifetime_ <= sf::Time::Zero)
        {
            bonusFruit_.reset();
            bonusFruitLifetime_ = sf::Time::Zero;
            scheduleNextBonusFruit();
            return;
        }

        if (bonusFruitLifetime_ <= BonusFruitBlinkThreshold)
        {
            const auto blinkSlice = bonusFruitLifetime_.asMilliseconds() / 150;
            bonusFruit_->setVisible(blinkSlice % 2 == 0);
        }
        else
        {
            bonusFruit_->setVisible(true);
        }
        return;
    }

    nextBonusFruitSpawn_ -= delta;
    if (nextBonusFruitSpawn_ <= sf::Time::Zero)
    {
        bonusFruit_ = createBonusFruit();
        bonusFruitLifetime_ = BonusFruitLifetime;
    }
}
