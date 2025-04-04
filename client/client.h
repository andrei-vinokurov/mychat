//класс клиента, содержит поля Имя, Адрес, Порт

#ifndef CLIENT_H
#define CLIENT_H

//подключение заголовочных файлов wxWidgets
#include <wx/string.h>


class client
{
public:
    client(wxString name = "", wxString address = "", wxString port = ""); //конструктор
    bool operator<(const client& r) const; //переопределение оператора < для сортировки объектов client в множестве set
    wxString GetName();
    wxString GetAddress();
    wxString GetPort();

private:
    wxString m_Name;
    wxString m_Address;
    wxString m_Port;
};

#endif