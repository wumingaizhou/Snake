#pragma once

/*
 * 模块说明：
 * OptionScreen 是设置界面，负责切换网格开关、网格颜色和背景颜色。
 */

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

#include "screen/Screen.h"
#include "screen/GameScreen.h"
#include "element/Grid.h"
#include "element/Button.h"
#include "element/OptionButton.h"

namespace sfSnake
{
    class OptionScreen : public Screen
    {
    public:
        OptionScreen();

        void handleInput(sf::RenderWindow &window) override;
        void update(sf::Time delta) override;
        void render(sf::RenderWindow &window) override;

    private:
        // 各设置项的标题文字。
        std::vector<OptionButton> optionName_;
        // 网格开关候选项。
        std::vector<OptionButton> GridShowOptions_;
        // 网格颜色候选项。
        std::vector<OptionButton> GridColorOptions_;
        // 背景颜色候选项。
        std::vector<OptionButton> BGColorOptions_;

        // 鼠标当前悬停到的网格开关选项索引。
        int GridShowfocused_;
        // 鼠标当前悬停到的网格颜色选项索引。
        int GridColorfocused_;
        // 鼠标当前悬停到的背景颜色选项索引。
        int BGColorfocused_;

        Grid grid_;

        // 返回上一界面的按钮。
        Button returnButton_;
    };
}
