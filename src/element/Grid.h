#pragma once

/*
 * 模块说明：
 * Grid 负责绘制游戏背景中的辅助网格线，便于玩家判断移动方向和间距。
 */

#include <SFML/Graphics.hpp>

namespace sfSnake
{
    class Grid
    {
    private:
        sf::RectangleShape gridVline;   // 复用的竖线图元
        sf::RectangleShape gridHline;   // 复用的横线图元
    public:
        Grid();
        ~Grid() = default;
        // 按当前全局配色和间距绘制完整网格。
        void render(sf::RenderWindow &window);
    };
}
