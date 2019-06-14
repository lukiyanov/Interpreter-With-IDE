//$$---- Form CPP ----
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UConsole.h"
#include "URunParams.h"
#include "Trace.h"
#include "Errors.h"
#include "Algorithm.h"
#include "UConsoleSettings.h"
#include "UnitMainForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfmConsole *fmConsole;
//---------------------------------------------------------------------------
__fastcall TfmConsole::TfmConsole(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfmConsole::FormShow(TObject *Sender)
{
	Memo1->Color = fmConsoleSettings->ColorBox1->Selected;
	Edit1->Color = fmConsoleSettings->ColorBox1->Selected;
	Memo1->Font->Color = fmConsoleSettings->ColorBox2->Selected;
	Edit1->Font->Color = fmConsoleSettings->ColorBox2->Selected;

	if (fmConsoleSettings->CheckBox2->Checked)
		Memo1->ScrollBars = ssBoth;
	else
		Memo1->ScrollBars = ssNone;
	Edit1->SetFocus();
	Edit1->Clear();

	for (int i = 0; i < Memo1->Lines->Count; i++)
		PostMessage(Memo1->Handle, WM_KEYDOWN, 40, 0);
}
//---------------------------------------------------------------------------

void __fastcall TfmConsole::Memo1Enter(TObject *Sender)
{
	Edit1->SetFocus();
}

//---------------------------------------------------------------------------

inline bool IsBoolConst(string str) {
	if (str == "ИСТИНА" || str == "ЛОЖЬ")
		return 1;
	return 0;
}

inline bool IsIntConst(string str) {
	try {
		StrToInt(str.c_str());
	} catch (...) {
		return 0;
	}
	return 1;
}

/* вспомогательная f(), '.' -> ',' для FloatToStr()*/
inline string AlgFloatToBrlnd(const string& str) {
	string retn(str);
	for (unsigned i = 0; i < str.size(); i++) {
		if (retn[i] == '.') {
			retn[i] = ',';
			break;
		}
	}
	return retn;
}

inline bool IsDoubleConst(string str) {
	if (str == ",")
		return 0;
	try {
		StrToFloat(AlgFloatToBrlnd(str).c_str());
	} catch (...) {
		return 0;
	}
	return 1;
}

//---------------------------------------------------------------------------

bool __fastcall TfmConsole::Read(TVariable& var) { // 1-OK, 0-FAILED
	if (m_str == "\n")
		throw RuntimeError("Ввод данных и выполнение алгоритма прерваны пользователем");

	switch (var.varType) {
	case vtVoid:
		DebugTrace("ВНИМАНИЕ! Произошло считывание переменной, имеющей тип НЕТ",always);
		return 1;
	case vtInt:
		if (!IsIntConst(m_str)) {
			Memo1->Lines->Add("Неверный ввод! Введите значение типа ЦЕЛ.");
			fmMainView->Memo4->Lines->Add("Неверный ввод! Введите значение типа ЦЕЛ.");
			return 0;
		}

		var.bDynamicInit = 1;
		var.Value.intValue = StrToInt(m_str.c_str());
		return 1;

	case vtDouble:
		if (!IsDoubleConst(m_str)) {
			Memo1->Lines->Add("Неверный ввод! Введите значение типа ВЕЩ.");
			fmMainView->Memo4->Lines->Add("Неверный ввод! Введите значение типа ВЕЩ.");
			return 0;
		}

		var.bDynamicInit = 1;
		var.Value.dblValue = StrToFloat(AlgFloatToBrlnd(m_str).c_str());
		return 1;

	case vtBool:
		m_str = String(m_str.c_str()).UpperCase().c_str();
		if (!IsBoolConst(m_str)) {
			Memo1->Lines->Add("Неверный ввод! Введите значение типа ЛОГ.");
			fmMainView->Memo4->Lines->Add("Неверный ввод! Введите значение типа ЛОГ.");
			return 0;
		}

		var.bDynamicInit = 1;
		var.Value.blnValue = (m_str == "ИСТИНА");
		return 1;

	case ::vtChar:
		if (m_str.size() != 1) {
		Memo1->Lines->Add("Неверный ввод! Введите значение типа СИМ.");
		fmMainView->Memo4->Lines->Add("Неверный ввод! Введите значение типа СИМ.");
			return 0;
		}

		var.bDynamicInit = 1;
		var.Value.chrValue = m_str[0];
		return 1;

	case ::vtString:
		var.bDynamicInit = 1;
		var.Value.strValue = m_str;
		return 1;
	}
}

//---------------------------------------------------------------------------

void __fastcall TfmConsole::ReadLn(TVariable& var) {
	fmConsole->Memo1->Lines->Add("");
	fmMainView->Memo4->Lines->Add("");
	Memo1->Lines->Add(String("Введите значение переменной \'")+
	var.GetStrName().c_str()+"\' типа "+var.GetStrType().c_str());
	fmMainView->Memo4->Lines->Add(String("Введите значение переменной \'")+
	var.GetStrName().c_str()+"\' типа "+var.GetStrType().c_str());

	if (var.varType != vtArray && var.varType != vtMatrix) {
		do {
			ShowModal ();
			Memo1->Lines->Add(m_str.c_str());
			fmMainView->Memo4->Lines->Add(m_str.c_str());
		} while (!Read(var));
	}

	else if (var.varType == vtArray) {
		for (int i = 1; i <= var.Value.arrValue.nColCnt; i++) {
			do {
				Memo1->Lines->Add(String("Введите ")+var.GetStrName().c_str()+
				"["+IntToStr(i)+"]:");
				fmMainView->Memo4->Lines->Add(String("Введите ")+var.GetStrName().c_str()+
				"["+IntToStr(i)+"]:");
				ShowModal();
				Memo1->Lines->Add(m_str.c_str());
				fmMainView->Memo4->Lines->Add(m_str.c_str());
			} while (!Read(var.Value.arrValue.arr[i-1]));
		}
		var.bDynamicInit = 1;
	}

	else /* var.varType == vtMatrix */ {
		for (int i = 1; i <= var.Value.mtxValue.nRowCnt; i++) {
			for (int j = 1; j <= var.Value.mtxValue.nColCnt; j++) {
				do {
					Memo1->Lines->Add(String("Введите ")+var.GetStrName().c_str()+
					"["+IntToStr(i)+"]["+IntToStr(j)+"]:");
					fmMainView->Memo4->Lines->Add(String("Введите ")+var.GetStrName().c_str()+
					"["+IntToStr(i)+"]["+IntToStr(j)+"]:");
					ShowModal();
					Memo1->Lines->Add(m_str.c_str());
					fmMainView->Memo4->Lines->Add(m_str.c_str());
				} while (!Read(var.Value.mtxValue.mtx[i-1][j-1]));
			}
		}
		var.bDynamicInit = 1;
	}

}

//---------------------------------------------------------------------------

void __fastcall TfmConsole::WriteVar(const TVariable& var) {
	int i,j;
	switch (var.varType) {
	case vtVoid:
		return;
	case vtInt:
	case vtDouble:
	case vtBool:
		Memo1->Lines->Strings[Memo1->Lines->Count-1] = Memo1->Lines->Strings[Memo1->Lines->Count-1]+var.GetStrValue().c_str();
		break;

	case ::vtString:
		Memo1->Lines->Strings[Memo1->Lines->Count-1] = Memo1->Lines->Strings[Memo1->Lines->Count-1]+var.Value.strValue.c_str();
		break;

	case ::vtChar:
		Memo1->Lines->Strings[Memo1->Lines->Count-1] = Memo1->Lines->Strings[Memo1->Lines->Count-1]+String()+var.Value.chrValue;
		break;

	case vtArray:
		for (i = 1; i <= var.Value.arrValue.nColCnt; i++) {
			Memo1->Lines->Add(String(var.GetStrName().c_str())+
			"["+IntToStr(i)+"] = "+var.Value.arrValue.arr[i-1].GetStrValue().c_str());
			fmMainView->Memo4->Lines->Add(String(var.GetStrName().c_str())+
			"["+IntToStr(i)+"] = "+var.Value.arrValue.arr[i-1].GetStrValue().c_str());
		}
		break;

	case vtMatrix:
		for (i = 1; i <= var.Value.mtxValue.nRowCnt; i++) {
			for (j = 1; j <= var.Value.mtxValue.nColCnt; j++) {
				Memo1->Lines->Add(String(var.GetStrName().c_str())+
				"["+IntToStr(i)+"]["+IntToStr(j)+"] = "+var.Value.mtxValue.mtx[i-1][j-1].GetStrValue().c_str());
				fmMainView->Memo4->Lines->Add(String(var.GetStrName().c_str())+
				"["+IntToStr(i)+"]["+IntToStr(j)+"] = "+var.Value.mtxValue.mtx[i-1][j-1].GetStrValue().c_str());
			}
		}
		break;
	}
}

//---------------------------------------------------------------------------

void __fastcall TfmConsole::WriteLn(const TVariable& var) {
	int i,j;
	switch (var.varType) {
	case vtVoid:
		return;
	case vtInt:
	case vtDouble:
	case ::vtString:
	case ::vtChar:
	case vtBool:
		Memo1->Lines->Add(String("Значение переменной \'")+
		var.GetStrName().c_str()+
		"\' равно: "+var.GetStrValue().c_str());
		fmMainView->Memo4->Lines->Add(String("Значение переменной \'")+
		var.GetStrName().c_str()+
		"\' равно: "+var.GetStrValue().c_str());
		break;
	case vtArray:
		Memo1->Lines->Add(String("Значение переменной \'")+
		var.GetStrName().c_str()+
		"\' равно: ");

		fmMainView->Memo4->Lines->Add(String("Значение переменной \'")+
		var.GetStrName().c_str()+
		"\' равно: ");

		for (i = 1; i <= var.Value.arrValue.nColCnt; i++) {
			Memo1->Lines->Add(String(var.GetStrName().c_str())+
			"["+IntToStr(i)+"] = "+var.Value.arrValue.arr[i-1].GetStrValue().c_str());
			fmMainView->Memo4->Lines->Add(String(var.GetStrName().c_str())+
			"["+IntToStr(i)+"] = "+var.Value.arrValue.arr[i-1].GetStrValue().c_str());
		}
		break;

	case vtMatrix:
		Memo1->Lines->Add(String("Значение переменной \'")+
		var.GetStrName().c_str()+
		"\' равно: ");
		fmMainView->Memo4->Lines->Add(String("Значение переменной \'")+
		var.GetStrName().c_str()+
		"\' равно: ");

		for (i = 1; i <= var.Value.mtxValue.nRowCnt; i++) {
			for (j = 1; j <= var.Value.mtxValue.nColCnt; j++) {
				Memo1->Lines->Add(String(var.GetStrName().c_str())+
				"["+IntToStr(i)+"]["+IntToStr(j)+"] = "+var.Value.mtxValue.mtx[i-1][j-1].GetStrValue().c_str());
				fmMainView->Memo4->Lines->Add(String(var.GetStrName().c_str())+
				"["+IntToStr(i)+"]["+IntToStr(j)+"] = "+var.Value.mtxValue.mtx[i-1][j-1].GetStrValue().c_str());
			}
		}
		break;
	}

	ShowModal ();
}
//---------------------------------------------------------------------------


void __fastcall TfmConsole::Edit1KeyDown(TObject *Sender, WORD &Key,
	  TShiftState Shift)
{
	if ( Key == 13) {
		m_str = Edit1->Text.c_str();
		Close();
	}
}
//---------------------------------------------------------------------------


void __fastcall TfmConsole::SpeedButton1Click(TObject *Sender)
{
	m_str = "\n";
	Close();
}
//---------------------------------------------------------------------------



void __fastcall TfmConsole::WriteExc(const TVariable& var, TAlgorithm* pAlg) {
	switch (var.varType) {
	case vtVoid:
		return;
	case vtInt:
	case vtDouble:
	case ::vtString:
	case ::vtChar:
	case vtBool:
		Memo1->Lines->Add(String("Во время выполнения алгоритма ")+pAlg->GetName().c_str()+" возникло исключение "+
		var.GetStrName().c_str()+":");
		fmMainView->Memo4->Lines->Add(String("Во время выполнения алгоритма ")+pAlg->GetName().c_str()+" возникло исключение "+
		var.GetStrName().c_str()+":");

		Memo1->Lines->Add(var.GetStrValue().c_str());
		fmMainView->Memo4->Lines->Add(var.GetStrValue().c_str());
		break;
	}

	ShowModal ();
}
void __fastcall TfmConsole::ApplicationEvents1Message(tagMSG &Msg,
	  bool &Handled)
{
	if (Msg.message == WM_KEYDOWN && Msg.wParam == 27) {
		m_str = "\n";
		Close();
	}
}
//---------------------------------------------------------------------------

