//---------------------------------------------------------------------------
#pragma hdrstop
#include "Algorithm.h"
#include "Emulator.h"
#include <vcl.h>
#include "Trace.h"
#include "UnitMainForm.h"
#include "UConsole.h"
#include "URunParams.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------

TAlgorithm::TAlgorithm(LecsemManager& lecsGen) {
	bStdAlg = 0;
	DebugTrace("����� ������ ���������...");
	{ // ������� ������ ���������:
		string str = lecsGen.GetNext();
		if ( str != "���"){
			str = "�� ������� ������ ���������, ��������� \'���\', ������� "+str;
			throw SyntaxError(str.c_str(), lecsGen.line);
		}
	}

	{ // ������� �������� ���������:
		Header.PushAlgName(lecsGen.GetNext(), lecsGen.line);
		DebugTrace(string("�������� ������: ")+GetName());
	}

	DebugTrace(string("������ ��������� ��������� ")+GetName());

	{ // ������� ����� ���������:
		int ln = lecsGen.line;
		int ls = lecsGen.lecs;

		bool bEndFound = 0;
		string str;
		do{
			str = lecsGen.GetNext();
			if (str == "���") {
				bEndFound = 1;
				break;
			}
		}while(str != "#");

		if (!bEndFound)
			throw SyntaxError((string("�� ������ ����� ��������� ")+Header.GetAlgName()+", ��������� \'���\'").c_str(), lecsGen.line); // line - ��������� ������

		lecsGen.line = ln;
		lecsGen.lecs = ls; 
	}


	{ // ��������� ������ ����������
		string str = lecsGen.GetNext();
		if ( str != "(") {
			str = "�� ������� ������ ������ ���������� ��������� "+
			Header.GetAlgName()+", ��������� \'(\', ������� \'"+str+"\'";
			throw SyntaxError(str.c_str(), lecsGen.line);
		}

		bool bVarFound, bVarLast, bLecsFound;
		do {
			TVariable var(ss_niSupportState);
			if (!FindType(lecsGen, var)) break;

			bVarFound  = 0;
			bVarLast   = 0;
			bLecsFound = 0;

			do{
				str = lecsGen.GetNext();

				if (str == ")" || str == ";") {
					if (!bVarFound )
						throw SyntaxError(string("����� �������� ���� ����������, � ��������� ��������� " +GetName()+", ��������� ������������ �� ����, ������ ����� ������� \'")+str+"\'",lecsGen.line);
					if (!bVarLast)
						throw SyntaxError(string("� ��������� ��������� ")+GetName()+" ������ ������� ����� �������� \'"+str+"\'", lecsGen.line);
					break;
				}

				if (bLecsFound && !bVarLast && str == ",")
					throw SyntaxError(string("��� ������� ������ ��� �������� ���������� � ��������� ")
					+GetName()+", ����� �������� ��������� ����������, ���� ������� ������", lecsGen.line);

				if (bVarLast && str != ",")
					throw SyntaxError(string("��� ���������� ������ ��� �������� ���������� � ��������� ")
					+GetName()+", ����� ���������� "+str+" ��������� �������", lecsGen.line);

				if ( !bVarFound && !bVarLast && bLecsFound)
					throw SyntaxError(string("����� �������� ���� ���������� � ��������� ��������� ")
					+GetName()+" ������� �������, ��� ������ �������, ������ ����� ��������� ��� ����������", lecsGen.line);


				if (str == ",") {
					bLecsFound = 1;
					bVarLast   = 0;
					continue;
				}

				if ((str != ")" && str != ";")) {
					Header.PushVar(var, str, lecsGen.line);
					bVarFound  = 1;
					bVarLast   = 1;
					bLecsFound = 1;
				}

			}while(1);

		}while(str == ";");

		if (lecsGen.GetPrev() == ";")
			throw SyntaxError(string("������ ������ \';\' ����� �������� \')\' � ��������� ��������� ")
			+GetName()+" ��� �������� ����������; ���� ������� ������ \';\', ���� ���������� ���������� ����������, �������� ����� ���� ��� ����", lecsGen.line);
	}

	{ // ������������� ���������
		string str = lecsGen.GetNext();
		if ( str != "���") {
			str = "�� ������� ������ ������ ���������� ��������� "+
			Header.GetAlgName()+", ��������� \'���\', ������� \'"+str+"\'";
			throw SyntaxError(str.c_str(), lecsGen.line);
		}

		str = lecsGen.GetNext();
		if (str != "���") {
			Header.ToArgument(str, lecsGen.line);
			vctParams.push_back(Header.GetVariable(str));
			while (1) {
				str = lecsGen.GetNext();
				if (str == "���") break;
				if (str != ",")
					throw SyntaxError(string("������ �������� ���������� ��������� ")
					+GetName()+": ��������� ������� ��� ����������� ��� \'���\' ��� ��������� ������, ������� \'"+str+"\'",lecsGen.line);
				str = lecsGen.GetNext();
				Header.ToArgument(str, lecsGen.line);
				vctParams.push_back(Header.GetVariable(str));
			}
		}
		else {
			str = lecsGen.GetNext();
			if (str != "���")
				throw SyntaxError(string("������ �������� ���������� ��������� ")
				+GetName()+": ��������� \'���\', ������� \'"+str+"\'",lecsGen.line);
		}

	}

	{ // ������������� ���������
		string str = lecsGen.GetNext();
		if (str != "���")
			Header.ToResult(str, lecsGen.line);
	}

	{ // ��� - ����
		string str = lecsGen.GetNext();
		if (str != "���")
			throw SyntaxError(string("������ �������� ��������� ")
			+GetName()+": ��������� \'���\', ������� \'"+str+"\'",lecsGen.line);

		str = lecsGen.GetNext();
		if (str != "(")
			throw SyntaxError(string("������ �������� ��������� ")
			+GetName()+": ��������� \'(\', ������� \'"+str+"\'",lecsGen.line);

		str = lecsGen.GetNext();
		if (str != ")" ){
			TVariable var(ss_niSupportState);
			CheckToSimpleType(str, var, lecsGen.line);
			str = lecsGen.GetNext();
			Header.PushVar(var, str, lecsGen.line);
			Header.ToExc(str);
			str = lecsGen.GetNext();
			if (str != ")")
				throw SyntaxError(string("������ �������� ���������� ��������� ")
				+GetName()+": ��������� \')\', ������� \'"+str+"\'",lecsGen.line);
		}
	}

	{ // ������������ �� ����� ���������:

		bool bEndFound = 0;
		string str;
		do{
			str = lecsGen.GetNext();
			if (str == "���") {
				bEndFound = 1;
				break;
			}
		}while(str != "#");

		if (!bEndFound)
			throw SyntaxError((string("�� ������ ����� ��������� ")+Header.GetAlgName()+", ��������� \'���\'").c_str(), lecsGen.line); // line - ��������� ������
	}

	DebugTrace(string("��������� ������"));
}


void TAlgorithm::Fill(LecsemManager& lecsGen) {
	{ // ������������ �� ������ ������:

		int count = 0;
		string str;
		do{
			str = lecsGen.GetNext();
			if (str == ")")
				count++;
			if (count == 2)
				break;
		}while(str != "#");

	}

	DebugTrace(string(">> ���������� ������..."));
	Command* cmd;
	while (cmd = FillLecsem(Header, lecsGen))
		vctBody.push_back(cmd);
	DebugTrace(string("���������� ���������"));
	DebugTrace("___________________________________________________________");
}

//---------------------------------------------------------------------------
void TAlgorithm::CheckToSimpleType(const string& str, TVariable& var, int line) {
	if (str == "���")
		var.varType = vtBool;

	else if (str == "���")
		var.varType = vtInt;

	else if (str == "���")
		var.varType = vtDouble;

	else if (str == "���")
		var.varType = ::vtString;

	else if (str == "���")
		var.varType = ::vtChar;

	else throw SyntaxError(string("������ �������� ���������� ��������� ")
	+GetName()+": ��������� ��� �������� ���� ��� \')\', ������� \'"+str+"\'", line);
}

//---------------------------------------------------------------------------
bool TAlgorithm::FindType(LecsemManager& lecsGen, TVariable& var) {
	string str = lecsGen.GetNext();

	if (str == ")")
		return 0;


	if (str == "���")
		var.varType = vtBool;

	else if (str == "���")
		var.varType = vtInt;

	else if (str == "���")
		var.varType = vtDouble;

	else if (str == "���")
		var.varType = ::vtString;

	else if (str == "���")
		var.varType = ::vtChar;

	else if (str == "����") {
		var.varType = vtArray;
		str = lecsGen.GetNext();

		if (str == "���")
			var.Value.arrValue.varElemType = vtBool;

		else if (str == "���")
			var.Value.arrValue.varElemType = vtInt;

		else if (str == "���")
			var.Value.arrValue.varElemType = vtDouble;

		else if (str == "���")
			var.Value.arrValue.varElemType = ::vtString;

		else if (str == "���")
			var.Value.arrValue.varElemType = ::vtChar;

		else throw SyntaxError(string("��������� ��� ���� ��������� �������, ������� \'")+str
		+"\' � ��������� ��������� "+Header.GetAlgName(), lecsGen.line);

		str = lecsGen.GetNext();
		if (str!="[")
			throw SyntaxError(string("�������� ������� � ��������� ��������� ")+GetName()
			+" ������ �������, ����� �������� ���� ��������� ������� ��������� ������ \'[\' � ����� �������� �����������, ������ ����� ������� \'"+str+"\'", lecsGen.line);

		str = lecsGen.GetNext();
		bool bFound;
		for (int i = 0; i < str.size(); i++) {
			bFound = 0;
			for (int j = 0; szFigures[j]!='\0'; j++) {
				if (szFigures[j] == str[i]) bFound = 1;
			}
			if (!bFound)
			throw SyntaxError(string("�������� ������� � ��������� ��������� ")+GetName()
			+" ������ �������, ����� ������� \'[\' ��������� �������� ����������� (����������� ����� �� 4096), ������ ����� ������� \'"+str+"\'", lecsGen.line);
		}

		if (!StrToInt(str.c_str()))
			throw SyntaxError(string("�������� ������� � ��������� ��������� ")+GetName()
			+" ������ �������, ����� ������� \'[\' ��������� �������� ����������� (����������� ����� �� 4096), ������ ����� ������ ����", lecsGen.line);

		if (StrToInt(str.c_str()) > 4096)
			throw SyntaxError(string("�������� ������� � ��������� ��������� ")+GetName()
			+" ������ �������, ��������� ���������� �������� ����������� (����������� ����� �� 4096): \'"+str+"\'", lecsGen.line);

		var.Value.arrValue.nColCnt = StrToInt(str.c_str());

		str = lecsGen.GetNext();
		if (str!="]")
			throw SyntaxError(string("�������� ������� � ��������� ��������� ")+GetName()
			+" ������ �������, ����� �������� ���������� ��������� ������� ��������� ������ \']\', ������ ����� ������� \'"+str+"\'", lecsGen.line);

	}
// �������: throw SyntaxError(string("")+str+""+GetName(), lecsGen.line);
	else if (str == "����") {
		var.varType = vtMatrix;
		str = lecsGen.GetNext();

		if (str == "���")
			var.Value.mtxValue.varElemType = vtBool;

		else if (str == "���")
			var.Value.mtxValue.varElemType = vtInt;

		else if (str == "���")
			var.Value.mtxValue.varElemType = vtDouble;

		else if (str == "���")
			var.Value.mtxValue.varElemType = ::vtString;

		else if (str == "���")
			var.Value.mtxValue.varElemType = ::vtChar;

		else throw SyntaxError(string("��������� ��� ���� ��������� �������, ������� \'")+str
		+"\' � ��������� ��������� "+Header.GetAlgName(), lecsGen.line);

		str = lecsGen.GetNext();
		if (str!="[")
			throw SyntaxError(string("�������� ������� � ��������� ��������� ")+GetName()
			+" ������ �������, ����� �������� ���� ��������� ��������� ������ \'[\' � ����� �������� ������������ ����� �������, ������ ����� ������� \'"+str+"\'", lecsGen.line);

		str = lecsGen.GetNext();
		bool bFound;
		for (int i = 0; i < str.size(); i++) {
			bFound = 0;
			for (int j = 0; szFigures[j]!='\0'; j++) {
				if (szFigures[j] == str[i]) bFound = 1;
			}
			if (!bFound)
			throw SyntaxError(string("�������� ������� � ��������� ��������� ")+GetName()
			+" ������ �������, ����� ������� \'[\' ��������� �������� ���������� ����� (����������� �����), ������ ����� ������� \'"+str+"\'", lecsGen.line);
		}

		if (!StrToInt(str.c_str()))
			throw SyntaxError(string("�������� ������� � ��������� ��������� ")+GetName()
			+" ������ �������, ����� ������� \'[\' ��������� �������� ���������� ����� (����������� �����), ������ ����� ������ ����", lecsGen.line);

		if (StrToInt(str.c_str()) > 512)
			throw SyntaxError(string("�������� ������� � ��������� ��������� ")+GetName()
			+" ������ �������, ��������� ���������� �������� ����������� ����� (����������� ����� �� 512): \'"+str+"\'", lecsGen.line);

		var.Value.mtxValue.nRowCnt = StrToInt(str.c_str());


		str = lecsGen.GetNext();
		if (str!=":")
			throw SyntaxError(string("�������� ������� � ��������� ��������� ")+GetName()
			+" ������ �������, ����� �������� ���������� ����� ��������� ������ \':\' � ����� �������� ���������� ��������, ������ ����� ������� \'"+str+"\'", lecsGen.line);

		str = lecsGen.GetNext();
		for (int i = 0; i < str.size(); i++) {
			bFound = 0;
			for (int j = 0; szFigures[j]!='\0'; j++) {
				if (szFigures[j] == str[i]) bFound = 1;
			}
			if (!bFound)
			throw SyntaxError(string("�������� ������� � ��������� ��������� ")+GetName()
			+" ������ �������, ����� ������� \':\' ��������� �������� ���������� �������� (����������� �����), ������ ����� ������� \'"+str+"\'", lecsGen.line);
		}

		if (!StrToInt(str.c_str()))
			throw SyntaxError(string("�������� ������� � ��������� ��������� ")+GetName()
			+" ������ �������, ����� ������� \':\' ��������� �������� ���������� �������� (����������� �����), ������ ����� ������ ����", lecsGen.line);

		if (StrToInt(str.c_str()) > 512)
			throw SyntaxError(string("�������� ������� � ��������� ��������� ")+GetName()
			+" ������ �������, ��������� ���������� �������� ����������� �������� (����������� ����� �� 512): \'"+str+"\'", lecsGen.line);

		var.Value.mtxValue.nColCnt = StrToInt(str.c_str());


		str = lecsGen.GetNext();
		if (str!="]")
			throw SyntaxError(string("�������� ������� � ��������� ��������� ")+GetName()
			+" ������ �������, ����� �������� ���������� �������� ��������� ������ \']\', ������ ����� ������� \'"+str+"\'", lecsGen.line);

	}

	else throw SyntaxError(string("��������� ��� ����, ������� \'")+str
	+"\' � ��������� ��������� "+Header.GetAlgName(), lecsGen.line);

	return 1;
}

//---------------------------------------------------------------------------

vector<TVariable> TAlgorithm::GetParams() {
	return vctParams;
}

//---------------------------------------------------------------------------
TVarDataType TAlgorithm::GetResultType() {
	for (unsigned i = 0; i < Header.vctVariables.size(); i++) {
		if (Header.vctVariables[i].varState == vsResult) {
			return Header.vctVariables[i].varType;
		}       
	}
	return vtVoid;
}

//---------------------------------------------------------------------------
TAlgorithm::~TAlgorithm() {
	DebugTrace(string("������ ���������� ��������� ")+GetName());
	for (int i = 0; i < vctBody.size(); i++)
		delete vctBody[i];
}




//---------------------------------------------------------------------------
void TAlgorithm::Step() {
	static const TValue value;
																		// ����...
	if (	pCurrentCommand == -1                                       // �������� ��������
			&& Emulator.algs[fmRunParams->GetMainAlgorithm()] == this   // ��� �������� ��������
			&& Header.IsResultPresent()									// ��� ���� ����� ���� ���������
			&& Header.GetResult().bDynamicInit                          // ��������� ��������������� (��� � �������������� ���������� ���������)
	) {
		fmConsole->WriteLn(Header.GetResult());
	}

	if (pCurrentCommand == -1) // �������� ��������.
		Emulator.bNeedToStop = 1; // ������������� ����������
	else {                                       // ��������� ��������� �������

		if ( vctBody[pCurrentCommand]->Execute(Header, this) ){ // ������ ���; ���� ��� ������� ���������...
			if (pCurrentCommand == (vctBody.size()-1)) 		    // ���� ��� ��������� � �������� ������.
				pCurrentCommand = -1;                         	// ������������� ������� ��������
			else {
				if (pCurrentCommand != -1) 						// TVariable::Copy ����� �������� � Execute
					pCurrentCommand++;   						// ����� �������� �� ���������.
			}
		}

	}

	RefreshVarWnd();
	fmMainView->StringGrid3->Row = GetLine();

	ShowPos();
}

//---------------------------------------------------------------------------
void TAlgorithm::RunInit() {
	if (!vctBody.size()) // �������� ����.
		pCurrentCommand = -1;
	else
		pCurrentCommand = 0; // ������������� �� ������.

	ShowPos();
}

//---------------------------------------------------------------------------
void TAlgorithm::ShowPos() {
	if(Emulator.bNeedToStop)
		return;

	if (Emulator.esState == esTracing) {
		if (pCurrentCommand != -1)
			DebugTrace(string("-> ������� �����������: [")+IntToStr(GetLine()+1).c_str()
			+"]: "+fmMainView->StringGrid3->Cells[0][GetLine()].c_str(), always);
		else
			DebugTrace("-> ����������� ���������, ��������� ����� �������� ����� ��������.", always);
	}
}
//---------------------------------------------------------------------------
int  TAlgorithm::GetLine() {
	if (pCurrentCommand == -1) {
		fmMainView->TabSheet5->Show();
		return 0;
	}

	int n = vctBody[pCurrentCommand]->GetLine();
	fmMainView->TabSheet5->Show();
	return n;
}
//---------------------------------------------------------------------------

void TAlgorithm::CheckCall	(const deque<TVariable*>& params, int line) {

	if (vctParams.size() != params.size()) {
		throw SyntaxError(string("������������ ���������� ���������� ��� ������ ��������� ")+GetName(), line);
	}

	for (unsigned i = 0; i < vctParams.size(); i++) {
		vctParams[i].CheckCopy(*params[i], line);
	}
}

//---------------------------------------------------------------------------

TVariable TAlgorithm::Call (const deque<TVariable>& params) {
try {

	for (int i = 0; i < vctParams.size(); i++) {
    	Header.GetVariable(vctParams[i].strName).Copy(params[i], this);    
	}

	if (!vctBody.size()) // �������� ����.
		pCurrentCommand = -1;
	else
		pCurrentCommand = 0; // ������������� �� ������.

	while(1){

		if (pCurrentCommand == -1) // �������� ��������.
			break;
		else                                        // ��������� ��������� �������
			if ( vctBody[pCurrentCommand]->Execute(Header, this) ){ // ������ ���; ���� ��� ������� ���������...
				if (pCurrentCommand == (vctBody.size()-1)) 		    // ���� ��� ��������� � �������� ������.
					pCurrentCommand = -1;                         	// ������������� ������� ��������
				else {
					if (pCurrentCommand != -1) 						// TVariable::Copy ����� �������� � Execute
						pCurrentCommand++;   						// ����� �������� �� ���������.
				}
			}
	}
	if (Header.IsResultPresent())
		return Header.GetResult();

	TVariable var(ss_niSupportState);
	var.varType = vtVoid;
	var.bDynamicInit = 1;
	var.varState = vsExpression;
	return var;


}
catch (RuntimeError& e) {
	throw e;
}
catch (EStackOverflow& e) {
	MessageBox(Application->Handle, "��������� ������������ �����.\n��-��������, ��� ��������� ����������� �������� � ���������.", "����������� ������ � ������ ���������", MB_ICONSTOP);
	throw RuntimeError("��������� ������������ �����. ��-��������, ��� ��������� ����������� �������� � ���������.");
}
catch (...) {
	MessageBox(Application->Handle, "��������� ��������������� ��������� ������ � ������ ��������������.", "����������� ������ � ������ ���������", MB_ICONSTOP);
	throw RuntimeError("��������� ��������� ������ � ������ ��������������.");
}
}
//---------------------------------------------------------------------------

