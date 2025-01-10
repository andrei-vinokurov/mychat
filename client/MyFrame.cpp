#include "MyFrame.h"

//конструктор основного окна
MyFrame::MyFrame() : wxFrame(nullptr, wxID_ANY, wxT("Чат"), wxDefaultPosition, wxSize(500, 500))
{
    wxMenu* menuFile = new wxMenu; //меню "Файл"
    menuFile->Append(CLIENT_OPEN, wxT("Подключение"), wxT("Подключиться к чату(IPV4)"));
#if wxUSE_IPV6
    menuFile->Append(CLIENT_OPENIPV6, wxT("Подключение (IPV6)"), wxT("Подключиться к чату(IPV6)"));
#endif
    menuFile->AppendSeparator();
    menuFile->Append(CLIENT_CLOSE, wxT("Отключение"), wxT("Отключиться от чата"));
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT, wxT("Выход"), wxT("Выйти из программы"));
    wxMenu* menuHelp = new wxMenu; //меню "Помощь"
    menuHelp->Append(wxID_ABOUT, wxT("О программе"), wxT("Краткая информация о работе программы"));
    wxMenuBar* menuBar = new wxMenuBar; //создание панели Меню
    menuBar->Append(menuFile, wxT("Файл")); //добавление пункта в панель Меню
    menuBar->Append(menuHelp, wxT("Помощь")); //добавление пункта в панель Меню
    SetMenuBar( menuBar ); //установка панели Меню в окно
    //wxPanel* panel = new wxPanel(this, ID_Panel, wxPoint (0,0), wxSize (0,0));
    //m_myPanel = new MyPanel(panel);
    CreateStatusBar(); //информационная панель
    SetStatusText(wxT("Добро пожаловать в Чат!")); //сообщение в информационной панели
    //привязка функций к пунктам меню
    Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);
    Bind(wxEVT_MENU, &MyFrame::OnOpenConnection, this, CLIENT_OPEN);
#if wxUSE_IPV6
    Bind(wxEVT_MENU, &MyFrame::OnOpenConnectionIPv6, this, CLIENT_OPENIPV6);
#endif
    Bind(wxEVT_MENU, &MyFrame::OnCloseConnection, this, CLIENT_CLOSE);
    

    //создаем сокет
    m_sock = new wxSocketClient();

    //устанавливаем обработчики событий сокета
    m_sock->SetEventHandler(*this, SOCKET_ID);
    m_sock->SetNotify(wxSOCKET_CONNECTION_FLAG |
                    wxSOCKET_INPUT_FLAG |
                    wxSOCKET_LOST_FLAG);
    m_sock->Notify(true);

    //создаем область перечня доступных клиентов
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
    wxMessageBox(wxT("Эта программа помогает обмениваться по сети текстовыми сообщениями."),
                 wxT("О программе"), wxOK | wxICON_INFORMATION);
}


void MyFrame::OnOpenConnection(wxCommandEvent& event)
{
    OpenConnection(wxSockAddress::IPV4);
}
#if wxUSE_IPV6
void MyFrame::OnOpenConnectionIPv6(wxCommandEvent& event)
{
    OpenConnection(wxSockAddress::IPV6);
}
#endif // wxUSE_IPV6


void MyFrame::OpenConnection(wxSockAddress::Family family)
{
    wxUnusedVar(family); // unused in !wxUSE_IPV6 case

    wxIPaddress* addr;
    wxIPV4address addr4;
#if wxUSE_IPV6
    wxIPV6address addr6;
    if (family == wxSockAddress::IPV6)
    addr = &addr6;
    else
#endif
    addr = &addr4;
    
    wxString hostname = wxGetTextFromUser(
    wxT("Введите адрес сервера"),
    wxT("Соединение ..."),
    "localhost");
    if ( hostname.empty() )
    return;
    

    addr->Hostname(hostname);
    addr->Service(3000);

    wxLogMessage("Trying to connect to %s:%d", hostname, addr->Service());

    m_sock->Connect(*addr, false);
}

void MyFrame::OnCloseConnection(wxCommandEvent& event)
{
  m_sock->Close();
}