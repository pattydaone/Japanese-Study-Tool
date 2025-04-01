#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif // WX_PRECOMP

class TestApp : public wxApp {
public:
    virtual bool OnInit();
};

class TestFrame : public wxFrame {
public:
    TestFrame();

private:
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnButton(wxCommandEvent& event);
};

enum {
    ID_hello = 1,
    ID_testbutton = 2,
    ID_testcombo = 3
};

bool TestApp::OnInit() {
    TestFrame *mainframe = new TestFrame();
    mainframe -> Show(true);
    return true;
}

TestFrame::TestFrame()
        : wxFrame(NULL, wxID_ANY, "Hello, World!") {
        wxMenu *menuFile = new wxMenu;
        menuFile -> Append(ID_hello, "&Hello...\tCtrl-H", "Help string shown in status bar for this menu item.");
        menuFile -> AppendSeparator();
        menuFile -> Append(wxID_ABOUT);

        wxMenu *menuHelp = new wxMenu;
        menuHelp -> Append(wxID_ABOUT);

        wxMenuBar *menuBar = new wxMenuBar;
        menuBar -> Append(menuFile, "&File");
        menuBar -> Append(menuHelp, "&Help");

        SetMenuBar(menuBar);

        wxButton *Button = new wxButton;
        wxPoint ButtonPoint {1, 51};
        wxSize ButtonSize {100, 30};
        Button -> Create(this, ID_testbutton, "BUTTON", ButtonPoint, ButtonSize);

        wxComboBox *TestBox = new wxComboBox;
        wxPoint ComboPoint {1,1};
        wxSize ComboSize {250, 50};
        TestBox -> Create(this, ID_testcombo, "COMBO BOX", ComboPoint, ComboSize);

        CreateStatusBar();
        SetStatusText("OMG hiiiiiiiiiiiiiiiiii");

        Bind(wxEVT_MENU, &TestFrame::OnHello, this, ID_hello);
        Bind(wxEVT_MENU, &TestFrame::OnAbout, this, wxID_ABOUT);
        Bind(wxEVT_MENU, &TestFrame::OnExit, this, wxID_EXIT);
        Bind(wxEVT_BUTTON, &TestFrame::OnButton, this, ID_testbutton);
}

void TestFrame::OnExit(wxCommandEvent& event) {
    Close(true);
}

void TestFrame::OnAbout(wxCommandEvent& event) {
    wxMessageBox("This is a wxWidgets Hello World example", "About Hello World", wxOK | wxICON_INFORMATION);
}

void TestFrame::OnHello(wxCommandEvent& event) {
    wxLogMessage("Hello world from wxWidgets!");
}

void TestFrame::OnButton(wxCommandEvent& event) {
    wxMessageBox("HIIII HI HIIIIIIIIIIII", "Teehee :3");
}

wxIMPLEMENT_APP(TestApp);
