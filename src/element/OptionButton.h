#pragma once

/*
 * 模块说明：
 * OptionButton 封装文字型按钮，主要用于设置页面和辅助菜单中的文本选项。
 */

#include <SFML/Graphics.hpp>

namespace sfSnake
{
    class OptionButton
    {
    private:
        // 按钮显示文字。
        sf::Text text_;
        // 保存默认颜色，便于取消高亮时恢复。
        sf::Color originColor_;

    public:
        OptionButton();
        ~OptionButton() = default;

        // 初始化文字内容、字号、颜色和位置。
        void settings(
            sf::String content,
            sf::Font &font,
            float setCharacterSize,
            sf::Color color,
            sf::Vector2f position);
        // 清除当前按钮的临时高亮状态。
        void clear();
        // 将按钮设置为“鼠标悬停”样式。
        void focused();
        // 将按钮设置为“当前已选中”样式。
        void seleted();

        // 绘制按钮文本。
        void render(sf::RenderWindow &window);

        template <typename T>
        bool contains(sf::Vector2<T> point);
    };

    template <typename T>
    bool OptionButton::contains(sf::Vector2<T> point)
    {
        // 文本按钮直接使用文本包围盒做命中检测。
        return text_.getGlobalBounds().contains(static_cast<sf::Vector2f>(point));
    }
}
