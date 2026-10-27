#include "Wordle.h"
#include "embedded_assets.h"

#ifdef __APPLE__
void SetMacDockIcon(const unsigned char *data, size_t len); // dock_icon_mac.mm
#endif

class MyApp : public wxApp
{
public:
    virtual bool OnInit()
    {
        wxInitAllImageHandlers(); // needed to decode the embedded .ico
        Game_mec::wordsSelect.assign((const char *)asset_words_select, asset_words_select_len);
        Game_mec::wordsValid.assign((const char *)asset_words_valid, asset_words_valid_len);
#ifdef __APPLE__
        SetMacDockIcon(asset_icon_ico, asset_icon_ico_len);
#endif
        MainFrame *frame = new MainFrame(NULL);
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);