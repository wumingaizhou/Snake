#include "HighScoreStorage.h"

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
        return scores;

    std::string line;
    while (std::getline(input, line))
    {
        const std::string trimmed = trim(line);
        if (trimmed.empty())
            continue;

        try
        {
            std::size_t processedLength = 0;
            const unsigned long value = std::stoul(trimmed, &processedLength);
            if (processedLength == trimmed.size())
                scores.push_back(static_cast<unsigned>(value));
        }
        catch (...)
        {
        }
    }

    normalizeScores(scores);
    return scores;
}

std::vector<unsigned> recordScore(unsigned score)
{
    std::vector<unsigned> scores = loadTopScores();
    scores.push_back(score);
    normalizeScores(scores);

    std::ofstream output(HighScoreFilePath, std::ios::trunc);
    if (output)
    {
        for (unsigned value : scores)
            output << value << '\n';
    }

    return scores;
}
} // namespace sfSnake
