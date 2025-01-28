#include "client.h"

client::client(wxString name,  wxString address,  wxString port, wxSocketBase* sock):
m_Name(name), m_Address(address), m_Port(port), m_Sock(sock)
{}

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

wxSocketBase* client::GetSock()
{
    return m_Sock;
}