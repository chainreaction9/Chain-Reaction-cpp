#pragma once
#ifndef INPUT_DIALOGS_H

#include "PrecompiledHeader.h" //precompiled header

class loginDetailsInput : public wxDialog {

public:
	loginDetailsInput(wxWindow* parent, const wxString& title, const wxString& question_lab, const wxString& answer_lab, std::vector<wxString>& destn);
	~loginDetailsInput(void);

private:

	enum { check_id = wxID_HIGHEST + 1 };
	wxPanel* top_panel, * btn_panel, * ques_panel, * ans_panel;
	wxBoxSizer* top_sizer, * btn_sizer, * ques_sizer, * ans_sizer;
	wxButton* btn_ok, * btn_cancel;
	wxCheckBox* check_btn;
	wxTextCtrl* pass_text, * plain_text;

	void OnEnter(wxCommandEvent& event);
	void OnCheck(wxCommandEvent& event);
};

class generalInput : public wxDialog {

public:
	generalInput(wxWindow* parent, const wxString& title, const std::vector<wxString>& question_lab, std::vector<wxString>& destn);
	~generalInput(void);

private:
	wxPanel* _top_panel, * _btn_panel;
	std::vector<wxPanel*> _quesPanelList;
	wxBoxSizer* _top_sizer, * _btn_sizer;
	std::vector<wxBoxSizer*> _quesPanelSizerList;
	std::vector<wxTextCtrl*> _textControlList;
	wxButton* _btnOk, * _btnCancel;

	void OnEnter(wxCommandEvent& event);
};

#define INPUT_DIALOGS_H
#endif //!INPUT_DIALOGS_H