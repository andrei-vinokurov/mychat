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

    //привязка функций к событиям сокета
    Bind(wxEVT_SOCKET, &MyFrame::OnServerEvent, this, SERVER_ID);
    Bind(wxEVT_SOCKET, &MyFrame::OnSocketEvent, this, SOCKET_ID);    

    //текстовая область
    m_text  = new wxTextCtrl(this, wxID_ANY,
                           wxT("Это серверная часть программы Чат\n"),
                           wxPoint (0, 0), wxSize (500, 200),
                           wxTE_MULTILINE | wxTE_READONLY);
    delete wxLog::SetActiveTarget(new wxLogTextCtrl(m_text));


    //создаем адрес
    IPaddress addr;
    addr.Service(3000);

    wxLogMessage(wxT("Создание сервера %s:%u"), addr.IPAddress(), addr.Service());

    //создаем сокет
    m_server = new wxSocketServer(addr);

    //используем IsOk() для проверки, прослушивает ли сокет
    if (!m_server->IsOk())
    {
      wxLogMessage(wxT("Не удалось прослушать указанный порт!"));
      return;
    }

    IPaddress addrReal;
    if (!m_server->GetLocal(addrReal))
    {
      wxLogMessage(wxT("ОШИБКА: не удалось получить адрес, к которому мы привязаны"));
    }
    else
    {
    wxLogMessage(wxT("Прослушивание сервера %s:%u"),
                 addrReal.IPAddress(), addrReal.Service());
    }

    //установки обработчика событий
    m_server->SetEventHandler(*this, SERVER_ID);
    m_server->SetNotify(wxSOCKET_CONNECTION_FLAG);
    m_server->Notify(true);

    //создаем область перечня подключенных клиентов
    m_listCtrl = new wxListCtrl (this, wxID_ANY, wxPoint (0, 200), wxSize (500, 300), wxLC_REPORT);
    m_listCtrl->AppendColumn (wxT("Имя"), wxLIST_FORMAT_CENTER, 200);
    m_listCtrl->AppendColumn ("IP", wxLIST_FORMAT_CENTER, 150);
    m_listCtrl->AppendColumn ("Port", wxLIST_FORMAT_CENTER, 150);

}


//выход
void MyFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}
 

//о программе
void MyFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox(wxT("Это серверная часть программы, которая помогает обмениваться по сети текстовыми сообщениями."),
                 wxT("О программе"), wxOK | wxICON_INFORMATION);
}


void MyFrame::OnServerEvent(wxSocketEvent& event)
{

  wxSocketBase* sock;

  switch(event.GetSocketEvent())
  {
    case wxSOCKET_CONNECTION : wxLogMessage(wxT("Соединение с клиентом")); break; //s.Append("wxSOCKET_CONNECTION\n"); break;
    default                  : wxLogMessage(wxT("Неопределенное событие")); break; //s.Append(_("Unexpected event !\n")); break;
  }

  sock = m_server->Accept(false);

  if (sock)
  {
    IPaddress addr;
    if (!sock->GetPeer(addr))
    {
      wxLogMessage(wxT("Новое подключение от неизвестного клиента принято"));
    }
    else
    {
      wxLogMessage(wxT("Новый клиент %s:%u присоединился"),
                   addr.IPAddress(), addr.Service());

      client newClient ("", addr.IPAddress(), wxString::Format(wxT("%d"), addr.Service()));
      m_clients.insert(newClient);

      m_sockets.insert(sock);

      sock->SetEventHandler(*this, SOCKET_ID);
      sock->SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
      sock->Notify(true);

      const char* c1 = newClient.GetName().utf8_str();
      unsigned char len1 = sizeof(c1) + 1;
      const char* c2 = newClient.GetAddress().utf8_str();
      unsigned char len2 = sizeof(c2) + 1;
      const char* c3 = newClient.GetPort().utf8_str();
      unsigned char len3 = sizeof(c3) + 1;
      for(auto i : m_sockets)
      {
        i->Write(&len1, 1);
        i->Write(c1, len1);
        i->Write(&len2, 1);
        i->Write(c2, len2);
        i->Write(&len3, 1);
        i->Write(c3, len3);
      }

      UpdateList();
      
      

      

/*
      m_listCtrl->InsertItem (0, "");
      m_listCtrl->SetItem (0, 0, "");  
      m_listCtrl->SetItem (0, 1, addr.IPAddress(), -1);     
      m_listCtrl->SetItem (0, 2, wxString::Format(wxT("%d"), addr.Service()), -1);
*/
    }

  }
  else
  {
    wxLogMessage(wxT("Ошибка: не удалось принять новое соединение"));
    return;
  }

  
  

}


void MyFrame::OnSocketEvent(wxSocketEvent& event)
{
  wxSocketBase* sock = event.GetSocket();
  IPaddress addr;
  sock->GetPeer(addr);

  switch(event.GetSocketEvent())
  {
    case wxSOCKET_INPUT : wxLogMessage("wxSOCKET_INPUT"); break; //s.Append("wxSOCKET_INPUT\n"); break;
    case wxSOCKET_LOST  : wxLogMessage("wxSOCKET_LOST"); break; //s.Append("wxSOCKET_LOST\n"); break;
    default             : wxLogMessage(wxT("Неизвестное событие")); break; //s.Append(_("Unexpected event !\n")); break;
  }

  switch(event.GetSocketEvent())
  {
    case wxSOCKET_INPUT:
    {

      sock->SetNotify(wxSOCKET_LOST_FLAG);
      // 
      sock->SetNotify(wxSOCKET_LOST_FLAG | wxSOCKET_INPUT_FLAG);
      break;
    }
    case wxSOCKET_LOST:
    {
      for(client c : m_clients)
      {
        if(c.GetAddress() == addr.IPAddress()) 
        {
          m_clients.erase(c);
          break;
        }
      }

      UpdateList();

      m_sockets.erase(sock);

      wxLogMessage(wxT("Клиент отключился"));
      sock->Destroy();
      break;
    }
    default: ;
  }

}

void MyFrame::UpdateList()
{
  m_listCtrl->DeleteAllItems();
  for(client c : m_clients)
  {
    m_listCtrl->InsertItem (0, "");
    m_listCtrl->SetItem (0, 0, c.GetName());  
    m_listCtrl->SetItem (0, 1, c.GetAddress(), -1);     
    m_listCtrl->SetItem (0, 2, c.GetPort(), -1);
  }
  //SendList();
}

void MyFrame::SendList()
{
  unsigned char len = sizeof(m_clients);
  const char* buf[len];
  memcpy(buf, &m_clients, len);

  for(auto c : m_sockets)
  {
     c->Write(&len, 1);
     c->Write(buf, len);
  }

}
