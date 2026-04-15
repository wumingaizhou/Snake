#include "Game.h"

/*
 * 模块说明：
 * 程序入口文件，只负责创建游戏对象并启动主循环。
 */

using namespace sfSnake;

int main()
{
    // 游戏生命周期由 Game 对象统一管理。
    Game game;
    game.run();

    return 0;
}
