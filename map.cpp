#include <cstddef>
#include <fstream>
#include <ios>
#include <iostream>
#include <wx/wxprec.h>
#include <vector>
#include <string>
#include <unordered_map>
#include "constants.h"
#include "canvas.h"

#ifndef WX_PRECOMP
    #include <wx/wx.h>
    #include <wx/graphics.h>
    #include <wx/dcbuffer.h>
#endif // WX_PRECOMP


inline std::ostream& operator<<(std::ostream& out, const wxPoint& point) {
    out << point.x << ", " << point.y;
    return out;
}

class Map : public wxFrame {
    using string_matrix = std::vector<std::vector<std::string>>;
    using Lines = std::vector<std::vector<wxPoint>>;
    enum {
        ID_writeCanvas,
        ID_variantKanjiLabel,
        ID_staticSizeLabel,
        ID_variantSizeLabel,
        ID_enterButton,
        ID_undoButton,
        ID_clearButton,
        ID_skipButton,
        ID_sizeSlider
    };

    // Data
    std::vector<std::string> questions;
    std::size_t turns { 0 };
    std::ofstream kanjiOutFile;
    std::ifstream kanjiInFile;
    std::string kanjiFileString;
    std::unordered_map<std::string, Lines> encodedDrawings;

    // Canvas
    Canvas* writeCanvas = new Canvas;

    // Canvas Points
    wxPoint PT_writeCanvas {};

    // Canvas Size
    wxSize SZ_writeCanvas { 425, 400 };

    // Labels
    wxStaticText* variantKanjiLabel = new wxStaticText;
    wxStaticText* staticSizeLabel = new wxStaticText;
    wxStaticText* variantSizeLabel = new wxStaticText;

    // Label points
    wxPoint PT_variantKanjiLabel {};
    wxPoint PT_staticSizeLabel {};
    wxPoint PT_variantSizeLabel {};

    // Label sizes
    wxSize SZ_variantKanjiLabel {};
    wxSize SZ_staticSizeLabel { 100, 25 };
    wxSize SZ_variantSizeLabel { 100, 25 };

    // Buttons
    wxButton* enterButton = new wxButton;
    wxButton* undoButton = new wxButton;
    wxButton* clearButton = new wxButton;
    wxButton* skipButton = new wxButton;

    // Button points
    wxPoint PT_enterButton {};
    wxPoint PT_undoButton {};
    wxPoint PT_clearButton {};
    wxPoint PT_skipButton {};

    // Button sizes
    wxSize SZ_enterButton { 50, 25 };
    wxSize SZ_undoButton { 50, 25 };
    wxSize SZ_clearButton { 50, 25 };
    wxSize SZ_skipButton { 50, 25 };

    // Slider
    wxSlider* sizeSlider = new wxSlider;

    // Slider point
    wxPoint PT_sizeSlider { 0, 30 };

    // Slider size
    wxSize SZ_sizeSlider { 50, 400 };

    void startGame() {
        ++turns;
        variantKanjiLabel -> SetLabel(wxString::FromUTF8(questions[turns - 1]));
    }

    void onEnter(wxCommandEvent& event) {
        auto kanjiCharacter { std::find(kanjiFileString.begin(), kanjiFileString.end(), questions[turns - 1]) };
        if (kanjiCharacter != kanjiFileString.end()) {
            auto kanjiCharacterEnd { std::find(kanjiCharacter, kanjiFileString.end(), '\0') };
            kanjiFileString.erase(kanjiCharacter, kanjiCharacterEnd + 2);
            kanjiOutFile.seekp(0, std::ios_base::beg);
            kanjiOutFile << kanjiFileString;
            kanjiOutFile.seekp(0, std::ios_base::end);
        }
        const Lines& drawing { writeCanvas -> getDrawing() };
        const std::vector<int>& lineSizes { writeCanvas -> getSizes() };

        kanjiOutFile << questions[turns - 1] << '\n';

        for (std::size_t i = 0; i < drawing.size(); ++i) {
            kanjiOutFile << lineSizes[i] << '\n';
            for (auto line : drawing[i]) {
                kanjiOutFile << line << '\n';
            }
        }
        kanjiOutFile << '\0' << std::endl;
    }

    void onSkip(wxCommandEvent& event) {
        writeCanvas -> clear();
        startGame();
    }

    void updateSlider(wxCommandEvent& event) {
    
    }

public:
    Map(std::vector<std::string> vec, int frameSizeX, int frameSizeY, std::string& kanjiPath)
        : wxFrame(NULL, wxID_ANY, "", wxDefaultPosition, wxSize { frameSizeX, frameSizeY })
        , questions { vec }
        , kanjiOutFile { kanjiPath, std::ios_base::app }
        , kanjiInFile { kanjiPath } {
        // Read file into String
        kanjiInFile >> kanjiFileString;

        // Canvas
        writeCanvas -> Create(this, ID_writeCanvas, PT_writeCanvas, SZ_writeCanvas);

        // Labels
        variantKanjiLabel -> Create(this, ID_variantKanjiLabel, wxEmptyString, PT_variantKanjiLabel, SZ_variantKanjiLabel);
        staticSizeLabel -> Create(this, ID_staticSizeLabel, "Size", PT_staticSizeLabel, SZ_staticSizeLabel);
        variantSizeLabel -> Create(this, ID_variantSizeLabel, wxEmptyString, PT_variantSizeLabel, SZ_variantSizeLabel);

        // Buttons
        enterButton -> Create(this, ID_enterButton, "Enter", PT_enterButton, SZ_enterButton);
        undoButton -> Create(this, ID_undoButton, "Undo", PT_undoButton, SZ_undoButton);
        clearButton -> Create(this, ID_clearButton, "Clear", PT_clearButton, SZ_clearButton);
        skipButton -> Create(this, ID_skipButton, "Skip", PT_skipButton, SZ_skipButton);

        // Slider
        sizeSlider -> Create(this, ID_sizeSlider, 1, 1, 10, PT_sizeSlider, SZ_sizeSlider, wxSL_VERTICAL);

        // Binds
        Bind(wxEVT_BUTTON, [this](wxCommandEvent& event) { writeCanvas -> clear(event); }, ID_clearButton);
        Bind(wxEVT_BUTTON, [this](wxCommandEvent& event) { writeCanvas -> undo(event); }, ID_undoButton);
        Bind(wxEVT_BUTTON, &Map::onEnter, this, ID_enterButton);
        Bind(wxEVT_BUTTON, &Map::onSkip, this, ID_skipButton);
        Bind(wxEVT_SLIDER, &Map::updateSlider, this, ID_sizeSlider);
    }
};
