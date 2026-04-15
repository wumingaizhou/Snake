#pragma once

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
        void refreshHighScores();

        std::vector<Button> button_;
        OptionButton helpButton_;
        OptionButton aboutButton_;
        sf::Text highScoresText_;
        std::vector<unsigned> topScores_;
        sf::Time highScoreRefreshElapsed_;
    };
}
