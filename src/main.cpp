#include "Wordle.h"
#include "assets.h"

#ifdef __APPLE__
void SetMacDockIcon(const char *path); // defined in dock_icon_mac.mm
#endif

class MyApp : public wxApp
{
public:
    virtual bool OnInit()
    {
        wxInitAllImageHandlers(); // needed to load the .ico
        Game_mec::dictionaryDir = AssetPath("dictionary").ToStdString();
#ifdef __APPLE__
        SetMacDockIcon(AssetPath("images/Wordle.ico").ToStdString().c_str());
#endif
        MainFrame *frame = new MainFrame(NULL);
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);