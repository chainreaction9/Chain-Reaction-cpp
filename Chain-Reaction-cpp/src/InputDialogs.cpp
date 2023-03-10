#include "PrecompiledHeader.h" //precompiled header
#include "InputDialogs.h"

loginDetailsInput::loginDetailsInput(wxWindow *parent, const wxString& title, const wxString& question_lab, const wxString& answer_lab, std::vector<wxString>& destn)  : wxDialog(parent,wxID_ANY,title) {

	top_panel = new wxPanel(this);
	top_sizer = new wxBoxSizer(wxVERTICAL);
	top_panel->SetSizer(top_sizer);

	btn_panel = new wxPanel(top_panel);
	btn_sizer = new wxBoxSizer(wxHORIZONTAL);
	btn_panel->SetSizer(btn_sizer);
	btn_ok = new wxButton(btn_panel,wxID_OK,"OK");
	btn_sizer->Add(btn_ok,1,wxEXPAND | wxALL, 5);
	btn_cancel = new wxButton(btn_panel,wxID_CANCEL,"Cancel");
	btn_sizer->Add(btn_cancel,1,wxEXPAND | wxALL,5);
	btn_sizer->FitInside(btn_panel);

	ques_panel = new wxPanel(top_panel);
	ques_sizer = new wxBoxSizer(wxHORIZONTAL);
	ques_panel->SetSizer(ques_sizer);
	wxStaticText* labl1 = new wxStaticText(ques_panel,wxID_ANY,question_lab);
	ques_sizer->Add(labl1,1,wxALL,5);
	wxTextCtrl* question = new wxTextCtrl(ques_panel,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxTE_PROCESS_ENTER);
	question->Bind(wxEVT_COMMAND_TEXT_ENTER,&loginDetailsInput::OnEnter,this);
	ques_sizer->Add(question,3,wxEXPAND | wxALL,5);
	ques_sizer->FitInside(ques_panel);
	top_sizer->Add(ques_panel,1,wxEXPAND);

	ans_panel = new wxPanel(top_panel);
	ans_sizer = new wxBoxSizer(wxHORIZONTAL);
	ans_panel->SetSizer(ans_sizer);
	wxStaticText* labl2 = new wxStaticText(ans_panel,wxID_ANY,answer_lab);
	ans_sizer->Add(labl2,1,wxALL,5);
	pass_text = new wxTextCtrl(ans_panel,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxTE_PASSWORD | wxTE_PROCESS_ENTER);
	pass_text->Bind(wxEVT_COMMAND_TEXT_ENTER,&loginDetailsInput::OnEnter,this);
	ans_sizer->Add(pass_text,3,wxEXPAND | wxALL,5);
	plain_text = new wxTextCtrl(ans_panel,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxTE_PROCESS_ENTER);
	plain_text->Bind(wxEVT_COMMAND_TEXT_ENTER,&loginDetailsInput::OnEnter,this);
	ans_sizer->Add(plain_text,3,wxEXPAND | wxALL,5);
	ans_sizer->Hide(plain_text);
	ans_sizer->FitInside(ans_panel);
	top_sizer->Add(ans_panel,1,wxEXPAND);

	check_btn = new wxCheckBox(top_panel,check_id,"Show entries");
	check_btn->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED,&loginDetailsInput::OnCheck,this);
	top_sizer->Add(check_btn,1,wxALL,5);

	top_sizer->Add(btn_panel,1,wxEXPAND);
	top_sizer->Fit(this);
	question->SetFocus();
	
	Centre();
	int id = this->ShowModal();
	if (id !=wxID_CANCEL){
		destn.push_back(question->GetValue());
		if (check_btn->IsChecked()) destn.push_back(plain_text->GetValue());
		else destn.push_back(pass_text->GetValue());
	}
	this->Destroy();
}

void loginDetailsInput::OnEnter(wxCommandEvent& event){
	wxCommandEvent ev(wxEVT_COMMAND_BUTTON_CLICKED,wxID_OK);
	btn_ok->GetEventHandler()->ProcessEvent(ev);
}

void loginDetailsInput::OnCheck(wxCommandEvent& event){
	if (this->check_btn->IsChecked()) {
		wxString val = pass_text->GetValue();
		long pos = pass_text->GetInsertionPoint();
		ans_sizer->Hide(pass_text);
		plain_text->SetValue(val);
		plain_text->SetFocus();
		plain_text->SetInsertionPoint(pos);
		ans_sizer->Show(plain_text);
	}
	else{
		wxString val = plain_text->GetValue();
		long pos = plain_text->GetInsertionPoint();
		ans_sizer->Hide(plain_text);
		pass_text->SetValue(val);
		pass_text->SetFocus();
		pass_text->SetInsertionPoint(pos);
		ans_sizer->Show(pass_text);
	}
	ans_sizer->Layout();
}

loginDetailsInput::~loginDetailsInput(void)
{
}

generalInput::generalInput(wxWindow *parent, const wxString& title, const std::vector<wxString>& question_lab, std::vector<wxString>& destn) : wxDialog(parent, wxID_ANY, title) {
	_top_panel = new wxPanel(this);
	_top_sizer = new wxBoxSizer(wxVERTICAL);
	_top_panel->SetSizer(_top_sizer);
	_btn_panel = new wxPanel(_top_panel);
	_btn_sizer = new wxBoxSizer(wxHORIZONTAL);
	_btn_panel->SetSizer(_btn_sizer);
	_btnOk = new wxButton(_btn_panel, wxID_OK, "OK");
	_btn_sizer->Add(_btnOk, 1, wxEXPAND | wxALL, 5);
	_btnCancel = new wxButton(_btn_panel, wxID_CANCEL, "Cancel");
	_btn_sizer->Add(_btnCancel, 1, wxEXPAND | wxALL, 5);
	_btn_sizer->FitInside(_btn_panel);

	unsigned int numQuestions = question_lab.size();
	for (unsigned int i = 0; i < numQuestions; i++) {
		wxPanel *quesPanel = new wxPanel(_top_panel);
		wxBoxSizer* quesSizer = new wxBoxSizer(wxHORIZONTAL);
		wxStaticText* quesLabel = new wxStaticText(quesPanel, wxID_ANY, question_lab.at(i));
		quesPanel->SetSizer(quesSizer);
		this->_quesPanelList.push_back(quesPanel);
		this->_quesPanelSizerList.push_back(quesSizer);
		quesSizer->Add(quesLabel, 1, wxALL, 5);
		wxTextCtrl* question = new wxTextCtrl(quesPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
		question->Bind(wxEVT_COMMAND_TEXT_ENTER, &generalInput::OnEnter, this);
		_textControlList.push_back(question);
		quesSizer->Add(question, 1, wxEXPAND | wxALL, 5);
		quesSizer->FitInside(quesPanel);
		_top_sizer->Add(quesPanel, 1, wxEXPAND);
	}

	_top_sizer->Add(_btn_panel, 1, wxEXPAND);
	_top_sizer->Fit(this);
	_textControlList[0]->SetFocus();

	Centre();
	int id = this->ShowModal();
	if (id != wxID_CANCEL) {
		for (auto question : _textControlList)
		destn.push_back(question->GetValue());
	}
	this->Destroy();
}
generalInput::~generalInput(void){
}
void generalInput::OnEnter(wxCommandEvent& event){
	wxCommandEvent ev(wxEVT_COMMAND_BUTTON_CLICKED, wxID_OK);
	_btnOk->GetEventHandler()->ProcessEvent(ev);
}