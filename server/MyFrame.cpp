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

    //создаем адрес
    IPaddress addr;
    addr.Service(3000);

    wxLogMessage("Creating server at %s:%u", addr.IPAddress(), addr.Service());

    //создаем сокет
    m_server = new wxSocketServer(addr);

    //используем IsOk() для проверки, прослушивает ли сокет
    if (!m_server->IsOk())
    {
      wxLogMessage("Could not listen at the specified port !");
      return;
    }

    IPaddress addrReal;
    if (!m_server->GetLocal(addrReal))
    {
      wxLogMessage("ERROR: couldn't get the address we bound to");
    }
    else
    {
    wxLogMessage("Server listening at %s:%u",
                 addrReal.IPAddress(), addrReal.Service());
    }

    //установки обработчика событий
    m_server->SetEventHandler(*this, SERVER_ID);
    m_server->SetNotify(wxSOCKET_CONNECTION_FLAG);
    m_server->Notify(true);

    //создаем область перечня подключенных клиентов
    m_listCtrl = new wxListCtrl (this, wxID_ANY, wxPoint (0,0), wxSize (500, 500), wxLC_REPORT);
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
      m_listCtrl->InsertItem (0, "");
    }
    else
    {
      wxLogMessage(wxT("Новый клиент %s:%u присоединился"),
                   addr.IPAddress(), addr.Service());

      m_listCtrl->InsertItem (0, "");
      m_listCtrl->SetItem (0, 0, addr.IPAddress(), -1);     
      m_listCtrl->SetItem (0, 1, wxString::Format(wxT("%d"), addr.Service()), -1);
    }

  }
  else
  {
    wxLogMessage(wxT("Ошибка: не удалось принять новое соединение"));
    return;
  }

  sock->SetEventHandler(*this, SOCKET_ID);
  sock->SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
  sock->Notify(true);

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
      wxLogMessage(wxT("Сокет удален"));
      sock->Destroy();
      break;
    }
    default: ;
  }

}
