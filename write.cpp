#include <wx/wxprec.h>
#include <vector>
#include <string>
#include <fstream>
#include "GameClass.h"
#include "constants.h"
#include "canvas.h"

#ifndef WX_PRECOMP
    #include <wx/wx.h>
    #include <wx/graphics.h>
    #include <wx/dcbuffer.h>
#endif // WX_PRECOMP


class Write : public wxFrame {
    using string_matrix = std::vector<std::vector<std::string>>;
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
    GameData data;
    string_matrix& write_questions; 
    std::vector<std::string>& write_answers; 
    int pen_size;
    std::ifstream kanjiFile;
    
    // Canvas Frames
    Canvas* write_canvas = new Canvas;
    Canvas* animation_canvas = new Canvas;

    // Canvas points
    wxPoint PT_write_canvas { 50, 60 };
    wxPoint PT_animation_canvas;

    // Canvas sizes
    wxSize SZ_write_canvas { 425, 400 };
    wxSize SZ_animation_canvas;

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

    void start_game() {
        ++data.turns;
        variant_on_label -> SetLabel(wxString::FromUTF8(write_questions[data.indices[data.turns - 1]][0]));
        variant_kun_label -> SetLabel(wxString::FromUTF8(write_questions[data.indices[data.turns - 1]][1]));
        variant_meaning_label -> SetLabel(wxString::FromUTF8(write_questions[data.indices[data.turns - 1]][2]));
    }

    void check_answer(wxCommandEvent& event) {
        Unbind(wxEVT_BUTTON, [this](wxCommandEvent& event) { write_canvas -> clear(event); }, ID_clear_button);
        Unbind(wxEVT_BUTTON, [this](wxCommandEvent& event) { write_canvas -> undo(event); }, ID_undo_button);
        Unbind(wxEVT_BUTTON, &Write::check_answer, this, ID_enter_button);
        Unbind(wxEVT_BUTTON, &Write::animation, this, ID_show_ans_button);

        animation(event);
        timer.StartOnce(2000);
    }

    void end_frame() {
    	int answer = wxMessageBox("You are done ! Would you like to play again?", "Play again?", wxYES_NO, this, 0, 125);
        if (answer == wxYES) { start_from_end(); }
        else { this -> Close(); }
        
    }

    void start_from_end() {
        data.reset();
        start_game();
    }

    void animation(wxCommandEvent& event) {
        ;
    }

    void restart_cycle(wxTimerEvent& event) {
        Bind(wxEVT_BUTTON, [this](wxCommandEvent& event) { write_canvas -> clear(event); }, ID_clear_button);
        Bind(wxEVT_BUTTON, [this](wxCommandEvent& event) { write_canvas -> undo(event); }, ID_undo_button);
        Bind(wxEVT_BUTTON, &Write::check_answer, this, ID_enter_button);
        Bind(wxEVT_BUTTON, &Write::animation, this, ID_show_ans_button);

        if (data.turns < (int)write_answers.size()) {
            start_game();
        } else { end_frame(); }
    }

    void update_slider_value(wxCommandEvent& event) {
        pen_size = size_slider -> GetValue();
        write_canvas -> setPenSize(pen_size);
        variant_size_label -> SetLabel(std::to_string(pen_size));
    }

public:
    Write(const std::vector<std::string>& vec, const string_matrix& matrix, int frameSizeX, int frameSizeY, const std::string& kanjiPath)
        : wxFrame(NULL, wxID_ANY, "", wxDefaultPosition, wxSize { frameSizeX, frameSizeY })
        , timer(this, TimerId::ID_write_timer) 
        , data { vec, matrix }
        , write_questions { data.stringMatrix }
        , write_answers { data.stringVec } 
        , kanjiFile { kanjiPath } {

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
        Bind(wxEVT_BUTTON, [this](wxCommandEvent& event) { write_canvas -> clear(event); }, ID_clear_button);
        Bind(wxEVT_BUTTON, [this](wxCommandEvent& event) { write_canvas -> undo(event); }, ID_undo_button);
        Bind(wxEVT_BUTTON, &Write::check_answer, this, ID_enter_button);
        Bind(wxEVT_BUTTON, &Write::animation, this, ID_show_ans_button);
        Bind(wxEVT_SLIDER, &Write::update_slider_value, this, ID_size_slider);

        // Begin main loop
        start_game();
    }

};

wxBEGIN_EVENT_TABLE(Write, wxFrame)
    EVT_TIMER(TimerId::ID_write_timer, Write::restart_cycle)
wxEND_EVENT_TABLE()
