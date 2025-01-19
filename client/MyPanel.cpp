#include "MyPanel.h"

//конструктор класса MyPanel
MyPanel::MyPanel(wxPanel* parent) : wxPanel (parent, wxID_ANY, wxDefaultPosition, wxSize (500, 600), wxBORDER_SUNKEN)
{
    m_parentPanel = parent; //получение доступа к родительской панели
}
