#pragma once
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/statline.h>
#include <functional>
#include <map>
#include "gamemechanics.h"
#include "timer.h"

#define PATH_TO_ICON "../../img/Wordle.ico"

// Tile / key states (also used to pick colours).
enum CellState { CS_EMPTY, CS_FILLED, CS_ABSENT, CS_PRESENT, CS_CORRECT };

// A single board square: a letter drawn on a coloured/bordered background.
class Tile : public wxWindow
{
public:
	Tile(wxWindow *parent, int size);
	void Set(const wxString &ch, CellState state);
	void Reset();

private:
	void OnPaint(wxPaintEvent &);
	wxString m_ch;
	CellState m_state = CS_EMPTY;
};

// An on-screen keyboard key (rounded, clickable), coloured by its best state.
class Key : public wxWindow
{
public:
	Key(wxWindow *parent, const wxString &label, const wxSize &size,
		std::function<void(const wxString &)> onPress);
	void SetState(CellState state);

private:
	void OnPaint(wxPaintEvent &);
	wxString m_label;
	CellState m_state = CS_EMPTY;
	std::function<void(const wxString &)> m_onPress;
};

class MainFrame : public wxFrame
{
public:
	MainFrame(wxWindow *parent, wxWindowID id = wxID_ANY, const wxString &title = wxT("Wordle"),
			  const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxSize(560, 820),
			  long style = wxDEFAULT_FRAME_STYLE);
	~MainFrame();

private:
	enum { ID_StartNewGame = 1002, ID_Authors = 1000 };

	// view
	Tile *m_board[6][5] = {};
	std::map<wxChar, Key *> m_keys;
	wxStaticText *m_status = nullptr;
	wxStaticText *m_timerText = nullptr;

	// state
	int m_row = 0, m_col = 0;
	wxString m_rowBuf;                   // letters typed in the current row
	std::map<wxChar, CellState> m_keyState; // best state seen per letter
	wxTimer m_clock;   // 1s game timer
	wxTimer m_reveal;  // staggered reveal animation
	int m_revealCol = 0;
	CellState m_pending[5] = {};
	wxString m_pendingWord;
	bool m_revealing = false;

	void NewGame();
	void HandleKey(const wxString &label); // letter, "ENTER" or "DEL"
	void SubmitGuess();
	void OnCharHook(wxKeyEvent &);
	void OnReveal(wxTimerEvent &);
	void OnClock(wxTimerEvent &);
	void UpgradeKey(wxChar ch, CellState state);
	void SetStatus(const wxString &msg, const wxColour &colour);
	wxString CurrentGuess() const;
	Key *AddKey(wxSizer *rowSizer, const wxString &label, int width);
};
