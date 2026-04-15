#pragma once

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

        void generateFruit();

    private:
        Fruit createBonusFruit();
        sf::Vector2f generateFruitPosition(float radius);
        bool isPositionValid(const sf::Vector2f &position, float radius) const;
        void scheduleNextBonusFruit();
        void updateBonusFruit(sf::Time delta);

        Snake snake_;
        std::deque<Fruit> fruit_;
        std::optional<Fruit> bonusFruit_;
        Grid grid_;

        Button pauseButton_;

        sf::Text score_;
        std::default_random_engine randomEngine_;
        sf::Time bonusFruitLifetime_;
        sf::Time nextBonusFruitSpawn_;
    };
}
