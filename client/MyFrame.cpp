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
    m_Panel = new wxPanel(this, wxID_ANY, wxPoint (0,0), wxSize (500, 600));
    //wxPanel* panel = new wxPanel(this, wxID_ANY, wxPoint (0,0), wxSize (0,0));
    //m_myPanel = new MyPanel(panel);
    CreateStatusBar(); //информационная панель
    SetStatusText(wxT("Добро пожаловать в Чат!")); //сообщение в информационной панели

    //создаем область перечня доступных клиентов
    m_listCtrl = new wxListCtrl (m_Panel, LIST_ID, wxPoint (0,0), wxSize (500, 500), wxLC_REPORT);
    m_listCtrl->AppendColumn (wxT("Имя"), wxLIST_FORMAT_CENTER, 200);
    m_listCtrl->AppendColumn ("IP", wxLIST_FORMAT_CENTER, 150);
    m_listCtrl->AppendColumn ("Port", wxLIST_FORMAT_CENTER, 150);

    //привязка функций и событий
    Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);
    Bind(wxEVT_MENU, &MyFrame::OnOpenConnection, this, CLIENT_OPEN);
#if wxUSE_IPV6
    Bind(wxEVT_MENU, &MyFrame::OnOpenConnectionIPv6, this, CLIENT_OPENIPV6);
#endif
    Bind(wxEVT_MENU, &MyFrame::OnCloseConnection, this, CLIENT_CLOSE);
    Bind(wxEVT_SOCKET, &MyFrame::OnSocketEvent, this, SOCKET_ID);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &MyFrame::OpenDialog, this, LIST_ID);
    

    //создаем сокет
    m_sock = new wxSocketClient();

    //устанавливаем обработчики событий сокета
    m_sock->SetEventHandler(*this, SOCKET_ID);
    m_sock->SetNotify(wxSOCKET_CONNECTION_FLAG |
                    wxSOCKET_INPUT_FLAG |
                    wxSOCKET_LOST_FLAG);
    m_sock->Notify(true);

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

//    wxLogMessage("Trying to connect to %s:%d", hostname, addr->Service());

    m_sock->Connect(*addr, false);
}

void MyFrame::OnSocketEvent(wxSocketEvent& event)
{

    switch ( event.GetSocketEvent() )
    {
        case wxSOCKET_INPUT:
        {
//            wxLogMessage("Input available on the socket");
            
            UpdateList();

            break;
        }
        case wxSOCKET_LOST:
            wxLogMessage("Socket connection was unexpectedly lost.");

            m_listCtrl->DeleteAllItems();
            m_clients.clear();

            break;

        case wxSOCKET_CONNECTION:
//          wxLogMessage("... socket is now connected.");

            break;

        default:
            wxLogMessage("Unknown socket event!!!");
            break;
    }

}

void MyFrame::OnCloseConnection(wxCommandEvent& event)
{
  m_sock->Close();
  m_listCtrl->DeleteAllItems();
  m_clients.clear();
}

void MyFrame::UpdateList()
{
  RecList();                                  
  m_listCtrl->DeleteAllItems();
  for(client c : m_clients)
  {
    m_listCtrl->InsertItem (0, "");
    m_listCtrl->SetItem (0, 0, c.GetName());  
    m_listCtrl->SetItem (0, 1, c.GetAddress(), -1);     
    m_listCtrl->SetItem (0, 2, c.GetPort(), -1);
  }
}

void MyFrame::RecList()
{
    m_clients.clear();
    unsigned char len;
    m_sock->Read(&len, 1);
    for(unsigned char i = 0; i < len; ++i)
    {
        unsigned char len1;
        m_sock->Read(&len1, 1);
        char c1[len1];
        m_sock->Read(c1, len1);
        wxString wS1(c1);

        unsigned char len2;
        m_sock->Read(&len2, 1); 
        char c2[len2];
        m_sock->Read(c2, len2);
        wxString wS2(c2);
   
        unsigned char len3;
        m_sock->Read(&len3, 1);
        char c3[len3];
        m_sock->Read(c3, len3);       
        wxString wS3(c3);

        client newClient(wS1, wS2, wS3);
        m_clients.insert(newClient);

    }
}

void MyFrame::OpenDialog(wxListEvent& event)
{
    //wxLogMessage("rabotaet!!!");
    //std::unique_ptr<MyDialog> myDial = std::make_unique<MyDialog>(m_Panel);
    MyDialog* myDial = new MyDialog(m_Panel);
    myDial->Show(true);
    //wxPanel* panel(this);
    //myDial->Create(panel, wxID_ANY, wxT("Диалог"), wxDefaultPosition, wxSize(500, 600));
    //myDial->SetParent(m_myPanel);

}