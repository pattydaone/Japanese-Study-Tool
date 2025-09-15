#include <wx/wxprec.h>
#include <vector>
#include <string>
#include <fstream>
#include <unordered_map>
#include "GameClass.h"
#include "constants.h"
#include "canvas.h"
#include "kanjiTxtProcessor.h"

#ifndef WX_PRECOMP
    #include <wx/wx.h>
    #include <wx/graphics.h>
    #include <wx/dcbuffer.h>
#endif // WX_PRECOMP


class Write : public wxFrame {
    using stringMatrix = std::vector<std::vector<std::string>>;
    using Lines = std::vector<std::vector<wxPoint>>;
    enum {
        ID_writeCanvas,
        ID_animationCanvas,
        ID_staticOnLabel,
        ID_staticKunLabel,
        ID_staticMeaningLabel,
        ID_variantOnLabel,
        ID_variantKunLabel,
        ID_variantMeaningLabel,
        ID_staticSizeLabel,
        ID_variantSizeLabel,
        ID_enterButton,
        ID_showAnswerButton,
        ID_undoButton,
        ID_clearButton,
        ID_sizeSlider,
    };

    // Data
    wxTimer timer;
    wxDECLARE_EVENT_TABLE();
    GameData data;
    stringMatrix& writeQuestions; 
    std::vector<std::string>& writeAnswers; 
    int penSize;
    std::ifstream kanjiFile;
    std::unordered_map<std::string, Lines> encodedDrawings;
    
    // Canvas
    Canvas* writeCanvas;
    AnimationCanvas* animationCanvas;

    // Canvas points
    wxPoint PT_writeCanvas     { 50, 60 };
    wxPoint PT_animationCanvas { 625, 60 };

    // Canvas sizes
    wxSize SZ_writeCanvas     { 425, 400 };
    wxSize SZ_animationCanvas { 425, 400 };

    // Labels
    wxStaticText* staticOnLabel;
    wxStaticText* variantOnLabel;
    wxStaticText* staticKunLabel;
    wxStaticText* variantKunLabel;
    wxStaticText* staticMeaningLabel;
    wxStaticText* variantMeaningLabel;
    wxStaticText* staticSizeLabel;
    wxStaticText* variantSizeLabel;

    // Label points
    wxPoint PT_staticOnLabel       { 100, 0 };
    wxPoint PT_variantOnLabel      { PT_staticOnLabel.x, PT_staticOnLabel.y + 25 };
    wxPoint PT_staticKunLabel      { 250, 0 };
    wxPoint PT_variantKunLabel     { PT_staticKunLabel.x, PT_staticKunLabel.y + 25 };
    wxPoint PT_staticMeaningLabel  { 400, 0 };
    wxPoint PT_variantMeaningLabel { PT_staticMeaningLabel.x, PT_staticMeaningLabel.y + 25 };
    wxPoint PT_staticSizeLabel     { 15, 0 };
    wxPoint PT_variantSizeLabel    { PT_staticSizeLabel.x, PT_staticSizeLabel.y + 15 };

    // Label sizes
    wxSize SZ_staticOnLabel       { 100, 25 };
    wxSize SZ_variantOnLabel      { 100, 25 };
    wxSize SZ_staticKunLabel      { 100, 25 };
    wxSize SZ_variantKunLabel     { 100, 25 };
    wxSize SZ_staticMeaningLabel  { 100, 25 };
    wxSize SZ_variantMeaningLabel { 100, 25 };
    wxSize SZ_staticSizeLabel     { 100, 25 };
    wxSize SZ_variantSizeLabel    { 100, 25 };

    // Buttons
    wxButton* enterButton;
    wxButton* showAnswerButton;
    wxButton* undoButton;
    wxButton* clearButton;

    // Button points
    wxPoint PT_enterButton      { 500, 60 };
    wxPoint PT_showAnswerButton { PT_enterButton.x, PT_enterButton.y + 60 };
    wxPoint PT_undoButton       { PT_showAnswerButton.x, PT_showAnswerButton.y + 90 };
    wxPoint PT_clearButton      { PT_undoButton.x, PT_undoButton.y + 60 };

    // Button sizes
    wxSize SZ_enterButton      { 50, 25 };
    wxSize SZ_showAnswerButton { 100, 25 };
    wxSize SZ_undoButton       { 50, 25 };
    wxSize SZ_clearButton      { 50, 25 };

    // Slider
    wxSlider* sizeSlider;

    // Slider point
    wxPoint PT_sizeSlider { 0, 30 };

    // Slider size
    wxSize SZ_sizeSlider { 50, 400 };

    void startGame() {
        writeCanvas -> clear();
        animationCanvas -> clear();
        ++data.turns;
        variantOnLabel -> SetLabel(wxString::FromUTF8(writeQuestions[data.indices[data.turns - 1]][0]));
        variantKunLabel -> SetLabel(wxString::FromUTF8(writeQuestions[data.indices[data.turns - 1]][1]));
        variantMeaningLabel -> SetLabel(wxString::FromUTF8(writeQuestions[data.indices[data.turns - 1]][2]));
    }

    void checkAnswer(wxCommandEvent& event) {
        Unbind(wxEVT_BUTTON, [this](wxCommandEvent& event) { writeCanvas -> clear(event); }, ID_clearButton);
        Unbind(wxEVT_BUTTON, [this](wxCommandEvent& event) { writeCanvas -> undo(event); }, ID_undoButton);
        Unbind(wxEVT_BUTTON, &Write::checkAnswer, this, ID_enterButton);
        Unbind(wxEVT_BUTTON, &Write::animation, this, ID_showAnswerButton);

        animation(event);
        timer.StartOnce(2000);
    }

    void endFrame() {
    	int answer = wxMessageBox("You are done ! Would you like to play again?", "Play again?", wxYES_NO, this, 0, 125);
        if (answer == wxYES) { startFromEnd(); }
        else { this -> Close(); }
    }

    void startFromEnd() {
        data.reset();
        startGame();
    }

    void animation(wxCommandEvent& event) {
        animationCanvas-> setDrawing(&encodedDrawings[writeAnswers[data.indices[data.turns - 1]]]);
    }

    void restartCycle(wxTimerEvent& event) {
        Bind(wxEVT_BUTTON, [this](wxCommandEvent& event) { writeCanvas -> clear(event); }, ID_clearButton);
        Bind(wxEVT_BUTTON, [this](wxCommandEvent& event) { writeCanvas -> undo(event); }, ID_undoButton);
        Bind(wxEVT_BUTTON, &Write::checkAnswer, this, ID_enterButton);
        Bind(wxEVT_BUTTON, &Write::animation, this, ID_showAnswerButton);

        if (data.turns < (int)writeAnswers.size()) {
            startGame();
        } else { endFrame(); }
    }

    void update_slider_value(wxCommandEvent& event) {
        penSize = sizeSlider -> GetValue();
        writeCanvas -> setPenSize(penSize);
        variantSizeLabel -> SetLabel(std::to_string(penSize));
    }

public:
    Write(const std::vector<std::string>& vec, const stringMatrix& matrix, int frameSizeX, int frameSizeY, const std::string& kanjiPath)
        : wxFrame(NULL, wxID_ANY, "", wxDefaultPosition, wxSize { frameSizeX, frameSizeY })
        , timer(this, TimerId::ID_writeTimer) 
        , data { vec, matrix }
        , writeQuestions{ data.matrix }
        , writeAnswers{ data.stringVec } 
        , kanjiFile { kanjiPath } {
        // Load in Kanjis
        Processor::decodeFromTxt(encodedDrawings, kanjiPath);

        // Labels
        staticSizeLabel     = new wxStaticText(this, ID_staticSizeLabel, "Size", PT_staticSizeLabel, SZ_staticSizeLabel);
        variantSizeLabel    = new wxStaticText(this, ID_variantSizeLabel, "2", PT_variantSizeLabel, SZ_variantSizeLabel);
        staticOnLabel       = new wxStaticText(this, ID_staticOnLabel, "On reading", PT_staticOnLabel, SZ_staticOnLabel);
        variantOnLabel      = new wxStaticText(this, ID_variantOnLabel, wxEmptyString, PT_variantOnLabel, SZ_variantOnLabel);
        staticKunLabel      = new wxStaticText(this, ID_staticKunLabel, "Kun reading", PT_staticKunLabel, SZ_staticKunLabel);
        variantKunLabel     = new wxStaticText(this, ID_variantKunLabel, wxEmptyString, PT_variantKunLabel, SZ_variantKunLabel);
        staticMeaningLabel  = new wxStaticText(this, ID_staticMeaningLabel, "Meaning", PT_staticMeaningLabel, SZ_variantMeaningLabel);
        variantMeaningLabel = new wxStaticText(this, ID_variantMeaningLabel, wxEmptyString, PT_variantMeaningLabel, SZ_variantMeaningLabel);

        // Buttons
        enterButton      = new wxButton(this, ID_enterButton, "Enter", PT_enterButton, SZ_enterButton);
        showAnswerButton = new wxButton(this, ID_showAnswerButton, "Show answer", PT_showAnswerButton, SZ_showAnswerButton);
        undoButton       = new wxButton(this, ID_undoButton, "Undo", PT_undoButton, SZ_undoButton);
        clearButton      = new wxButton(this, ID_clearButton, "Clear", PT_clearButton, SZ_clearButton);

        // Slider
        sizeSlider = new wxSlider(this, ID_sizeSlider, 1, 1, 10, PT_sizeSlider, SZ_sizeSlider, wxSL_VERTICAL);

        // Canvas
        writeCanvas     = new Canvas(this, ID_writeCanvas, PT_writeCanvas, SZ_writeCanvas);
        animationCanvas = new AnimationCanvas(this, ID_animationCanvas, PT_animationCanvas, SZ_animationCanvas);

        // Event binds
        Bind(wxEVT_BUTTON, [this](wxCommandEvent& event) { writeCanvas -> clear(event); }, ID_clearButton);
        Bind(wxEVT_BUTTON, [this](wxCommandEvent& event) { writeCanvas -> undo(event); }, ID_undoButton);
        Bind(wxEVT_BUTTON, &Write::checkAnswer, this, ID_enterButton);
        Bind(wxEVT_BUTTON, &Write::animation, this, ID_showAnswerButton);
        Bind(wxEVT_SLIDER, &Write::update_slider_value, this, ID_sizeSlider);

        // Begin main loop
        startGame();
    }

};

wxBEGIN_EVENT_TABLE(Write, wxFrame)
    EVT_TIMER(TimerId::ID_writeTimer, Write::restartCycle)
wxEND_EVENT_TABLE()
