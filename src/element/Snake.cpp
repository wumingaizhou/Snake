#include <SFML/Graphics.hpp>

/*
 * 模块说明：
 * 该文件实现贪吃蛇主体的行为逻辑，包括输入控制、移动、成长、
 * 穿墙、碰撞检测以及蛇身绘制。
 */

#include <memory>
#include <iostream>

#include "element/Snake.h"
#include "Game.h"
#include "element/Fruit.h"

#include "screen/GameOverScreen.h"

using namespace sfSnake;

const int Snake::InitialSize = 5;

Snake::Snake()
    : hitSelf_(false),
      speedup_(false),
      direction_(Direction(0, -1)),
      nodeRadius_(Game::GlobalVideoMode.size.x / 100.0f),
      tailOverlap_(0u),
      nodeShape(nodeRadius_),
      nodeMiddle(sf::Vector2f(nodeRadius_ * std::sqrt(3), nodeRadius_)),
      headSprite(headTexture),
      score_(InitialSize),
      pickupSound_(pickupBuffer_),
      dieSound_(dieBuffer_)
{
    // 先构造初始路径，再配置外观和音效资源。
    initNodes();

    nodeShape.setFillColor(sf::Color(0xf1c40fff));

    nodeMiddle.setFillColor(sf::Color(0x1c2833ff));

    setOriginMiddle(nodeShape);
    setOriginMiddle(nodeMiddle);

    if (headTexture.loadFromFile("assets/image/snakeHeadImage.png"))
    {
        headTexture.setSmooth(true);
        const sf::Vector2u textureSize = headTexture.getSize();
        // 根据贴图高度缩放蛇头，让它和身体节点尺寸匹配。
        const float headScale = nodeRadius_ / textureSize.y * 2.6f;
        headSprite.setTexture(headTexture, true);
        headSprite.setScale({headScale, headScale});
    }
    else
    {
        std::cerr << "Failed to load snake head texture: assets/image/snakeHeadImage.png\n";
    }

    setOriginMiddle(headSprite);

    if (pickupBuffer_.loadFromFile("assets/sounds/pickup.wav"))
    {
        pickupSound_.setBuffer(pickupBuffer_);
        pickupSound_.setVolume(30);
    }
    else
    {
        std::cerr << "Failed to load pickup sound: assets/sounds/pickup.wav\n";
    }

    if (dieBuffer_.loadFromFile("assets/sounds/die.wav"))
    {
        dieSound_.setBuffer(dieBuffer_);
        dieSound_.setVolume(50);
    }
    else
    {
        std::cerr << "Failed to load death sound: assets/sounds/die.wav\n";
    }
}

void Snake::initNodes()
{
    // 蛇从窗口中心生成，身体沿当前反方向向后展开。
    path_.push_back(SnakePathNode(
        Game::GlobalVideoMode.size.x / 2.0f,
        Game::GlobalVideoMode.size.y / 2.0f));
    for (int i = 1; i <= 10 * InitialSize; i++)
    {
        path_.push_back(SnakePathNode(
            Game::GlobalVideoMode.size.x / 2.0f -
                direction_.x * i * nodeRadius_ / 5.0,
            Game::GlobalVideoMode.size.y / 2.0f -
                direction_.y * i * nodeRadius_ / 5.0));
    }
}

void Snake::handleInput(sf::RenderWindow &window)
{
    // 键盘输入优先按四个正交方向切换运动方向。
    if (
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
        direction_ = Direction(0, -1);
    else if (
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
        direction_ = Direction(0, 1);
    else if (
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
        direction_ = Direction(-1, 0);
    else if (
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
        direction_ = Direction(1, 0);

    static double directionSize;

    if (!Game::mouseButtonLocked)
    {
        if (
            sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) ||
            sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
        {
            static sf::Vector2i MousePosition;
            MousePosition = sf::Mouse::getPosition(window);
            if (
                dis(MousePosition,
                    sf::Vector2f(
                        Game::GlobalVideoMode.size.x / 15.0f * 14.0f,
                        Game::GlobalVideoMode.size.x / 15.0f)) >
                Game::GlobalVideoMode.size.x / 16.0f)
            {
                // 鼠标控制时，朝“蛇头到鼠标”的方向做归一化移动。
                direction_ =
                    static_cast<sf::Vector2f>(MousePosition) -
                    toWindow(path_.front());
                directionSize = length(direction_);
                direction_.x /= directionSize;
                direction_.y /= directionSize;
            }
        }
    }

    // 空格只影响速度，不修改方向。
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
        speedup_ = true;
    else
        speedup_ = false;
}

void Snake::update(sf::Time delta)
{
    // 真实移动步进与 delta 解耦，速度主要由每帧推进次数决定。
    move();
    static int count = 0;
    if (++count >= 40)
    {
        // 边界重定位不需要每帧都做，间隔执行即可。
        checkOutOfWindow();
        count -= 40;
    }
    checkSelfCollisions();
}

bool Snake::canEatFruit(const Fruit &fruit) const
{
    // 以“蛇头半径 + 水果半径”为阈值做圆形碰撞判断。
    return dis(
               fruit.getPosition(),
               toWindow(path_.front())) <
           nodeRadius_ + fruit.getRadius();
}

void Snake::eatFruit(const Fruit &fruit)
{
    pickupSound_.play();
    grow(fruit.score_);
}

void Snake::grow(int score)
{
    // 通过延迟删除尾节点来实现“身体增长”的视觉效果。
    tailOverlap_ += score * 10;
    score_ += score;
}

unsigned Snake::getScore() const
{
    return score_;
}

sf::Vector2f Snake::getHeadPosition() const
{
    return toWindow(path_.front());
}

float Snake::getHeadRadius() const
{
    return nodeRadius_;
}

bool Snake::hitSelf() const
{
    return hitSelf_;
}

void Snake::move()
{
    SnakePathNode &headNode = path_.front();
    int times = speedup_ ? 2 : 1;
    for (int i = 1; i <= times; i++)
    {
        // 把新头节点插到队首，形成连续运动轨迹。
        path_.push_front(SnakePathNode(
            headNode.x + direction_.x * i * nodeRadius_ / 5.0,
            headNode.y + direction_.y * i * nodeRadius_ / 5.0));
        if (tailOverlap_)
            // 若还有待增长长度，就先不删尾巴。
            tailOverlap_--;
        else
            path_.pop_back();
    }
}

void Snake::checkSelfCollisions()
{
    SnakePathNode head = toWindow(path_.front());
    int count = 0;

    for (auto i = path_.begin(); i != path_.end(); i++, count++)
        if (count >= 30 && dis(head, toWindow(*i)) < 2.0f * nodeRadius_)
        {
            // 前面 30 个采样点离蛇头太近，属于正常身体连续部分，不算自撞。
            dieSound_.play();
            sf::sleep(sf::seconds(dieBuffer_.getDuration().asSeconds()));
            hitSelf_ = true;
            break;
        }
}

void Snake::checkOutOfWindow()
{
    auto inWindow = [](SnakePathNode &node) -> bool
    {
        return node.x >= 0 &&
               node.x <= Game::GlobalVideoMode.size.x &&
               node.y >= 0 &&
               node.y <= Game::GlobalVideoMode.size.y;
    };
    bool OutOfWindow = true;
    for (auto i : path_)
    {
        if (inWindow(i))
            OutOfWindow = false;
    }
    if (OutOfWindow)
    {
        // 只有整条蛇都离开屏幕后，才整体平移到另一侧，形成穿墙效果。
        SnakePathNode &tail = path_.back();
        if (tail.x < 0)
            for (auto &i : path_)
                i.x = i.x + Game::GlobalVideoMode.size.x;
        else if (tail.x > Game::GlobalVideoMode.size.x)
            for (auto &i : path_)
                i.x = i.x - Game::GlobalVideoMode.size.x;
        else if (tail.y < 0)
            for (auto &i : path_)
                i.y = i.y + Game::GlobalVideoMode.size.y;
        else if (tail.y > Game::GlobalVideoMode.size.y)
            for (auto &i : path_)
                i.y = i.y - Game::GlobalVideoMode.size.y;
    }
}

SnakePathNode Snake::toWindow(SnakePathNode node) const
{
    // 把逻辑路径点折返到可见窗口坐标内，避免穿墙后绘制到屏幕外。
    while (node.x < 0)
        node.x = node.x + Game::GlobalVideoMode.size.x;
    while (node.x > Game::GlobalVideoMode.size.x)
        node.x = node.x - Game::GlobalVideoMode.size.x;
    while (node.y < 0)
        node.y = node.y + Game::GlobalVideoMode.size.y;
    while (node.y > Game::GlobalVideoMode.size.y)
        node.y = node.y - Game::GlobalVideoMode.size.y;
    return node;
}

void Snake::render(sf::RenderWindow &window)
{
    static int count;
    static SnakePathNode lastSnakeNode, lastMiddleNode, nowSnakeNode;
    static float angle;
    static sf::Vector2f recDirection;
    static SnakePathNode wNowHeadNode;

    lastSnakeNode = *path_.begin();
    wNowHeadNode = toWindow(lastSnakeNode);
    headSprite.setPosition(wNowHeadNode);
    recDirection = direction_;
    // 通过方向向量反算蛇头朝向，让蛇头图片始终面向前进方向。
    angle =
        std::acos(recDirection.y / length(recDirection)) /
        3.14159265358979323846 * 180.0;
    if (direction_.x > 0)
        angle = -angle;
    headSprite.setRotation(sf::degrees(angle));

    renderNode(wNowHeadNode, headSprite, window, 3);

    count = 1;
    for (auto i = path_.begin() + 1; i != path_.end(); i++, count++)
    {
        if (count % 5 == 0)
        {
            if (count % 2)
                // 奇数段作为中间连接块的参考点。
                lastMiddleNode = *i;
            else
            {
                nowSnakeNode = *i;

                // 偶数段用于摆放身体圆节点，并推导连接矩形的旋转角度。
                recDirection = nowSnakeNode - lastSnakeNode;
                angle =
                    std::acos(recDirection.y / length(recDirection)) /
                    3.14159265358979323846 * 180.0;
                if (recDirection.x > 0)
                    angle = -angle;
                nodeMiddle.setRotation(sf::degrees(angle));

                static SnakePathNode wNowSnakeNode;
                static SnakePathNode wMiddleNode;
                wNowSnakeNode = toWindow(nowSnakeNode);
                wMiddleNode = toWindow(lastMiddleNode);
                renderNode(wNowSnakeNode, nodeShape, window, 0);
                renderNode(wMiddleNode, nodeMiddle, window, 0);

                lastSnakeNode = nowSnakeNode;
            }
        }
    }
}

template <typename T>
void Snake::renderNode(sf::Vector2f &nowPosition, T &shape, sf::RenderWindow &window, int offset)
{
    shape.setPosition(nowPosition);
    window.draw(shape);

    // 如果节点靠近边界，就在对侧额外补绘一份，保证穿墙时视觉连续。
    if (nowPosition.x <= nodeRadius_ + offset)
    {
        shape.setPosition(nowPosition + sf::Vector2f(Game::GlobalVideoMode.size.x, 0.f));
        window.draw(shape);
    }
    else if (nowPosition.x >= Game::GlobalVideoMode.size.x - nodeRadius_ - offset)
    {
        shape.setPosition(nowPosition - sf::Vector2f(Game::GlobalVideoMode.size.x, 0.f));
        window.draw(shape);
    }

    if (nowPosition.y <= nodeRadius_ + offset)
    {
        shape.setPosition(nowPosition + sf::Vector2f(0.f, Game::GlobalVideoMode.size.y));
        window.draw(shape);
    }
    else if (nowPosition.y >= Game::GlobalVideoMode.size.y - nodeRadius_ - offset)
    {
        shape.setPosition(nowPosition - sf::Vector2f(0.f, Game::GlobalVideoMode.size.y));
        window.draw(shape);
    }

    if (nowPosition.x <= nodeRadius_ + offset && nowPosition.y <= nodeRadius_ + offset)
    {
        shape.setPosition(nowPosition + sf::Vector2f(Game::GlobalVideoMode.size.x, Game::GlobalVideoMode.size.y));
        window.draw(shape);
    }
    else if (nowPosition.x >= Game::GlobalVideoMode.size.x - nodeRadius_ - offset && nowPosition.y >= Game::GlobalVideoMode.size.y - nodeRadius_ - offset)
    {
        shape.setPosition(nowPosition - sf::Vector2f(Game::GlobalVideoMode.size.x, Game::GlobalVideoMode.size.y));
        window.draw(shape);
    }
}
