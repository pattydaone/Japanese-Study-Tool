#include <wx/wxprec.h>
#include <vector>
#include <string>
#include <fstream>
#include "wx/xrc/xmlres.h"
#include "CSVreader.h"
#include "Textreader.h"
#include "split.h"
#include "type.cpp"
#include "write.cpp"
#include "map.cpp"
#include "flashcards.cpp"

#ifndef WX_PRECOMP
    #include <wx/wx.h>
    #include <wx/graphics.h>
#endif // WX_PRECOMP

using stringMatrix = std::vector<std::vector<std::string>>;

class Preferences : public wxFrame {
    // Data
    std::ifstream                    csvFile;
    CSVRow                           row;
    std::string                      path       { "Book_" };
    std::vector<std::string>         vocabularyVec;
    stringMatrix                     vocabularyMatrix;
    const std::array<std::string, 4> kanjiTypes { "Hiragana", "Katakana", "Kanji", "Kanji Words" };
    bool                             isKanji    { false };
    const std::array<std::string, 4> gameTypes  { "Type", "Write", "Map", "Flashcards" };

    // Labels
    wxStaticText* vocabularySetLabel;
    wxStaticText* chapterLabel;
    wxStaticText* gameLabel;
    wxStaticText* lboxLabel;

    // Combo Boxes
    wxComboBox* vocabularySetCombo;
    wxComboBox* chapterCombo;
    wxComboBox* gameCombo;

    // Button
    wxButton* finishedButton;

    // Lbox
    wxListBox* vocabularyTypesLbox;

public:
    Preferences() {
        wxXmlResource::Get() -> LoadFrame(this, nullptr, "Preferences");
        // Labels
        vocabularySetLabel = XRCCTRL(*this, "vocabularySetLabel", wxStaticText);
        chapterLabel = XRCCTRL(*this, "chapterLabel", wxStaticText);
        gameLabel = XRCCTRL(*this, "gameLabel", wxStaticText);
        lboxLabel = XRCCTRL(*this, "lboxLabel", wxStaticText);

        // Combo Boxes
        vocabularySetCombo = XRCCTRL(*this, "vocabularySetCombo", wxComboBox);
        chapterCombo = XRCCTRL(*this, "chapterCombo", wxComboBox);
        gameCombo = XRCCTRL(*this, "gameCombo", wxComboBox);

        // Button
        finishedButton = XRCCTRL(*this, "finishedButton", wxButton);

        // Listbox
        vocabularyTypesLbox = XRCCTRL(*this, "vocabularyTypesLbox", wxListBox);

        Bind(wxEVT_COMBOBOX, &Preferences::setChapters, this, XRCID("vocabularySetCombo"));
        Bind(wxEVT_COMBOBOX, &Preferences::setVocabularyTypes, this, XRCID("chapterCombo"));
        Bind(wxEVT_BUTTON, &Preferences::startGame, this, XRCID("finishedButton"));

        for (auto i : gameTypes) {
            gameCombo -> Append(i);
        }

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
            isKanji = false;
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

        if (gameInput == "Flashcards") {
            Flashcards* cards = new Flashcards(vocabularyVec, vocabularyMatrix, 1000, 600, isKanji);
            cards -> Show();
        }
        
        if (isKanji) {
            if (path.find("_vocab_types.txt") != std::string::npos) path.erase(path.find("_vocab_types.txt"));
            path += "_encoded_kanji.txt";
            if (gameInput == "Write") {
                Write* write = new Write(vocabularyVec, vocabularyMatrix, 600, 400, path);
                write -> Show();
            } 

            else if (gameInput == "Map") {
                Map* map = new Map(vocabularyMatrix, vocabularyVec, 600, 400, path);
                map -> Show();
            }
            
            else { /* TODO: Raise a useful error to the user here*/ }
        }
        
        else {
            if (gameInput == "Type") {
                Type* type = new Type(vocabularyVec, vocabularyMatrix, 465, 200);
                type -> Show();
            } 
            
            else { /*TODO: Raise a useful error to the user here*/ }
        }
    }
};

class App : public wxApp {
public:
    virtual bool OnInit() {
        wxXmlResource::Get() -> InitAllHandlers();
        wxXmlResource::Get() ->Load("MyProjectBase.xrc");
        wxFrame* preferences = new Preferences;
        preferences -> Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(App);
