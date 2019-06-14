//---------------------------------------------------------------------------
#pragma hdrstop

#include <windows.h>
#include <vcl.h>
#include "Emulator.h"
#include "Trace.h"
#include "URunParams.h"
#include "UConsole.h"
#include "StdAlgorithms.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
TEmulator Emulator;   // DebugTrace("");
const char szLegalChars[] = " \t0987654321,._+-*/><=:;()[]abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ�����Ũ����������������������������������������������������������";
const char szDividers[]	  = " \t<>=()[]+-*/=:;,";
const int  nDblLecsQwo = 5;
const char aszDblLecs[nDblLecsQwo][2] = {":=","<>",">=","<=","**"};
//---------------------------------------------------------------------------

TEmulator::TEmulator() {
	esState = esStoped;
	bNeedToStop = 0;
}

//---------------------------------------------------------------------------
String TabsToSpacesForString(String str) {
	String result;
	for (int i = 1; i <= str.Length(); i++)
		if (str[i] == '\t')
			result += "    ";
		else
			result += str[i];
	return result;
}
//---------------------------------------------------------------------------
void TabsToSpaces(vector<string>& vct) {
	for (int i = 0; i < vct.size(); i++)
		for (int j = 0; j < vct[i].size(); j++)
			if (vct[i][j] == '\t') vct[i][j] = ' ';
}
//---------------------------------------------------------------------------

void UpperCase(vector<string>& vct) {
	bool bIsString;
	char ch;
	for (int i = 0; i < vct.size(); i++) {
		bIsString = 0;
		for (int j = 0; j < vct[i].size(); j++) {
			if (vct[i][j]=='\'') bIsString = !bIsString;
			if (!bIsString){
				ch = String(vct[i][j]).UpperCase().c_str()[0];
				vct[i][j] = ch;
			}
		}
	}
}

//---------------------------------------------------------------------------

TAlgorithm* TEmulator::GetAlgorithmPointer(string strName) {
	for (unsigned i = 0; i < algs.size(); i++) {
		if ((algs[i]->GetName() == strName))
			return algs[i];
	}
	return 0;
}

//---------------------------------------------------------------------------
vector<string> DeleteComment(const vector<string>& vct) { // DONE
	DebugTrace(">> �������� ������������...", always);
	vector<string> result;
	string str;
	bool bIsString;
	for (int i = 0; i < vct.size(); i++) {
		str.clear();
		bIsString = 0;
		for (int j = 0; j < vct[i].size(); j++) {
			if (vct[i][j]=='\'') bIsString = !bIsString;	// ���������: ������
			if (vct[i][j]=='|' && !bIsString) break;
			str+=vct[i][j];
		}
		result.push_back(str);
	}

	DebugTrace(">> ���������. ���������� ��������:");
	DebugTrace("========================================================");
	for (int i = 0; i < result.size(); i++) {
		DebugTrace(string(IntToStr(i+1).c_str())+". "+result[i]);
	}
	DebugTrace("========================================================");

	return result;
}

//---------------------------------------------------------------------------

bool TEmulator::IsAlgPresent(string strAlgName) {
	for (int i = 0; i < algs.size(); i++)
    	if (algs[i]->GetName() == strAlgName) return 1;
	return 0;
}

//---------------------------------------------------------------------------
// �����������?
bool IsDividerSym(char ch) {
	for (int i = 0; szDividers[i]!='\0'; i++)
		if (szDividers[i]==ch) return 1;
	return 0;
}

// ���������� ������ (������ �� � ����)?
bool IsLegalSym(char ch) {
	for (int i = 0; szLegalChars[i]!='\0'; i++)
		if (szLegalChars[i]==ch) return 1;
	return 0;
}

// ��� ��� ������� - ���� �������?
// ��� ����������� ������ ���� :=, <>, >=, <=, **
bool IsDblLecs(char first, char last) {
	for (int i = 0; i < nDblLecsQwo; i++) {
			if (aszDblLecs[i][0]==first && aszDblLecs[i][1]==last)
			return 1;
	}
	return 0;
}

// ��������� ������ �� �������
vector<string> GetLecsems(string line, int lineNum){
	vector<string> vstr;
	bool bIsString = 0;
	string lecsem;
	for (int i = 0; i < line.size(); i++) {
		if (bIsString) {            // ���� ����� ���� ������� ������...
			lecsem+=line[i];       		// ���������� � ���� line[i] == '\'' � ���� ���
			if (line[i]=='\'') {    	// ���� �������� ������...
				bIsString = 0;          	// ���������� ������� ������
				vstr.push_back(lecsem); 	// ��������� ������� �������
				lecsem.clear();         	// ������� ����� �������
			}
			continue;                   // ���������� ��������� �� ����������,
										// ��� �� ���������� �� else{...}
		}
									// ����������: ���� �� �����, �� ������ �� �������
		if (line[i]=='\'') {        // ���� ����������� ������...
			bIsString = 1;          // ������������� ������� ������

			if (!lecsem.empty()) {  //	���� �� ������ �� ���-�� ������ � �����...
				vstr.push_back(lecsem); 	// ��������� �������
				lecsem.clear();         	// ������� ����� �������
			}

			lecsem+=line[i];        // ������ � ����� '\''
			continue;                   // ���������� ��������� �� ����������,
										// ��� �� ���������� �� ��� ������ else{...}
		}
									// ����������: ���� �� �����, �� ���� ���������� �� � ������

		if (!IsLegalSym(line[i]))   // ���-�� ���������� ���� ����������...
			throw SyntaxError(string("������ ������������ ������: \'")+line[i]+'\'',lineNum);

		if (IsDividerSym(line[i])){ // ���� ���������� �����������

			// ��������� ������� ������ BEGIN
			if ( (!lecsem.empty()) && IsDblLecs(lecsem[0],line[i])) { // ��� ���������� ������, ����������� ����� ���� � lecsem ������ ����
				vstr.push_back(lecsem+line[i]);
				lecsem.clear();
				continue;
			}
			// ��������� ������� ������ END


			if (!lecsem.empty())
				vstr.push_back(lecsem); // ����������� � ������ ��, ��� � ��� ���� � ������,
										// ���� �� �� ����
			lecsem.clear();
			if (line[i]!=' ') {     // ������� �� ��������� � vstr
				lecsem = line[i];   	// ������ �����������-����������
			}
			continue;               // ���������� �� ���������
		}

		if ((!lecsem.empty()) && IsDividerSym(lecsem[0])) { // ���� ����� ����������� ���� �� �����������
			vstr.push_back(lecsem);
			lecsem.clear();
			// �� ���� continue!
		}

		lecsem+=line[i];			// ���� �� �����������, ��������� ��������� ������ � �����
	}

	if (!lecsem.empty())     		// �� ������, ���� ��������� ������ ������ - �� �����������
		vstr.push_back(lecsem);

	if (bIsString)					// ���� ������ �� ���� �������...
		throw SyntaxError   			// ���������� ����������
		(string("������� ���������� ������ (")+lecsem+"), ��������� ������ \' � ����� ������",lineNum);
									// ��� ��� �����
	return vstr;
}

vector< vector<string> > DivToLecsems(const vector<string>& vct) {
	DebugTrace(">> ��������� �� �������...", always);

	vector< vector<string> > vvstr;
	for (int i = 0; i < vct.size(); i++) {
		vvstr.push_back(GetLecsems(vct[i],i));
	}


	DebugTrace(">> ���������. ��������� ���������:");
	DebugTrace("========================================================");
	string str;
	for (int i = 0; i < vvstr.size(); i++) {
		str = (IntToStr(i+1)+". ").c_str();
		for (int j = 0; j < vvstr[i].size(); j++) {
			str += "\'";
			str += vvstr[i][j];
			str += "\'  ";
		}
		DebugTrace(str.c_str());
	}
	DebugTrace("========================================================");
	return vvstr;
}


/////////////////////////////////////////////////////////////////////////////
// FILL SESSION
void TEmulator::FillSession(vector<string>& vct) {
	bNeedToStop = 0;
	DebugTrace("_______________________________________ ����� ������ ������� _______________________________________");
	DebugTrace("***>> ���������� � ��������/����������, ���� ���������� ��������... <<***", always);
	UpperCase(vct);
	TabsToSpaces(vct);
	vct = DeleteComment(vct);
	LecsemManager lecsGen(DivToLecsems(vct));
	DebugTrace(">> ��������� ������ ��������������� � ����� � ������");

	if (!lecsGen.IsLecsPresents())
		throw LogicalError("�� ������� �� ������ ������������ �������� � �������� ���������, ������� ��������",0);

	#ifndef ALLOW_MEMORY_LOOSE
	// ���� ������� ������ ����� ����� ���������� Run(),
	// �� ������ ����� ������������� �������� ���������� ����� �������.
	// �������� ��������� ����� � ������ ������ ��� ((������ FillSession())-1).
	// �������� ������ ����� ��� ���� �� ����� ��������� �
	// ����������� ������������ ������ Algorithm.
	for (int i = 0; i < algs.size(); i++)
		if (!algs[i]->bStdAlg)
			delete algs[i];
	#endif

	algs.clear();

	// FILL STD
	for (int i = 0; i < vctStdAlgs.size(); i++) {
		algs.push_back(vctStdAlgs[i]);
	}

	DebugTrace(">> ���� ����� ����������...");
	DebugTrace(">> ���� �������� ����������� ����������...");
	lecsGen.line = -1;
	bool bLastLex = 1; // 0-��� 1-���
	string str;
	do{
		str = lecsGen.GetNext();
		if (str == "���") {
			if (bLastLex)
				throw SyntaxError("������� ��� \'���\' ������", lecsGen.line);
			bLastLex = 1;
		}
		if (str == "���") {
			if (!bLastLex)
				throw SyntaxError("��������� ��������� �� �����������, �������� �������� �������� ������ \'���\' ������ ��� ������ �����", lecsGen.line);
			bLastLex = 0;
		}
	}while(str != "#");
	DebugTrace(">> �������� ����������� ���������� ���������, ��������� ���������� �� �������");

	// FILL USER
	DebugTrace(">> ���� �������� ���������� ����������...");
	lecsGen.line = -1;
	while (lecsGen.IsLecsPresents()) {
		algs.push_back(new TAlgorithm(lecsGen));
	}
	DebugTrace(">> �������� ���������� ���������� ���������");
	DebugTrace(">> ����� ���������� ��������");

	DebugTrace(">> ���� �������� ����������...", always);
	lecsGen.line = -1;
	int i = vctStdAlgs.size();
	while (lecsGen.IsLecsPresents()) {
		algs[i]->Fill(lecsGen);
		i++;
	}
	DebugTrace(">> �������� ���������� ���������, ������ �� �������");


	RefreshVarWnd();
}

void TEmulator::Run(bool bStepOver) {
	bNeedToStop = 0;
	DebugTrace("***>> ������ ���������... <<***", always);

	fmConsole->Memo1->Lines->Add("_______________________________________________");
	fmMainView->Memo4->Lines->Add("_______________________________________________");
	if (fmRunParams->CheckBox1->Checked) {
		fmConsole->Memo1->Clear();
		fmConsole->Edit1->Clear();
	}
	fmConsole->Memo1->Lines->Add(String("����������� �������� ")
	+algs[fmRunParams->GetMainAlgorithm()]->GetName().c_str());
	fmMainView->Memo4->Lines->Add(String("����������� �������� ")
	+algs[fmRunParams->GetMainAlgorithm()]->GetName().c_str());

	if (bStepOver)
		esState = esTracing;
	else
		esState = esRunning;

	fmMainView->StringGrid3->RowCount = fmMainView->Memo2->Lines->Count;
	for (int i = 0; i < fmMainView->Memo2->Lines->Count; i++) {
		fmMainView->StringGrid3->Cells[0][i] = TabsToSpacesForString
			(fmMainView->Memo2->Lines->Strings[i]);
	}

	TAlgorithm* alg = algs[fmRunParams->GetMainAlgorithm()];
	vector<TVariable> vctInputList = alg->GetParams();
	for (int i = 0; i < vctInputList.size(); i++) {
		fmConsole->ReadLn(alg->Header.GetVariable(vctInputList[i].GetStrName()));
	}

	DebugTrace("***>> �������� ������� ��������������� � �������. <<***", always);

	if (!bStepOver) {
		ShowTab("����������",1);
		alg->RunInit();
		Continue();
	}
	else {
		DebugTrace("-> �������� ������������� � ����� � ���������� ������, ������� ��� ��� ���������� ��������� ������� ��� ������ ��� �������� � ������� ����� ���������� ���������.", always);
		alg->RunInit();
		ShowTab("����������",0);
		ShowTab("����������",1);
		fmMainView->Invalidate();
	}
}

void TEmulator::StepIt() {
	if (esState == esStoped) {
		MessageBox(Application->Handle, "��� ������� ����������, ��� ��� ������������� ������� �������\n (������->������ �������)", "������ ������� ���������", MB_ICONINFORMATION);
		return;
	}

	if (esState == esRunning) {
		MessageBox(Application->Handle, "��� ������� ����������, �.� �������� ��� ������� � ������� ������,\n ��� ������������� ���� ������� ������� �������\n (������->������ �������)", "������ ������� ���������", MB_ICONINFORMATION);
		return;
	}

	algs[fmRunParams->GetMainAlgorithm()]->Step();

	if (bNeedToStop) {
		DebugTrace("***>> �������� �������� ������, � ���� �������� ������ �� ��������. <<***", always);
		esState = esStoped;
		ShowTab("����������",0);
		ShowTab("����������",1);
	}

}

void TEmulator::Stop() {
	if (esState == esStoped) {
		MessageBox(Application->Handle, "�������� �� �������", "������ ������ � ����������", MB_ICONINFORMATION);
		return;
	}

	DebugTrace("***>> ���������� ��������� �������� �������������. <<***", always);
	bNeedToStop = 1;

	esState = esStoped;
	if (esState == esTracing) {
		StepIt();
		ShowTab("����������",0);
		ShowTab("����������",1);
	}
}

void TEmulator::Continue() {
	esState = esRunning;
	while (!Emulator.bNeedToStop) {
		algs[fmRunParams->GetMainAlgorithm()]->Step();
		Application->ProcessMessages();
	}

	DebugTrace("***>> �������� �������� ������, � ���� �������� ������ �� ��������. <<***", always);
	esState = esStoped;
}
