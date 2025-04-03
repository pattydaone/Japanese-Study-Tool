#include <wx/wxprec.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <fstream>
#include "CSVreader.h"
#include "Textreader.h"

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif // WX_PRECOMP

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
    std::unordered_map<std::string, std::vector<std::string>> vocabulary;

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
        vocabulary_types_lbox -> Create(this, ID_vocabulary_types_lbox, PT_vocabulary_types_lbox, SZ_vocabulary_types_lbox);

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

    void create_widgets() {
    }

private:
    void create_hashmap() {

    }

    void set_chapters(wxCommandEvent& event) {
        std::string path {"Book_/Available_Chapters.txt"};
        path.insert(5, vocabulary_set_combo -> GetStringSelection());
        std::string chapter_str;
        std::ifstream* chapter_file = new std::ifstream;
        chapter_file -> open(path);

        while (*chapter_file >> chapter_str) {
            chapter_combo -> Append(chapter_str);
        }
    }

    void set_vocabulary_types(wxCommandEvent& event) {
        wxMessageBox("PLACEHOLDER", "Set vocabulary types.");
    }

    void start_game(wxCommandEvent& event) {
        wxMessageBox("PLACEHOLDER", "Start game.");
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
