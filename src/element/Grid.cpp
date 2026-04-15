#include "element/Grid.h"
#include "Game.h"

/*
 * 模块说明：
 * 该文件实现背景网格的构造与渲染。
 */

using namespace sfSnake;

Grid::Grid()
    : gridVline(sf::Vector2f(Game::GlobalVideoMode.size.x / 500.0f, Game::GlobalVideoMode.size.y)),
      gridHline(sf::Vector2f(Game::GlobalVideoMode.size.x, Game::GlobalVideoMode.size.x / 500.0f))
{
}

void Grid::render(sf::RenderWindow &window)
{
    gridVline.setFillColor(Game::Color::Grid[Game::GridColor]);
    gridHline.setFillColor(Game::Color::Grid[Game::GridColor]);

    // 网格间距固定为窗口宽度的 1/50，保证缩放时仍然较为均匀。
    static float gridSpace = Game::GlobalVideoMode.size.x / 50.0f;
    for (float i = 0.f; i < Game::GlobalVideoMode.size.x; i += gridSpace)
    {
        gridVline.setPosition({i, 0.f});
        window.draw(gridVline);
    }

    for (float i = 0.f; i < Game::GlobalVideoMode.size.y; i += gridSpace)
    {
        gridHline.setPosition({0.f, i});
        window.draw(gridHline);
    }
}
