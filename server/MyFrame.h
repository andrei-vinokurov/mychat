#ifndef MYFRAME_H
#define MYFRAME_H

//подключение заголовочных файлов wxWidgets и приложения, используемых в этом файле (h и cpp)
#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/socket.h>
#include <wx/listctrl.h>
#include <wx/log.h>
#include <wx/msgdlg.h>
#include <wx/utils.h> 
#include <set>
#include <map>
#include "client.h"

// В настоящее время этот пример написан для использования сокетов только IP или только IPv6
// В будущем его следует расширить, чтобы разрешить использование любого из них
#if wxUSE_IPV6
    typedef wxIPV6address IPaddress;
#else
    typedef wxIPV4address IPaddress;
#endif


class MyFrame : public wxFrame
{
public:
    MyFrame();

private:
    wxSocketServer* m_server;
    wxTextCtrl* m_text;
    wxListCtrl* m_listCtrl;
    std::set <client> m_clients;
    std::map <wxString, client> m_mapClients;
    //std::set <wxSocketBase*> m_sockets;


    void OnExit(wxCommandEvent& event); //выход
    void OnAbout(wxCommandEvent& event); //о программе

    void OnServerEvent(wxSocketEvent& event);
    void OnSocketEvent(wxSocketEvent& event);

    void UpdateList();

    void SendList();

};

//идентификаторы некоторых элементов приложения
enum
{
  SERVER_ID,
  SOCKET_ID
};

#endif