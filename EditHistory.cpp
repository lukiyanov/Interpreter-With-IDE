//---------------------------------------------------------------------------
#pragma hdrstop
#include "EditHistory.h"
#include "UnitMainForm.h"
#include "UEditorSettings.h"
#pragma package(smart_init)

//---------------------------------------------------------------------------
CEditHistory::CEditHistory(): nUndoCnt(-1) {}

void CEditHistory::Undo(TCustomMemo* edit) {
	if (CanUndo()) {
		nUndoCnt--;
		TNotifyEvent p = fmMainView->Memo2->OnChange;
		fmMainView->Memo2->OnChange = 0;

		edit->Text	    = dquState[nUndoCnt].strText;
		edit->SelStart 	= dquState[nUndoCnt].nSelStart;
		edit->SelLength = dquState[nUndoCnt].nSelLength;

		fmMainView->Memo2->OnChange = p;
	}
}

void CEditHistory::Redo(TCustomMemo* edit) {
	if (CanRedo()) {
		nUndoCnt++;

		TNotifyEvent p = fmMainView->Memo2->OnChange;
		fmMainView->Memo2->OnChange = 0;

		edit->Text	    = dquState[nUndoCnt].strText;
		edit->SelStart 	= dquState[nUndoCnt].nSelStart;
		edit->SelLength = dquState[nUndoCnt].nSelLength;

		fmMainView->Memo2->OnChange = p;
	}
}

bool CEditHistory::CanUndo() {
	if (fmEditorSettings->CheckBox4->Checked
	&& nUndoCnt > 0)
		return 1;
	return 0;
}

bool CEditHistory::CanRedo() {
	if (fmEditorSettings->CheckBox4->Checked
	&& nUndoCnt < (dquState.size()-1))
		return 1;
	return 0;
}

void CEditHistory::AddState(TCustomMemo* edit) {
	if (!fmEditorSettings->CheckBox4->Checked)
		return;

	if (dquState.size() >= (fmEditorSettings->CSpinEdit1->Value + 1))
		dquState.pop_front();

	dquState.push_back(CEditState(edit->Text, edit->SelStart, edit->SelLength));
	nUndoCnt = dquState.size()-1;
}

void CEditHistory::Clear() {
	nUndoCnt = -1;
	dquState.clear();
	dquState.push_back(CEditState(fmMainView->Memo2->Text,
	 fmMainView->Memo2->SelStart, fmMainView->Memo2->SelLength));
}
//---------------------------------------------------------------------------
CEditHistory ehHistoryManager;
//---------------------------------------------------------------------------

