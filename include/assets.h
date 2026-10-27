#pragma once
#include <wx/string.h>

// Absolute path to a file under assets/, resolved relative to the running
// executable (not the current directory), so the game works from anywhere.
wxString AssetPath(const wxString &relative);
