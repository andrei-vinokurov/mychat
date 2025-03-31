//класс основного окна клиента
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
#include "MyDialog.h"
#include "client.h"
#include "res.h"

//#if wxUSE_IPV6
//    typedef wxIPV6address IPaddress;
//#else
    typedef wxIPV4address IPaddress;
//#endif

class MyFrame : public wxFrame
{
public:
    MyFrame();
    wxSocketClient* GetSocket();
    wxString GetName();
    std::vector <MyDialog*> m_vecDial;
private:
    wxSocketClient* m_sock; //сокет
    wxString m_name; //имя клиента
    IPaddress m_addr; //адрес самого клиента
    wxListCtrl* m_listCtrl; //список клиентов, представленный в окне клиента
    std::set <client> m_clients; //отсортированный список (множество) клиентов
    wxPanel* m_Panel; //вспомогательный член данных для связи с диалогом

    void OnExit(wxCommandEvent& event); //выход
    void OnAbout(wxCommandEvent& event); //о программе
    
    void OnOpenConnection(wxCommandEvent& event); //подключение IPv4
    void OnCloseConnection(wxCommandEvent& event); //отключение от сервера

#if wxUSE_IPV6
    void OnOpenConnectionIPv6(wxCommandEvent& event); //подключение IPv6
#endif

    void OpenConnection(wxSockAddress::Family family); //подключение к серверу

    void OnSocketEvent(wxSocketEvent& event); //отслеживания событий сокета

    void UpdateList(); //обновить список клиентов

    void RecList(); //получить список клиентов

    void OpenDialog(wxListEvent& event); //открыть диалог с клиентом

    void GetMsg(); //получить сообщение

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
};

#endif