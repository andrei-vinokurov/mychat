#ifndef MYDIALOG_H
#define MYDIALOG_H

//подключение заголовочных файлов wxWidgets и приложения, используемых в этом файле (h и cpp)
#include <wx/dialog.h>
#include <wx/msgdlg.h>
#include <wx/textdlg.h>
#include <wx/button.h>

class MyDialog : public wxDialog
{
public:
    MyDialog(wxPanel* parent);
    void SendText(wxCommandEvent& event);

private:
    wxTextCtrl* m_text1;
    wxTextCtrl* m_text2;
    wxButton* m_send;

};

#endif