#include <SFML/Graphics.hpp>

/*
 * 模块说明：
 * 该文件实现 Game 的主循环、全局资源初始化以及统一的输入/更新/渲染流程。
 * 这里也是各个界面共享状态的定义位置。
 */

#include <memory>
#include <iostream>
#include <string>

#include "screen/MenuScreen.h"
#include "Game.h"

using namespace sfSnake;

/* Global Color settings
 * 全局颜色设置
 */

const sf::Color Game::Color::Yellow =
    sf::Color(0xf1c40fff);
const sf::Color Game::Color::Green =
    sf::Color(0x2ecc71ff);
const sf::Color Game::Color::Background[] =
    {
        sf::Color(0xeaeaeaee), // White
        sf::Color(0x747474ff), // Black
        sf::Color(0x9f6133aa)  // Brown
};
const sf::Color Game::Color::Grid[] =
    {
        sf::Color(0xfbfbfbaa), // White
        sf::Color(0x31313116), // Black
        sf::Color(0x5a351d16)  // Brown
};
const sf::Color Game::Color::NotSeleted =
    sf::Color(0x00000055);
const sf::Color Game::Color::Fruit[] =
    {
        sf::Color(0x212f3dff), // Black
        sf::Color(0x935116ff), // Brown
        sf::Color(0xe74c3cff), // Red
        sf::Color(0x5dade2ff), // Blue
        sf::Color(0x2ecc71ff)  // Green
};

/* static Golbal VideoMode settings
 * 静态全局视频设置，便于创建合适大小的窗口
 * 并且在界面初始化时定位组件
 */

sf::VideoMode Game::initVideoMode_()
{
    // 默认取桌面分辨率的一半作为窗口大小，让窗口在多数设备上都不会过大。
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    return sf::VideoMode(
        {desktopMode.size.x / 2u, desktopMode.size.y / 2u},
        desktopMode.bitsPerPixel);
}

sf::VideoMode Game::GlobalVideoMode = Game::initVideoMode_();

/* Global element
 * 全局的组件就在这里定义好
 * 避免了代码重复，和元素的重复构建
 */

sf::Font Game::GlobalFont = sf::Font();
TitleSprite Game::GlobalTitle = TitleSprite();

/* Global Screen create, originally MenuScreen
 * 生成全局界面对象，开局先进入菜单界面
 * 另外两个界面作为按钮的跳转表
 * 当界面足够复杂和庞大的时候
 * 可能需要用树来存储访问路径
 */

std::shared_ptr<Screen> Game::MainScreen = std::make_shared<MenuScreen>();
std::shared_ptr<Screen> Game::TmpScreen = nullptr;
std::shared_ptr<Screen> Game::TmpGameScreen = nullptr;

/* static global settings
 * 全局设置
 * 包括网格可见性，背景颜色，网格颜色
 */

int Game::GridVisibility = 0;
int Game::GridColor = 0;
int Game::BackgroundColor = 0;

/* static keyboard and mouse lock to prevent misoperation
 * 键盘锁和鼠标锁用于防误触
 */
sf::Clock Game::mouseButtonClock = sf::Clock();
sf::Time Game::mouseButtonCDtime = sf::Time();
bool Game::mouseButtonLocked = false;

sf::Clock Game::keyboardClock = sf::Clock();
sf::Time Game::keyboardCDtime = sf::Time();
bool Game::keyboardLocked = false;

/* 开局显示帮助
 */
bool Game::ifShowedHelp = false;

Game::Game()
    : TimePerFrame_(sf::seconds(1.f / 100.f))
{
    // 创建游戏窗口，并限制渲染帧率，避免无意义地占满 CPU。
    window_.create(
        GlobalVideoMode,   // videoMode
        "sfSnakePro",      // window name
        sf::Style::Close); // window Style
    window_.setFramerateLimit(60);

    sf::Image icon;
    // 设置窗口图标；若资源丢失，只输出错误信息，不阻止程序运行。
    if (icon.loadFromFile("assets/image/favicon.png"))
        window_.setIcon(icon);
    else
        std::cerr << "Failed to load icon: assets/image/favicon.png\n";

    // 载入并循环播放背景音乐。
    if (bgMusic_.openFromFile("assets/music/Roa - Bloom.wav"))
    {
        bgMusic_.setVolume(30);
        bgMusic_.setLooping(true);
        bgMusic_.play();
    }
    else
    {
        std::cerr << "Failed to load background music: assets/music/Roa - Bloom.wav\n";
    }
}

void Game::handleInput()
{
    // 先处理窗口级事件，例如关闭按钮。
    while (const std::optional<sf::Event> event = window_.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
            window_.close();
    }

    // 只有在窗口聚焦且鼠标位于窗口范围内时，才把输入交给当前界面。
    if (window_.hasFocus() &&
        sf::FloatRect(
            {0.f, 0.f},
            sf::Vector2f(
                static_cast<float>(Game::GlobalVideoMode.size.x),
                static_cast<float>(Game::GlobalVideoMode.size.y)))
            .contains(static_cast<sf::Vector2f>(
                sf::Mouse::getPosition(window_))))
        Game::MainScreen->handleInput(window_);
}

void Game::update(sf::Time delta)
{
    // 当前界面的逻辑更新统一由这里调度。
    Game::MainScreen->update(delta);
}

void Game::render()
{
    // 每一帧都先清屏，再绘制当前界面，最后显示到窗口。
    window_.clear(Color::Background[BackgroundColor]);
    Game::MainScreen->render(window_);
    window_.display();
}

void Game::run()
{
    // 使用“固定逻辑步长 + 独立渲染”的经典游戏循环结构。
    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    mouseButtonClock.restart();
    keyboardClock.restart();

    while (window_.isOpen())
    {
        sf::Time delta = clock.restart();
        timeSinceLastUpdate += delta;

        // 输入每帧都立即处理，保证操作手感及时。
        handleInput();

        // 如果这一帧积累了多份逻辑时间，就补做多次更新，避免速度忽快忽慢。
        while (timeSinceLastUpdate > TimePerFrame_)
        {
            timeSinceLastUpdate -= TimePerFrame_;
            update(TimePerFrame_);
        }

        render();

        // 鼠标锁用于阻止一次点击在多个界面或按钮上重复触发。
        delta = mouseButtonClock.restart();
        mouseButtonCDtime += delta;

        if (mouseButtonCDtime.asSeconds() > 0.5f)
        {
            mouseButtonCDtime -= sf::seconds(0.5f);
            mouseButtonLocked = false;
        }

        // 键盘锁和鼠标锁机制类似，给需要节流的按键操作预留支持。
        delta = keyboardClock.restart();
        keyboardCDtime += delta;

        if (keyboardCDtime.asSeconds() > 0.5f)
        {
            keyboardCDtime -= sf::seconds(0.5f);
            keyboardLocked = false;
        }
    }
}
