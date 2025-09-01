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

using string_matrix = std::vector<std::vector<std::string>>;

struct GameData {
    std::vector<int> indices;
    std::vector<std::string> stringVec;
    string_matrix stringMatrix;
    int turns { 0 };
    bool cycle = true;
    int amnt_correct { 0 };
    int amnt_incorrect { 0 };
    std::random_device rd;
    std::mt19937 g{ rd() };

    GameData(const std::vector<std::string>& vec, const string_matrix& matrix)
    : stringVec { vec }, stringMatrix { matrix }
    {
        indices.resize(vec.size());
        std::iota(indices.begin(), indices.end(), 0);
        std::shuffle(indices.begin(), indices.end(), g);
    }

    int getCurrentIndex() {
        return indices[turns - 1];
    }

    void reset() {
        turns = 0;
        amnt_correct = 0;
        amnt_incorrect = 0;
        std::shuffle(indices.begin(), indices.end(), g);
    }

};

#endif // GAMECLASS_H
