#pragma once

/*
 * 模块说明：
 * GameScreen 是实际游玩界面，负责维护蛇、普通水果、奖励水果、
 * 暂停按钮、分数文本和背景网格。
 */

#include <SFML/Graphics.hpp>
#include <deque>
#include <optional>
#include <random>

#include "screen/Screen.h"
#include "element/Snake.h"
#include "element/Fruit.h"
#include "element/Grid.h"
#include "element/Button.h"

namespace sfSnake
{
    class GameScreen : public Screen
    {
    public:
        GameScreen();

        void handleInput(sf::RenderWindow &window) override;
        void update(sf::Time delta) override;
        void render(sf::RenderWindow &window) override;

        // 补充一个普通水果到场景中。
        void generateFruit();

    private:
        // 创建一枚带特殊效果的奖励水果。
        Fruit createBonusFruit();
        // 随机生成一个适合放水果的位置。
        sf::Vector2f generateFruitPosition(float radius);
        // 检查新水果位置是否离蛇头和其他水果太近。
        bool isPositionValid(const sf::Vector2f &position, float radius) const;
        // 随机安排下一次奖励水果出现时间。
        void scheduleNextBonusFruit();
        // 更新奖励水果的出现、倒计时和闪烁逻辑。
        void updateBonusFruit(sf::Time delta);

        // 玩家控制的蛇对象。
        Snake snake_;
        // 当前场景中的普通水果集合。
        std::deque<Fruit> fruit_;
        // 当前场景中的奖励水果，未出现时为空。
        std::optional<Fruit> bonusFruit_;
        Grid grid_;

        // 右上角暂停按钮。
        Button pauseButton_;

        // 顶部分数字样。
        sf::Text score_;
        // 随机数引擎，统一供水果生成使用。
        std::default_random_engine randomEngine_;
        // 奖励水果剩余存活时间。
        sf::Time bonusFruitLifetime_;
        // 距离下次生成奖励水果还剩的时间。
        sf::Time nextBonusFruitSpawn_;
    };
}
