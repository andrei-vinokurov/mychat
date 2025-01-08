#include "MyFrame.h"

MyFrame::MyFrame() : wxFrame(nullptr, wxID_ANY, wxT("Чат_СЕРВЕР"), wxDefaultPosition, wxSize(500, 500))
{
    wxMenu* menuFile = new wxMenu; //меню "Файл"
    menuFile->Append(wxID_EXIT, wxT("Выход"), wxT("Выйти из программы"));
    wxMenu* menuHelp = new wxMenu; //меню "Помощь"
    menuHelp->Append(wxID_ABOUT, wxT("О программе"), wxT("Краткая информация о работе программы"));
    wxMenuBar* menuBar = new wxMenuBar; //создание панели Меню
    menuBar->Append(menuFile, wxT("Файл")); //добавление пункта в панель Меню
    menuBar->Append(menuHelp, wxT("Помощь")); //добавление пункта в панель Меню
    SetMenuBar( menuBar ); //установка панели Меню в окно
    CreateStatusBar(); //информационная панель
    SetStatusText(wxT("Добро пожаловать в Чат_СЕРВЕР!")); //сообщение в информационной панели
    //привязка функций к пунктам меню
    Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);
}


void MyFrame::OnServerEvent(wxSocketEvent& event)
{
  wxString s = "OnServerEvent: ";
  wxSocketBase *sock;

  switch(event.GetSocketEvent())
  {
    case wxSOCKET_CONNECTION : s.Append("wxSOCKET_CONNECTION\n"); break;
    default                  : s.Append(_("Unexpected event !\n")); break;
  }

  //??????????????????????????????????????????????
  //m_text->AppendText(s);

  // Accept new connection if there is one in the pending
  // connections queue, else exit. We use Accept(false) for
  // non-blocking accept (although if we got here, there
  // should ALWAYS be a pending connection).

  sock = m_server->Accept(false);

  if (sock)
  {
    IPaddress addr;
    if ( !sock->GetPeer(addr) )
    {
      wxLogMessage("New connection from unknown client accepted.");
      m_listCtrl->InsertItem (0, "");
    }
    else
    {
      wxLogMessage("New client connection from %s:%u accepted",
                   addr.IPAddress(), addr.Service());

      m_listCtrl->InsertItem (0, "");
      m_listCtrl->SetItem (0, 0, addr.IPAddress(), -1);     
      m_listCtrl->SetItem (0, 1, wxString::Format(wxT("%d"), addr.Service()), -1);
    }
    /*
    m_listCtrl->SetItemData(0, m_Nclient);
    m_map.emplace(addr.IPAddress() + " " + wxString::Format(wxT("%d"), addr.Service()), m_Nclient);
    m_Nclient++;
    */
  }
  else
  {
    wxLogMessage("Error: couldn't accept a new connection");
    return;
  }

  sock->SetEventHandler(*this, SOCKET_ID);
  sock->SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
  sock->Notify(true);

}
