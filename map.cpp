#include <cstddef>
#include <wx/wxprec.h>
#include <vector>
#include <string>
#include <unordered_map>
#include "canvas.h"
#include "kanjiTxtProcessor.h"
#include "constants.h"

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
        ID_staticKanjiLabel,
        ID_variantKanjiLabel,
        ID_staticOnLabel,
        ID_variantOnLabel,
        ID_staticKunLabel,
        ID_variantKunLabel,
        ID_staticMeaningLabel,
        ID_variantMeaningLabel,
        ID_enterButton,
        ID_undoButton,
        ID_clearButton,
        ID_skipButton,
        ID_finishedButton
    };

    // Data
    std::vector<std::string>& questions;
    stringMatrix& aboutKanji;
    std::size_t turns { 0 };
    std::string kanjiPath;
    std::unordered_map<std::string, Lines> encodedDrawings;

    // Canvas
    Canvas* writeCanvas;

    // Canvas Points
    wxPoint PT_writeCanvas { 0, 50 };

    // Canvas Size
    wxSize SZ_writeCanvas { 425, 400 };

    // Labels
    wxStaticText* staticKanjiLabel;
    wxStaticText* variantKanjiLabel;
    wxStaticText* staticOnLabel;
    wxStaticText* variantOnLabel;
    wxStaticText* staticKunLabel;
    wxStaticText* variantKunLabel;
    wxStaticText* staticMeaningLabel;
    wxStaticText* variantMeaningLabel;

    // Label points
    wxPoint PT_staticKanjiLabel { 0, 0 };
    wxPoint PT_variantKanjiLabel { 0, 30 };
    wxPoint PT_staticOnLabel { PT_staticKanjiLabel.x + 125, 0 };
    wxPoint PT_variantOnLabel { PT_staticOnLabel.x, 30};
    wxPoint PT_staticKunLabel { PT_staticOnLabel.x + 125, 0 };
    wxPoint PT_variantKunLabel { PT_staticKunLabel.x, 30 };
    wxPoint PT_staticMeaningLabel { PT_staticKunLabel.x + 125, 0};
    wxPoint PT_variantMeaningLabel { PT_staticMeaningLabel.x, 30 };

    // Label sizes
    wxSize SZ_staticKanjiLabel { 100, 25 };
    wxSize SZ_variantKanjiLabel { 100, 25 };
    wxSize SZ_staticOnLabel { 100, 25 };
    wxSize SZ_variantOnLabel { 100, 25 };
    wxSize SZ_staticKunLabel { 100, 25 };
    wxSize SZ_variantKunLabel { 100, 25 };
    wxSize SZ_staticMeaningLabel { 100, 25 };
    wxSize SZ_variantMeaningLabel { 100, 25 };

    // Buttons
    wxButton* enterButton;
    wxButton* undoButton;
    wxButton* clearButton;
    wxButton* skipButton;
    wxButton* finishedButton;

    // Button points
    wxPoint PT_enterButton { SZ_variantKanjiLabel.x + SZ_writeCanvas.x + 25, 110 };
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
        variantOnLabel -> SetLabel(wxString::FromUTF8(aboutKanji[turns - 1][KanjiPlacements::on]));
        variantKunLabel -> SetLabel(wxString::FromUTF8(aboutKanji[turns -1][KanjiPlacements::kun]));
        variantMeaningLabel -> SetLabel(wxString::FromUTF8(aboutKanji[turns - 1][KanjiPlacements::meaning]));
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
    Map(stringMatrix& matrix, std::vector<std::string>& vec, int frameSizeX, int frameSizeY, std::string& kanjiPath)
        : wxFrame(NULL, wxID_ANY, "", wxDefaultPosition, wxSize { frameSizeX, frameSizeY })
        , questions { vec }
        , aboutKanji { matrix }
        , kanjiPath { kanjiPath } {
        // Load in previous definitions
        Processor::decodeFromTxt(encodedDrawings, kanjiPath);

        // Canvas
        writeCanvas = new Canvas(this, ID_writeCanvas, PT_writeCanvas, SZ_writeCanvas);

        // Labels
        staticKanjiLabel    = new wxStaticText(this, ID_staticKanjiLabel, "Kanji", PT_staticKanjiLabel, SZ_staticKanjiLabel);
        variantKanjiLabel   = new wxStaticText(this, ID_variantKanjiLabel, wxEmptyString, PT_variantKanjiLabel, SZ_variantKanjiLabel);
        staticOnLabel       = new wxStaticText(this, ID_staticOnLabel, "On reading", PT_staticOnLabel, SZ_staticOnLabel);
        variantOnLabel      = new wxStaticText(this, ID_variantOnLabel, wxEmptyString, PT_variantOnLabel, SZ_variantOnLabel);
        staticKunLabel      = new wxStaticText(this, ID_staticKunLabel, "Kun reading", PT_staticKunLabel, SZ_staticKunLabel);
        variantKunLabel     = new wxStaticText(this, ID_variantKunLabel, wxEmptyString, PT_variantKunLabel, SZ_variantKunLabel);
        staticMeaningLabel  = new wxStaticText(this, ID_staticMeaningLabel, "Meaning", PT_staticMeaningLabel, SZ_staticMeaningLabel);
        variantMeaningLabel = new wxStaticText(this, ID_variantMeaningLabel, wxEmptyString, PT_variantMeaningLabel, SZ_variantMeaningLabel);

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
