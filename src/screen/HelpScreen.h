#pragma once

/*
 * 模块说明：
 * HelpScreen 展示游戏规则和操作说明，是首次游玩时的引导界面。
 */

#include <SFML/Graphics.hpp>

#include "screen/Screen.h"
#include "element/Button.h"

#include <vector>

namespace sfSnake
{
    class HelpScreen : public Screen
    {
    public:
        HelpScreen();

        void handleInput(sf::RenderWindow &window) override;
        void update(sf::Time delta) override;
        void render(sf::RenderWindow &window) override;

    private:
        // 左上角返回按钮。
        Button returnButton_;
        // 居中的帮助说明文本。
        sf::Text text_;
    };
}
