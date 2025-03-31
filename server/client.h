//класс клиента, содержит поля Имя, Адрес, Порт, Сокет

#ifndef CLIENT_H
#define CLIENT_H

//подключение заголовочных файлов wxWidgets
#include <wx/string.h>
#include <wx/socket.h>


class client
{
public:
    client(wxString name = "", wxString address = "", wxString port = "", wxSocketBase* sock = nullptr);
    bool operator<(const client& r) const;
    wxString GetName();
    wxString GetAddress();
    wxString GetPort();
    wxSocketBase* GetSock();

private:
    wxString m_Name;
    wxString m_Address;
    wxString m_Port;
    wxSocketBase* m_Sock;
};

#endif