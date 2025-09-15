#include <wx/wxprec.h>
#include <vector>
#include <string>
#include "GameClass.h"
#include "Join.h"
#include "constants.h"

#ifndef WX_PRECOMP
    #include <wx/wx.h>
    #include <wx/graphics.h>
#endif // WX_PRECOMP

using stringMatrix = std::vector<std::vector<std::string>>;

class Type : public wxFrame {
    enum {
        ID_staticTurn,
        ID_variantTurn,
        ID_staticQuestion,
        ID_variantQuestion,
        ID_entry,
        ID_enterButton,
        ID_staticCorrectLabel,
        ID_variantCorrectLabel,
    };

    // Data
    GameData                  data;
    wxString                  question;
    stringMatrix&             answers;
    std::vector<std::string>& questions;
    std::string               textEntry;
    wxTimer                   timer;
    wxDECLARE_EVENT_TABLE();

    // Labels
    wxStaticText* staticTurn;
    wxStaticText* variantTurn;
    wxStaticText* staticQuestion;
    wxStaticText* variantQuestion;
    wxStaticText* staticCorrectLabel;
    wxStaticText* variantCorrectLabel;

    // Label points
    wxPoint PT_staticTurn          { 10, 0 };
    wxPoint PT_variantTurn         { PT_staticTurn.x - 10, PT_staticTurn.y + 25 };
    wxPoint PT_staticQuestion      { 0, 0 }; // x redefined later in terms of other variables for centering
    wxPoint PT_variantQuestion     { 0, PT_staticQuestion.y + 25 }; // x, again, defined later
    wxPoint PT_staticCorrectLabel  { 100, 100 };
    wxPoint PT_variantCorrectLabel { 100, 125 };

    // Label sizes
    wxSize SZ_staticTurn          { 50, 25 };
    wxSize SZ_variantTurn         { 50, 25 };
    wxSize SZ_staticQuestion      { 50, 25 };
    wxSize SZ_variantQuestion     { 300, 25 };
    wxSize SZ_staticCorrectLabel  { 200, 25 };
    wxSize SZ_variantCorrectLabel { 200, 25 };

    // Text control
    wxTextCtrl* entry;

    // Control point
    wxPoint PT_entry { 100, 75 };

    // Control size
    wxSize SZ_entry { 150, 25 };

    // Button
    wxButton* enterButton;

    // Button point
    wxPoint PT_enterButton { PT_entry.x + SZ_entry.x, PT_entry.y };

    // Button size
    wxSize SZ_enterButton { 50, 25 };

    void startGame() {
        ++data.turns;
        variantTurn     -> SetLabel(std::to_string(data.turns));
        variantQuestion -> SetLabel(questions[data.getCurrentIndex()]);
        variantQuestion -> Wrap(SZ_variantQuestion.x/2 + 50);
    }

    void checkAnswer(wxCommandEvent& event) {
        Unbind(wxEVT_TEXT_ENTER, &Type::checkAnswer, this, ID_entry);
        Unbind(wxEVT_BUTTON, &Type::checkAnswer, this, ID_enterButton);
        entry -> SetWindowStyle(wxTE_READONLY);
        textEntry = (entry -> GetLineText(0)).utf8_string();


        if (std::find(answers[data.getCurrentIndex()].begin(), answers[data.getCurrentIndex()].end(), textEntry) != answers[data.getCurrentIndex()].end()) {
            ++data.amountCorrect;
            staticCorrectLabel -> SetLabel("Correct!");
        }

        else {
            ++data.amountIncorrect;
            staticCorrectLabel  -> SetLabel("Incorrect :(");
            variantCorrectLabel -> SetLabel(wxString::FromUTF8(join(answers[data.getCurrentIndex()])));
            variantCorrectLabel -> Wrap(SZ_variantCorrectLabel.x/2 + 50);
        }

        timer.StartOnce(2000);	
    }

    void endFrame() {
        double percentage { ((double)data.amountCorrect/(double)data.amountIncorrect)*100 };
    	int    answer = wxMessageBox("You are done!\n You got " + std::to_string(data.amountCorrect) + " questions right and " + std::to_string(data.amountIncorrect) + " questions wrong with a percentage of " 
                                  + std::to_string(percentage) + "!\n Would you like to play again?", "Play again?", wxYES_NO, this, 0, 125);
        if (answer == wxYES) { startFromEnd(); }
        else { this -> Close(); }
    }

    void startFromEnd() {
        data.reset();
        startGame();
    }

public:
    Type(std::vector<std::string>& vec, stringMatrix& matrix, int frameSizeX, int frameSizeY)
        : wxFrame(NULL, wxID_ANY, "", wxDefaultPosition, wxSize { frameSizeX, frameSizeY })
        , data { vec, matrix }
        , answers { data.matrix }
        , questions { data.stringVec } 
        , timer(this, TimerId::ID_typeTimer)
        {
        // Setting some label points
        PT_staticQuestion.x = PT_entry.x + (SZ_entry.x + SZ_enterButton.x - SZ_staticQuestion.x)/2;
        PT_variantQuestion.x = PT_entry.x + (SZ_entry.x + SZ_enterButton.x - SZ_variantQuestion.x)/2;

        // Labels
        staticTurn          = new wxStaticText(this, ID_staticTurn, "Turn:", PT_staticTurn, SZ_staticTurn);
        variantTurn         = new wxStaticText(this, ID_variantTurn, wxEmptyString, PT_variantTurn, SZ_variantTurn, wxALIGN_CENTRE_HORIZONTAL);
        staticQuestion      = new wxStaticText(this, ID_staticQuestion, "Word: ", PT_staticQuestion, SZ_staticQuestion, wxALIGN_CENTRE_HORIZONTAL);
        variantQuestion     = new wxStaticText(this, ID_variantQuestion, wxEmptyString, PT_variantQuestion, SZ_variantQuestion, wxALIGN_CENTRE_HORIZONTAL);
        staticCorrectLabel  = new wxStaticText(this, ID_staticCorrectLabel, wxEmptyString, PT_staticCorrectLabel, SZ_staticCorrectLabel, wxALIGN_CENTRE_HORIZONTAL);
        variantCorrectLabel = new wxStaticText(this, ID_variantCorrectLabel, wxEmptyString, PT_variantCorrectLabel, SZ_variantCorrectLabel, wxALIGN_CENTRE_HORIZONTAL);

        // Text Control
        entry = new wxTextCtrl(this, ID_entry, wxEmptyString, PT_entry, SZ_entry, wxTE_PROCESS_ENTER);

        // Button	
        enterButton = new wxButton(this, ID_enterButton, "Enter", PT_enterButton, SZ_enterButton);

        Bind(wxEVT_TEXT_ENTER, &Type::checkAnswer, this, ID_entry);
        Bind(wxEVT_BUTTON, &Type::checkAnswer, this, ID_enterButton);

        // Start main loop
        startGame();
    }

    void restart_cycle(wxTimerEvent& event) {
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
