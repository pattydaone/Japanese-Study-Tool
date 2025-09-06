#include <cstddef>
#include <wx/wxprec.h>
#include <vector>
#include <string>
#include <unordered_map>
#include "canvas.h"
#include "kanjiTxtProcessor.h"

#ifndef WX_PRECOMP
    #include <wx/wx.h>
    #include <wx/graphics.h>
    #include <wx/dcbuffer.h>
#endif // WX_PRECOMP

class Map : public wxFrame {
    using string_matrix = std::vector<std::vector<std::string>>;
    using Lines = std::vector<std::vector<wxPoint>>;
    enum {
        ID_writeCanvas,
        ID_variantKanjiLabel,
        ID_enterButton,
        ID_undoButton,
        ID_clearButton,
        ID_skipButton,
    };

    // Data
    std::vector<std::string> questions;
    std::size_t turns { 0 };
    std::string kanjiPath;
    std::unordered_map<std::string, Lines> encodedDrawings;

    // Canvas
    Canvas* writeCanvas = new Canvas;

    // Canvas Points
    wxPoint PT_writeCanvas { 450, 0 };

    // Canvas Size
    wxSize SZ_writeCanvas { 425, 400 };

    // Labels
    wxStaticText* variantKanjiLabel = new wxStaticText;

    // Label points
    wxPoint PT_variantKanjiLabel { 0, 0 };

    // Label sizes
    wxSize SZ_variantKanjiLabel { 425, 400 };

    // Buttons
    wxButton* enterButton = new wxButton;
    wxButton* undoButton = new wxButton;
    wxButton* clearButton = new wxButton;
    wxButton* skipButton = new wxButton;

    // Button points
    wxPoint PT_enterButton { 475, 0 };
    wxPoint PT_undoButton { 475, 30};
    wxPoint PT_clearButton { 475, 100 };
    wxPoint PT_skipButton { 475, 130 };

    // Button sizes
    wxSize SZ_enterButton { 50, 25 };
    wxSize SZ_undoButton { 50, 25 };
    wxSize SZ_clearButton { 50, 25 };
    wxSize SZ_skipButton { 50, 25 };

    void startGame() {
        ++turns;
        variantKanjiLabel -> SetLabel(wxString::FromUTF8(questions[turns - 1]));
    }

    void onEnter(wxCommandEvent& event) {
        encodedDrawings[questions[turns - 1]] = writeCanvas -> getDrawing();
        if (turns < questions.size()) startGame();
        else {
            Processor::encodeToTxt(kanjiPath, encodedDrawings);
            this -> Close();
        }
    }

    void onSkip(wxCommandEvent& event) {
        writeCanvas -> clear();
        startGame();
    }

public:
    Map(std::vector<std::string>& vec, int frameSizeX, int frameSizeY, std::string& kanjiPath)
        : wxFrame(NULL, wxID_ANY, "", wxDefaultPosition, wxSize { frameSizeX, frameSizeY })
        , questions { vec }
        , kanjiPath { kanjiPath } {
        // Load in previous definitions
        Processor::decodeFromTxt(encodedDrawings, kanjiPath);

        // Canvas
        writeCanvas -> Create(this, ID_writeCanvas, PT_writeCanvas, SZ_writeCanvas);

        // Labels
        variantKanjiLabel -> Create(this, ID_variantKanjiLabel, wxEmptyString, PT_variantKanjiLabel, SZ_variantKanjiLabel);

        // Buttons
        enterButton -> Create(this, ID_enterButton, "Enter", PT_enterButton, SZ_enterButton);
        undoButton -> Create(this, ID_undoButton, "Undo", PT_undoButton, SZ_undoButton);
        clearButton -> Create(this, ID_clearButton, "Clear", PT_clearButton, SZ_clearButton);
        skipButton -> Create(this, ID_skipButton, "Skip", PT_skipButton, SZ_skipButton);

        // Binds
        Bind(wxEVT_BUTTON, [this](wxCommandEvent& event) { writeCanvas -> clear(event); }, ID_clearButton);
        Bind(wxEVT_BUTTON, [this](wxCommandEvent& event) { writeCanvas -> undo(event); }, ID_undoButton);
        Bind(wxEVT_BUTTON, &Map::onEnter, this, ID_enterButton);
        Bind(wxEVT_BUTTON, &Map::onSkip, this, ID_skipButton);
    }

    ~Map() {
        Processor::encodeToTxt(kanjiPath, encodedDrawings);
    }
};
