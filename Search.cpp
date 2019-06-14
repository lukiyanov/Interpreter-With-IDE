//---------------------------------------------------------------------------
#pragma hdrstop
#include "Search.h"
#include "UnitMainForm.h"
#pragma package(smart_init)
#include <string>
using namespace std;

//---------------------------------------------------------------------------
CSearch mSearch;

//---------------------------------------------------------------------------
void CSearch::SearchInit() {

	// Копия для поиска
	string text; // Текст
	string sstr; // Подстрока

	// Последствия флажка "С учетом регистра"
	if (1) {
		text = fmMainView->Memo2->Lines->Text.UpperCase().c_str();
		sstr = fmMainView->FindDialog1->FindTextA.UpperCase().c_str();
	}
	else {
		text = fmMainView->Memo2->Lines->Text.c_str();
		sstr = fmMainView->FindDialog1->FindTextA.c_str();
	}

	// Ищем...
	int		nLastPos;
	if (fmMainView->FindDialog1->Options.Contains(frDown))
		nLastPos = text.find(sstr, fmMainView->Memo2->SelStart+1);
	else
		nLastPos = text.rfind(sstr, fmMainView->Memo2->SelStart-1);


	if (nLastPos != string::npos) {
		fmMainView->Memo2->SelStart = nLastPos-1;
		fmMainView->Memo2->SelLength = fmMainView->FindDialog1->FindTextA.Length();
		ShowWindow("Алгоритм", 1);
		fmMainView->Memo2->SetFocus();
	}
	else
		MessageBox(fmMainView->Handle,
		(String("Фрагмент \"")+fmMainView->FindDialog1->FindTextA+"\" не найден").c_str(), "Поиск", MB_ICONINFORMATION);
}

//---------------------------------------------------------------------------
void CSearch::SearchAgain() {
//	String text = fmMainView->Memo2->Lines->Text.UpperCase();

}
//---------------------------------------------------------------------------

