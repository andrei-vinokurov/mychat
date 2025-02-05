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

  if (sock /*&& m_sockets.find(sock) == m_sockets.end()*/)
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

      sock->SetEventHandler(*this, SOCKET_ID);
      sock->SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
      sock->Notify(true);

      client newClient (wxString::FromUTF8("noname"), addr.IPAddress(), wxString::Format(wxT("%d"), addr.Service()), sock);
      m_clients.insert(newClient);
      m_mapClients.emplace(wxString::Format(wxT("%d"), addr.Service()), newClient);

      //m_sockets.insert(sock);
      
      UpdateList();
      
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
      unsigned char len1;
      sock->Read(&len1, 1);
      char c1[len1];
      sock->Read(c1, len1);
      wxString wS1(c1);

      unsigned char len2;
      sock->Read(&len2, 1);
      char c2[len2];
      sock->Read(c2, len2);
      wxString wS2(c2);
      
//      unsigned char len3;
//      sock->Read(&len3, 1);
      wxString wS3;
      /*if(len3 < 128){
        char c3[len3];
        sock->Read(c3, len3);
        wxString wS4(c3);
        wS3 = wS4;
      }
      else
      {*/
/*      unsigned int len30 = len3 * 1024;
wxLogMessage(wxT("Большой объем %d"), len30);

        wxCharBuffer buf(len3 * 1024);
        sock->Read(buf.data(), len3 * 1024);
        wxString wS4(buf.data());
        wS3 = wS4;
*/      //}
      unsigned char a;
      sock->Read(&a, 1);
      unsigned int len3;
      
      switch(a)
      {
        case 0xEE:
        {
        char buf[4096];
        wxUint32 len4 = sock->ReadMsg(buf, sizeof(buf)).LastCount();
        wxString wS4(buf);
        wS3 = wS4;
        len3 = len4;
        break;
        }
    
        case 0xFE:
        {
        unsigned char len5;
        sock->Read(&len5, 1);
        char c3[len5];
        sock->Read(c3, len5);
        wxString wS5(c3);
        wS3 = wS5;
        len3 = len5;
        break;
        }

        default: break;
      }
      
      wxLogMessage("|| %s | %s | %s", wS1, wS2, wS3);

//      client* cl = nullptr;
//      for(client i : m_clients)
//      {
//        if(/*i.GetAddress() == wS1 &&*/ i.GetPort() == wS2/* && i.GetSock()->IsOk()*/)
//        {
//          cl = &i;
//          break;
//       }
//      }

      client* cl = &m_mapClients.find(wS2)->second;

      if(cl)
      {
          cl->GetSock()->WaitForWrite(-1, 500);
          unsigned char c = 0xCE;
          cl->GetSock()->Write(&c, 1);

          //const char* c1 = i.GetAddress().utf8_str();
          const char* c1 = addr.IPAddress();
          unsigned char len1 = (unsigned char)(wxStrlen(c1) + 1);
          cl->GetSock()->Write(&len1, 1);
          cl->GetSock()->Write(c1, len1);
          
          //const char* c2 = i.GetPort().utf8_str();
          const char* c2 = wxString::Format(wxT("%d"), addr.Service());
          unsigned char len2 = (unsigned char)(wxStrlen(c2) + 1);
          cl->GetSock()->Write(&len2, 1);
          cl->GetSock()->Write(c2, len2);

          if(len3 > 255)
          {
            unsigned char a = 0xEE;
            cl->GetSock()->Write(&a, 1);
            cl->GetSock()->WriteMsg(wS3, len3);
          }
          else
          {
            unsigned char a = 0xFE;
            cl->GetSock()->Write(&a, 1);
            const char* c3 = wS3;
            //unsigned char len3 = (unsigned char) a;
            cl->GetSock()->Write(&len3, 1);
            cl->GetSock()->Write(c3, len3);

          }
          
          unsigned char v;
          cl->GetSock()->Read(&v, 1);
//          if(v == 0xAA) 
          sock->Write(&v, 1);
          //else sock->Write(0, 1);
          //const char* c3 = wS3.mb_str(wxConvLibc);
          //unsigned char len3 = (unsigned char)(wxStrlen(c3) + 1);
//          cl->GetSock()->Write(&len3, 1);
          /*if(len3 < 128)
          {
            cl->GetSock()->Write(c3, len3);
          }
          else
          {*/
//            wxCharBuffer buf1(wS3.mb_str(wxConvLibc));
//            cl->GetSock()->Write(buf1, len3 * 1024);
          //}

          //wxLogMessage(wxT("Все отправлено ") + wS2);
      }
      else
      {
          unsigned char c = 0xDE;
          sock->Write(&c, 1);

          const char* c1 = wS1;
          unsigned char len1 = (unsigned char)(wxStrlen(c1) + 1);
          sock->Write(&len1, 1);
          sock->Write(c1, len1);
            
          const char* c2 = wS2;
          unsigned char len2 = (unsigned char)(wxStrlen(c2) + 1);
          sock->Write(&len2, 1);
          sock->Write(c2, len2);
      }

wxLogMessage("send to || %s | %s | %s", wS1, wS2, wS3);

      sock->SetNotify(wxSOCKET_LOST_FLAG | wxSOCKET_INPUT_FLAG);
      break;
    }
    case wxSOCKET_LOST:
    {
      
      for(client c : m_clients)
      {
        /*
        if(c.GetAddress() == addr.IPAddress() && 
           c.GetPort() == wxString::Format(wxT("%d"), addr.Service())) 
        */
        if(c.GetSock() == sock)
        {
          m_clients.erase(c);
          break;
        }
      }
      

      //m_sockets.erase(sock);
      
      UpdateList();

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
  for(client i : m_clients)
  {
    m_listCtrl->InsertItem (0, "");
    m_listCtrl->SetItem (0, 0, i.GetName());  
    m_listCtrl->SetItem (0, 1, i.GetAddress(), -1);     
    m_listCtrl->SetItem (0, 2, i.GetPort(), -1);
  }
  SendList();
}

void MyFrame::SendList()
{
    unsigned char c = 0xBE;
    unsigned char len = m_clients.size();
    /*for(auto i : m_sockets)
    {
      i->Write(&len, 1);
      for(client j : m_clients)
      {
        const char* c1 = j.GetName().utf8_str();
        unsigned char len1 = (unsigned char)(wxStrlen(c1) + 1);
        i->Write(&len1, 1);
        i->Write(c1, len1);
        //wxString wS1(c1);
        wxMicroSleep(1000);

        const char* c2 = j.GetAddress().utf8_str();
        unsigned char len2 = (unsigned char)(wxStrlen(c2) + 1);
        i->Write(&len2, 1);
        i->Write(c2, len2);
        //wxString wS2(c2);
        wxMicroSleep(1000);

        const char* c3 = j.GetPort().utf8_str();
        unsigned char len3 = (unsigned char)(wxStrlen(c3) + 1);
        i->Write(&len3, 1);
        i->Write(c3, len3);
        //wxString wS3(c3);
        wxMicroSleep(1000);

        //wxLogMessage("|| %s | %s | %s", wS1, wS2, wS3);
        //wxLogMessage("");

      }
    }*/

    for(auto i : m_clients)
    {
      i.GetSock()->Write(&c, 1);
      i.GetSock()->Write(&len, 1);
      for(client j : m_clients)
      {
        const char* c1 = j.GetName().mb_str(wxConvLibc);
        unsigned char len1 = (unsigned char)(wxStrlen(c1) + 1);
        i.GetSock()->Write(&len1, 1);
        i.GetSock()->Write(c1, len1);
        //wxString wS1(c1);
        wxMicroSleep(1000);

        const char* c2 = j.GetAddress().utf8_str();
        unsigned char len2 = (unsigned char)(wxStrlen(c2) + 1);
        i.GetSock()->Write(&len2, 1);
        i.GetSock()->Write(c2, len2);
        //wxString wS2(c2);
        wxMicroSleep(1000);

        const char* c3 = j.GetPort().utf8_str();
        unsigned char len3 = (unsigned char)(wxStrlen(c3) + 1);
        i.GetSock()->Write(&len3, 1);
        i.GetSock()->Write(c3, len3);
        //wxString wS3(c3);
        wxMicroSleep(1000);

        //wxLogMessage("|| %s | %s | %s", wS1, wS2, wS3);
        //wxLogMessage("");

      }
    }
}
