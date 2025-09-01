#ifndef CANVAS_H
#define CANVAS_H

#include <cstddef>
#include <vector>
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
    #include <wx/graphics.h>
    #include <wx/dcbuffer.h>
#endif // WX_PRECOMP

class Canvas : public wxPanel {
    using Line = std::vector<wxPoint>;
    using Lines = std::vector<Line>;

    Lines drawn;
    std::vector<int> lineSizes;
    int pen_size { 1 };
    wxPen pen { *wxBLACK, pen_size, wxPENSTYLE_SOLID };

    void paintEvent(wxPaintEvent &event) {
        wxAutoBufferedPaintDC dc(this);
        dc.SetBackground(*wxWHITE_BRUSH);
        dc.Clear();

        for (std::size_t i = 0; i < drawn.size(); ++i) {
            pen.SetWidth(lineSizes[i]);
            dc.SetPen(pen);
            dc.DrawLines(drawn[i].size(), &drawn[i][0]);
        }
    }

    void addPoint(const wxPoint& point) {
        drawn.back().push_back(point);
        Refresh();
        Update();
    }

    void mouse_lc(wxMouseEvent &event) {
        CaptureMouse();
        lineSizes.push_back(pen_size);
        drawn.push_back(Line());
        addPoint(event.GetPosition());
    }

    void mouse_lc_release(wxMouseEvent &event) {
        if (HasCapture()) {
            ReleaseMouse();
        }
    }

    void mouse_motion(wxMouseEvent &event) {
        if (HasCapture() && event.Dragging() && event.LeftIsDown()) {
            addPoint(event.GetPosition());
        }
    }
public:
    Canvas() {
        SetBackgroundStyle(wxBG_STYLE_PAINT);

        Bind(wxEVT_LEFT_DOWN, &Canvas::mouse_lc, this);
        Bind(wxEVT_LEFT_UP, &Canvas::mouse_lc_release, this);
        Bind(wxEVT_MOTION, &Canvas::mouse_motion, this);
        Bind(wxEVT_PAINT, &Canvas::paintEvent, this);
    }

    void undo(wxCommandEvent& event) {
        if (drawn.size() != 0) { 
            drawn.pop_back();
            lineSizes.pop_back();
            Refresh();
            Update();
        }
    }

    void clear(wxCommandEvent& event) {
        drawn.clear();
        lineSizes.clear();
        Refresh();
        Update();
    }

    void clear() {
        drawn.clear();
        lineSizes.clear();
        Refresh();
        Update();
    }

    void setPenSize(int size) {
        pen_size = size;
    }

    const Lines& getDrawing() {
        return drawn;
    }

    const std::vector<int>& getSizes() {
        return lineSizes;
    }
    
};

#endif
