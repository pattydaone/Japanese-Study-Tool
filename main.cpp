#include <wx/wxprec.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <string_view>
#include <fstream>
#include "CSVreader.h"
#include "Textreader.h"
#include "split.h"
#include "GameClass.h"
#include "Join.h"


#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif // WX_PRECOMP

using string_matrix = std::vector<std::vector<std::string>>;

enum TimerId {
    ID_timer,
};

class Type : public Game {
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
    wxString q;
    std::vector<std::string> a;
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
	
    virtual void start_game() {
        ++turns;
        variant_turn -> SetLabel(std::to_string(turns));
        variant_question -> SetLabel(questions[indices[turns-1]]);
	variant_question -> Wrap(SZ_variant_question.x/2 + 50);
    }

    virtual void check_answer(wxCommandEvent& event) {
	Unbind(wxEVT_TEXT_ENTER, &Type::check_answer, this, ID_entry);
	Unbind(wxEVT_BUTTON, &Type::check_answer, this, ID_enter_button);
	entry -> SetWindowStyle(wxTE_READONLY);
        text_entry = (entry -> GetLineText(0)).utf8_string();


        if (std::find(answers[turns - 1].begin(), answers[turns - 1].end(), text_entry) != answers[turns - 1].end()) {
            ++amnt_correct;
	    static_correct_label -> SetLabel("Correct!");
        }
	else {
	    ++amnt_incorrect;
	    static_correct_label -> SetLabel("Incorrect :(");
	    variant_correct_label -> SetLabel(wxString::FromUTF8(join(answers[indices[turns - 1]])));
	    variant_correct_label -> Wrap(SZ_variant_correct_label.x/2 + 50);
	}
	timer.StartOnce(2000);	
    }

    virtual void end_frame() {
	double percentage { (static_cast<double>(amnt_correct)/amnt_incorrect)*100 };
    	int answer = wxMessageBox("You are done!\n You got " + std::to_string(amnt_correct) + " questions right and " + std::to_string(amnt_incorrect) + " questions wrong with a percentage of " 
				  + std::to_string(percentage) + "!\n Would you like to play again?", "Play again?", wxYES_NO, this, 0, 125);
	if (answer == wxYES) { start_from_end(); }
	else { this -> Close(); }
    }

    virtual void start_from_end() {
    	turns = 0;
	amnt_correct = 0;
	amnt_incorrect = 0;
	std::shuffle(indices.begin(), indices.end(), g);
	start_game();
    }

public:
    Type(std::vector<std::string>& vec, string_matrix& matrix)
	: Game(vec, matrix)
        , timer(this, ID_timer) {
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
	if (turns < static_cast<int>(indices.size())) {
		entry -> Clear();
		entry -> SetWindowStyle(wxTE_PROCESS_ENTER);
		start_game();
	}
	else { end_frame(); }
    }
};

class Preferences : public wxFrame {
    enum {
        ID_vocabulary_set_lbl,
        ID_chapter_lbl,
        ID_game_lbl,
        ID_lbox_lbl,
        ID_vocabulary_set_combo,
        ID_chapter_combo,
        ID_game_combo,
        ID_finished_button,
        ID_vocabulary_types_lbox,
    };

    // Data
    std::ifstream csv_file;
    CSVRow row;
    std::string path { "Book_" };
    std::vector<std::string> vocabulary_vec;
    string_matrix vocabulary_matrix;

    // Labels
    wxStaticText *vocabulary_set_lbl = new wxStaticText;
    wxStaticText *chapter_lbl = new wxStaticText;
    wxStaticText *game_lbl = new wxStaticText;
    wxStaticText *lbox_lbl = new wxStaticText;

    // Label points
    wxPoint PT_vocabulary_set_lbl {0,0};
    wxPoint PT_chapter_lbl {0,50};
    wxPoint PT_game_lbl {0,100};
    wxPoint PT_lbox_lbl {255,0};

    // Label sizes
    wxSize SZ_vocabulary_set_lbl {75, 25};
    wxSize SZ_chapter_lbl {75, 25};
    wxSize SZ_game_lbl {75, 25};
    wxSize SZ_lbox_lbl {150, 25};

    // Combo Boxes
    wxComboBox *vocabulary_set_combo = new wxComboBox;
    wxComboBox *chapter_combo = new wxComboBox;
    wxComboBox *game_combo = new wxComboBox;

    // Combo box points
    wxPoint PT_vocabulary_set_combo {PT_vocabulary_set_lbl.x, PT_vocabulary_set_lbl.y + 25};
    wxPoint PT_chapter_combo { PT_chapter_lbl.x, PT_chapter_lbl.y + 25 };
    wxPoint PT_game_combo { PT_game_lbl.x, PT_game_lbl.y + 25 };

    // Combo box sizes
    wxSize SZ_vocabulary_set_combo {230, 25};
    wxSize SZ_chapter_combo {230, 25};
    wxSize SZ_game_combo {230, 25};

    // Button
    wxButton *finished_button = new wxButton;

    // Button point
    wxPoint PT_finished_button { 0, 170 };

    // Button size
    wxSize SZ_finished_button {175, 25};

    // List box
    wxListBox *vocabulary_types_lbox = new wxListBox;

    // List box point
    wxPoint PT_vocabulary_types_lbox { PT_lbox_lbl.x, PT_lbox_lbl.y + 25};

    // List box size
    wxSize SZ_vocabulary_types_lbox {200, 150};

public:
    Preferences()
        : wxFrame(NULL, wxID_ANY, "Japanese Study Tool", wxDefaultPosition, wxSize { 475, 200 }) {
        // Labels
        vocabulary_set_lbl -> Create(this, ID_vocabulary_set_lbl, "Vocabulary Set", PT_vocabulary_set_lbl, SZ_vocabulary_set_lbl);
        chapter_lbl -> Create(this, ID_chapter_lbl, "Chapter", PT_chapter_lbl, SZ_chapter_lbl);
        game_lbl -> Create(this, ID_game_lbl, "Game", PT_game_lbl, SZ_game_lbl);
        lbox_lbl -> Create(this, ID_lbox_lbl, "Select Vocabulary Word Type", PT_lbox_lbl, SZ_lbox_lbl);

        // Combo Boxes
        vocabulary_set_combo -> Create(this, ID_vocabulary_set_combo, "Please select a vocabulary set", PT_vocabulary_set_combo, SZ_vocabulary_set_combo);
        chapter_combo -> Create(this, ID_chapter_combo, "Please select a chapter", PT_chapter_combo, SZ_chapter_combo);
        game_combo -> Create(this, ID_game_combo, "Please select a game", PT_game_combo, SZ_game_combo);

        // Button
        finished_button -> Create(this, ID_finished_button, "Finished!", PT_finished_button, SZ_finished_button);

        // List box
        vocabulary_types_lbox -> Create(this, ID_vocabulary_types_lbox, PT_vocabulary_types_lbox, SZ_vocabulary_types_lbox,
                                        0, NULL, wxLB_EXTENDED);

        Bind(wxEVT_COMBOBOX, &Preferences::set_chapters, this, ID_vocabulary_set_combo);
        Bind(wxEVT_COMBOBOX, &Preferences::set_vocabulary_types, this, ID_chapter_combo);
        Bind(wxEVT_BUTTON, &Preferences::start_game, this, ID_finished_button);

        std::string vocabulary_set_str;
        std::ifstream vocabulary_set_file;
        vocabulary_set_file.open("Available_books.txt");

        while (vocabulary_set_file >> vocabulary_set_str) {
            vocabulary_set_combo -> Append(vocabulary_set_str);
        }
    }

private:
    void parse_csv(const std::string& superset, const std::string& subset, const std::string& vocab_type) {
        csv_file.open("vocabulary.csv");
        std::vector<std::string> values;
        values.reserve(4);

        while (csv_file >> row) {
            if (row[0] == superset && row[1] == subset && row[2] == vocab_type) {
                split(row[4], values);
                vocabulary_vec.emplace_back(row[3]);
                vocabulary_matrix.emplace_back(values);
                values.clear();
            }
        }
	csv_file.close();
    }

    void set_chapters(wxCommandEvent& event) {
        chapter_combo -> Clear();
        vocabulary_types_lbox -> Clear();
        std::string chapter_str;

        path.erase(5);
        path += (vocabulary_set_combo -> GetStringSelection() + "/Available_Chapters.txt");
        std::ifstream chapter_file;
        chapter_file.open(path);
        path.erase(path.find("Available"));

        while (chapter_file >> chapter_str) {
            chapter_combo -> Append(chapter_str);
        }
    }

    void set_vocabulary_types(wxCommandEvent& event) {
        if (path.find("/Chapter_") != std::string::npos) path.erase(path.find("/Chapter_"));
        path += ("/Chapter_" + chapter_combo -> GetStringSelection() + "_vocab_types.txt");

        std::string vocabulary_types_str;
        std::ifstream vocabulary_types_file;
        vocabulary_types_file.open(path);

        vocabulary_types_lbox -> Clear();
        while (vocabulary_types_file >> vocabulary_types_str) {
            vocabulary_types_lbox -> Append(vocabulary_types_str);
        }

    }

    void start_game(wxCommandEvent& event) {
	vocabulary_vec.clear();
	vocabulary_matrix.clear();
        wxArrayInt vocabulary_type_indices;
        vocabulary_types_lbox -> GetSelections(vocabulary_type_indices);
        for (auto i : vocabulary_type_indices) {
            /*
            IMPORTANT: casting from wxString to std::string *could* be destructive, I am especially concerned about what it'll do
            since I'm using a lot of non-english characters; if there are issues, look here.
            */
            parse_csv((vocabulary_set_combo -> GetStringSelection()).utf8_string(),
                           (chapter_combo -> GetStringSelection()).utf8_string(),
                           (vocabulary_types_lbox -> GetString(i)).utf8_string());
        }
	
        Type *type = new Type(vocabulary_vec, vocabulary_matrix);
        type -> Show();

    }
};

wxBEGIN_EVENT_TABLE(Type, wxFrame)
    EVT_TIMER(ID_timer, Type::restart_cycle)
wxEND_EVENT_TABLE()

class App : public wxApp {
public:
    virtual bool OnInit() {
        Preferences *mainframe = new Preferences();
        mainframe -> Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(App);
