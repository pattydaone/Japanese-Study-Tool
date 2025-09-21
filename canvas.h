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
    int penSize { 1 };
    wxPen pen { *wxWHITE, penSize, wxPENSTYLE_SOLID };

    void paintEvent(wxPaintEvent &event) {
        wxAutoBufferedPaintDC dc(this);
        dc.Clear();
        for (std::size_t i = 0; i < drawn.size(); ++i) {
            pen.SetWidth(lineSizes[i]);
            dc.SetPen(pen);
            if (drawn[i].size() > 1) dc.DrawLines(drawn[i].size(), &drawn[i][0]);
        }
    }

    void addPoint(const wxPoint& point) {
        drawn.back().push_back(point);
        Refresh();
        Update();
    }

    void mouse_lc(wxMouseEvent &event) {
        // CaptureMouse();
        lineSizes.push_back(penSize);
        drawn.push_back(Line());
        addPoint(event.GetPosition());
    }

    void mouse_lc_release(wxMouseEvent &event) {
        /*
        if (HasCapture()) {
            ReleaseMouse();
        }*/
    }

    void mouse_motion(wxMouseEvent &event) {
        if (event.Dragging() && event.LeftIsDown()) {
            addPoint(event.GetPosition());
        }
    }
public:
    Canvas(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size)
        : wxPanel(parent, id, pos, size) {
        SetBackgroundStyle(wxBG_STYLE_PAINT);
        Bind(wxEVT_PAINT, &Canvas::paintEvent, this);
        Bind(wxEVT_LEFT_DOWN, &Canvas::mouse_lc, this);
        Bind(wxEVT_LEFT_UP, &Canvas::mouse_lc_release, this);
        Bind(wxEVT_MOTION, &Canvas::mouse_motion, this);
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
        penSize = size;
    }

    Lines getDrawing() {
        return drawn;
    }

    const std::vector<int>& getSizes() {
        return lineSizes;
    }
};

class AnimationCanvas : public wxPanel {
    using Line = std::vector<wxPoint>;
    using Lines = std::vector<Line>;
    Lines* toDraw { nullptr };
    wxPen pen { *wxWHITE, 1, wxPENSTYLE_SOLID };

    void paintEvent(wxPaintEvent &event) {
        wxAutoBufferedPaintDC dc(this);
        dc.Clear();
        dc.SetPen(pen);
        if (toDraw != nullptr) {
            for (std::size_t i = 0; i < toDraw -> size(); ++i) {
                dc.DrawLines((*toDraw)[i].size(), &(*toDraw)[i][0]);
            }
        }
    }

public:
    AnimationCanvas(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size)
        : wxPanel(parent, id, pos, size) {
        SetBackgroundStyle(wxBG_STYLE_PAINT);
        Bind(wxEVT_PAINT, &AnimationCanvas::paintEvent, this);
    }

    void clear() {
        toDraw = nullptr;
        Refresh();
        Update();
    }

    void setDrawing(Lines* const drawing) {
        toDraw = drawing;
        Refresh();
        Update();
    }
};

#endif
