#include "assets.h"
#include <wx/stdpaths.h>
#include <wx/filename.h>

// The executable lives in build/<variant>/, so assets/ is two levels up.
wxString AssetPath(const wxString &relative)
{
	wxFileName dir = wxFileName::DirName(
		wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetPath());
	dir.RemoveLastDir(); // build/<variant> -> build
	dir.RemoveLastDir(); // build          -> project root
	dir.AppendDir("assets");
	return dir.GetPathWithSep() + relative;
}
