#include <vector>
#include <string>
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
    #include <wx/graphics.h>
#endif // WX_PRECOMP
#include "wx/xrc/xmlres.h"
#include "constants.h"
#include "GameClass.h"
#include "Join.h"

class Flashcards : public wxFrame {
    using stringMatrix = std::vector<std::vector<std::string>>;
    GameData data;
    bool isKanji;

    // Panels
    wxPanel* questionPanel;
    wxPanel* answerPanel;

    // Labels
    wxStaticText* staticMeaningLabel;
    wxStaticText* variantMeaningLabel;
    wxStaticText* staticOnLabel;
    wxStaticText* variantOnLabel;
    wxStaticText* staticKunLabel;
    wxStaticText* variantKunLabel;
    wxStaticText* staticAnswerLabel;
    wxStaticText* variantAnswerLabel;

    // Buttons
    wxButton* nextCardButton;
    wxButton* previousCardButton;

    void startGame() {
        if (isKanji) {
            variantOnLabel -> SetLabel(wxString::FromUTF8(data.matrix[data.indices[data.turns]][KanjiPlacements::on]));
            variantOnLabel ->Wrap(250);
            variantKunLabel -> SetLabel(wxString::FromUTF8(data.matrix[data.indices[data.turns]][KanjiPlacements::kun]));
            variantKunLabel ->Wrap(250);
            variantMeaningLabel -> SetLabel(wxString::FromUTF8(data.matrix[data.indices[data.turns]][KanjiPlacements::meaning]));
        }

        else {
            variantMeaningLabel -> SetLabel(wxString::FromUTF8(data.stringVec[data.indices[data.turns]]));
        }
        variantMeaningLabel ->Wrap(250);
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
        variantAnswerLabel -> Wrap(200);
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
    : data { vec, matrix }, isKanji { isKanji }
    {
        wxXmlResource::Get() -> LoadFrame(this, nullptr, "Flashcards");
        // Learn Mode
        int learnMode = wxMessageBox("Would you like to use Learn Mode? (Questions not randomized)", "Yes!", wxYES_NO, this, 0, 125);
        if (learnMode == wxYES) data.setConsecutiveIndices();

        // Panels
        questionPanel = XRCCTRL(*this, "questionPanel", wxPanel);
        answerPanel   = XRCCTRL(*this, "answerPanel", wxPanel);

        // Labels
        staticOnLabel       = XRCCTRL(*this, "staticOnLabel", wxStaticText);
        variantOnLabel      = XRCCTRL(*this, "variantOnLabel", wxStaticText);
        staticKunLabel      = XRCCTRL(*this, "staticKunLabel", wxStaticText);
        variantKunLabel     = XRCCTRL(*this, "variantKunLabel", wxStaticText);
        staticMeaningLabel  = XRCCTRL(*this, "staticMeaningLabel", wxStaticText);
        variantMeaningLabel = XRCCTRL(*this, "variantMeaningLabel", wxStaticText);
        staticAnswerLabel   = XRCCTRL(*this, "staticAnswerLabel", wxStaticText);
        variantAnswerLabel  = XRCCTRL(*this, "variantAnswerLabel", wxStaticText);

        // Buttons
        nextCardButton     = XRCCTRL(*this, "nextCardButton", wxButton);
        previousCardButton = XRCCTRL(*this, "previousCardButton", wxButton);

        Bind(wxEVT_BUTTON, &Flashcards::nextCardButtonEvt, this, XRCID("nextCardButton"));
        Bind(wxEVT_KEY_DOWN, &Flashcards::keyEvents, this, wxID_ANY);
        questionPanel -> Bind(wxEVT_KEY_DOWN, &Flashcards::keyEvents, this, wxID_ANY);
        answerPanel -> Bind(wxEVT_KEY_DOWN, &Flashcards::keyEvents, this, wxID_ANY);

        startGame();
    }
};
