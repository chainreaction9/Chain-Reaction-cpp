#include "PrecompiledHeader.h" //precompiled header
#include "AppGUIFrame.h"
#include "MainGame.h"
#include "Utilities.h"
//*********************************** static variable initialization **********************************
const unsigned int AppGUIFrame::_minimumNumberOfRows = (unsigned int) 6;
const unsigned int AppGUIFrame::_maximumNumberOfRows = (unsigned int) 10;
const unsigned int AppGUIFrame::_minimumNumberOfColumns = (unsigned int) 6;
const unsigned int AppGUIFrame::_maximumNumberOfColumns = (unsigned int) 10;
const unsigned int AppGUIFrame::_maximumNumberOfPlayers = (unsigned int) 8;
const std::vector<wxString> AppGUIFrame::_colorList = {"Red","Blue","Green","Yellow","Cyan","Purple","Violet","Pink","Orange","Green 1","Brown","Maroon","Green 2","Grey"};
const std::unordered_map<wxString, wxColor> AppGUIFrame::_colorMap = AppGUIFrame::_createColormap();
//*****************************************************************************************************

AppGUIFrame::AppGUIFrame(const wxString& title = "", const wxPoint& position = wxDefaultPosition, const wxSize& size = wxDefaultSize, const wxSize& minsize = wxSize(0,0)): wxFrame(NULL, wxID_ANY, title, position, size){	
	//*************************************
	//The topmost sizer of the application frame
	wxBoxSizer *topsizer = new wxBoxSizer(wxHORIZONTAL);
	this->SetSizer(topsizer);
	//Topmost parent window of the leftside control panel.
	wxSashLayoutWindow *window1 = new wxSashLayoutWindow(this,wxID_ANY,wxPoint(0,0),wxDefaultSize,wxSW_3D);
	window1->SetOrientation(wxLAYOUT_VERTICAL);
	window1->SetAlignment(wxLAYOUT_LEFT);
	window1->SetExtraBorderSize(2);

	topsizer->Add(window1,0,wxEXPAND);
	wxPanel *panelwindow = new wxPanel(window1);
	this->_topsizerForLeftWindow = new wxBoxSizer(wxVERTICAL);
	panelwindow->SetSizer(this->_topsizerForLeftWindow);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	this->_panelOutsideGame = new wxPanel(panelwindow,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxSUNKEN_BORDER);
	this->_panelOutsideGame->SetBackgroundColour(wxColor(128,128,128));

	this->_topsizerForLeftWindow->Add(this->_panelOutsideGame,1,wxEXPAND);

	wxBoxSizer *control_panelsizer  = new wxBoxSizer(wxVERTICAL);
	this->_panelOutsideGame->SetSizer(control_panelsizer);
	//////
	control_panelsizer->AddStretchSpacer(1);
	wxPanel *subpanelForBoardDimension = new wxPanel(this->_panelOutsideGame, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	subpanelForBoardDimension->SetBackgroundColour(wxColor(200,200,200));
	wxBoxSizer * subsizerForBoardDimension = new wxBoxSizer(wxHORIZONTAL);
	///
	wxStaticText *label = new wxStaticText(subpanelForBoardDimension,wxID_STATIC,wxT("BOARD : "),wxDefaultPosition,wxDefaultSize, wxALIGN_CENTER);
	label->SetBackgroundColour(wxColor(200,200,200));
	subsizerForBoardDimension->Add(label,0,wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL,3);
	///
	wxArrayString rowList;
	for (unsigned int i = AppGUIFrame::_minimumNumberOfRows; i < (AppGUIFrame::_maximumNumberOfRows + 1); i++) rowList.Add(wxString::Format("%d", i));
	this->_rowChoice = new wxComboBox(subpanelForBoardDimension, ID_ROW_CHOICE, wxString::Format("%d", AppGUIFrame::_minimumNumberOfRows + 2), wxDefaultPosition, wxDefaultSize, rowList, wxCB_READONLY);
	subsizerForBoardDimension->Add(this->_rowChoice,0,wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL,3);
	///
	label = new wxStaticText(subpanelForBoardDimension,wxID_STATIC,wxT(" x "),wxDefaultPosition,wxDefaultSize,wxALIGN_CENTER);
	label->SetBackgroundColour(wxColor(200,200,200));
	subsizerForBoardDimension->Add(label,0,wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL,3);
	///
	wxArrayString columnList;
	for (unsigned int i = AppGUIFrame::_minimumNumberOfColumns; i < (AppGUIFrame::_maximumNumberOfColumns + 1); i++) columnList.Add(wxString::Format("%d", i));
	this->_columnChoice = new wxComboBox(subpanelForBoardDimension, ID_COL_CHOICE, wxString::Format("%d", AppGUIFrame::_minimumNumberOfColumns), wxDefaultPosition, wxDefaultSize, columnList, wxCB_READONLY);
	subsizerForBoardDimension->Add(this->_columnChoice,0,wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL,3);
	///
	subpanelForBoardDimension->SetSizer(subsizerForBoardDimension);
	///
	control_panelsizer->Add(subpanelForBoardDimension,0,wxALL | wxEXPAND,5);
	//////
	control_panelsizer->AddStretchSpacer(1);
	wxPanel *subpanelForNumberOfPlayers = new wxPanel(this->_panelOutsideGame, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	subpanelForNumberOfPlayers->SetBackgroundColour(wxColor(200,200,200));
	wxBoxSizer *subsizer2  = new wxBoxSizer(wxHORIZONTAL);
	///
	label = new wxStaticText(subpanelForNumberOfPlayers,wxID_STATIC,wxT("Number of Players : "),wxDefaultPosition,wxDefaultSize, wxALIGN_CENTER);
	label->SetBackgroundColour(wxColour(200,200,200));
	subsizer2->Add(label,0,wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL,3);
	///
	wxArrayString playerList;
	for (unsigned int i= 2; i < AppGUIFrame::_maximumNumberOfPlayers + 1; i++) playerList.Add(wxString::Format("%d",i));
	this->_playerChoice = new wxComboBox(subpanelForNumberOfPlayers,ID_PLAYER_CHOICE, wxT("2"), wxDefaultPosition, wxDefaultSize, playerList, wxCB_READONLY);
	subsizer2->Add(this->_playerChoice,1,wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL,3);
	///
	subpanelForNumberOfPlayers->SetSizer(subsizer2);
	///
	control_panelsizer->Add(subpanelForNumberOfPlayers,0,wxALL | wxEXPAND,5);
	//////
	control_panelsizer->AddStretchSpacer(1);
	for (unsigned int i = 0; i < AppGUIFrame::_maximumNumberOfPlayers; i++) {
		wxPanel * subpanelForPlayerColor = new wxPanel(this->_panelOutsideGame, wxID_ANY, wxDefaultPosition, wxDefaultSize);
		subpanelForPlayerColor->SetBackgroundColour(wxColor(200, 200, 200));
		label = new wxStaticText(subpanelForPlayerColor, wxID_STATIC, wxString::Format("Player %d:", i + 2), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
		label->SetBackgroundColour(wxColour(200, 200, 200));
		wxBoxSizer* subsizerForPlayerColor = new wxBoxSizer(wxHORIZONTAL);
		subsizerForPlayerColor->Add(label, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL, 3);
		///
		wxArrayString playerColorList;
		for (unsigned int j = 0; j < AppGUIFrame::_colorList.size(); j++) playerColorList.Add(_colorList[j]);
		this->_playerColors.push_back(playerColorList);
		wxComboBox *playerColorOptionsWidget = new wxComboBox(subpanelForPlayerColor, ID_COLOR_CHOICE + i, AppGUIFrame::_colorList[i], wxDefaultPosition, wxDefaultSize, playerColorList, wxCB_READONLY);
		subsizerForPlayerColor->Add(playerColorOptionsWidget, 1, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL, 3);
		subpanelForPlayerColor->SetSizer(subsizerForPlayerColor);
		control_panelsizer->Add(subpanelForPlayerColor, 0, wxALL | wxEXPAND, 5);
		this->_playerColorsWidgetList.push_back(playerColorOptionsWidget);
		if (i > 1) playerColorOptionsWidget->Disable();
		control_panelsizer->AddStretchSpacer(1);
	}
	wxPanel *subpanelForStartButton = new wxPanel(this->_panelOutsideGame, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	subpanelForStartButton->SetBackgroundColour(wxColor(180,180,180));
	///
	wxBoxSizer *subsizerForStartButton = new wxBoxSizer(wxHORIZONTAL);
	///
	wxButton *startButton = new wxButton(subpanelForStartButton, wxID_OK, "Start Game");
	startButton->SetBackgroundColour(wxColor(180,180,180));
	subsizerForStartButton->Add(startButton, 1, wxALIGN_CENTRE_HORIZONTAL | wxALIGN_CENTER_VERTICAL);
	subpanelForStartButton->SetSizer(subsizerForStartButton);
	subsizerForStartButton->Fit(subpanelForStartButton);

	control_panelsizer->Add(subpanelForStartButton, 0, wxALL | wxEXPAND, 5);
	///////
	control_panelsizer->AddStretchSpacer(1);
	wxPanel *subpanelForRegistration = new wxPanel(this->_panelOutsideGame,wxID_ANY,wxDefaultPosition,wxDefaultSize);
	subpanelForRegistration->SetBackgroundColour(wxColor(180,180,180));
	///
	wxBoxSizer *subsizerForRegistration = new wxBoxSizer(wxHORIZONTAL);
	///
	wxButton *registerOnlineButton = new wxButton(subpanelForRegistration, ID_REGISTER_ONLINE, "Register Online");
	registerOnlineButton->SetBackgroundColour(wxColor(180,180,180));
	subsizerForRegistration->Add(registerOnlineButton,1,wxALIGN_CENTRE_HORIZONTAL | wxALIGN_CENTER_VERTICAL);
	subpanelForRegistration->SetSizer(subsizerForRegistration);
	subsizerForRegistration->Fit(subpanelForRegistration);

	control_panelsizer->Add(subpanelForRegistration,0,wxALL | wxEXPAND,5);
	control_panelsizer->AddStretchSpacer(1);

	control_panelsizer->FitInside(this->_panelOutsideGame);
	this->_panelInsideGame = new wxPanel(panelwindow,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxSUNKEN_BORDER);
	this->_topsizerForLeftWindow->Add(this->_panelInsideGame, 1, wxEXPAND);
	wxBoxSizer *game_panelsizer  = new wxBoxSizer(wxVERTICAL);
	this->_panelInsideGame->SetSizer(game_panelsizer);

	this->_panelInsideGame->SetBackgroundColour(wxColor(128,128,128));
	//////
	wxPanel *gameSubpanelForBoardLabel = new wxPanel(this->_panelInsideGame, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	gameSubpanelForBoardLabel->SetBackgroundColour(wxColor(200,200,200));
	wxBoxSizer *gameSubsizerForBoardLabel  = new wxBoxSizer(wxHORIZONTAL);
	///
	this->_boardLabel = new wxStaticText(gameSubpanelForBoardLabel, wxID_ANY, "BOARD : 10 x 10  ", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL);
	this->_boardLabel->SetBackgroundColour(wxColor(200,200,200));
	this->_boardLabel->SetFont(wxFont(13,wxFONTFAMILY_DECORATIVE,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,wxT("Times")));
	this->_boardLabel->SetForegroundColour(wxColor(20,20,200));
	///
	gameSubsizerForBoardLabel->Add(this->_boardLabel,1,wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL);
	gameSubpanelForBoardLabel->SetSizer(gameSubsizerForBoardLabel);

	game_panelsizer->AddStretchSpacer(1);
	game_panelsizer->Add(gameSubpanelForBoardLabel,0,wxEXPAND | wxALL,5);
	//////
	wxPanel *gameSubpanelForPlayerLabel = new wxPanel(this->_panelInsideGame, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	gameSubpanelForPlayerLabel->SetBackgroundColour(wxColor(200,200,200));
	wxBoxSizer *gameSubsizerForPlayerLabel  = new wxBoxSizer(wxHORIZONTAL);
	///
	this->_playerLabel = new wxStaticText(gameSubpanelForPlayerLabel,wxID_ANY,"Players : 2  ",wxDefaultPosition,wxDefaultSize,wxALIGN_CENTER_HORIZONTAL);
	this->_playerLabel->SetBackgroundColour(wxColor(200,200,200));
	this->_playerLabel->SetFont(wxFont(13,wxFONTFAMILY_DECORATIVE,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,wxT("Times")));
	this->_playerLabel->SetForegroundColour(wxColor(20,20,200));
	///
	gameSubsizerForPlayerLabel->Add(this->_playerLabel, 1, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL);
	gameSubpanelForPlayerLabel->SetSizer(gameSubsizerForPlayerLabel);

	game_panelsizer->AddStretchSpacer(1);
	game_panelsizer->Add(gameSubpanelForPlayerLabel,0,wxEXPAND | wxALL,5);
	//////
	wxPanel *gameSubpanelForTurnLabel = new wxPanel(this->_panelInsideGame, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	gameSubpanelForTurnLabel->SetBackgroundColour(wxColor(200,200,200));
	this->_gameSubsizerForTurnLabel = new wxBoxSizer(wxHORIZONTAL);
	///
	this->_turnLabelStatic = new wxStaticText(gameSubpanelForTurnLabel, wxID_ANY, " Turn :");
	this->_turnLabelStatic->SetBackgroundColour(wxColor(200, 200, 200));
	this->_turnLabelStatic->SetFont(wxFont(13, wxFONTFAMILY_DECORATIVE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Times")));
	this->_turnLabelStatic->SetForegroundColour(wxColor(20, 20, 200));
	///
	this->_gameSubsizerForTurnLabel->Add(this->_turnLabelStatic,0,wxALIGN_CENTRE_HORIZONTAL | wxALIGN_CENTER_VERTICAL);
	///
	this->_turnLabel = new wxStaticText(gameSubpanelForTurnLabel, wxID_ANY, "Player 1", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
	this->_turnLabel->SetFont(wxFont(13, wxFONTFAMILY_DECORATIVE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Times")));
	this->_turnLabel->SetBackgroundColour(AppGUIFrame::_colorMap.at("red"));
	///
	this->_gameSubsizerForTurnLabel->Add(this->_turnLabel, 1, wxALIGN_CENTRE_HORIZONTAL | wxALIGN_CENTER_VERTICAL);
	gameSubpanelForTurnLabel->SetSizer(this->_gameSubsizerForTurnLabel);
	
	game_panelsizer->AddStretchSpacer(1);
	game_panelsizer->Add(gameSubpanelForTurnLabel, 0, wxEXPAND | wxALL, 5);
	//////
	wxPanel *gameSubpanelForMoveOrderButton = new wxPanel(this->_panelInsideGame, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	gameSubpanelForMoveOrderButton->SetBackgroundColour(wxColor(180,180,180));
	wxBoxSizer *gameSubsizerForMoveOrderButton = new wxBoxSizer(wxHORIZONTAL);
	///
	this->_infoButton = new wxButton(gameSubpanelForMoveOrderButton, ID_INFO_BUTTON, "Move Order");
	this->_infoButton->SetBackgroundColour(wxColor(180,180,180));
	///
	gameSubsizerForMoveOrderButton->Add(this->_infoButton, 1, wxALIGN_CENTRE_HORIZONTAL | wxALIGN_CENTER_VERTICAL);
	gameSubpanelForMoveOrderButton->SetSizer(gameSubsizerForMoveOrderButton);

	game_panelsizer->AddStretchSpacer(1);
	game_panelsizer->Add(gameSubpanelForMoveOrderButton, 0, wxEXPAND | wxALL, 5);
	//////
	wxPanel *gameSubpanelForUndoButton = new wxPanel(this->_panelInsideGame, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	gameSubpanelForUndoButton->SetBackgroundColour(wxColor(180,180,180));
	wxBoxSizer *gameSubsizerForUndoButton = new wxBoxSizer(wxHORIZONTAL);
	///
	wxButton *undo = new wxButton(gameSubpanelForUndoButton, wxID_UNDO);
	undo->SetBackgroundColour(wxColor(180,180,180));
	///
	gameSubsizerForUndoButton->Add(undo, 1, wxALIGN_CENTRE_HORIZONTAL | wxALIGN_CENTER_VERTICAL);
	gameSubpanelForUndoButton->SetSizer(gameSubsizerForUndoButton);

	game_panelsizer->AddStretchSpacer(1);
	game_panelsizer->Add(gameSubpanelForUndoButton, 0, wxEXPAND | wxALL, 5);
	//////
	wxPanel *gameSubpanelForNewGameButton = new wxPanel(this->_panelInsideGame, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	gameSubpanelForNewGameButton->SetBackgroundColour(wxColor(180,180,180));
	wxBoxSizer *gameSubsizerForNewGameButton = new wxBoxSizer(wxHORIZONTAL);
	///
	wxButton *start_new = new wxButton(gameSubpanelForNewGameButton, ID_START_NEW_BUTTON, "Start &New Game");
	start_new->SetBackgroundColour(wxColor(180,180,180));
	///
	gameSubsizerForNewGameButton->Add(start_new, 1, wxALIGN_CENTRE_HORIZONTAL | wxALIGN_CENTER_VERTICAL);
	gameSubpanelForNewGameButton->SetSizer(gameSubsizerForNewGameButton);

	game_panelsizer->AddStretchSpacer(1);
	game_panelsizer->Add(gameSubpanelForNewGameButton,0,wxEXPAND | wxALL,5);
	//////
	wxPanel *gameSubpanelForMainMenuButton = new wxPanel(this->_panelInsideGame, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	gameSubpanelForMainMenuButton->SetBackgroundColour(wxColor(180,180,180));
	wxBoxSizer *gameSubsizerForMainMenuButton = new wxBoxSizer(wxHORIZONTAL);
	///
	wxButton *main_menu = new wxButton(gameSubpanelForMainMenuButton, ID_MAIN_MENU, "&Main Menu");
	main_menu->SetBackgroundColour(wxColor(180,180,180));
	///
	gameSubsizerForMainMenuButton->Add(main_menu, 1, wxALIGN_CENTRE_HORIZONTAL | wxALIGN_CENTER_VERTICAL);
	gameSubpanelForMainMenuButton->SetSizer(gameSubsizerForMainMenuButton);

	game_panelsizer->AddStretchSpacer(1);
	game_panelsizer->Add(gameSubpanelForMainMenuButton, 0, wxEXPAND | wxALL, 5);
	//////
	wxPanel *gameSubpanelForQuitButton = new wxPanel(this->_panelInsideGame, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	gameSubpanelForQuitButton->SetBackgroundColour(wxColor(180,180,180));
	wxBoxSizer *gameSubsizerForQuitButton = new wxBoxSizer(wxHORIZONTAL);
	///
	wxButton *quit = new wxButton(gameSubpanelForQuitButton, wxID_EXIT);
	quit->SetBackgroundColour(wxColor(180,180,180));
	///
	gameSubsizerForQuitButton->Add(quit,1,wxALIGN_CENTRE_HORIZONTAL | wxALIGN_CENTER_VERTICAL);
	gameSubpanelForQuitButton->SetSizer(gameSubsizerForQuitButton);

	game_panelsizer->AddStretchSpacer(1);
	game_panelsizer->Add(gameSubpanelForQuitButton, 0, wxEXPAND | wxALL,5);
	game_panelsizer->AddStretchSpacer(1);

	game_panelsizer->Fit(this->_panelInsideGame);

	this->_topsizerForLeftWindow->Hide(this->_panelInsideGame);
	this->_topsizerForLeftWindow->Layout();
	this->_topsizerForLeftWindow->Fit(panelwindow);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	wxSashLayoutWindow *window2 = new wxSashLayoutWindow(this,wxID_ANY,wxPoint(0,0),wxSize(500,500),wxSW_3D);
	window2->SetOrientation(wxLAYOUT_VERTICAL);
	window2->SetAlignment(wxLAYOUT_LEFT);
	window2->SetBackgroundColour(wxColour(128, 128, 150));
	window2->SetExtraBorderSize(2);
	topsizer->Add(window2,1,wxEXPAND);
	this->canvas_panel = new AppGLCanvas(window2);
	wxLogDebug(wxString::Format("[Chain-Reaction] Created wxGLCanvas (OpenGL) object %p ...", this->canvas_panel));
	topsizer->Fit(this);
	this->SetMinSize(minsize);
	
	//********************** Bind all event-handlers dynamically *********************************************
	wxLogDebug("[Chain-Reaction] Performing dynamic event-bindings to the application-frame object ...");
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &AppGUIFrame::OnButtonStart, this, wxID_OK);
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &AppGUIFrame::OnButtonTurnInfo, this, ID_INFO_BUTTON);
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &AppGUIFrame::OnButtonUndo, this, wxID_UNDO);
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &AppGUIFrame::OnButtonStartNew, this, ID_START_NEW_BUTTON);
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &AppGUIFrame::OnButtonMainMenu, this, ID_MAIN_MENU);
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &AppGUIFrame::OnButtonQuit, this, wxID_EXIT);
	Bind(wxEVT_COMBOBOX, &AppGUIFrame::OnPlayerSelection, this, ID_PLAYER_CHOICE);
	for (unsigned int i = 0; i < AppGUIFrame::_maximumNumberOfPlayers; i++) {
		Bind(wxEVT_COMBOBOX_DROPDOWN, &AppGUIFrame::OnDropDown, this, ID_COLOR_CHOICE + i);
	}
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &AppGUIFrame::OnButtonRegisterOnline, this, ID_REGISTER_ONLINE);
	wxLogDebug("[Chain-Reaction] Dynamic event-bindings to the application-frame object is completed ...");
	//*********************************************************************************************************
}
void AppGUIFrame::OnButtonStart(wxCommandEvent& event){
	//Reject start of a game if OpenGL canvas is not yet ready for rendering...
	if (!(this->canvas_panel && this->canvas_panel->IsCanvasReadyForRendering())) {
		wxLogDebug(wxString::Format("[Chain-Reaction] Error: failed to start the game. OpenGL canvas instance %p is not yet ready for rendering ...", this->canvas_panel));
		wxMessageBox(wxT("OpenGL canvas is not yet ready for rendering!"), wxT("Error!"), wxOK | wxICON_INFORMATION);
		return;
	}
	//Configure game parameters before the start of a game ....
	unsigned int numberOfRows = this->_rowChoice->GetSelection() + 6;
	unsigned int numberOfColumns = this->_columnChoice->GetSelection() + 6;
	unsigned int player_no = this->_playerChoice->GetSelection() + 2;
	std::vector<std::string> players;
	for (unsigned int i = 0; i < player_no; i++){
		const auto& color = this->_playerColorsWidgetList[i]->GetStringSelection();
		players.push_back(std::string(color));
	}
	std::set<std::string> player_set = std::set<std::string>(players.begin(),players.end());
	if ((unsigned int) player_set.size() != (unsigned int) players.size()){
		wxMessageBox(wxT("Same colors are not allowed for multiple players!"),wxT("Error!"),wxOK | wxICON_INFORMATION);
		return;
	}
	//Retrieve the game object ...
	GameUtilities::GameState* gameInfo = (GameUtilities::GameState*)this->canvas_panel->GetClientData();
	if (!gameInfo) {
		wxLogDebug("[Chain-Reaction] Error: failed to start the game. Game information has not been generated yet ...");
		wxMessageBox(wxT("Failed to start the game. Game information has not been generated yet!"), wxT("Error!"), wxOK | wxICON_INFORMATION);
		return;
	}
	MainGame* game = (MainGame*) gameInfo->game;
	if (!game) {
		wxLogDebug("[Chain-Reaction] Error: failed to start the game. Game object has not been created yet ...");
		wxMessageBox(wxT("Failed to start the game. Game object has not been created yet!"), wxT("Error!"), wxOK | wxICON_INFORMATION);
		return;
	}
	//Inform the game object about the current canvas dimension.
	wxSize size = this->canvas_panel->GetClientSize();
	game->setCanvasSize(size.x, size.y);
	game->setupCamera();

	//Pass the game parameters to the game object ...
	game->resetGameVariables();
	game->setAttribute(numberOfRows, numberOfColumns, players);
	game->updateTurn();
	wxString dummyLabel = wxString::Format("BOARD : %d x %d  ", numberOfRows, numberOfColumns);
	this->_boardLabel->SetLabel(dummyLabel);
	dummyLabel = wxString::Format("Players : %d  ",player_no);
	this->_playerLabel->SetLabel(dummyLabel);

	//Display the control panel containing game information ...
	ShowGamePanel(true);
	//Switch the flag indicators for the start of a game ..
	gameInfo->game_started = 1;
	gameInfo->isOnline = false;
	gameInfo->game_ended = 0;

}
void AppGUIFrame::OnButtonTurnInfo(wxCommandEvent& event){
	int id = event.GetId();
	GameUtilities::GameState* gameInfo = (GameUtilities::GameState*)this->canvas_panel->GetClientData();
	if (!gameInfo) {
		wxLogDebug("[Chain-Reaction] Error: failed to start the game. Game information has not been generated yet ...");
		wxMessageBox(wxT("Failed to start the game. Game information has not been generated yet!"), wxT("Error!"), wxOK | wxICON_INFORMATION);
		return;
	}
	if (id == ID_INFO_BUTTON){ //This event was triggered in offline game mode.
		unsigned int player_no = this->_playerChoice->GetSelection() + 2;
		MainGame* offlineGameObject = (MainGame*)gameInfo->game;
		wxMenu infoMenu(wxT("Current order of players: "));
		if (!offlineGameObject) return;
		for (unsigned int i = 0; i < player_no; i++) {
			const auto& playerColor = this->_playerColorsWidgetList[i]->GetStringSelection();
			std::string lowerCasePlayerColor = playerColor.Lower().ToStdString();
			if (!offlineGameObject->isEliminated(lowerCasePlayerColor)) infoMenu.Append(wxID_ANY, playerColor);
		}
		this->_infoButton->PopupMenu(&infoMenu);
	}
}
void AppGUIFrame::OnButtonUndo(wxCommandEvent& event){
	GameUtilities::GameState* gameInfo = (GameUtilities::GameState*) this->canvas_panel->GetClientData();
	if (gameInfo) {
		MainGame* game = (MainGame*)gameInfo->game;
		if (game && !game->isBlastAnimationRunning()) game->undo();
	}
	else {
		wxLogDebug("[Chain-Reaction] Error: failed to start the game. Game information has not been generated yet ...");
		wxMessageBox(wxT("Failed to start the game. Game information has not been generated yet!"), wxT("Error!"), wxOK | wxICON_INFORMATION);
		return;
	}
}
void AppGUIFrame::OnButtonStartNew(wxCommandEvent& event){
	int result = wxMessageBox(wxT("New game will start. Continue?"),wxT("Confirmation"),wxNO_DEFAULT | wxYES_NO | wxICON_INFORMATION,this);
	if (result == wxNO) return;
	else{
		GameUtilities::GameState* gameInfo = (GameUtilities::GameState*) this->canvas_panel->GetClientData();
		if (gameInfo && gameInfo->game_started) gameInfo->new_game = 1;
	}
}
void AppGUIFrame::OnButtonMainMenu(wxCommandEvent& event){
	int result = wxMessageBox(wxT("Current game will be lost. Continue?"),wxT("Confirmation"),wxNO_DEFAULT | wxYES_NO | wxICON_INFORMATION,this);
	if (result == wxNO) return;
	else{
		GameUtilities::GameState* gameInfo = (GameUtilities::GameState*) this->canvas_panel->GetClientData();
		if (gameInfo && gameInfo->game_started) gameInfo->go_back_main = REQUESTED_IN_OFFLINE_MODE;
	}
}
void AppGUIFrame::OnButtonQuit(wxCommandEvent& event) {
	this->Destroy();
}
void AppGUIFrame::swapBuffers(void){
	this->canvas_panel->SwapBuffers();
}
void AppGUIFrame::UpdateTurnInGUI(unsigned int player){
	wxString playerLabel = wxString::Format("Player %d  ", player);
	this->_turnLabel->SetLabel(playerLabel);
	wxString playerColor = this->_playerColorsWidgetList[player-1]->GetStringSelection().Lower();
	this->_turnLabel->SetBackgroundColour(AppGUIFrame::_colorMap.at(playerColor));
	this->_gameSubsizerForTurnLabel->Layout();
	this->_turnLabelStatic->Update();
	this->_turnLabel->Update();
}
void AppGUIFrame::OnDropDown(wxCommandEvent& event){
	const int controlID = event.GetId();
	wxComboBox* controller = nullptr;
	bool isOnline = false;
	for (unsigned int i = 0; i < this->_playerColorsWidgetList.size(); i++) {
		if (controlID == ID_COLOR_CHOICE + i) {
			controller = this->_playerColorsWidgetList[i];
			break;
		}
	}
	if (!controller) return;
	wxString selectedColor = controller->GetStringSelection();
	controller->Clear();
	for (unsigned int i = 0; i < AppGUIFrame::_colorList.size(); i++) {
		bool isEntryValid = true;
		const wxString& colorEntry = AppGUIFrame::_colorList[i];
		for (unsigned int j = 0; j < this->_playerColorsWidgetList.size(); j++) {
			if (controlID == ID_COLOR_CHOICE + j) continue;
			const wxChoice* colorWidget = this->_playerColorsWidgetList[j];
			if (colorWidget->IsEnabled() && colorWidget->GetStringSelection() == colorEntry) {
				isEntryValid = false;
				break;
			}
		}
		if (!isEntryValid) continue;
		controller->Append(colorEntry);
	}
	controller->SetStringSelection(selectedColor);
}
void AppGUIFrame::OnPlayerSelection(wxCommandEvent& event){
	bool online = false;
	int id = event.GetId();
	unsigned int selectedOption = (unsigned int) event.GetSelection() + 2;
	if (!online){
		for(unsigned int i = 0; i < this->_playerColorsWidgetList.size(); i++){
			wxChoice *dummyWidget = this->_playerColorsWidgetList[i];
			if (i<selectedOption){
				if (!(dummyWidget->IsEnabled())) dummyWidget->Enable();
			}
			else{
				if (dummyWidget->IsEnabled()) dummyWidget->Disable();
			}
		}
	}
}
void AppGUIFrame::ShowGamePanel(bool state){
	if (!state){
		this->_topsizerForLeftWindow->Hide(this->_panelInsideGame);
		this->_topsizerForLeftWindow->Show(this->_panelOutsideGame);
		this->_topsizerForLeftWindow->Layout();
		this->_panelOutsideGame->Update();
	}
	else{
		this->_topsizerForLeftWindow->Hide(this->_panelOutsideGame);
		this->_topsizerForLeftWindow->Show(this->_panelInsideGame);
		this->_topsizerForLeftWindow->Layout();
		this->_panelInsideGame->Update();
	}
}
std::unordered_map<wxString,wxColor> AppGUIFrame::_createColormap(void){
	std::unordered_map<wxString,wxColor> temp;
	temp["green 1"] = wxColor(0,255,127);
	temp["grey"] = wxColor(170,170,170);
	temp["violet"] = wxColor(110,100,255);
	temp["green 2"] = wxColor(115,200,115);
	temp["brown"] = wxColor(180,80,50);
	temp["orange"] = wxColor(255,69,0);
	temp["pink"] = wxColor(255,105,180);
	temp["red"] = wxColor(255,0,0);
	temp["blue"] = wxColor(25,75,255);
	temp["green"] = wxColor(0,255,0);
	temp["yellow"] = wxColor(255,255,0);
	temp["cyan"] = wxColor(0,255,255);
	temp["purple"] = wxColor(148,0,211);
	temp["maroon"] = wxColor(179,46,92);
	temp["white"] = wxColor(255,255,255);
	return temp;
}
void AppGUIFrame::OnButtonRegisterOnline(wxCommandEvent& event){
	wxString url("https://");
	url.Append(REGISTRATION_HOSTNAME);
	wxLaunchDefaultBrowser(url);
}
