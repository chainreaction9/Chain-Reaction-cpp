#pragma once
#ifndef APPGUIFRAME_H
#define APPGUIFRAME_H

#include "PrecompiledHeader.h"
#include "InputDialogs.h"
#include "Shaders.h"
#include "ObjLoader.h"
#include "AppGLCanvas.h"

#define FPS_RATE 60.0 //default frame rate for rendering OpenGL.
#define REQUESTED_IN_OFFLINE_MODE 1
//*********************** These macro definitions are used only for the option of online play. ****************
constexpr auto REGISTRATION_HOSTNAME = "chainserver.pythonanywhere.com";

//********************************************************************************************************
/**
* @brief Control ID for wxWidget objects such as, button, dropdown menu, etc.
*/
enum{
	ID_ROW_CHOICE = wxID_HIGHEST + 1,
	ID_COL_CHOICE,
	ID_PLAYER_CHOICE,
	ID_INFO_BUTTON,
	ID_START_NEW_BUTTON,
	ID_MAIN_MENU,
	ID_REGISTER_ONLINE,
	ID_COLOR_CHOICE //This needs to have the highest ID and hence is positioned at the last.
};
/*
* @brief A derived class of 'wxFrame' that constructs the main GUI interface of the wxWidget application.
*/
class AppGUIFrame: public wxFrame{
public:
	/**
	* @brief Constructs the GUI interface of the App.
	*/
	AppGUIFrame(const wxString& title, const wxPoint& pos, const wxSize& size, const wxSize& minsize);
	/*
	* @brief Callback function for the 'Start Game' button press event.
	*/
	void OnButtonStart(wxCommandEvent& event);
	/*
	* @brief Callback function for the 'Move Order' button press event.
	*/
	void OnButtonTurnInfo(wxCommandEvent& event);
	/*
	* @brief Callback function for the 'Undo' button press event.
	*/
	void OnButtonUndo(wxCommandEvent& event);
	/*
	* @brief Callback function for the 'Start New Game' button press event.
	*/
	void OnButtonStartNew(wxCommandEvent& event);
	/*
	* @brief Callback function for the 'Main Menu' button press event.
	*/
	void OnButtonMainMenu(wxCommandEvent& event);
	/*
	* @brief Callback function for the 'Quit' button press event.
	*/
	void OnButtonQuit(wxCommandEvent& event);
	/*
	* @brief Callback function that is invoked before a dropdown menu containing color information of a player appears.
	* It updates the available options for choosing a color that is not already taken by other players.
	*/
	void OnDropDown(wxCommandEvent& event);
	/*
	* @brief Callback function that is invoked after a choice for the 'Number of Players' is made.
	* It toggles the state (enabled/disabled) of the dropdown menus designated for choosing color of a player.
	*/
	void OnPlayerSelection(wxCommandEvent& event);
	/*@brief Updates the current player in the GUI interface.*/
	void UpdateTurnInGUI(unsigned int player = 2);
	/*
	* @brief Swaps frame buffers of the wxGLCanvas (OpenGL) instance.
	*/
	void swapBuffers(void);
	/*
	* @brief Toggles the visibility status of the inside-game-menu and outside-game-menu.
	*/
	void ShowGamePanel(bool state);
	/*
	* @brief Callback function for the 'Register Online' button press event.
	* It opens a webpage (in the default browser) where the online version of the game can be played.
	*/
	void OnButtonRegisterOnline(wxCommandEvent& event);
	//An AppGLCanvas instance derived from the baseclass wxGLCanvas, which serves as the main canvas for OpenGL rendering.
	AppGLCanvas *canvas_panel;
	//An instance of wxMenu to display the current order of players in the game.
	wxMenu *info_menu;

private:
	static const unsigned int _minimumNumberOfRows, _minimumNumberOfColumns, _maximumNumberOfRows, _maximumNumberOfColumns, _maximumNumberOfPlayers;
	static const wxString _playerList[];
	std::vector<wxArrayString> _playerColors;
	wxComboBox *_rowChoice, *_columnChoice, *_playerChoice;
	wxStaticText *_boardLabel, *_playerLabel, *_turnLabel, *_turnLabelStatic;
	wxButton* _infoButton;
	wxBoxSizer *_gameSubsizerForTurnLabel, *_topsizerForLeftWindow;
	wxPanel *_panelOutsideGame, *_panelInsideGame;
	std::vector<wxComboBox*> _playerColorsWidgetList;
	static const std::vector<wxString> _colorList;
	//An unordered map that points each color name to its corresponding RGB value.
	static const std::unordered_map<wxString,wxColor> _colorMap;
	/*
	* @brief Assigns RGB values to each color string.
	* @returns {unordered_map<wxString,wxColor>} An unordered map that points each color name to its corresponding RGB value.
	*/
	static std::unordered_map<wxString,wxColor> _createColormap(void);
};
#endif