#include "client.h"

//конструктор
client::client(wxString name,  wxString address,  wxString port):
m_Name(name), m_Address(address), m_Port(port)
{}

//переопределение оператора < для сортировки объектов client в множестве set
bool client::operator<(const client& r) const
{
    return (
        (m_Name < r.m_Name) ||
        ((m_Name == r.m_Name) && (m_Address < r.m_Address)) ||
        ((m_Name == r.m_Name) && (m_Address == r.m_Address) && (m_Port < r.m_Port))
    );
}

wxString client::GetName()
{
    return m_Name;
}

wxString client::GetAddress()
{
    return m_Address;
}

wxString client::GetPort()
{
    return m_Port;
}
