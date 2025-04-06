#ifndef GAMECLASS_H
#define GAMECLASS_H

#include <wx/wxprec.h>
#include <vector>
#include <string_view>
#include <numeric>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif // WX_PRECOMP

using string_matrix = std::vector<std::vector<std::string_view>>;

class Game : public wxFrame {
protected:
    // Data
    std::vector<int> indices;
    std::vector<std::string_view> questions;
    string_matrix answers;
    int turns { 0 };
    bool cycle = true;
    int amnt_correct { 0 };
    int amnt_incorrect { 0 };

    //virtual void start_game() = 0;
    //virtual void check_answer() = 0;
    //virtual void start_from_end() = 0;

public:
    Game(const std::vector<std::string_view>& vec, const string_matrix& matrix )
                : wxFrame(NULL, wxID_ANY, "", wxDefaultPosition, wxSize { 475, 200 })
                , questions { vec }, answers { matrix }

    {
        indices.reserve(vec.size());
        std::iota(indices.begin(), indices.end(), 0);
    }

};

#endif // GAMECLASS_H
