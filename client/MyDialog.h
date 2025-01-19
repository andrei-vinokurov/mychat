#ifndef MYDIALOG_H
#define MYDIALOG_H

//подключение заголовочных файлов wxWidgets и приложения, используемых в этом файле (h и cpp)
#include <wx/dialog.h>
#include <wx/msgdlg.h>
#include <wx/textdlg.h>

class MyDialog : public wxDialog
{
public:
    MyDialog();
    ~MyDialog();

private:
    wxTextCtrl* m_text;

};

#endif