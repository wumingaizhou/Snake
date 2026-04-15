#pragma once

/*
 * 模块说明：
 * GameOverScreen 是结算界面，负责显示最终分数并提供重开、设置、
 * 帮助、关于和退出入口。
 */

#include <SFML/Graphics.hpp>
#include <vector>

#include "screen/Screen.h"
#include "element/Button.h"
#include "screen/OptionScreen.h"

namespace sfSnake
{
    class GameOverScreen : public Screen
    {
    public:
        GameOverScreen(std::size_t score);

        void handleInput(sf::RenderWindow &window) override;
        void update(sf::Time delta) override;
        void render(sf::RenderWindow &window) override;

    private:
        // 居中的最终分数字样。
        sf::Text text_;

        // 三个主按钮：设置、重开、退出。
        std::vector<Button> button_;
        // 当前局得分，会写入排行榜。
        unsigned score_;

        OptionButton helpButton_;
        OptionButton aboutButton_;
    };
}
