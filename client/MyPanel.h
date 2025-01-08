#ifndef MYPANEL_H
#define MYPANEL_H

//подключение заголовочных файлов wxWidgets и приложения, используемых в этом файле (h и cpp)
#include <wx/panel.h>

class MyPanel : public wxPanel
{
public:
    MyPanel(wxPanel* parent);
    wxPanel* m_parentPanel; //член данных, для получения доступа к родительской панели

private:

};

//идентификаторы некоторых элементов приложения
enum
{
    ID_Panel,
};

#endif