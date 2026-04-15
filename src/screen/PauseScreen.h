#pragma once

/*
 * 模块说明：
 * PauseScreen 是暂停界面，允许玩家继续游戏、重新开始、打开设置、
 * 查看帮助/关于，或者直接退出。
 */

#include <SFML/Graphics.hpp>

#include "screen/Screen.h"
#include "element/Button.h"
#include "element/OptionButton.h"
#include "Game.h"

#include <vector>
#include <string>

namespace sfSnake
{
    class PauseScreen : public Screen
    {
    public:
        PauseScreen();

        void handleInput(sf::RenderWindow &window) override;
        void update(sf::Time delta) override;
        void render(sf::RenderWindow &window) override;

    private:
        // 三个主按钮：设置、重开、退出。
        std::vector<Button> button_;
        // 左上角继续游戏按钮。
        Button returnButton_;
        OptionButton helpButton_;
        OptionButton aboutButton_;
    };
}
