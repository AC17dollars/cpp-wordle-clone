#include "Wordle.h"
#include "assets.h"

// NYT-Wordle palette.
namespace
{
const wxColour BG(255, 255, 255);
const wxColour TEXT_DARK(26, 26, 27);
const wxColour BORDER_EMPTY(211, 214, 223);
const wxColour BORDER_FILLED(135, 138, 140);
const wxColour ABSENT(120, 124, 126);
const wxColour PRESENT(201, 180, 88);
const wxColour CORRECT(106, 170, 100);
const wxColour KEY_BG(211, 214, 223);
const wxColour WHITE(255, 255, 255);
const wxColour MUTED(120, 124, 126);
const wxColour ERRORC(211, 70, 70);

wxColour FillFor(CellState s)
{
	switch (s)
	{
	case CS_ABSENT: return ABSENT;
	case CS_PRESENT: return PRESENT;
	case CS_CORRECT: return CORRECT;
	default: return BG;
	}
}
int Rank(CellState s) { return s == CS_CORRECT ? 3 : s == CS_PRESENT ? 2 : s == CS_ABSENT ? 1 : 0; }
}

///////////////////////////////////////////////////////////////////////////// Tile

Tile::Tile(wxWindow *parent, int size) : wxWindow(parent, wxID_ANY, wxDefaultPosition, wxSize(size, size))
{
	SetBackgroundStyle(wxBG_STYLE_PAINT);
	SetMinSize(wxSize(size, size));
	Bind(wxEVT_PAINT, &Tile::OnPaint, this);
}

void Tile::Set(const wxString &ch, CellState state)
{
	m_ch = ch;
	m_state = state;
	Refresh();
	Update();
}

void Tile::Reset()
{
	m_ch.clear();
	m_state = CS_EMPTY;
	Refresh();
}

void Tile::OnPaint(wxPaintEvent &)
{
	wxAutoBufferedPaintDC dc(this);
	const wxSize sz = GetClientSize();
	dc.SetBrush(wxBrush(BG));
	dc.SetPen(*wxTRANSPARENT_PEN);
	dc.DrawRectangle(0, 0, sz.x, sz.y);

	const bool blank = (m_state == CS_EMPTY || m_state == CS_FILLED);
	if (blank)
	{
		dc.SetBrush(wxBrush(BG));
		dc.SetPen(wxPen(m_state == CS_FILLED ? BORDER_FILLED : BORDER_EMPTY, 2));
		dc.DrawRectangle(1, 1, sz.x - 2, sz.y - 2);
	}
	else
	{
		dc.SetBrush(wxBrush(FillFor(m_state)));
		dc.SetPen(*wxTRANSPARENT_PEN);
		dc.DrawRectangle(0, 0, sz.x, sz.y);
	}

	if (!m_ch.empty())
	{
		dc.SetTextForeground(blank ? TEXT_DARK : WHITE);
		dc.SetFont(wxFont(wxFontInfo(sz.y * 42 / 100).Bold()));
		const wxSize te = dc.GetTextExtent(m_ch);
		dc.DrawText(m_ch, (sz.x - te.x) / 2, (sz.y - te.y) / 2);
	}
}

///////////////////////////////////////////////////////////////////////////// Key

Key::Key(wxWindow *parent, const wxString &label, const wxSize &size,
		 std::function<void(const wxString &)> onPress)
	: wxWindow(parent, wxID_ANY, wxDefaultPosition, size), m_label(label), m_onPress(std::move(onPress))
{
	SetBackgroundStyle(wxBG_STYLE_PAINT);
	SetMinSize(size);
	SetCursor(wxCursor(wxCURSOR_HAND));
	Bind(wxEVT_PAINT, &Key::OnPaint, this);
	Bind(wxEVT_LEFT_DOWN, [this](wxMouseEvent &) { if (m_onPress) m_onPress(m_label); });
}

void Key::SetState(CellState state)
{
	m_state = state;
	Refresh();
}

void Key::OnPaint(wxPaintEvent &)
{
	wxAutoBufferedPaintDC dc(this);
	const wxSize sz = GetClientSize();
	dc.SetBrush(wxBrush(BG));
	dc.SetPen(*wxTRANSPARENT_PEN);
	dc.DrawRectangle(0, 0, sz.x, sz.y);

	const bool coloured = (m_state == CS_ABSENT || m_state == CS_PRESENT || m_state == CS_CORRECT);
	dc.SetBrush(wxBrush(coloured ? FillFor(m_state) : KEY_BG));
	dc.SetPen(*wxTRANSPARENT_PEN);
	dc.DrawRoundedRectangle(0, 0, sz.x, sz.y, 5);

	dc.SetTextForeground(coloured ? WHITE : TEXT_DARK);
	dc.SetFont(wxFont(wxFontInfo(m_label.length() > 1 ? 11 : 16).Bold()));
	const wxSize te = dc.GetTextExtent(m_label);
	dc.DrawText(m_label, (sz.x - te.x) / 2, (sz.y - te.y) / 2);
}

///////////////////////////////////////////////////////////////////////////// MainFrame

MainFrame::MainFrame(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos,
					 const wxSize &size, long style)
	: wxFrame(parent, id, title, pos, size, style)
{
	SetBackgroundColour(WHITE);
	SetIcon(wxIcon(AssetPath("images/Wordle.ico"), wxBITMAP_TYPE_ICO));

	wxMenuBar *mb = new wxMenuBar();
	wxMenu *game = new wxMenu();
	game->Append(ID_StartNewGame, wxT("New Game\tCtrl-N"));
	game->AppendSeparator();
	game->Append(wxID_EXIT, wxT("Quit"));
	mb->Append(game, wxT("Game"));
	wxMenu *help = new wxMenu();
	help->Append(ID_Authors, wxT("About"));
	mb->Append(help, wxT("Help"));
	SetMenuBar(mb);

	wxBoxSizer *rootS = new wxBoxSizer(wxVERTICAL);

	// header: title + timer
	wxBoxSizer *header = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *titleText = new wxStaticText(this, wxID_ANY, wxT("Wordle"));
	titleText->SetFont(wxFont(wxFontInfo(30).Bold()));
	titleText->SetForegroundColour(TEXT_DARK);
	m_timerText = new wxStaticText(this, wxID_ANY, wxT("Time Elapsed: 00:00:00"));
	m_timerText->SetForegroundColour(MUTED);
	header->Add(titleText, 0, wxALIGN_CENTER_VERTICAL);
	header->AddStretchSpacer();
	header->Add(m_timerText, 0, wxALIGN_CENTER_VERTICAL);
	rootS->Add(header, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 16);
	rootS->Add(new wxStaticLine(this), 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 8);

	// status message
	m_status = new wxStaticText(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize,
								wxALIGN_CENTRE_HORIZONTAL | wxST_NO_AUTORESIZE);
	m_status->SetFont(wxFont(wxFontInfo(12).Bold()));
	rootS->Add(m_status, 0, wxEXPAND | wxTOP, 8);

	// board
	const int tile = 60;
	wxGridSizer *grid = new wxGridSizer(6, 5, 6, 6);
	for (int r = 0; r < 6; r++)
		for (int c = 0; c < 5; c++)
		{
			m_board[r][c] = new Tile(this, tile);
			grid->Add(m_board[r][c]);
		}
	rootS->Add(grid, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP, 10);

	rootS->AddStretchSpacer();

	// keyboard
	const char *rows[3] = {"QWERTYUIOP", "ASDFGHJKL", "ZXCVBNM"};
	wxBoxSizer *kb = new wxBoxSizer(wxVERTICAL);
	for (int r = 0; r < 3; r++)
	{
		wxBoxSizer *row = new wxBoxSizer(wxHORIZONTAL);
		if (r == 2)
			AddKey(row, wxT("ENTER"), 58);
		for (const char *p = rows[r]; *p; ++p)
			m_keys[(wxChar)*p] = AddKey(row, wxString(wxUniChar(*p)), 40);
		if (r == 2)
			AddKey(row, wxT("DEL"), 58);
		kb->Add(row, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM, 6);
	}
	rootS->Add(kb, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM, 16);

	SetSizer(rootS);
	SetMinSize(wxSize(520, 760));

	// timers
	m_clock.SetOwner(this, 1);
	m_reveal.SetOwner(this, 2);
	Bind(wxEVT_TIMER, &MainFrame::OnClock, this, 1);
	Bind(wxEVT_TIMER, &MainFrame::OnReveal, this, 2);

	// input + menus
	Bind(wxEVT_CHAR_HOOK, &MainFrame::OnCharHook, this);
	Bind(wxEVT_MENU, [this](wxCommandEvent &) { NewGame(); }, ID_StartNewGame);
	Bind(wxEVT_MENU, [this](wxCommandEvent &) { Close(true); }, wxID_EXIT);
	Bind(wxEVT_MENU, [this](wxCommandEvent &)
		 { wxMessageBox(wxT("A C++ / wxWidgets Wordle clone.\n\nType a 5-letter word and press Enter."),
						wxT("About Wordle")); }, ID_Authors);

	NewGame();
	Centre();
}

MainFrame::~MainFrame() {}

Key *MainFrame::AddKey(wxSizer *rowSizer, const wxString &label, int width)
{
	Key *k = new Key(this, label, wxSize(width, 56), [this](const wxString &l) { HandleKey(l); });
	rowSizer->Add(k, 0, wxALL, 2);
	return k;
}

void MainFrame::SetStatus(const wxString &msg, const wxColour &colour)
{
	m_status->SetForegroundColour(colour);
	m_status->SetLabel(msg);
}

wxString MainFrame::CurrentGuess() const { return m_rowBuf; }

void MainFrame::NewGame()
{
	if (m_reveal.IsRunning())
		m_reveal.Stop();
	m_revealing = false;
	m_row = m_col = m_revealCol = 0;
	m_rowBuf.clear();

	Timer::OnStopTimer(&m_clock, m_timerText);
	m_timerText->SetLabel(wxT("Time Elapsed: 00:00:00"));

	for (int r = 0; r < 6; r++)
		for (int c = 0; c < 5; c++)
			m_board[r][c]->Reset();
	for (auto &kv : m_keys)
		kv.second->SetState(CS_EMPTY);
	m_keyState.clear();
	SetStatus(wxT(""), TEXT_DARK);

	Game_mec::no_of_guesses = 0;
	Game_mec::gethidword(); // picks a new word and sets game_started
}

void MainFrame::HandleKey(const wxString &label)
{
	if (m_revealing)
		return;
	if (label == wxT("ENTER"))
	{
		SubmitGuess();
		return;
	}
	if (label == wxT("DEL"))
	{
		if (m_col > 0)
		{
			m_col--;
			m_rowBuf.RemoveLast();
			m_board[m_row][m_col]->Reset();
		}
		return;
	}
	// a letter
	if (!Game_mec::game_started || m_row >= 6 || m_col >= 5)
		return;
	m_board[m_row][m_col]->Set(label, CS_FILLED);
	m_rowBuf += label;
	m_col++;
}

void MainFrame::SubmitGuess()
{
	if (m_revealing || !Game_mec::game_started)
		return;
	if (m_col < 5)
	{
		SetStatus(wxT("Not enough letters"), MUTED);
		return;
	}
	const wxString guess = CurrentGuess();
	const std::string lower = guess.Lower().ToStdString();
	if (!Game_mec::val_checker(lower))
	{
		SetStatus(wxT("Not in word list"), ERRORC);
		return;
	}
	SetStatus(wxT(""), TEXT_DARK);

	if (Game_mec::no_of_guesses == 0)
		Timer::OnTimerStart(&m_clock, m_timerText);

	Game_mec::let_checker(lower);
	for (int j = 0; j < 5; j++)
		m_pending[j] = Game_mec::posn[j] == 1 ? CS_CORRECT
					   : Game_mec::posn[j] == 2 ? CS_PRESENT
												: CS_ABSENT;
	m_pendingWord = guess;
	m_revealing = true;
	m_revealCol = 0;
	m_reveal.Start(220);
}

void MainFrame::OnReveal(wxTimerEvent &)
{
	const int j = m_revealCol;
	m_board[m_row][j]->Set(wxString(m_pendingWord[j]), m_pending[j]);
	UpgradeKey(m_pendingWord[j], m_pending[j]);
	if (++m_revealCol < 5)
		return;

	m_reveal.Stop();
	m_revealing = false;
	Game_mec::no_of_guesses++;

	if (m_pendingWord.Lower().ToStdString() == Game_mec::hidword)
	{
		Timer::OnStopTimer(&m_clock, m_timerText);
		SetStatus(wxT("You won!"), CORRECT);
		Game_mec::game_started = false;
		return;
	}
	m_row++;
	m_col = 0;
	m_rowBuf.clear();
	if (m_row >= 6)
	{
		Timer::OnStopTimer(&m_clock, m_timerText);
		SetStatus(wxT("You lost - the word was ") + wxString(Game_mec::hidword).Upper(), ERRORC);
		Game_mec::game_started = false;
	}
}

void MainFrame::UpgradeKey(wxChar ch, CellState state)
{
	auto k = m_keys.find(ch);
	if (k == m_keys.end())
		return;
	const CellState cur = m_keyState.count(ch) ? m_keyState[ch] : CS_EMPTY;
	if (Rank(state) > Rank(cur))
	{
		m_keyState[ch] = state;
		k->second->SetState(state);
	}
}

void MainFrame::OnClock(wxTimerEvent &) { Timer::UpdateDisplayedTime(&m_clock, m_timerText); }

void MainFrame::OnCharHook(wxKeyEvent &e)
{
	const int k = e.GetKeyCode();
	if (k >= 'A' && k <= 'Z')
		HandleKey(wxString(wxUniChar(k)));
	else if (k == WXK_BACK)
		HandleKey(wxT("DEL"));
	else if (k == WXK_RETURN || k == WXK_NUMPAD_ENTER)
		HandleKey(wxT("ENTER"));
	else
		e.Skip();
}
