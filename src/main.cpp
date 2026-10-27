#include "Wordle.h"

#ifdef __APPLE__
void SetMacDockIcon(const char *path); // defined in dock_icon_mac.mm
#endif

class MyApp : public wxApp
{
public:
    virtual bool OnInit()
    {
        wxInitAllImageHandlers(); // needed to load the .bmp header / .ico
#ifdef __APPLE__
        SetMacDockIcon(PATH_TO_ICON);
#endif
        MainFrame *frame = new MainFrame(NULL);
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);