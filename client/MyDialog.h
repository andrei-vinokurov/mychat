#ifndef MYDIALOG_H
#define MYDIALOG_H

//подключение заголовочных файлов wxWidgets и приложения, используемых в этом файле (h и cpp)
#include <wx/dialog.h>
#include <wx/msgdlg.h>
#include <wx/textdlg.h>
#include <wx/button.h>
#include <wx/datetime.h>

class MyDialog : public wxDialog
{
public:
    MyDialog(wxPanel* parent, wxString name, wxString addr, wxString port);
    ~MyDialog();
    void SendText(wxCommandEvent& event);
    wxString GetName();
    wxString GetAddr();
    wxString GetPort();
    wxTextCtrl* m_text1;
    wxTextCtrl* m_text2;

private:
    wxButton* m_send;
    wxPanel* m_parent;
    wxString m_name;
    wxString m_addr;
    wxString m_port;

};

#endif