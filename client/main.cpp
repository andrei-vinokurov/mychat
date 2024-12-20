#include <wx/app.h> //файл wx для приложения
#include <wx/intl.h> //файл wx для wxLocale (для кириллицы)
//подключение заголовочных файлов приложения
#include "MyFrame.h"
#include "MyPanel.h"
#include "MyDialog.h"



//объявление класса приложения
class MyApp : public wxApp
{
public:
    wxLocale m_locale; // член данных для подключения кириллицы
    virtual bool OnInit() override; // функция инициализации приложения
    virtual int OnExit() override; // функция выхода из приложения
};


//макрос, заменяющий int main()
wxIMPLEMENT_APP(MyApp);  


//инициализация приложения
bool MyApp::OnInit()
{
    m_locale.Init(wxLANGUAGE_RUSSIAN);
    MyFrame* frame = new MyFrame();
    frame->Show(true);
    return true;
}


//выход из приложения
int MyApp::OnExit()
{
    return wxApp::OnExit();
}