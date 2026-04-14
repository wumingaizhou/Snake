#include <SFML/Graphics.hpp>

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
    initNodes();

    nodeShape.setFillColor(sf::Color(0xf1c40fff));

    nodeMiddle.setFillColor(sf::Color(0x1c2833ff));

    setOriginMiddle(nodeShape);
    setOriginMiddle(nodeMiddle);

    if (headTexture.loadFromFile("assets/image/snakeHeadImage.png"))
    {
        headTexture.setSmooth(true);
        const sf::Vector2u textureSize = headTexture.getSize();
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
                direction_ =
                    static_cast<sf::Vector2f>(MousePosition) -
                    toWindow(path_.front());
                directionSize = length(direction_);
                direction_.x /= directionSize;
                direction_.y /= directionSize;
            }
        }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
        speedup_ = true;
    else
        speedup_ = false;
}

void Snake::update(sf::Time delta)
{
    move();
    static int count = 0;
    if (++count >= 40)
    {
        checkOutOfWindow();
        count -= 40;
    }
    checkSelfCollisions();
}

void Snake::checkFruitCollisions(std::deque<Fruit> &fruits)
{
    auto toRemove = fruits.end();
    SnakePathNode headnode = path_.front();

    for (auto i = fruits.begin(); i != fruits.end(); ++i)
    {
        if (dis(
                i->shape_.getPosition(), toWindow(headnode)) <
            nodeRadius_ + i->shape_.getRadius())
            toRemove = i;
    }

    if (toRemove != fruits.end())
    {
        pickupSound_.play();
        grow(toRemove->score_);
        fruits.erase(toRemove);
    }
}

void Snake::grow(int score)
{
    tailOverlap_ += score * 10;
    score_ += score;
}

unsigned Snake::getScore() const
{
    return score_;
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
        path_.push_front(SnakePathNode(
            headNode.x + direction_.x * i * nodeRadius_ / 5.0,
            headNode.y + direction_.y * i * nodeRadius_ / 5.0));
        if (tailOverlap_)
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

SnakePathNode Snake::toWindow(SnakePathNode node)
{
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
                lastMiddleNode = *i;
            else
            {
                nowSnakeNode = *i;

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
