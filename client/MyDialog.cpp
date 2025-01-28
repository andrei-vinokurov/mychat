#include "MyDialog.h"
#include "MyFrame.h"

MyDialog::MyDialog(wxPanel* parent, wxString name, wxString addr, wxString port) : wxDialog(parent, wxID_ANY, wxT("Диалог"), wxDefaultPosition, wxSize(500, 600))
{
    m_parent = parent;
    m_name = name;
    m_addr = addr;
    m_port = port;
    m_text1 = new wxTextCtrl(this, wxID_ANY, "", wxPoint (0, 0), wxSize (500, 300), wxTE_MULTILINE | wxTE_READONLY);
    m_text2 = new wxTextCtrl(this, wxID_ANY, "", wxPoint (0, 300), wxSize (500, 220), wxTE_MULTILINE);
    m_text1->SetBackgroundColour(*wxLIGHT_GREY);
    m_text2->SetFocus();
    m_send = new wxButton(this, wxID_ANY, wxT("Отправить"), wxPoint (200, 530), wxSize (100, 30));
    m_send->Bind(wxEVT_BUTTON, &MyDialog::SendText, this);
}

MyDialog::~MyDialog()
{
    MyFrame* frameFromDialDestr = (MyFrame*) m_parent->GetParent();
    for (unsigned int i = 0; i < frameFromDialDestr->m_vecDial.size(); ++i)
    {
        if(this == frameFromDialDestr->m_vecDial.at(i))
        {
            frameFromDialDestr->m_vecDial.erase(frameFromDialDestr->m_vecDial.begin() + i);
        }
    }

}

void MyDialog::SendText(wxCommandEvent& event)
{   
    if(m_text2->GetValue() != "")
    {
        m_text1->AppendText(m_text2->GetValue() + "\n");
        MyFrame* frameFromDialog = (MyFrame*) m_parent->GetParent();
        
        const char* c1 = m_addr.utf8_str();
        unsigned char len1 = (unsigned char)(wxStrlen(c1) + 1);
        frameFromDialog->GetSocket()->Write(&len1, 1);
        frameFromDialog->GetSocket()->Write(c1, len1);
        
        const char* c2 = m_port.utf8_str();
        unsigned char len2 = (unsigned char)(wxStrlen(c2) + 1);
        frameFromDialog->GetSocket()->Write(&len2, 1);
        frameFromDialog->GetSocket()->Write(c2, len2);
        
        const char* c3 = m_text2->GetValue().utf8_str();
        unsigned char len3 = (unsigned char)(wxStrlen(c3) + 1);
        frameFromDialog->GetSocket()->Write(&len3, 1);
        frameFromDialog->GetSocket()->Write(c3, len3);
        
        m_text2->SetValue("");
    
    }
    m_text2->SetFocus();

}

wxString MyDialog::GetName(){return m_name;}
wxString MyDialog::GetAddr(){return m_addr;}
wxString MyDialog::GetPort(){return m_port;}