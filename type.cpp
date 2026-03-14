#include <wx/wxprec.h>
#include <vector>
#include <string>
#include "GameClass.h"
#include "Join.h"
#include "constants.h"
#include "wx/xrc/xmlres.h"

#ifndef WX_PRECOMP
    #include <wx/wx.h>
    #include <wx/graphics.h>
#endif // WX_PRECOMP

class Type : public wxFrame {
    using stringMatrix = std::vector<std::vector<std::string>>;

    // Data
    GameData                  data;
    wxString                  question;
    stringMatrix&             answers;
    std::vector<std::string>& questions;
    std::string               textEntry;
    wxTimer                   basicTimer;
    wxTimer                   timer;
    wxDECLARE_EVENT_TABLE();

    // Labels
    wxStaticText* staticTurnLabel;
    wxStaticText* variantTurnLabel;
    wxStaticText* staticQuestionLabel;
    wxStaticText* variantQuestionLabel;
    wxStaticText* staticCorrectLabel;
    wxStaticText* variantCorrectLabel;

    // Text controls
    wxTextCtrl* entry;

    // Id
    int ID_entry;

    // Button
    wxButton* enterButton;

    // Id
    int ID_enterButton;

    void startGame() {
        ++data.turns;
        variantTurnLabel     -> SetLabel(std::to_string(data.turns));
        variantQuestionLabel -> SetLabel(questions[data.getCurrentIndex()]);
        variantQuestionLabel -> Wrap(200);
    }

    void checkAnswer(wxCommandEvent& event) {
        Unbind(wxEVT_TEXT_ENTER, &Type::checkAnswer, this, ID_entry);
        Unbind(wxEVT_BUTTON, &Type::checkAnswer, this, ID_enterButton);
        entry -> SetWindowStyle(wxTE_READONLY);
        textEntry = (entry -> GetLineText(0)).utf8_string();


        if (std::find(answers[data.getCurrentIndex()].begin(), answers[data.getCurrentIndex()].end(), textEntry) != answers[data.getCurrentIndex()].end()) {
            ++data.amountCorrect;
            staticCorrectLabel -> SetLabel("Correct!");
            timer.StartOnce(1000);
        }

        else {
            ++data.amountIncorrect;
            staticCorrectLabel  -> SetLabel("Incorrect :(");
            variantCorrectLabel -> SetLabel(wxString::FromUTF8(join(answers[data.getCurrentIndex()])));
            variantCorrectLabel -> Wrap(200);
            ifWrong();
        }
    }

    void checkSecondEntry(wxCommandEvent& event) {
        textEntry = (entry -> GetLineText(0)).utf8_string();
        if (std::find(answers[data.getCurrentIndex()].begin(), answers[data.getCurrentIndex()].end(), textEntry) != answers[data.getCurrentIndex()].end()) {
            staticCorrectLabel -> SetLabel("Correct!");
            variantCorrectLabel -> SetLabel(wxEmptyString);
            timer.StartOnce(2000);
        }

        else if (textEntry == "") {
            Unbind(wxEVT_TEXT_ENTER, &Type::checkSecondEntry, this, ID_entry);
            Unbind(wxEVT_BUTTON, &Type::checkSecondEntry, this, ID_enterButton);
            restart_cycle();
        }

        else {
            variantQuestionLabel -> SetLabel("Still wrong. Try again.");
            basicTimer.StartOnce(2000);
            ifWrong();
        }
    }

    void ifWrong() {
        Bind(wxEVT_TEXT_ENTER, &Type::checkSecondEntry, this, ID_entry);
        Bind(wxEVT_BUTTON, &Type::checkSecondEntry, this, ID_enterButton);
        entry -> Clear();
        entry -> SetWindowStyle(wxTE_PROCESS_ENTER);
        variantQuestionLabel -> SetLabel("Please enter correct answer; Press enter immediately to skip.");
    }

    void endFrame() {
        double percentage { ((double)data.amountCorrect/(double)data.turns)*100 };
    	int    answer = wxMessageBox("You are done!\n You got " + std::to_string(data.amountCorrect) + " questions right and " + std::to_string(data.amountIncorrect) + " questions wrong with a percentage of " 
                                  + std::to_string(percentage) + "!\n Would you like to play again?", "Play again?", wxYES_NO, this, 0, 125);
        if (answer == wxYES) { startFromEnd(); }
        else { this -> Close(); }
    }

    void startFromEnd() {
        data.reset();
        Bind(wxEVT_TEXT_ENTER, &Type::checkAnswer, this, ID_entry);
        Bind(wxEVT_BUTTON, &Type::checkAnswer, this, ID_enterButton);
        entry -> Clear();
        entry -> SetWindowStyle(wxTE_PROCESS_ENTER);
        startGame();
    }

public:
    Type(std::vector<std::string>& vec, stringMatrix& matrix, int frameSizeX, int frameSizeY)
        : data { vec, matrix }, answers { data.matrix }
        , questions { data.stringVec }, timer(this, TimerId::ID_typeTimer) {

        wxXmlResource::Get() -> LoadFrame(this, nullptr, "Type");

        // Labels
        staticTurnLabel      = XRCCTRL(*this, "staticTurnLabel", wxStaticText);
        variantTurnLabel     = XRCCTRL(*this, "variantTurnLabel", wxStaticText);
        staticQuestionLabel  = XRCCTRL(*this, "staticQuestionLabel", wxStaticText);
        variantQuestionLabel = XRCCTRL(*this, "variantQuestionLabel", wxStaticText);
        staticCorrectLabel   = XRCCTRL(*this, "staticCorrectLabel", wxStaticText);
        variantCorrectLabel  = XRCCTRL(*this, "variantCorrectLabel", wxStaticText);

        // Text Control
        entry    = XRCCTRL(*this, "entry", wxTextCtrl);
        ID_entry = XRCID("entry");

        // Button	
        enterButton    = XRCCTRL(*this, "enterButton", wxButton);
        ID_enterButton = XRCID("enterButton");

        Bind(wxEVT_TEXT_ENTER, &Type::checkAnswer, this, ID_entry);
        Bind(wxEVT_BUTTON, &Type::checkAnswer, this, ID_enterButton);

        // Start main loop
        startGame();
    }

    void restart_cycle(wxTimerEvent& event) {
        restart_cycle();
    }

    void restart_cycle() {
        staticCorrectLabel-> SetLabel(wxEmptyString);
        variantCorrectLabel-> SetLabel(wxEmptyString);
        Bind(wxEVT_TEXT_ENTER, &Type::checkAnswer, this, ID_entry);
        Bind(wxEVT_BUTTON, &Type::checkAnswer, this, ID_enterButton);
        if (data.turns < static_cast<int>(questions.size())) {
            entry -> Clear();
            entry -> SetWindowStyle(wxTE_PROCESS_ENTER);
            startGame();
        }

        else { endFrame(); }
    }
};

wxBEGIN_EVENT_TABLE(Type, wxFrame)
    EVT_TIMER(TimerId::ID_typeTimer, Type::restart_cycle)
wxEND_EVENT_TABLE()
