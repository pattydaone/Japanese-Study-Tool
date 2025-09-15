#include <wx/wxprec.h>
#include <vector>
#include <string>
#include <fstream>
#include "CSVreader.h"
#include "Textreader.h"
#include "split.h"
#include "type.cpp"
#include "write.cpp"
#include "map.cpp"

#ifndef WX_PRECOMP
    #include <wx/wx.h>
    #include <wx/graphics.h>
#endif // WX_PRECOMP

using stringMatrix = std::vector<std::vector<std::string>>;

class Preferences : public wxFrame {
    enum {
        ID_vocabularySetLabel,
        ID_chapterLabel,
        ID_gameLabel,
        ID_lboxLabel,
        ID_vocabularySetCombo,
        ID_chapterCombo,
        ID_gameCombo,
        ID_finishedButton,
        ID_vocabularyTypesLbox,
    };

    // Data
    std::ifstream                    csvFile;
    CSVRow                           row;
    std::string                      path       { "Book_" };
    std::vector<std::string>         vocabularyVec;
    stringMatrix                     vocabularyMatrix;
    const std::array<std::string, 4> kanjiTypes { "Hiragana", "Katakana", "Kanji", "Kanji Words" };
    bool                             isKanji    { false };

    // Labels
    wxStaticText *vocabularySetLabel;
    wxStaticText *chapterLabel;
    wxStaticText *gameLabel;
    wxStaticText *lboxLabel;

    // Label points
    wxPoint PT_vocabularySetLabel { 0, 0 };
    wxPoint PT_chapterLabel       { 0, 50 };
    wxPoint PT_gameLabel          { 0, 100 };
    wxPoint PT_lboxLabel          { 255, 0 };

    // Label sizes
    wxSize SZ_vocabularySetLabel { 75, 25 };
    wxSize SZ_chapterLabel       { 75, 25 };
    wxSize SZ_gameLabel          { 75, 25 };
    wxSize SZ_lboxLabel          { 150, 25 };

    // Combo Boxes
    wxComboBox *vocabularySetCombo;
    wxComboBox *chapterCombo;
    wxComboBox *gameCombo;

    // Combo box points
    wxPoint PT_vocabularySetCombo { PT_vocabularySetLabel.x, PT_vocabularySetLabel.y + 25 };
    wxPoint PT_chapterCombo       { PT_chapterLabel.x, PT_chapterLabel.y + 25 };
    wxPoint PT_gameCombo          { PT_gameLabel.x, PT_gameLabel.y + 25 };

    // Combo box sizes
    wxSize SZ_vocabularySetCombo { 230, 25 };
    wxSize SZ_chapterCombo       { 230, 25 };
    wxSize SZ_gameCombo          { 230, 25 };

    // Button
    wxButton *finishedButton;

    // Button point
    wxPoint PT_finishedButton { 0, 170 };

    // Button size
    wxSize SZ_finishedButton { 175, 25 };

    // List box
    wxListBox *vocabularyTypesLbox;

    // List box point
    wxPoint PT_vocabularyTypesLbox { PT_lboxLabel.x, PT_lboxLabel.y + 25 };

    // List box size
    wxSize SZ_vocabularyTypesLbox { 200, 150 };

public:
    Preferences()
        : wxFrame(NULL, wxID_ANY, "Japanese Study Tool", wxDefaultPosition, wxSize { 475, 200 }) {
        // Labels
        vocabularySetLabel = new wxStaticText(this, ID_vocabularySetLabel, "Vocabulary Set", PT_vocabularySetLabel, SZ_vocabularySetLabel);
        chapterLabel       = new wxStaticText(this, ID_chapterLabel, "Chapter", PT_chapterLabel, SZ_chapterLabel);
        gameLabel          = new wxStaticText(this, ID_gameLabel, "Game", PT_gameLabel, SZ_gameLabel);
        lboxLabel          = new wxStaticText(this, ID_lboxLabel, "Select Vocabulary Word Type", PT_lboxLabel, SZ_lboxLabel, 0);

        // Combo Boxes
        vocabularySetCombo = new wxComboBox(this, ID_vocabularySetCombo, "Please select a vocabulary set", PT_vocabularySetCombo, SZ_vocabularySetCombo);
        chapterCombo       = new wxComboBox(this, ID_chapterCombo, "Please select a chapter", PT_chapterCombo, SZ_chapterCombo);
        gameCombo          = new wxComboBox(this, ID_gameCombo, "Please select a game", PT_gameCombo, SZ_gameCombo, {"Type", "Write", "Map"});

        // Button
        finishedButton = new wxButton(this, ID_finishedButton, "Finished!", PT_finishedButton, SZ_finishedButton);

        // List box
        vocabularyTypesLbox = new wxListBox(this, ID_vocabularyTypesLbox, PT_vocabularyTypesLbox, SZ_vocabularyTypesLbox, 0, NULL, wxLB_EXTENDED);

        Bind(wxEVT_COMBOBOX, &Preferences::setChapters, this, ID_vocabularySetCombo);
        Bind(wxEVT_COMBOBOX, &Preferences::setVocabularyTypes, this, ID_chapterCombo);
        Bind(wxEVT_BUTTON, &Preferences::startGame, this, ID_finishedButton);

        std::string   vocabularySetString;
        std::ifstream vocabularySetFile;
        vocabularySetFile.open("Available_books.txt");

        while (vocabularySetFile >> vocabularySetString) {
            vocabularySetCombo -> Append(vocabularySetString);
        }
    }

private:
    void parseCsv(const std::string& superset, const std::string& subset, const std::string& vocabularyType) {
        if (std::find(kanjiTypes.begin(), kanjiTypes.end(), vocabularyType) != kanjiTypes.end()) {
            csvFile.open("kanjis.csv");

            while (csvFile >> row) {
                if (row[0] == superset && row[1] == subset && row[2] == vocabularyType) {
                    vocabularyVec.emplace_back(row[6]);
                    vocabularyMatrix.emplace_back(std::vector<std::string>{row[3], row[4], row[5]});
                }
            }
            isKanji = true;
        } else {
            csvFile.open("vocabulary.csv");
            std::vector<std::string> values;
            values.reserve(4);

            while (csvFile >> row) {
                if (row[0] == superset && row[1] == subset && row[2] == vocabularyType) {
                    split(row[4], values);
                    vocabularyVec.emplace_back(row[3]);
                    vocabularyMatrix.emplace_back(values);
                    values.clear();
                }
            }
        }
        csvFile.close();
    }

    void setChapters(wxCommandEvent& event) {
        chapterCombo        -> Clear();
        vocabularyTypesLbox -> Clear();
        std::string   chapterString;
        std::ifstream chapterFile;

        path.erase(5);
        path += (vocabularySetCombo -> GetStringSelection() + "/Available_Chapters.txt");
        chapterFile.open(path);
        path.erase(path.find("Available"));

        while (chapterFile >> chapterString) {
            chapterCombo -> Append(chapterString);
        }
    }

    void setVocabularyTypes(wxCommandEvent& event) {
        if (path.find("Chapter_") != std::string::npos) path.erase(path.find("Chapter_"));
        path += ("Chapter_" + chapterCombo -> GetStringSelection() + "_vocab_types.txt");

        std::string   vocabularyTypesString;
        std::ifstream vocabularyTypesFile;
        vocabularyTypesFile.open(path);

        vocabularyTypesLbox -> Clear();
        while (vocabularyTypesFile >> vocabularyTypesString) {
            vocabularyTypesLbox -> Append(vocabularyTypesString);
        }
    }

    void startGame(wxCommandEvent& event) {
        vocabularyVec.clear();
        vocabularyMatrix.clear();
        wxArrayInt vocabularyTypesIndices;
        vocabularyTypesLbox -> GetSelections(vocabularyTypesIndices);
        for (auto i : vocabularyTypesIndices) {
            // TODO: Raise a useful error to the user here.
            if (std::find(kanjiTypes.begin(), kanjiTypes.end(), (vocabularyTypesLbox -> GetString(i)).utf8_string()) == kanjiTypes.end() && isKanji) { break; }
            /*
            IMPORTANT: casting from wxString to std::string *could* be destructive, I am especially concerned about what it'll do
            since I'm using a lot of non-english characters; if there are issues, look here.
            */
            parseCsv((vocabularySetCombo  -> GetStringSelection()).utf8_string(),
                     (chapterCombo        -> GetStringSelection()).utf8_string(),
                     (vocabularyTypesLbox -> GetString(i)).utf8_string());
        }

    	wxString gameInput { gameCombo -> GetStringSelection() };
        
        if (isKanji) {
            if (path.find("_vocab_types.txt") != std::string::npos) path.erase(path.find("_vocab_types.txt"));
            path += "_encoded_kanji.txt";
            if (gameInput == "Write") {
                Write* write = new Write(vocabularyVec, vocabularyMatrix, 600, 400, path);
                write -> Show();
            } 

            else if (gameInput == "Map") {
                Map* map = new Map(vocabularyVec, 600, 400, path);
                map -> Show();
            }
            
            else { /* TODO: Raise a useful error to the user here*/ }
        } else {
            if (gameInput == "Type") {
                Type* type = new Type(vocabularyVec, vocabularyMatrix, 465, 200);
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
