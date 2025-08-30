#include <wx/wxprec.h>
#include <vector>
#include <string>
#include "GameClass.h"
#include "constants.h"

#ifndef WX_PRECOMP
    #include <wx/wx.h>
    #include <wx/graphics.h>
#endif // WX_PRECOMP

using string_matrix = std::vector<std::vector<std::string>>;

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
    
    // Each element in "drawn" follows a template: lastx, lasty, nextx, nexty, tag, size
    std::vector<std::array<int, 6>> drawn { std::array<int, 6> { 0, 0, 0, 0, -1, 1 } };
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
    wxPen pen { *wxBLACK, pen_size, wxPENSTYLE_SOLID };

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
        if (to_undo.size() != 0) { 
            int undo_tag = to_undo.back();
            to_undo.pop_back();
        
            std::array<int, 6>* element = &(drawn.back());
            while ((*element)[4] == undo_tag) {
                    drawn.pop_back();
                    element = &(drawn.back());
            }
            write_canvas -> Refresh();
        }
    }
    
    void paintEvent(wxPaintEvent &event) {
        wxPaintDC dc(write_canvas);

        for (auto i : drawn) {
            pen.SetWidth(i[5]);
            dc.SetPen(pen);
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
            drawn.push_back(std::array<int, 6>{lastx, lasty, nextx, nexty, tag, size_slider -> GetValue()});
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
    
