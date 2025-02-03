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
    //m_text1->Destroy();
    //m_text2->Destroy();
    //m_send->Destroy();

    
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
    MyFrame* frameFromDialog = (MyFrame*) m_parent->GetParent();
    if(m_text2->GetValue() != "" && frameFromDialog->GetSocket()->IsOk())
    {
        //wxMicroSleep(1000);
        
        

        const char* c1 = m_addr.utf8_str();
        unsigned char len1 = (unsigned char)(wxStrlen(c1) + 1);
        frameFromDialog->GetSocket()->Write(&len1, 1);
        frameFromDialog->GetSocket()->Write(c1, len1);
        
        const char* c2 = m_port.utf8_str();
        unsigned char len2 = (unsigned char)(wxStrlen(c2) + 1);
        frameFromDialog->GetSocket()->Write(&len2, 1);
        frameFromDialog->GetSocket()->Write(c2, len2);

        unsigned int len3 = m_text2->GetValue().mb_str(wxConvLibc).length();
        if(len3 > 255)
        {
            unsigned char a = 0xEE;
            frameFromDialog->GetSocket()->Write(&a, 1);
            frameFromDialog->GetSocket()->WriteMsg(m_text2->GetValue().mb_str(wxConvLibc), len3);
        }
        else
        {
            unsigned char a = 0xFE;
            frameFromDialog->GetSocket()->Write(&a, 1);
            const char* c3 = m_text2->GetValue().mb_str(wxConvLibc);
            //unsigned char len3 = (unsigned char) a;
            frameFromDialog->GetSocket()->Write(&len3, 1);
            frameFromDialog->GetSocket()->Write(c3, len3);

        }
/*
        unsigned char v;
        frameFromDialog->GetSocket()->Read(&v, 1);

        switch(v)
        {
            case 0xAA:
            {*/
                m_text1->SetDefaultStyle(wxTextAttr(*wxRED));
                m_text1->AppendText(wxNow() + wxT(" (Вы)")  + "\n"); //wxDateTime::GetHour().FormatISOTime());
                m_text1->SetDefaultStyle(wxTextAttr(*wxBLACK));
                m_text1->AppendText(m_text2->GetValue() + "\n\n");
/*                break;
            }
            default: 
            {
                m_text1->SetDefaultStyle(wxTextAttr(*wxGREEN));
                m_text1->AppendText(wxT("Сообщение не доставлено!\n\n"));
                m_text1->SetDefaultStyle(wxTextAttr(*wxBLACK));
                break;
            }
        }
*/
    /*    
        const char* c3 = m_text2->GetValue().mb_str(wxConvLibc);
        //unsigned char len3 = (unsigned char)(wxStrlen(c3) + 1);
        unsigned char len3 = (unsigned char)(m_text2->GetValue().mb_str(wxConvLibc).length() + 1);
        unsigned char len30 = len3 / 1024 + 1;
        frameFromDialog->GetSocket()->Write(&len30, 1);
    */    /*if(len3 < 128)
        {   
            frameFromDialog->GetSocket()->Write(c3, len3);
            wxLogMessage(wxT("Маленький объем %d"), len3);
        }
        else
        {*/
    /*
            wxCharBuffer buf(m_text2->GetValue().mb_str(wxConvLibc));
            frameFromDialog->GetSocket()->Write(buf, len3);
            wxLogMessage(wxT("Большой объем %d"), len3);
    */    //}
        m_text2->SetValue("");
    
    }
    m_text2->SetFocus();

}

wxString MyDialog::GetName(){return m_name;}
wxString MyDialog::GetAddr(){return m_addr;}
wxString MyDialog::GetPort(){return m_port;}