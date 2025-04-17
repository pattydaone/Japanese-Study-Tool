#ifndef GAMECLASS_H
#define GAMECLASS_H

#include <wx/wxprec.h>
#include <vector>
#include <string_view>
#include <numeric>
#include <algorithm>
#include <random>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif // WX_PRECOMP

using string_matrix = std::vector<std::vector<std::string>>;

class Game : public wxFrame {
protected:
    // Data
    std::vector<int> indices;
    std::vector<std::string> questions;
    string_matrix answers;
    int turns { 0 };
    bool cycle = true;
    int amnt_correct { 0 };
    int amnt_incorrect { 0 };
    std::random_device rd;
    std::mt19937 g{ rd() };

    virtual void start_game() = 0;
    virtual void check_answer(wxCommandEvent& event) = 0;
    virtual void end_frame() = 0;
    virtual void start_from_end() = 0;

public:
    Game(const std::vector<std::string>& vec, const string_matrix& matrix, int x, int y )
                : wxFrame(NULL, wxID_ANY, "", wxDefaultPosition, wxSize { x, y })
                , questions { vec }, answers { matrix }

    {
        indices.resize(vec.size());
        std::iota(indices.begin(), indices.end(), 0);
        std::shuffle(indices.begin(), indices.end(), g);
    }

};

#endif // GAMECLASS_H
