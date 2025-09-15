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
    using stringMatrix = std::vector<std::vector<std::string>>;
    using Lines = std::vector<std::vector<wxPoint>>;
    enum {
        ID_writeCanvas,
        ID_variantKanjiLabel,
        ID_enterButton,
        ID_undoButton,
        ID_clearButton,
        ID_skipButton,
        ID_finishedButton
    };

    // Data
    std::vector<std::string> questions;
    std::size_t turns { 0 };
    std::string kanjiPath;
    std::unordered_map<std::string, Lines> encodedDrawings;

    // Canvas
    Canvas* writeCanvas;

    // Canvas Points
    wxPoint PT_writeCanvas { 200, 0 };

    // Canvas Size
    wxSize SZ_writeCanvas { 425, 400 };

    // Labels
    wxStaticText* variantKanjiLabel;

    // Label points
    wxPoint PT_variantKanjiLabel { 0, 0 };

    // Label sizes
    wxSize SZ_variantKanjiLabel { 200, 400 };

    // Buttons
    wxButton* enterButton;
    wxButton* undoButton;
    wxButton* clearButton;
    wxButton* skipButton;
    wxButton* finishedButton;

    // Button points
    wxPoint PT_enterButton { SZ_variantKanjiLabel.x + SZ_writeCanvas.x + 25, 60 };
    wxPoint PT_undoButton { PT_enterButton.x, PT_enterButton.y + 60 };
    wxPoint PT_clearButton { PT_enterButton.x, PT_undoButton.y + 90 };
    wxPoint PT_skipButton { PT_enterButton.x, PT_clearButton.y + 60 };
    wxPoint PT_finishedButton { PT_enterButton.x, PT_skipButton.y + 90 };

    // Button sizes
    wxSize SZ_enterButton { 50, 25 };
    wxSize SZ_undoButton { 50, 25 };
    wxSize SZ_clearButton { 50, 25 };
    wxSize SZ_skipButton { 50, 25 };
    wxSize SZ_finishedButton { 50, 25 };

    void startGame() {
        ++turns;
        variantKanjiLabel -> SetLabel(wxString::FromUTF8(questions[turns - 1]));
    }

    void onEnter(wxCommandEvent& event) {
        encodedDrawings[questions[turns - 1]] = writeCanvas -> getDrawing();
        writeCanvas -> clear();
        if (turns < questions.size()) startGame();
        else {
            Processor::encodeToTxt(kanjiPath, encodedDrawings);
            Close();
        }
    }

    void onSkip(wxCommandEvent& event) {
        writeCanvas -> clear();
        startGame();
    }

    void onFinish(wxCommandEvent& event) {
        Processor::encodeToTxt(kanjiPath, encodedDrawings);
        Close();
    }

public:
    Map(std::vector<std::string>& vec, int frameSizeX, int frameSizeY, std::string& kanjiPath)
        : wxFrame(NULL, wxID_ANY, "", wxDefaultPosition, wxSize { frameSizeX, frameSizeY })
        , questions { vec }
        , kanjiPath { kanjiPath } {
        // Load in previous definitions
        Processor::decodeFromTxt(encodedDrawings, kanjiPath);

        // Canvas
        writeCanvas = new Canvas(this, ID_writeCanvas, PT_writeCanvas, SZ_writeCanvas);

        // Labels
        variantKanjiLabel = new wxStaticText(this, ID_variantKanjiLabel, wxEmptyString, PT_variantKanjiLabel, SZ_variantKanjiLabel);

        // Buttons
        enterButton    = new wxButton(this, ID_enterButton, "Enter", PT_enterButton, SZ_enterButton);
        undoButton     = new wxButton(this, ID_undoButton, "Undo", PT_undoButton, SZ_undoButton);
        clearButton    = new wxButton(this, ID_clearButton, "Clear", PT_clearButton, SZ_clearButton);
        skipButton     = new wxButton(this, ID_skipButton, "Skip", PT_skipButton, SZ_skipButton);
        finishedButton = new wxButton(this, ID_finishedButton, "Finished!", PT_finishedButton, SZ_finishedButton);

        // Binds
        Bind(wxEVT_BUTTON, [this](wxCommandEvent& event) { writeCanvas -> clear(event); }, ID_clearButton);
        Bind(wxEVT_BUTTON, [this](wxCommandEvent& event) { writeCanvas -> undo(event); }, ID_undoButton);
        Bind(wxEVT_BUTTON, &Map::onEnter, this, ID_enterButton);
        Bind(wxEVT_BUTTON, &Map::onSkip, this, ID_skipButton);
        Bind(wxEVT_BUTTON, &Map::onFinish, this, ID_finishedButton);

        // Start Main Loop
        startGame();
    }
};
