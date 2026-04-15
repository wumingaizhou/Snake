#pragma once

/*
 * 模块说明：
 * MenuScreen 是游戏主菜单界面，负责展示标题、开始/设置/退出按钮，
 * 以及历史前三成绩入口。
 */

#include <SFML/Graphics.hpp>

#include "screen/Screen.h"
#include "element/Button.h"
#include "element/OptionButton.h"

#include <vector>

namespace sfSnake
{
    class MenuScreen : public Screen
    {
    public:
        MenuScreen();

        void handleInput(sf::RenderWindow &window) override;
        void update(sf::Time delta) override;
        void render(sf::RenderWindow &window) override;

    private:
        // 重新拼装排行榜显示文本。
        void refreshHighScores();

        // 主菜单的三个图片按钮：设置、开始、退出。
        std::vector<Button> button_;
        // 文本入口：帮助。
        OptionButton helpButton_;
        // 文本入口：关于。
        OptionButton aboutButton_;
        // 右上角历史成绩文本。
        sf::Text highScoresText_;
        // 缓存的前三成绩。
        std::vector<unsigned> topScores_;
        // 定时刷新排行榜，避免频繁读文件。
        sf::Time highScoreRefreshElapsed_;
    };
}
