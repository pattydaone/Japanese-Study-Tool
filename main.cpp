#include <wx/wxprec.h>
#include <vector>
#include <string>
#include <fstream>
#include "CSVreader.h"
#include "Textreader.h"
#include "split.h"
#include "GameClass.h"
#include "Join.h"


#ifndef WX_PRECOMP
    #include <wx/wx.h>
    #include <wx/graphics.h>
#endif // WX_PRECOMP

using string_matrix = std::vector<std::vector<std::string>>;

enum TimerId {
    ID_type_timer,
    ID_write_timer,
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


        if (std::find(answers[indices[turns - 1]].begin(), answers[indices[turns - 1]].end(), text_entry) != answers[indices[turns - 1]].end()) {
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
        double percentage { ((double)amnt_correct/(double)amnt_incorrect)*100 };
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
        : Game(vec, matrix, 465, 200)
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
        if (turns < static_cast<int>(indices.size())) {
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

class Write : public Game {
    enum {
        ID_write_canvas,
        ID_animation_canvas,
        ID_static_on_label,
        ID_static_kun_label,
        ID_static_meaning_label,
        ID_variant_on_label,
        ID_variant_kun_label,
        ID_variant_meaning_label,
        ID_static_size_label,
        ID_variant_size_label,
        ID_enter_button,
        ID_show_ans_button,
        ID_undo_button,
        ID_clear_button,
        ID_size_slider,
    };

    // Data
    wxTimer timer;
    wxDECLARE_EVENT_TABLE();
    // Reordering these because data is stored opposite with respect to written vocabulary
    // This shit sucks but hopefully its ok since im using references.... should I even use a template class in the first place?
    string_matrix& write_questions { answers }; 
    std::vector<std::string>& write_answers { questions }; 
    
    // Each element in "drawn" follows a template: lastx, lasty, nextx, nexty, tag
    std::vector<std::array<int, 5>> drawn { std::array<int, 5> { 0, 0, 0, 0, -1 } };
    std::vector<int> to_undo;
    int tag {}, lastx {}, lasty {}, nextx {}, nexty {}, pen_size { 1 };

    // Canvas Frames
    wxPanel* write_canvas = new wxPanel;
    wxPanel* animation_canvas = new wxPanel;

    // Canvas points
    wxPoint PT_write_canvas { 50, 60 };
    wxPoint PT_animation_canvas;

    // Canvas sizes
    wxSize SZ_write_canvas { 425, 400 };
    wxSize SZ_animation_canvas;

    // Pen
    wxPen pen {};

    // Labels
    wxStaticText* static_on_label = new wxStaticText;
    wxStaticText* variant_on_label = new wxStaticText;
    wxStaticText* static_kun_label = new wxStaticText;
    wxStaticText* variant_kun_label = new wxStaticText;
    wxStaticText* static_meaning_label = new wxStaticText;
    wxStaticText* variant_meaning_label = new wxStaticText;
    wxStaticText* static_size_label = new wxStaticText;
    wxStaticText* variant_size_label = new wxStaticText;

    // Label points
    wxPoint PT_static_on_label { 100, 0 };
    wxPoint PT_variant_on_label { PT_static_on_label.x, PT_static_on_label.y + 25 };
    wxPoint PT_static_kun_label { 250, 0 };
    wxPoint PT_variant_kun_label { PT_static_kun_label.x, PT_static_kun_label.y + 25 };
    wxPoint PT_static_meaning_label { 400, 0 };
    wxPoint PT_variant_meaning_label { PT_static_meaning_label.x, PT_static_meaning_label.y + 25 };
    wxPoint PT_static_size_label { 15, 0 };
    wxPoint PT_variant_size_label { PT_static_size_label.x, PT_static_size_label.y + 15 };

    // Label sizes
    wxSize SZ_static_on_label { 100, 25 };
    wxSize SZ_variant_on_label { 100, 25 };
    wxSize SZ_static_kun_label { 100, 25 };
    wxSize SZ_variant_kun_label { 100, 25 };
    wxSize SZ_static_meaning_label { 100, 25 };
    wxSize SZ_variant_meaning_label { 100, 25 };
    wxSize SZ_static_size_label { 100, 25 };
    wxSize SZ_variant_size_label { 100, 25 };

    // Buttons
    wxButton* enter_button = new wxButton;
    wxButton* show_ans_button = new wxButton;
    wxButton* undo_button = new wxButton;
    wxButton* clear_button = new wxButton;

    // Button points
    wxPoint PT_enter_button { 500, 60 };
    wxPoint PT_show_ans_button { PT_enter_button.x, PT_enter_button.y + 60 };
    wxPoint PT_undo_button { PT_show_ans_button.x, PT_show_ans_button.y + 90 };
    wxPoint PT_clear_button {PT_undo_button.x, PT_undo_button.y + 60 };

    // Button sizes
    wxSize SZ_enter_button { 50, 25 };
    wxSize SZ_show_ans_button { 100, 25 };
    wxSize SZ_undo_button { 50, 25 };
    wxSize SZ_clear_button { 50, 25 };

    // Slider
    wxSlider* size_slider = new wxSlider;

    // Slider point
    wxPoint PT_size_slider { 0, 30 };

    // Slider size
    wxSize SZ_size_slider { 50, 400 };

    virtual void start_game() {
        ++turns;
        variant_on_label -> SetLabel(wxString::FromUTF8(write_questions[indices[turns - 1]][0]));
        variant_kun_label -> SetLabel(wxString::FromUTF8(write_questions[indices[turns - 1]][1]));
        variant_meaning_label -> SetLabel(wxString::FromUTF8(write_questions[indices[turns - 1]][2]));
    }

    virtual void check_answer(wxCommandEvent& event) {
        Unbind(wxEVT_BUTTON, &Write::clear, this, ID_clear_button);
        Unbind(wxEVT_BUTTON, &Write::check_answer, this, ID_enter_button);
        Unbind(wxEVT_BUTTON, &Write::undo, this, ID_undo_button);
        Unbind(wxEVT_BUTTON, &Write::animation, this, ID_show_ans_button);

        animation(event);
        timer.StartOnce(2000);
    }

    virtual void end_frame() {
    	int answer = wxMessageBox("You are done ! Would you like to play again?", "Play again?", wxYES_NO, this, 0, 125);
        if (answer == wxYES) { start_from_end(); }
        else { this -> Close(); }
        
    }

    virtual void start_from_end() {
        std::shuffle(indices.begin(), indices.end(), g);
        turns = 0;
        start_game();
    }

    void animation(wxCommandEvent& event) {
        ;
    }

    void restart_cycle(wxTimerEvent& event) {
        Bind(wxEVT_BUTTON, &Write::clear, this, ID_clear_button);
        Bind(wxEVT_BUTTON, &Write::check_answer, this, ID_enter_button);
        Bind(wxEVT_BUTTON, &Write::undo, this, ID_undo_button);
        Bind(wxEVT_BUTTON, &Write::animation, this, ID_show_ans_button);

        if (turns < (int)write_answers.size()) {
            start_game();
        } else { end_frame(); }
    }

    void clear(wxCommandEvent& event) {
        drawn.clear();
        write_canvas -> Refresh();
    }

    void undo(wxCommandEvent& event) {
        if (!to_undo.empty()) { 
            int undo_tag = to_undo.back();
            to_undo.pop_back();
        
            std::array<int, 5>* element = &(drawn.back());
            while ((*element)[4] == undo_tag) {
                    drawn.pop_back();
                    element = &(drawn.back());
            }
            write_canvas -> Refresh();
        }
    }
    
    void paintEvent(wxPaintEvent &event) {
        wxPaintDC dc(write_canvas);
        //dc.SetPen(pen);

        for (auto i : drawn) {
            dc.DrawLine(i[0], i[1], i[2], i[3]);
        }
    }

    void mouse_lc(wxMouseEvent &event) {
        lastx = event.GetX();
        lasty = event.GetY();
    }

    void mouse_lc_release(wxMouseEvent &event) {
        to_undo.push_back(tag);
        ++tag;
    }

    void mouse_motion(wxMouseEvent &event) {
        if (event.Dragging()) {
            nextx = event.GetX();
            nexty = event.GetY();
            drawn.push_back(std::array<int, 5>{lastx, lasty, nextx, nexty, tag});
            lastx = nextx;
            lasty = nexty;
            write_canvas -> Refresh();
        }
    }

    void update_slider_value(wxCommandEvent& event) {
        pen_size = size_slider -> GetValue();
        variant_size_label -> SetLabel(std::to_string(pen_size));
        pen.SetWidth(pen_size);
    }
    
public:
    Write(std::vector<std::string> vec, string_matrix matrix)
        : Game(vec, matrix, 600, 400)
        , timer(this, TimerId::ID_write_timer) {
 
        // Labels
        static_size_label -> Create(this, ID_static_size_label, "Size", PT_static_size_label, SZ_static_size_label);
        variant_size_label -> Create(this, ID_variant_size_label, "2", PT_variant_size_label, SZ_variant_size_label);
        static_on_label -> Create(this, ID_static_on_label, "On reading", PT_static_on_label, SZ_static_on_label);
        variant_on_label -> Create(this, ID_variant_on_label, wxEmptyString, PT_variant_on_label, SZ_variant_on_label);
        static_kun_label -> Create(this, ID_variant_kun_label, "Kun reading", PT_static_kun_label, SZ_static_kun_label);
        variant_kun_label -> Create(this, ID_variant_kun_label, wxEmptyString, PT_variant_kun_label, SZ_variant_kun_label);
        static_meaning_label -> Create(this, ID_static_meaning_label, "Meaning", PT_static_meaning_label, SZ_variant_meaning_label);
        variant_meaning_label -> Create(this, ID_variant_meaning_label, wxEmptyString, PT_variant_meaning_label, SZ_variant_meaning_label);

        // Buttons
        enter_button -> Create(this, ID_enter_button, "Enter", PT_enter_button, SZ_enter_button);
        show_ans_button -> Create(this, ID_show_ans_button, "Show answer", PT_show_ans_button, SZ_show_ans_button);
        undo_button -> Create(this, ID_undo_button, "Undo", PT_undo_button, SZ_undo_button);
        clear_button -> Create(this, ID_clear_button, "Clear", PT_clear_button, SZ_clear_button);

        // Slider
        size_slider -> Create(this, ID_size_slider, 1, 1, 10, PT_size_slider, SZ_size_slider, wxSL_VERTICAL);

        // Canvas
        write_canvas -> Create(this, ID_write_canvas, PT_write_canvas, SZ_write_canvas);
        // animation_canvas -> Create(this, ID_animation_canvas, wxEmptyString, PT_animation_canvas, SZ_animation_canvas);
        
        // Event binds
        write_canvas -> Bind(wxEVT_LEFT_DOWN, &Write::mouse_lc, this);
        write_canvas -> Bind(wxEVT_LEFT_UP, &Write::mouse_lc_release, this);
        write_canvas -> Bind(wxEVT_MOTION, &Write::mouse_motion, this);
        write_canvas -> Bind(wxEVT_PAINT, &Write::paintEvent, this);
        Bind(wxEVT_BUTTON, &Write::clear, this, ID_clear_button);
        Bind(wxEVT_BUTTON, &Write::check_answer, this, ID_enter_button);
        Bind(wxEVT_BUTTON, &Write::undo, this, ID_undo_button);
        Bind(wxEVT_BUTTON, &Write::animation, this, ID_show_ans_button);
        Bind(wxEVT_SLIDER, &Write::update_slider_value, this, ID_size_slider);

        // Begin main loop
        start_game();
    }

};

wxBEGIN_EVENT_TABLE(Write, wxFrame)
    EVT_TIMER(TimerId::ID_write_timer, Write::restart_cycle)
wxEND_EVENT_TABLE()
    

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
    const std::array<std::string, 4> kanji_types { "Hiragana", "Katakana", "Kanji", "Kanji Words" };
    bool is_kanji { false };

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
        lbox_lbl -> Create(this, ID_lbox_lbl, "Select Vocabulary Word Type", PT_lbox_lbl, SZ_lbox_lbl, 0);

        // Combo Boxes
        vocabulary_set_combo -> Create(this, ID_vocabulary_set_combo, "Please select a vocabulary set", PT_vocabulary_set_combo, SZ_vocabulary_set_combo);
        chapter_combo -> Create(this, ID_chapter_combo, "Please select a chapter", PT_chapter_combo, SZ_chapter_combo);
        game_combo -> Create(this, ID_game_combo, "Please select a game", PT_game_combo, SZ_game_combo, {"Type", "Write"});

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
        if (std::find(kanji_types.begin(), kanji_types.end(), vocab_type) != kanji_types.end()) {
            csv_file.open("kanjis.csv");

            while (csv_file >> row) {
                if (row[0] == superset && row[1] == subset && row[2] == vocab_type) {
                    vocabulary_vec.emplace_back(row[6]);
                    vocabulary_matrix.emplace_back(std::vector<std::string>{row[3], row[4], row[5]});
                }
            }
            is_kanji = true;
        } else {
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
            // TODO: Raise a useful error to the user here.
            if (std::find(kanji_types.begin(), kanji_types.end(), (vocabulary_types_lbox -> GetString(i)).utf8_string()) == kanji_types.end() && is_kanji) { break; }
            /*
            IMPORTANT: casting from wxString to std::string *could* be destructive, I am especially concerned about what it'll do
            since I'm using a lot of non-english characters; if there are issues, look here.
            */
            parse_csv((vocabulary_set_combo -> GetStringSelection()).utf8_string(),
                      (chapter_combo -> GetStringSelection()).utf8_string(),
                      (vocabulary_types_lbox -> GetString(i)).utf8_string());
        }

    	wxString game_inp { game_combo -> GetStringSelection() };
        
        if (is_kanji) {
            if (game_inp == "Write") {
                Write *write = new Write(vocabulary_vec, vocabulary_matrix);
                write -> Show();
            } else { /* TODO: Raise a useful error to the user here*/ }
        } else {
            if (game_inp == "Type") {
                Type *type = new Type(vocabulary_vec, vocabulary_matrix);
                type -> Show();
            } else { /*TODO: Raise a useful error to the user here*/ }
        }
    }
};



class App : public wxApp {
public:
    virtual bool OnInit() {
        Preferences *mainframe = new Preferences();
        mainframe -> Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(App);
