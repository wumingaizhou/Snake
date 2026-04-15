#include "HighScoreStorage.h"

/*
 * 模块说明：
 * 该文件实现高分榜的读取、清洗、排序和持久化逻辑。
 */

#include <algorithm>
#include <cctype>
#include <fstream>
#include <string>

namespace sfSnake
{
namespace
{
constexpr char HighScoreFilePath[] = "highscores.txt";
constexpr std::size_t HighScoreLimit = 3;

std::string trim(const std::string &text)
{
    // 去掉每一行首尾的空白字符，避免解析成绩时受到格式干扰。
    const auto begin = std::find_if_not(
        text.begin(),
        text.end(),
        [](unsigned char ch)
        { return std::isspace(ch) != 0; });
    const auto end = std::find_if_not(
        text.rbegin(),
        text.rend(),
        [](unsigned char ch)
        { return std::isspace(ch) != 0; })
                         .base();
    if (begin >= end)
        return "";
    return std::string(begin, end);
}

void normalizeScores(std::vector<unsigned> &scores)
{
    // 统一把成绩降序排列，并只保留前三名。
    std::sort(scores.begin(), scores.end(), std::greater<unsigned>());
    if (scores.size() > HighScoreLimit)
        scores.resize(HighScoreLimit);
}
} // namespace

std::vector<unsigned> loadTopScores()
{
    std::ifstream input(HighScoreFilePath);
    std::vector<unsigned> scores;
    if (!input)
        // 首次运行时文件可能不存在，这种情况直接返回空榜单即可。
        return scores;

    std::string line;
    while (std::getline(input, line))
    {
        const std::string trimmed = trim(line);
        if (trimmed.empty())
            continue;

        try
        {
            // 只接受完全由数字组成的一整行，脏数据会被自动忽略。
            std::size_t processedLength = 0;
            const unsigned long value = std::stoul(trimmed, &processedLength);
            if (processedLength == trimmed.size())
                scores.push_back(static_cast<unsigned>(value));
        }
        catch (...)
        {
            // 文件中若出现非法内容，不抛出到上层，直接跳过即可。
        }
    }

    normalizeScores(scores);
    return scores;
}

std::vector<unsigned> recordScore(unsigned score)
{
    // 先读取旧榜单，再把本次成绩合并进去，最后整体重排。
    std::vector<unsigned> scores = loadTopScores();
    scores.push_back(score);
    normalizeScores(scores);

    std::ofstream output(HighScoreFilePath, std::ios::trunc);
    if (output)
    {
        // 使用覆盖写入，保证文件内容和当前排行榜完全一致。
        for (unsigned value : scores)
            output << value << '\n';
    }

    return scores;
}
} // namespace sfSnake
