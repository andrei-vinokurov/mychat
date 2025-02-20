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

      sock->SetEventHandler(*this, SOCKET_ID);
      sock->SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
      sock->Notify(true);

      wxLogMessage("Yes1");
      char buf[128];
      wxLogMessage("Yes1-1");
      sock->ReadMsg(buf, sizeof(buf));
      wxLogMessage("Yes1-2");
      wxString name1(buf);
      wxLogMessage("Yes1-3");
      


      wxLogMessage("Yes2");
      client newClient (name1, addr.IPAddress(), wxString::Format(wxT("%d"), addr.Service()), sock);
      m_mapClients.emplace(wxString::Format(wxT("%d"), addr.Service()), newClient);
      
      wxLogMessage("Yes3");    
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
  sock->SetTimeout(5);
/*
  switch(event.GetSocketEvent())
  {
    case wxSOCKET_INPUT : wxLogMessage(wxT("Присоединение клиента")); break;
    case wxSOCKET_LOST  : wxLogMessage(wxT("Отключение клиента")); break;
    default             : wxLogMessage(wxT("Неизвестное событие")); break;
  }
*/

  switch(event.GetSocketEvent())
  {
    case wxSOCKET_INPUT:
    {
      wxLogMessage("Yes1");
      sock->SetNotify(wxSOCKET_LOST_FLAG);

      unsigned char err = 0;

      if(sock->IsData() || (!(sock->IsData()) && sock->WaitForRead()))
      {
        wxLogMessage("Yes2");
      unsigned char a;
      sock->Read(&a, 1);
      switch(a)
      {
        case 0x01:
        {
          
          char buf[128];
          sock->ReadMsg(buf, sizeof(buf));
          wxString name1(buf);

          client newClient (name1, addr.IPAddress(), wxString::Format(wxT("%d"), addr.Service()), sock);
          m_mapClients.emplace(wxString::Format(wxT("%d"), addr.Service()), newClient);
          
          UpdateList();
            
          break;
        }

        case 0x02:
        {

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
          
          wxString wS3;

          unsigned char b;
          sock->Read(&b, 1);
          unsigned int len3;
          
          switch(b)
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

    

  ////////////////////////////////////
        if(sock->Error())
        {
          wxLogMessage("Error1");
          err = 0xAB;
          goto label;
        }
  ///////////////////////////////////
        
  //      wxLogMessage("|| %s | %s | %s", wS1, wS2, wS3); //для проверки


        if(m_mapClients.find(wS2) != m_mapClients.end())
        {
            client* cl = &m_mapClients.find(wS2)->second;

            unsigned char c = 0xCE;
            cl->GetSock()->Write(&c, 1);

            ////////////////////////////////////
            if(cl->GetSock()->Error())
            {
              wxLogMessage("Error2-1");
              err = 0xAB;
              goto label;
            }
            ////////////////////////////////////

            const char* c1 = addr.IPAddress();
            unsigned char len1 = (unsigned char)(wxStrlen(c1) + 1);
            cl->GetSock()->Write(&len1, 1);
            cl->GetSock()->Write(c1, len1);

            ////////////////////////////////////
            if(cl->GetSock()->Error())
            {
              wxLogMessage("Error2-2");
              err = 0xAB;
              goto label;
            }
            ////////////////////////////////////
            
            const char* c2 = wxString::Format(wxT("%d"), addr.Service());
            unsigned char len2 = (unsigned char)(wxStrlen(c2) + 1);
            cl->GetSock()->Write(&len2, 1);
            cl->GetSock()->Write(c2, len2);

            ////////////////////////////////////
            if(cl->GetSock()->Error())
            {
              wxLogMessage("Error2-3");
              err = 0xAB;
              goto label;
            }
            ////////////////////////////////////

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
              cl->GetSock()->Write(&len3, 1);
              cl->GetSock()->Write(c3, len3);

            }

            ///////////////////////////
            if(cl->GetSock()->Error())
            {
              wxLogMessage("Error3");
              err = 0xAB;
              goto label;
            }
            ///////////////////////////
            
            else
            {
              //wxLogMessage("send to || %s | %s | %s", wS1, wS2, wS3); //для проверки
              
              unsigned char v;
              cl->GetSock()->Read(&v, 1);
              
              ///////////////////////////
              if(cl->GetSock()->Error()) 
              {
                wxLogMessage("Error4");
                err = 0xAB;
                goto label;
              }
              ///////////////////////////
              else if(v == 0xAB)
              {
                err = v;
                goto label;
              }
              else
              {
                sock->Write(&v, 1);
              }                      
            }            
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
        break;
        }
      default: {wxLogMessage("Yes_default"); break;}
      }
      }

    label:
      if(err == 0xAB) sock->Write(&err, 1);
      sock->Discard();
      sock->SetNotify(wxSOCKET_LOST_FLAG | wxSOCKET_INPUT_FLAG);
      break;

  }


    case wxSOCKET_LOST:
    {
      if(auto search = m_mapClients.find(wxString::Format(wxT("%d"), addr.Service())); search != m_mapClients.end())
      {
        m_mapClients.erase(search);
      }
      
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
  for(auto& i : m_mapClients)
  {
    m_listCtrl->InsertItem (0, "");
    m_listCtrl->SetItem (0, 0, i.second.GetName());  
    m_listCtrl->SetItem (0, 1, i.second.GetAddress(), -1);     
    m_listCtrl->SetItem (0, 2, i.second.GetPort(), -1);
  }
  SendList();
}

void MyFrame::SendList()
{
    unsigned char c = 0xBE;
    unsigned char len = m_mapClients.size();
    for(auto& i : m_mapClients)
    {
      i.second.GetSock()->Write(&c, 1);
      i.second.GetSock()->Write(&len, 1);
      for(auto& j : m_mapClients)
      {
        const char* c1 = j.second.GetName().mb_str(wxConvLibc);
        unsigned char len1 = (unsigned char)(wxStrlen(c1) + 1);
        i.second.GetSock()->Write(&len1, 1);
        i.second.GetSock()->Write(c1, len1);
        wxMicroSleep(1000);

        const char* c2 = j.second.GetAddress().utf8_str();
        unsigned char len2 = (unsigned char)(wxStrlen(c2) + 1);
        i.second.GetSock()->Write(&len2, 1);
        i.second.GetSock()->Write(c2, len2);
        wxMicroSleep(1000);

        const char* c3 = j.second.GetPort().utf8_str();
        unsigned char len3 = (unsigned char)(wxStrlen(c3) + 1);
        i.second.GetSock()->Write(&len3, 1);
        i.second.GetSock()->Write(c3, len3);
        wxMicroSleep(1000);
      }
    }
}
