#include <SFML/Graphics.hpp>

/*
 * 模块说明：
 * 该文件实现实际游戏界面的更新流程，包括水果生成、奖励水果计时、
 * 分数显示、暂停入口以及游戏结束切换。
 */

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
    // 暂停按钮固定放在右上角，避免和蛇的主要活动区域重叠。
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

    // 开局就先安排一次奖励水果刷新计划。
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
            // 暂停时保留当前游戏页，返回后可以继续原局游戏。
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
    // 保持场上始终有固定数量的普通水果。
    while (fruit_.size() < NormalFruitTargetCount)
        generateFruit();

    updateBonusFruit(delta);
    snake_.update(delta);

    // 找到第一个被吃到的普通水果并移除。
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
        // 奖励水果被吃掉后，立即重新进入下一轮生成倒计时。
        snake_.eatFruit(*bonusFruit_);
        bonusFruit_.reset();
        bonusFruitLifetime_ = sf::Time::Zero;
        scheduleNextBonusFruit();
    }

    if (snake_.hitSelf())
    {
        // 一旦撞到自己，切换到结算界面并携带最终分数。
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

    // 使用简单的权重分布控制不同颜色水果的出现概率与分值。
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
    // 奖励水果比普通水果更大、更显眼，并且分数更高。
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
    // 给水果预留 UI 安全边距，避免刷到标题、分数或暂停按钮附近。
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
        // 尝试多次找到不冲突的位置；若始终失败，就返回最后一次结果。
        position = sf::Vector2f(xPos(randomEngine_), yPos(randomEngine_));
        if (isPositionValid(position, radius))
            return position;
    }

    return position;
}

bool GameScreen::isPositionValid(const sf::Vector2f &position, float radius) const
{
    // 新水果不要离蛇头太近，避免一生成就被立刻吃掉。
    const float minHeadDistance = std::max(
        snake_.getHeadRadius() * 6.0f,
        radius * 8.0f);
    if (dis(position, snake_.getHeadPosition()) < minHeadDistance)
        return false;

    for (const auto &fruit : fruit_)
    {
        // 普通水果之间也保持一定间距，避免视觉上挤成一团。
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
    // 奖励水果在一个随机时间窗口内刷新，增加游戏节奏变化。
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
            // 超时未吃到就让奖励水果消失，并重新排下一轮刷新。
            bonusFruit_.reset();
            bonusFruitLifetime_ = sf::Time::Zero;
            scheduleNextBonusFruit();
            return;
        }

        if (bonusFruitLifetime_ <= BonusFruitBlinkThreshold)
        {
            // 临近消失时闪烁，提醒玩家抓紧吃掉。
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
        // 倒计时结束后正式生成奖励水果。
        bonusFruit_ = createBonusFruit();
        bonusFruitLifetime_ = BonusFruitLifetime;
    }
}
