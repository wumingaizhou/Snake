#pragma once

/*
 * 模块说明：
 * AboutScreen 用于展示作业信息、作者信息和许可说明。
 */

#include <SFML/Graphics.hpp>

#include "screen/Screen.h"
#include "element/Button.h"

#include <vector>

namespace sfSnake
{
    class AboutScreen : public Screen
    {
    public:
        AboutScreen();

        void handleInput(sf::RenderWindow &window) override;
        void update(sf::Time delta) override;
        void render(sf::RenderWindow &window) override;

    private:
        // 左上角返回按钮。
        Button returnButton_;
        // 关于页面正文。
        sf::Text text_;
    };
}
