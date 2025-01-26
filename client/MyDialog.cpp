#include "MyDialog.h"

MyDialog::MyDialog(wxPanel* parent) : wxDialog(parent, wxID_ANY, wxT("Диалог"), wxDefaultPosition, wxSize(500, 600))
{
    m_text = new wxTextCtrl(this, wxID_ANY, "", wxPoint (0, 0), wxSize (500, 300), wxTE_MULTILINE | wxTE_READONLY);
}

MyDialog::~MyDialog()
{
    delete m_text;
}
