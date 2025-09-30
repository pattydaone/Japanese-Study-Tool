#include <vector>
#include <string>
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
    #include <wx/graphics.h>
#endif // WX_PRECOMP
#include "constants.h"
#include "GameClass.h"
#include "Join.h"

class Flashcards : public wxFrame {
    using stringMatrix = std::vector<std::vector<std::string>>;
    enum {
        NULLID = 0,
        ID_questionPanel,
        ID_answerPanel,
        ID_staticMeaningLabel,
        ID_variantMeaningLabel,
        ID_staticOnLabel,
        ID_variantOnLabel,
        ID_staticKunLabel,
        ID_variantKunLabel,
        ID_variantAnswerLabel,
        ID_showAnswerButton,
        ID_nextCardButton
    };
    GameData data;
    bool isKanji;

    // Panels
    wxPanel* questionPanel;
    wxPanel* answerPanel;

    // Panel Points
    wxPoint PT_questionPanel {   0, 0 };
    wxPoint PT_answerPanel   { 500, 0 };

    // Panel Sizes
    wxSize SZ_questionPanel { 500, 500 };
    wxSize SZ_answerPanel   { 500, 500 };

    // Labels
    wxStaticText* staticMeaningLabel;
    wxStaticText* variantMeaningLabel;
    wxStaticText* staticOnLabel;
    wxStaticText* variantOnLabel;
    wxStaticText* staticKunLabel;
    wxStaticText* variantKunLabel;
    wxStaticText* variantAnswerLabel;

    // Label Points
    wxPoint PT_staticMeaningLabel  { 225, 100 };
    wxPoint PT_variantMeaningLabel {  50, 150 };
    wxPoint PT_staticOnLabel       { 225, 200 };
    wxPoint PT_variantOnLabel      { 200, 250 };
    wxPoint PT_staticKunLabel      { 225, 300 };
    wxPoint PT_variantKunLabel     { 200, 350 };
    wxPoint PT_variantAnswerLabel  {  50, 100 };

    // Label Sizes
    wxSize SZ_staticMeaningLabel  {  50,  25 };
    wxSize SZ_variantMeaningLabel { 400, 100 };
    wxSize SZ_staticOnLabel       {  50,  25 };
    wxSize SZ_variantOnLabel      { 100,  50 };
    wxSize SZ_staticKunLabel      {  50,  25 };
    wxSize SZ_variantKunLabel     { 100,  50 };
    wxSize SZ_variantAnswerLabel  { 400, 100 };

    // Buttons
    wxButton* showAnswerButton;
    wxButton* nextCardButton;

    // Button Points
    wxPoint PT_showAnswerButton { 400, 525 };
    wxPoint PT_nextCardButton   { 600, 525 };

    // Button Sizes
    wxSize SZ_showAnswerButton { 50, 25 };
    wxSize SZ_nextCardButton   { 50, 25 };

    void startGame() {
        if (isKanji) {
            variantOnLabel -> SetLabel(wxString::FromUTF8(data.matrix[data.indices[data.turns]][KanjiPlacements::on]));
            variantKunLabel -> SetLabel(wxString::FromUTF8(data.matrix[data.indices[data.turns]][KanjiPlacements::kun]));
            variantMeaningLabel -> SetLabel(wxString::FromUTF8(data.matrix[data.indices[data.turns]][KanjiPlacements::meaning]));
        }

        else {
            variantMeaningLabel -> SetLabel(wxString::FromUTF8(data.stringVec[data.indices[data.turns]]));
        }
    }

    void showAnswer(wxCommandEvent& event) {
        showAnswer();
    }

    void showAnswer() {
        if (isKanji) {
            variantAnswerLabel -> SetLabel(wxString::FromUTF8(data.stringVec[data.indices[data.turns]]));
        }

        else {
            variantAnswerLabel -> SetLabel(wxString::FromUTF8(join(data.matrix[data.indices[data.turns]])));
        }
    }

    void nextCardButtonEvt(wxCommandEvent& event) {
        nextCard();
    }

    void keyEvents(wxKeyEvent& event) {
        switch (event.GetKeyCode()) {
            case (WXK_RETURN): nextCard(); break;
            case (WXK_RIGHT): nextCard(); break;
            case (WXK_LEFT): previousCard(); break;
            case (WXK_SPACE): showAnswer(); break;
        }
    }

    void nextCard() {
        if (data.turns + 1 < data.stringVec.size()) {
            variantAnswerLabel -> SetLabel(wxEmptyString);
            ++data.turns;
            startGame();
        }

        else {
            // Do something
        }
    }

    void previousCard() {
        if (data.turns > 0) {
            variantAnswerLabel -> SetLabel(wxEmptyString);
            --data.turns;
            startGame();
        }

        else {
            // Do something... Or nothing!
        }
    }

public:
    Flashcards(std::vector<std::string>& vec, stringMatrix& matrix, int frameSizeX, int frameSizeY, bool isKanji)
    : wxFrame(NULL, wxID_ANY, "", wxDefaultPosition, wxSize { frameSizeX, frameSizeY })
    , data { vec, matrix }
    , isKanji { isKanji }
    {
        // Learn Mode
        int learnMode = wxMessageBox("Would you like to use Learn Mode? (Questions not randomized)", "Yes!", wxYES_NO, this, 0, 125);
        if (learnMode) data.setConsecutiveIndices();

        // Panels
        questionPanel = new wxPanel(this, ID_questionPanel, PT_questionPanel, SZ_questionPanel);
        answerPanel   = new wxPanel(this, ID_answerPanel, PT_answerPanel, SZ_answerPanel);

        // Labels
        if (isKanji) {
            staticOnLabel   = new wxStaticText(questionPanel, ID_staticOnLabel, "On Reading: ", PT_staticOnLabel, SZ_staticOnLabel, wxALIGN_CENTRE_HORIZONTAL);
            variantOnLabel  = new wxStaticText(questionPanel, ID_variantOnLabel, wxEmptyString, PT_variantOnLabel, SZ_variantOnLabel, wxALIGN_CENTRE_HORIZONTAL);
            staticKunLabel  = new wxStaticText(questionPanel, ID_staticKunLabel, "Kun Reading: ", PT_staticKunLabel, SZ_staticKunLabel, wxALIGN_CENTRE_HORIZONTAL);
            variantKunLabel = new wxStaticText(questionPanel, ID_variantKunLabel, wxEmptyString, PT_variantKunLabel, SZ_variantKunLabel, wxALIGN_CENTRE_HORIZONTAL);
        }
        staticMeaningLabel  = new wxStaticText(questionPanel, ID_staticMeaningLabel, "Meaning: ", PT_staticMeaningLabel, SZ_staticMeaningLabel, wxALIGN_CENTRE_HORIZONTAL);
        variantMeaningLabel = new wxStaticText(questionPanel, ID_variantMeaningLabel, wxEmptyString, PT_variantMeaningLabel, SZ_variantMeaningLabel, wxALIGN_CENTRE_HORIZONTAL);
        variantAnswerLabel  = new wxStaticText(answerPanel, ID_variantAnswerLabel, wxEmptyString, PT_variantAnswerLabel, SZ_variantAnswerLabel, wxALIGN_CENTRE_HORIZONTAL);

        // Buttons
        showAnswerButton = new wxButton(this, ID_showAnswerButton, "Show Answer", PT_showAnswerButton, SZ_showAnswerButton);
        nextCardButton   = new wxButton(this, ID_nextCardButton, "Next", PT_nextCardButton, SZ_nextCardButton);

        Bind(wxEVT_BUTTON, &Flashcards::showAnswer, this, ID_showAnswerButton);
        Bind(wxEVT_BUTTON, &Flashcards::nextCardButtonEvt, this, ID_nextCardButton);
        Bind(wxEVT_KEY_DOWN, &Flashcards::keyEvents, this, wxID_ANY);
        questionPanel -> Bind(wxEVT_KEY_DOWN, &Flashcards::keyEvents, this, wxID_ANY);
        answerPanel -> Bind(wxEVT_KEY_DOWN, &Flashcards::keyEvents, this, wxID_ANY);

        startGame();
    }
};
