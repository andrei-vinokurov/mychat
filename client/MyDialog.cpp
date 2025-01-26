#include "MyDialog.h"

MyDialog::MyDialog(wxPanel* parent) : wxDialog(parent, wxID_ANY, wxT("Диалог"), wxDefaultPosition, wxSize(500, 600))
{
    m_text1 = new wxTextCtrl(this, wxID_ANY, "", wxPoint (0, 0), wxSize (500, 300), wxTE_MULTILINE | wxTE_READONLY);
    m_text2 = new wxTextCtrl(this, wxID_ANY, "", wxPoint (0, 300), wxSize (500, 220), wxTE_MULTILINE);
    m_send = new wxButton(this, wxID_ANY, wxT("Отправить"), wxPoint (200, 530), wxSize (100, 30));
    m_send->Bind(wxEVT_BUTTON, &MyDialog::SendText, this);
    
}

void MyDialog::SendText(wxCommandEvent& event)
{
    m_text1->AppendText(m_text2->GetValue());

}
