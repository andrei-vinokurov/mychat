#include "MyDialog.h"
#include "MyFrame.h"

MyDialog::MyDialog(wxPanel* parent) : wxDialog(parent, wxID_ANY, wxT("Диалог"), wxDefaultPosition, wxSize(500, 600))
{
    m_parent = parent;
    m_text1 = new wxTextCtrl(this, wxID_ANY, "", wxPoint (0, 0), wxSize (500, 300), wxTE_MULTILINE | wxTE_READONLY);
    m_text2 = new wxTextCtrl(this, wxID_ANY, "", wxPoint (0, 300), wxSize (500, 220), wxTE_MULTILINE);
    m_text1->SetBackgroundColour(*wxLIGHT_GREY);
    m_text2->SetFocus();
    m_send = new wxButton(this, wxID_ANY, wxT("Отправить"), wxPoint (200, 530), wxSize (100, 30));
    m_send->Bind(wxEVT_BUTTON, &MyDialog::SendText, this);
}

void MyDialog::SendText(wxCommandEvent& event)
{   
    m_text1->AppendText(m_text2->GetValue() + "\n");
    m_text2->SetValue("");
    m_text2->SetFocus();

    MyFrame* frameFromDialog = (MyFrame*) m_parent->GetParent();
    const char* c = m_text2->GetValue().utf8_str();
    unsigned char len = (unsigned char)(wxStrlen(c) + 1);
    frameFromDialog->GetSocket()->Write(&len, 1);
    frameFromDialog->GetSocket()->Write(c, len);

}
