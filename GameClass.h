#ifndef GAMECLASS_H
#define GAMECLASS_H

#include <wx/wxprec.h>
#include <vector>
#include <numeric>
#include <algorithm>
#include <random>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif // WX_PRECOMP

using stringMatrix = std::vector<std::vector<std::string>>;

struct GameData {
    std::vector<int> indices;
    std::vector<std::string> stringVec;
    stringMatrix matrix;
    int turns { 0 };
    bool cycle = true;
    int amountCorrect { 0 };
    int amountIncorrect { 0 };
    std::random_device randomDevice;
    std::mt19937 generator { randomDevice() };

    GameData(const std::vector<std::string>& vec, const stringMatrix& matrix)
    : stringVec { vec }, matrix { matrix }
    {
        indices.resize(vec.size());
        std::iota(indices.begin(), indices.end(), 0);
        std::shuffle(indices.begin(), indices.end(), generator);
    }

    int getCurrentIndex() {
        return indices[turns - 1];
    }

    void reset() {
        turns = 0;
        amountCorrect = 0;
        amountIncorrect = 0;
        std::shuffle(indices.begin(), indices.end(), generator);
    }
};

#endif // GAMECLASS_H
