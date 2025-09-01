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

using string_matrix = std::vector<std::vector<std::string>>;

class Type : public wxFrame {
    enum {
        ID_static_turn,
        ID_variant_turn,
        ID_static_question,
        ID_variant_question,
        ID_entry,
        ID_enter_button,
        ID_static_correct_label,
        ID_variant_correct_label,
    };

    // Data
    GameData data;
    wxString q;
    string_matrix& answers;
    std::vector<std::string>& questions;
    std::string text_entry;
    wxTimer timer;
    wxDECLARE_EVENT_TABLE();

    // Labels
    wxStaticText* static_turn = new wxStaticText;
    wxStaticText* variant_turn = new wxStaticText;
    wxStaticText* static_question = new wxStaticText;
    wxStaticText* variant_question = new wxStaticText;
    wxStaticText* static_correct_label = new wxStaticText;
    wxStaticText* variant_correct_label = new wxStaticText;

    // Label points
    wxPoint PT_static_turn { 10, 0 };
    wxPoint PT_variant_turn { PT_static_turn.x - 10, PT_static_turn.y + 25 };
    wxPoint PT_static_question { 0, 0 }; // x redefined later in terms of other variables for centering
    wxPoint PT_variant_question { 0, PT_static_question.y + 25 }; // x, again, defined later
    wxPoint PT_static_correct_label { 100, 100 };
    wxPoint PT_variant_correct_label { 100, 125 };

    // Label sizes
    wxSize SZ_static_turn { 50, 25 };
    wxSize SZ_variant_turn { 50, 25 };
    wxSize SZ_static_question { 50, 25 };
    wxSize SZ_variant_question { 300, 25 };
    wxSize SZ_static_correct_label { 200, 25 };
    wxSize SZ_variant_correct_label { 200, 25 };

    // Text control
    wxTextCtrl* entry = new wxTextCtrl;

    // Control point
    wxPoint PT_entry { 100, 75 };

    // Control size
    wxSize SZ_entry { 150, 25 };

    // Button
    wxButton* enter_button = new wxButton;

    // Button point
    wxPoint PT_enter_button { PT_entry.x + SZ_entry.x, PT_entry.y };

    // Button size
    wxSize SZ_enter_button { 50, 25 };

    void start_game() {
        ++data.turns;
        variant_turn -> SetLabel(std::to_string(data.turns));
        variant_question -> SetLabel(questions[data.getCurrentIndex()]);
        variant_question -> Wrap(SZ_variant_question.x/2 + 50);
    }

    void check_answer(wxCommandEvent& event) {
        Unbind(wxEVT_TEXT_ENTER, &Type::check_answer, this, ID_entry);
        Unbind(wxEVT_BUTTON, &Type::check_answer, this, ID_enter_button);
        entry -> SetWindowStyle(wxTE_READONLY);
        text_entry = (entry -> GetLineText(0)).utf8_string();


        if (std::find(answers[data.getCurrentIndex()].begin(), answers[data.getCurrentIndex()].end(), text_entry) != answers[data.getCurrentIndex()].end()) {
            ++data.amnt_correct;
            static_correct_label -> SetLabel("Correct!");
        }

        else {
            ++data.amnt_incorrect;
            static_correct_label -> SetLabel("Incorrect :(");
            variant_correct_label -> SetLabel(wxString::FromUTF8(join(answers[data.getCurrentIndex()])));
            variant_correct_label -> Wrap(SZ_variant_correct_label.x/2 + 50);
        }

        timer.StartOnce(2000);	
    }

    void end_frame() {
        double percentage { ((double)data.amnt_correct/(double)data.amnt_incorrect)*100 };
    	int answer = wxMessageBox("You are done!\n You got " + std::to_string(data.amnt_correct) + " questions right and " + std::to_string(data.amnt_incorrect) + " questions wrong with a percentage of " 
                                  + std::to_string(percentage) + "!\n Would you like to play again?", "Play again?", wxYES_NO, this, 0, 125);
        if (answer == wxYES) { start_from_end(); }
        else { this -> Close(); }
    }

    void start_from_end() {
        data.reset();
        start_game();
    }

public:
    Type(std::vector<std::string>& vec, string_matrix& matrix, int frameSizeX, int frameSizeY)
        : wxFrame(NULL, wxID_ANY, "", wxDefaultPosition, wxSize { frameSizeX, frameSizeY })
        , data { vec, matrix }
        , answers { data.stringMatrix }
        , questions { data.stringVec} 
        , timer(this, TimerId::ID_type_timer) {
        // Setting some label points
        PT_static_question.x = PT_entry.x + (SZ_entry.x + SZ_enter_button.x - SZ_static_question.x)/2;
        PT_variant_question.x = PT_entry.x + (SZ_entry.x + SZ_enter_button.x - SZ_variant_question.x)/2;

        // Labels
        static_turn -> Create(this, ID_static_turn, "Turn:", PT_static_turn, SZ_static_turn);
        variant_turn -> Create(this, ID_variant_turn, wxEmptyString, PT_variant_turn, SZ_variant_turn, wxALIGN_CENTRE_HORIZONTAL);
        static_question -> Create(this, ID_static_question, "Word: ", PT_static_question, SZ_static_question, wxALIGN_CENTRE_HORIZONTAL);
        variant_question -> Create(this, ID_variant_question, wxEmptyString, PT_variant_question, SZ_variant_question, wxALIGN_CENTRE_HORIZONTAL);
        static_correct_label -> Create(this, ID_static_correct_label, wxEmptyString, PT_static_correct_label, SZ_static_correct_label, wxALIGN_CENTRE_HORIZONTAL);
        variant_correct_label -> Create(this, ID_variant_correct_label, wxEmptyString, PT_variant_correct_label, SZ_variant_correct_label, wxALIGN_CENTRE_HORIZONTAL);

        // Text Control
        entry -> Create(this, ID_entry, wxEmptyString, PT_entry, SZ_entry, wxTE_PROCESS_ENTER);

        // Button	
        enter_button -> Create(this, ID_enter_button, "Enter", PT_enter_button, SZ_enter_button);

        Bind(wxEVT_TEXT_ENTER, &Type::check_answer, this, ID_entry);
        Bind(wxEVT_BUTTON, &Type::check_answer, this, ID_enter_button);

        // Start main loop
        start_game();
    }

    void restart_cycle(wxTimerEvent& event) {
        static_correct_label -> SetLabel(wxEmptyString);
        variant_correct_label -> SetLabel(wxEmptyString);
        Bind(wxEVT_TEXT_ENTER, &Type::check_answer, this, ID_entry);
        Bind(wxEVT_BUTTON, &Type::check_answer, this, ID_enter_button);
        if (data.turns < static_cast<int>(questions.size())) {
            entry -> Clear();
            entry -> SetWindowStyle(wxTE_PROCESS_ENTER);
            start_game();
        }

        else { end_frame(); }
    }
};

wxBEGIN_EVENT_TABLE(Type, wxFrame)
    EVT_TIMER(TimerId::ID_type_timer, Type::restart_cycle)
wxEND_EVENT_TABLE()

