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

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif // WX_PRECOMP

using string_matrix = std::vector<std::vector<std::string_view>>;

enum {
    ID_vocabulary_set_lbl = 1,
    ID_chapter_lbl = 2,
    ID_game_lbl = 3,
    ID_lbox_lbl = 4,
    ID_vocabulary_set_combo = 5,
    ID_chapter_combo = 6,
    ID_game_combo = 7,
    ID_finished_button = 8,
    ID_vocabulary_types_lbox = 9,
};

class Preferences : public wxFrame {
    // Data
    std::ifstream csv_file;
    CSVRow row;
    std::string path { "Book_" };
    std::vector<std::string_view> vocabulary_vec;
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
        std::ifstream* vocabulary_set_file = new std::ifstream;
        vocabulary_set_file -> open("Available_books.txt");

        while (*vocabulary_set_file >> vocabulary_set_str) {
            vocabulary_set_combo -> Append(vocabulary_set_str);
        }
    }

private:
    void parse_csv(const std::string_view& superset, const std::string_view& subset, const std::string_view& vocab_type) {
        csv_file.open("vocabulary.csv");
        std::vector<std::string_view> values;
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

    void set_chapters(wxCommandEvent& event) {
        chapter_combo -> Clear();
        vocabulary_types_lbox -> Clear();
        std::string chapter_str;

        path.erase(5);
        path += (vocabulary_set_combo -> GetStringSelection() + "/Available_Chapters.txt");
        std::ifstream* chapter_file = new std::ifstream;
        chapter_file -> open(path);
        path.erase(path.find("Available"));

        while (*chapter_file >> chapter_str) {
            chapter_combo -> Append(chapter_str);
        }
    }

    void set_vocabulary_types(wxCommandEvent& event) {
        if (path.find("/Chapter_") != std::string::npos) path.erase(path.find("/Chapter_"));
        path += ("/Chapter_" + chapter_combo -> GetStringSelection() + "_vocab_types.txt");

        std::string vocabulary_types_str;
        std::ifstream* vocabulary_types_file = new std::ifstream;
        vocabulary_types_file -> open(path);

        vocabulary_types_lbox -> Clear();
        while (*vocabulary_types_file >> vocabulary_types_str) {
            vocabulary_types_lbox -> Append(vocabulary_types_str);
        }

    }

    void start_game(wxCommandEvent& event) {
        wxArrayInt vocabulary_type_indices;
        vocabulary_types_lbox -> GetSelections(vocabulary_type_indices);
        for (auto i : vocabulary_type_indices) {
            /*
            IMPORTANT: casting from wxString to std::string *could* be destructive, I am especially concerned about what it'll do
            since I'm using a lot of non-english characters; if there are issues, look here.
            */
            parse_csv((vocabulary_set_combo -> GetStringSelection()).ToStdString(),
                           (chapter_combo -> GetStringSelection()).ToStdString(),
                           (vocabulary_types_lbox -> GetString(i)).ToStdString());
        }


    }
};

class Type : public Game {
// Labels
wxStaticText* static_turn = new wxStaticText;
wxStaticText* variant_turn = new wxStaticText;
wxStaticText* static_question = new wxStaticText;
wxStaticText* variant_question = new wxStaticText;

// Label points
wxPoint PT_static_turn;
wxPoint PT_variant_turn;
wxPoint PT_static_question;
wxPoint PT_variant_question;

// Label sizes
wxSize SZ_static_turn;
wxSize SZ_variant_turn;
wxSize SZ_static_question;
wxSize SZ_variant_question;

// Text control
wxTextCtrl* entry = new wxTextCtrl;

// Control point
wxPoint PT_entry;

// Control size
wxSize SZ_entry;

public:
    Type(std::vector<std::string_view>& vec, string_matrix& matrix)
        : Game(vec, matrix) {

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
