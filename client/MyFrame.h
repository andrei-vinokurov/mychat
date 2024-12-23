#ifndef MYFRAME_H
#define MYFRAME_H

//подключение заголовочных файлов wxWidgets и приложения, используемых в этом файле (h и cpp)
#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/listctrl.h>
//#include "MyPanel.h"
#include "MyDialog.h"


class MyFrame : public wxFrame
{
public:
    MyFrame();
    //MyPanel* m_myPanel; //окно класса MyPanel, где расположены основные объекты


private:
    wxListCtrl* m_listCtrl;

    void OnExit(wxCommandEvent& event); //выход
    void OnAbout(wxCommandEvent& event); //о программе

};

#endif