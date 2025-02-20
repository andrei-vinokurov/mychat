#include "MyDialog.h"
#include "MyFrame.h"

MyDialog::MyDialog(wxPanel* parent, wxString name, wxString addr, wxString port) : wxDialog(parent, wxID_ANY, wxT("Диалог"), wxDefaultPosition, wxSize(500, 600))
{
    m_parent = parent;
    m_name = name;
    m_addr = addr;
    m_port = port;
    m_text1 = new wxTextCtrl(this, wxID_ANY, "", wxPoint (0, 0), wxSize (500, 300), wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH);
    m_text2 = new wxTextCtrl(this, wxID_ANY, "", wxPoint (0, 300), wxSize (500, 220), wxTE_MULTILINE);
    m_text1->SetBackgroundColour(*wxLIGHT_GREY);
    m_text2->SetFocus();
    m_send = new wxButton(this, wxID_ANY, wxT("Отправить"), wxPoint (200, 530), wxSize (100, 30));
    m_send->Bind(wxEVT_BUTTON, &MyDialog::SendText, this);
    Bind(wxEVT_BUTTON, &MyDialog::SendText, this);
    Bind(wxEVT_CLOSE_WINDOW, [this](wxCloseEvent&) {this->Destroy();});
}

MyDialog::~MyDialog()
{

    MyFrame* frameFromDialDestr = (MyFrame*) m_parent->GetParent();
    for (unsigned int i = 0; i < frameFromDialDestr->m_vecDial.size(); ++i)
    {
        if(this == frameFromDialDestr->m_vecDial.at(i))
        {
            frameFromDialDestr->m_vecDial.erase(frameFromDialDestr->m_vecDial.begin() + i);
            break;
        }
    }
    
}


void MyDialog::SendText(wxCommandEvent& event)
{   
    if(!m_waitButton)
    {
    m_waitButton = true;
    MyFrame* frameFromDialog = (MyFrame*) m_parent->GetParent();
    unsigned char a = 0x02;
    frameFromDialog->GetSocket()->Write(&a, 1);

    if(m_text2->GetValue() != "" && frameFromDialog->GetSocket()->IsOk())
    {
        
        frameFromDialog->GetSocket()->SetFlags(wxSOCKET_WAITALL);

        const char* c1 = m_addr.utf8_str();
        unsigned char len1 = (unsigned char)(wxStrlen(c1) + 1);
        frameFromDialog->GetSocket()->Write(&len1, 1);
        frameFromDialog->GetSocket()->Write(c1, len1);

////////////////////////////////////////

        if(frameFromDialog->GetSocket()->Error())
        {
            m_text1->SetDefaultStyle(wxTextAttr(*wxCYAN));
            m_text1->AppendText(wxT("Сообщение не отправлено! Повторите попытку.\n\n"));
            m_text1->SetDefaultStyle(wxTextAttr(*wxBLACK));
        }

////////////////////////////////////////

        const char* c2 = m_port.utf8_str();
        unsigned char len2 = (unsigned char)(wxStrlen(c2) + 1);
        frameFromDialog->GetSocket()->Write(&len2, 1);
        frameFromDialog->GetSocket()->Write(c2, len2);

////////////////////////////////////////

        if(frameFromDialog->GetSocket()->Error())
        {
            m_text1->SetDefaultStyle(wxTextAttr(*wxCYAN));
            m_text1->AppendText(wxT("Сообщение не отправлено! Повторите попытку.\n\n"));
            m_text1->SetDefaultStyle(wxTextAttr(*wxBLACK));
        }

////////////////////////////////////////

        unsigned int len3 = m_text2->GetValue().mb_str(wxConvLibc).length() + 1;
        if(len3 > 255)
        {
            unsigned char b = 0xEE;
            frameFromDialog->GetSocket()->Write(&b, 1);
            frameFromDialog->GetSocket()->WriteMsg(m_text2->GetValue().mb_str(wxConvLibc), len3);
        }
        else
        {
            unsigned char b = 0xFE;
            frameFromDialog->GetSocket()->Write(&b, 1);
            const char* c3 = m_text2->GetValue().mb_str(wxConvLibc);
            //unsigned char len3 = (unsigned char) a;
            frameFromDialog->GetSocket()->Write(&len3, 1);
            frameFromDialog->GetSocket()->Write(c3, len3);

        }

////////////////////////////////////////

        if(frameFromDialog->GetSocket()->Error())
        {
            m_text1->SetDefaultStyle(wxTextAttr(*wxCYAN));
            m_text1->AppendText(wxT("Сообщение не отправлено! Повторите попытку.\n\n"));
            m_text1->SetDefaultStyle(wxTextAttr(*wxBLACK));
        }

////////////////////////////////////////
        
        //frameFromDialog->GetSocket()->WaitForRead(2);
        unsigned char v = 0;
        frameFromDialog->GetSocket()->Read(&v, 1);

        if(frameFromDialog->GetSocket()->Error())
        {
            m_text1->SetDefaultStyle(wxTextAttr(*wxCYAN));
            m_text1->AppendText(wxT("Статус доставки сообщения неизвестен.\n\n"));
            m_text1->SetDefaultStyle(wxTextAttr(*wxBLACK));
        }

        else 
        {
            if(v == 0xAA)
            {
                m_text1->SetDefaultStyle(wxTextAttr(*wxRED));
                m_text1->AppendText(wxNow() + wxT(" (Вы)")  + "\n"); //wxDateTime::GetHour().FormatISOTime());
                m_text1->SetDefaultStyle(wxTextAttr(*wxBLACK));
                m_text1->AppendText(m_text2->GetValue() + "\n");

                m_text1->SetDefaultStyle(wxTextAttr(*wxGREEN));
                m_text1->AppendText(wxT("Сообщение доставлено\n\n"));
                m_text1->SetDefaultStyle(wxTextAttr(*wxBLACK));

                m_text2->SetValue("");
            }
            if(v == 0xAB)
            {
                m_text1->SetDefaultStyle(wxTextAttr(*wxCYAN));
                m_text1->AppendText(wxT("Статус доставки сообщения неизвестен.\n\n"));
                m_text1->SetDefaultStyle(wxTextAttr(*wxBLACK));
            }
        }

            frameFromDialog->GetSocket()->Discard();
    
    }

    wxMicroSleep(300000);

    m_waitButton = false;

    }

    m_text2->SetFocus();

}

wxString MyDialog::GetName(){return m_name;}
wxString MyDialog::GetAddr(){return m_addr;}
wxString MyDialog::GetPort(){return m_port;}