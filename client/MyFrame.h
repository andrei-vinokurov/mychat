#ifndef MYFRAME_H
#define MYFRAME_H

//подключение заголовочных файлов wxWidgets и приложения, используемых в этом файле (h и cpp)
#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/socket.h>
#include <wx/listctrl.h>
#include <wx/log.h>
//#include "MyPanel.h"
#include "MyDialog.h"


class MyFrame : public wxFrame
{
public:
    MyFrame();
    //MyPanel* m_myPanel; //окно класса MyPanel, где расположены основные объекты


private:
    wxSocketClient* m_sock;
    wxListCtrl* m_listCtrl;

    void OnExit(wxCommandEvent& event); //выход
    void OnAbout(wxCommandEvent& event); //о программе
    
    void OnOpenConnection(wxCommandEvent& event);
    void OnCloseConnection(wxCommandEvent& event);

#if wxUSE_IPV6
    void OnOpenConnectionIPv6(wxCommandEvent& event);
#endif

    void OpenConnection(wxSockAddress::Family family);

};

//идентификаторы некоторых элементов приложения
enum
{
    // id сокета
    SOCKET_ID,
    CLIENT_OPEN,
#if wxUSE_IPV6
    CLIENT_OPENIPV6,
#endif
    CLIENT_CLOSE
};

#endif