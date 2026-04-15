#pragma once

/*
 * 模块说明：
 * 该文件声明最高分读写接口，用于在本地文本文件中保存历史前三成绩。
 */

#include <vector>

namespace sfSnake
{
    // 读取本地高分榜，并返回按分数从高到低排序后的结果。
    std::vector<unsigned> loadTopScores();
    // 记录一条新成绩，并返回更新后的高分榜。
    std::vector<unsigned> recordScore(unsigned score);
}
