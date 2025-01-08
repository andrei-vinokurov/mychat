#ifndef MYFRAME_H
#define MYFRAME_H

//подключение заголовочных файлов wxWidgets и приложения, используемых в этом файле (h и cpp)
#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/socket.h>
#include <wx/listctrl.h>
#include <wx/log.h>


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
    wxListCtrl* m_listCtrl;

    void OnExit(wxCommandEvent& event); //выход
    void OnAbout(wxCommandEvent& event); //о программе

    void OnServerEvent(wxSocketEvent& event);
    void OnSocketEvent(wxSocketEvent& event);

};

//идентификаторы некоторых элементов приложения
enum
{
  SERVER_ID,
  SOCKET_ID
};

#endif