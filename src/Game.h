#pragma once

/*
 * 模块说明：
 * 该文件声明游戏主控制类 Game，负责驱动主循环，并集中维护窗口、
 * 全局资源、全局界面状态以及一些通用工具函数。
 */

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <vector>
#include <memory>
#include <cmath>

#include "screen/Screen.h"
#include "element/TitleSprite.h"

namespace sfSnake
{
    /*
     * Game 是整个程序的总控对象：
     * 1. 创建并维护主窗口；
     * 2. 按固定节奏执行输入、更新和渲染；
     * 3. 保存界面切换时会共享的全局资源和状态。
     */
    class Game
    {
    public:
        Game();
        ~Game() = default;

        // 启动游戏主循环，直到窗口关闭为止。
        void run();

    private:
        // 处理窗口事件，并把输入分发给当前界面。
        void handleInput();
        // 更新当前界面的游戏逻辑。
        void update(sf::Time delta);
        // 绘制当前界面。
        void render();

    public:
        /* Global Color settings
         * 全局颜色设置
         */
        class Color
        {
        public:
            static const sf::Color Yellow;
            static const sf::Color Green;
            static const sf::Color Background[3];
            static const sf::Color Grid[3];
            static const sf::Color NotSeleted;
            static const sf::Color Fruit[5];
        };

        static sf::VideoMode initVideoMode_();

        static sf::VideoMode GlobalVideoMode;

        static sf::Font GlobalFont;
        static TitleSprite GlobalTitle;

        static std::shared_ptr<Screen> MainScreen;
        static std::shared_ptr<Screen> TmpScreen;
        static std::shared_ptr<Screen> TmpGameScreen;

        static int GridVisibility;
        static int GridColor;
        static int BackgroundColor;

        static bool mouseButtonLocked;
        static sf::Clock mouseButtonClock;
        static sf::Time mouseButtonCDtime;

        static bool keyboardLocked;
        static sf::Clock keyboardClock;
        static sf::Time keyboardCDtime;

        static bool ifShowedHelp;

    private:
        // 主渲染窗口。
        sf::RenderWindow window_;

        // 固定更新步长，用于让逻辑更新频率更稳定。
        sf::Time TimePerFrame_;

        // 背景音乐播放器。
        sf::Music bgMusic_;
    };

    /**
     * @brief set any shape's origin to its middle.
     * @param shape any shape
     *
     * @code {.c++}
     * setOriginMiddle(titleSprite_);
     * @endcode
     *
     * @return sf::FloatRect shapeBounds
     *
     */
    template <typename T>
    inline sf::FloatRect setOriginMiddle(T &shape)
    {
        // 将可绘制对象的原点移到几何中心，便于统一缩放和定位。
        sf::FloatRect shapeBounds = shape.getLocalBounds();
        shape.setOrigin(shapeBounds.getCenter());
        return shapeBounds;
    }

    /**
     * @brief calculate the distance between two nodes
     *
     * @param node sf::Vector2
     *
     * @return double
     */
    template <typename T1, typename T2>
    inline double dis(
        sf::Vector2<T1> node1,
        sf::Vector2<T2> node2) noexcept
    {
        // 使用欧几里得距离判断两个点之间的直线距离。
        return std::sqrt(
            std::pow(
                (
                    static_cast<double>(node1.x) -
                    static_cast<double>(node2.x)),
                2) +
            std::pow(
                (
                    static_cast<double>(node1.y) -
                    static_cast<double>(node2.y)),
                2));
    }

    /**
     * @brief calculate the length of a vector
     *
     * @param node sf::Vector2
     *
     * @return double
     */
    template <typename T>
    inline double length(sf::Vector2<T> node) noexcept
    {
        // 计算向量长度，常用于方向归一化。
        return std::sqrt(
            std::pow(static_cast<double>(node.x), 2) +
            std::pow(static_cast<double>(node.y), 2));
    }
}
