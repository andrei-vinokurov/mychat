#ifndef MYFRAME_H
#define MYFRAME_H

//подключение заголовочных файлов wxWidgets и приложения, используемых в этом файле (h и cpp)
#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/socket.h>
#include <wx/listctrl.h>
#include <wx/log.h>
#include <set>
#include <vector>
//#include "MyPanel.h"
#include "MyDialog.h"
#include "client.h"


class MyFrame : public wxFrame
{
public:
    MyFrame();
    wxSocketClient* GetSocket();
    std::vector <MyDialog*> m_vecDial;
    //MyPanel* m_myPanel; //окно класса MyPanel, где расположены основные объекты
    
private:
    wxSocketClient* m_sock;
    wxListCtrl* m_listCtrl;
    std::set <client> m_clients;
    wxPanel* m_Panel;


    void OnExit(wxCommandEvent& event); //выход
    void OnAbout(wxCommandEvent& event); //о программе
    
    void OnOpenConnection(wxCommandEvent& event);
    void OnCloseConnection(wxCommandEvent& event);

#if wxUSE_IPV6
    void OnOpenConnectionIPv6(wxCommandEvent& event);
#endif

    void OpenConnection(wxSockAddress::Family family);

    void OnSocketEvent(wxSocketEvent& event);

    void UpdateList();

    void RecList();

    void OpenDialog(wxListEvent& event);

    void GetMsg();

    void NoAnswer();

    //void CloseDialog(wxCloseEvent& event);

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
    CLIENT_CLOSE,
    LIST_ID,
    //DIAL_ID
};

#endif