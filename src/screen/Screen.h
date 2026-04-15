#pragma once

/*
 * 模块说明：
 * Screen 是所有界面的抽象基类，统一规定输入、更新和渲染三个核心接口。
 */

#include <SFML/Graphics.hpp>

class Screen
{
public:
	// 处理当前界面的用户输入。
	virtual void handleInput(sf::RenderWindow &window) = 0;
	// 推进当前界面的状态更新。
	virtual void update(sf::Time delta) = 0;
	// 把当前界面内容绘制到窗口。
	virtual void render(sf::RenderWindow &window) = 0;
};
