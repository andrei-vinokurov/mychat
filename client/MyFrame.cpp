#include "MyFrame.h"

//конструктор основного окна
MyFrame::MyFrame() : wxFrame(nullptr, wxID_ANY, wxT("Чат"), wxDefaultPosition, wxSize(500, 500))
{
    SetIcon (wxICON(icon_frame));  //подключаем логотип через файл res.h
    wxMenu* menuFile = new wxMenu; //меню "Файл"
    menuFile->Append(CLIENT_OPEN, wxT("Подключение"), wxT("Подключиться к чату(IPV4)"));
//#if wxUSE_IPV6
//    menuFile->Append(CLIENT_OPENIPV6, wxT("Подключение (IPV6)"), wxT("Подключиться к чату(IPV6)"));
//#endif
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


//подключение IPv4
void MyFrame::OnOpenConnection(wxCommandEvent& event)
{
    OpenConnection(wxSockAddress::IPV4);
}
//подключение IPv6
#if wxUSE_IPV6
void MyFrame::OnOpenConnectionIPv6(wxCommandEvent& event)
{
    OpenConnection(wxSockAddress::IPV6);
}
#endif


//подключение к серверу
void MyFrame::OpenConnection(wxSockAddress::Family family)
{
    wxUnusedVar(family);

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
    if (hostname.empty()) return;
    
    addr->Hostname(hostname);
    addr->Service(3000);

    m_sock->Connect(*addr, false);
    wxMicroSleep(1000);   
    m_sock->GetLocal(m_addr);
    
}


//отслеживания событий сокета - входящие сообщения, подключение, отключение
void MyFrame::OnSocketEvent(wxSocketEvent& event)
{

    switch ( event.GetSocketEvent() )
    {
        case wxSOCKET_INPUT:
        {
            unsigned char c;
            m_sock->Read(&c, 1);

            switch (c)
            {
                case 0xBE:
                { 
                    UpdateList(); //обновить список
                    break;
                }
                case 0xCE:
                {
                    GetMsg(); //получить сообщение
                    break;
                }
                
                default: break;
            }
            break;
        }
        case wxSOCKET_LOST:    
            wxLogMessage(wxT("Соединение было потеряно"));
            m_listCtrl->DeleteAllItems();     
            m_clients.clear();

            break;

        case wxSOCKET_CONNECTION:
        {
            m_name = wxGetTextFromUser(
                wxT("Укажите Ваше имя в чате"),
                wxT("Получение имени"),
                "");

            m_sock->WriteMsg(m_name.mb_str(wxConvLibc), (m_name.mb_str(wxConvLibc).length() + 1));

            break;
        }

        default:
            wxLogMessage(wxT("Неизвестное событие"));
            break;
    }

}


//отключение от сервера
void MyFrame::OnCloseConnection(wxCommandEvent& event)
{
  m_sock->Close();
  m_listCtrl->DeleteAllItems();     
  m_clients.clear();
}


//обновить список клиентов
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


//получить список клиентов
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

        if(m_addr.IPAddress() != wS2 || wxString::Format(wxT("%d"),m_addr.Service()) != wS3)
        {
            client newClient(wS1, wS2, wS3);
            m_clients.insert(newClient);
        }

    }
    
    m_sock->Discard();
}


//открыть диалог с клиентом
void MyFrame::OpenDialog(wxListEvent& event)
{
    MyDialog* mD = nullptr;
    for(MyDialog* i : m_vecDial)
    {
        if(i->GetAddr() == m_listCtrl->GetItemText(event.GetIndex(), 1) && i->GetPort() == m_listCtrl->GetItemText(event.GetIndex(), 2))
        {
            mD = i;
            break;   
        }
    }
    if(!mD)
    {
        mD = new MyDialog(m_Panel, m_listCtrl->GetItemText(event.GetIndex(), 0), m_listCtrl->GetItemText(event.GetIndex(), 1), m_listCtrl->GetItemText(event.GetIndex(), 2));
        m_vecDial.push_back(mD);
    }
    mD->Show(true);

}


//возвращает сокет
wxSocketClient* MyFrame::GetSocket()
{
    return m_sock;
}


//получить сообщение
void MyFrame::GetMsg()
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

    wxString wS3;

    unsigned char a;
    m_sock->Read(&a, 1);
    unsigned int len3;

    switch(a)
    {
        case 0xEE: //получение большого сообщения
        {
        char buf[4096];
        wxUint32 len4 = m_sock->ReadMsg(buf, sizeof(buf)).LastCount();
        wxString wS4(buf);
        wS3 = wS4;
        len3 = len4;
        break;
        }
    
        case 0xFE: //получение небольшого сообщения
        {
        unsigned char len5;
        m_sock->Read(&len5, 1);
        char c3[len5];
        m_sock->Read(c3, len5);
        wxString wS5(c3);
        wS3 = wS5;
        len3 = len5;
        break;
        }

        default: break;
    }

    //открытие диалогового окна при получении сообщения
    MyDialog* mD = nullptr;
    for(MyDialog* i : m_vecDial)
    {
        if(i->GetAddr() == wS1 && i->GetPort() == wS2)
        {
            mD = i;
            break;   
        }
    }
    if(!mD)
    {
        wxString nameDialog = "noname";
        for(client c : m_clients)
        {
            if(c.GetAddress() == wS1 && c.GetPort() == wS2) 
            {
                nameDialog = c.GetName();
                break;
            }
        }
        mD = new MyDialog(m_Panel, nameDialog, wS1, wS2);
        m_vecDial.push_back(mD);
    }
    mD->Show(true);
    mD->m_text1->SetDefaultStyle(wxTextAttr(*wxBLUE));
    mD->m_text1->AppendText(wxNow() + " " + mD->GetName() + "\n");
    mD->m_text1->SetDefaultStyle(wxTextAttr(*wxBLACK));
    mD->m_text1->AppendText(wS3 + "\n\n");

    unsigned char v;
    if(m_sock->Error())
    {
        v = 0xAB;
    }
    else
    {
        v = 0xAA;
    }
    m_sock->Write(&v, 1);

m_sock->Discard();
}


//возвращает имя
wxString MyFrame::GetName()
{
    return m_name;
}