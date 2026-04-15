#pragma once

/*
 * 模块说明：
 * Snake 封装贪吃蛇本体的移动、成长、碰撞检测、输入控制和渲染。
 * 这里是整个游戏玩法逻辑最核心的对象。
 */

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <deque>

#include "Fruit.h"

namespace sfSnake
{
    // Direction 用单位方向向量表示当前前进朝向。
    typedef sf::Vector2f Direction;
    // SnakePathNode 保存蛇身体轨迹上的离散采样点。
    typedef sf::Vector2f SnakePathNode;

    class Snake
    {
    public:
        Snake();

        // 读取键盘和鼠标输入，更新蛇的运动方向与加速状态。
        void handleInput(sf::RenderWindow &window);
        // 推进蛇的移动，并做边界与碰撞检查。
        void update(sf::Time delta);
        // 按当前轨迹绘制蛇头与蛇身。
        void render(sf::RenderWindow &window);

        // 判断蛇头是否已经接触到某个水果。
        bool canEatFruit(const Fruit &fruit) const;
        // 吃到水果后播放音效并增长长度/分数。
        void eatFruit(const Fruit &fruit);

        // 返回当前是否已经撞到自己。
        bool hitSelf() const;

        // 返回当前累计得分。
        unsigned getScore() const;
        // 返回蛇头在窗口坐标中的位置。
        sf::Vector2f getHeadPosition() const;
        // 返回蛇头半径，用于与水果做碰撞检测。
        float getHeadRadius() const;

        void printhead() const;

    private:
        // 初始化蛇的起始身体节点。
        void initNodes();

        // 根据吃到的水果分数延长蛇身。
        void grow(int score);
        // 按当前方向推动蛇前进。
        void move();

        // 处理从窗口一侧穿出、从另一侧进入的逻辑。
        void checkOutOfWindow();
        // 检查蛇头是否撞到自身身体。
        void checkSelfCollisions();

        template <typename T>
        void renderNode(sf::Vector2f &nowPosition, T &shape, sf::RenderWindow &window, int offset);

        // 将逻辑路径点换算到窗口内，便于穿墙后仍能正确显示和检测。
        SnakePathNode toWindow(SnakePathNode node) const;
        // 标记本局是否结束。
        bool hitSelf_;
        // 标记是否处于加速状态。
        bool speedup_;

        // 当前移动方向。
        Direction direction_;
        // 蛇身体圆节点半径。
        float nodeRadius_;
        // 记录蛇身体运动轨迹，用路径点而不是固定格子来实现平滑移动。
        std::deque<SnakePathNode> path_;
        // 尾部还应额外保留的路径点数量，用于实现“成长”。
        unsigned tailOverlap_;

        // 蛇身体的圆形节点。
        sf::CircleShape nodeShape;
        // 相邻节点之间的连接矩形，让蛇身看起来更连续。
        sf::RectangleShape nodeMiddle;
        // 蛇头专用贴图。
        sf::Texture headTexture;
        // 蛇头精灵。
        sf::Sprite headSprite;

        // 初始长度对应的基础得分。
        static const int InitialSize;
        unsigned int score_;

        // 吃到水果时播放的提示音。
        sf::SoundBuffer pickupBuffer_;
        sf::Sound pickupSound_;

        // 撞到自己时播放的死亡音效。
        sf::SoundBuffer dieBuffer_;
        sf::Sound dieSound_;
    };
}
