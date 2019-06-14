//---------------------------------------------------------------------------
#pragma hdrstop
#include <vcl.h>
#include "Commands.h"
#include "Emulator.h"
#include "Algorithm.h"
#include "Errors.h"
#include <deque.h>
#include "Trace.h"
#pragma package(smart_init)
using namespace std;


// �������� ������� ���������� ���������, ���������� ��� ����������
TVariable CheckFullExpression (TVarHeader& hdr, TVarHeader& buffer,
	 deque<string>& vct/*Expression*/, int line);

//---------------------------------------------------------------------------
Command* ReedCommand(string str, TVarHeader& hdrHost, LecsemManager& lecsGen) {
	/* DONE 5 : MODIFI, IF ADD Cmd */

	if ( str == "����" ) {
		DebugTrace("������ ����");
		return new CmdWhileExpression(hdrHost, lecsGen);
	}

	if ( str == "����" ) {
		DebugTrace("������� ���������");
		return new CmdIfExpression(hdrHost, lecsGen);
	}

	if ( Emulator.IsAlgPresent(str) ) {
		DebugTrace(string("������ ��������� ����� ��������� ")+str);
		return new CmdAlgExpression(hdrHost, lecsGen);
	}

	DebugTrace("������� ������ �� �������, ��������� ���������������� ��� �������������");
	return new CmdCopyExpression(hdrHost, lecsGen);
}

//---------------------------------------------------------------------------
// ��������� � ���������� ��������� �������,
// ���������� �� Algorithm::Fill � �� ������������� ������-����������� ���� ���� � ����
Command* FillLecsem(TVarHeader& hdrHost, LecsemManager& lecsGen) {
	DebugTrace(string("@Command* FillLecsem(...)"));
	DebugTrace(string("����� ����� �������..."));

	int ln = lecsGen.line;
	int ls = lecsGen.lecs;

	string str = lecsGen.GetNext();
	DebugTrace(string("���� ������������� \'")+str+"\'");
	if (str == "���") {
		DebugTrace(string("������ �� �������"));
		return 0;
	}

	lecsGen.line = ln;
	lecsGen.lecs = ls;

	return ReedCommand(str, hdrHost, lecsGen);
}

// ����������� ����������
Command::~Command(){}

//---------------------------------------------------------------------------
// ����� ���� ����������� ���� �������, �������������� ������� - ����������,
// �������� �� ���������� �� ������� ���������� ���������������� ����

inline bool IsBoolConst(string str) {
	if (str == "������" || str == "����")
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

/* ��������������� f(), '.' -> ',' ��� FloatToStr()*/
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

inline bool IsCharConst(string str) {
	if (str.size() == 3 && str[0] == '\'' && str[2] == '\'')
		return 1;
	return 0;
}

inline bool IsStringConst(string str) {
	if (str.size() > 3 && str[0] == '\'' && str[str.size()-1] == '\'')
		return 1;
	return 0;
}


// �������� ��������� � ������ �� ����� �������� ����������,
// ��� �������� �������� �������� ��������� � �����.
// ������� ���������� ���� ��������� ������� ��� ����������� �������� ���������
void TranslateConstToVar(deque<string>& vct, TVarHeader& buffer, int line){
	DebugTrace(string("@TranslateConstToVar"));
	TVariable var(ss_niSupportState);
	DebugTrace(string(">> ���� ����� ��������..."));
	for (unsigned i = 0; i < vct.size(); i++) {
		DebugTrace(string("  �������� ������� \'")+vct[i]+"\'");
		if (IsBoolConst(vct[i])) {
			var.bStaticInit = 1;
			var.bDynamicInit = 1;
			var.varType = vtBool;
			var.varState = vsConst;
			var.Value.blnValue = (vct[i] == "������");
			var.strName = string("ConstBufferVar#");
			var.strName += (IntToStr(buffer.vctVariables.size())).c_str(); // ���������� � 0
			buffer.vctVariables.push_back(var);
			vct[i] = var.strName;
//DebugTrace(string(""));
			DebugTrace(string("  ������� ��������� ���� ���, ��� � ������: ")+var.strName);
		}
		else if (IsIntConst(vct[i])) {
			var.bStaticInit = 1;
			var.bDynamicInit = 1;
			var.varType = vtInt;
			var.varState = vsConst;
			var.Value.intValue = StrToInt(vct[i].c_str());
			var.strName = string("ConstBufferVar#");
			var.strName += (IntToStr(buffer.vctVariables.size())).c_str(); // ���������� � 0
			buffer.vctVariables.push_back(var);
			vct[i] = var.strName;
			DebugTrace(string("  ������� ��������� ���� ���, ��� � ������: ")+var.strName);
		}
		else if (IsDoubleConst(vct[i])) {
			var.bStaticInit = 1;
			var.bDynamicInit = 1;
			var.varType = vtDouble;
			var.varState = vsConst;
			var.Value.dblValue = StrToFloat(AlgFloatToBrlnd(vct[i]).c_str());
			var.strName = string("ConstBufferVar#");
			var.strName += (IntToStr(buffer.vctVariables.size())).c_str(); // ���������� � 0
			buffer.vctVariables.push_back(var);
			vct[i] = var.strName;
			DebugTrace(string("  ������� ��������� ���� ���, ��� � ������: ")+var.strName);
		}
		else if (IsCharConst(vct[i])) { // ����� String
			var.bStaticInit = 1;
			var.bDynamicInit = 1;
			var.varType = ::vtChar;
			var.varState = vsConst;
			var.Value.chrValue = vct[i][1];
			var.strName = string("ConstBufferVar#");
			var.strName += (IntToStr(buffer.vctVariables.size())).c_str(); // ���������� � 0
			buffer.vctVariables.push_back(var);
			vct[i] = var.strName;
			DebugTrace(string("  ������� ��������� ���� ���, ��� � ������: ")+var.strName);
		}
		else if (IsStringConst(vct[i])) {
			var.bStaticInit = 1;
			var.bDynamicInit = 1;
			var.varType = ::vtString;
			var.varState = vsConst;
			var.Value.strValue = vct[i].substr(1,vct[i].size()-2);
			var.strName = string("ConstBufferVar#");
			var.strName += (IntToStr(buffer.vctVariables.size())).c_str(); // ���������� � 0
			buffer.vctVariables.push_back(var);
			vct[i] = var.strName;
			DebugTrace(string("  ������� ��������� ���� ���, ��� � ������: ")+var.strName);
		}

	}

	DebugTrace(string("�������� ���������:"));
	for (unsigned i = 0; i < vct.size(); i++) {
		DebugTrace(string("  ")+IntToStr(i+1).c_str()+") "+vct[i]);
	}
	DebugTrace(string("����� �������� ��������"));
	DebugTrace(string("@TranslateConstToVar exit"));
}

//---------------------------------------------------------------------------

// �������� ������������ ����������� ������
void VerifyBrakets(deque<string>& vct, int line) {
	DebugTrace(string("@VerifyBrackets"));
	DebugTrace(string("�������� ������������ ����������� ������..."));
	int nOpen = 0, nClose = 0;
	DebugTrace("���������:");
	for (unsigned i = 0; i < vct.size(); i++) {
		DebugTrace(string("  ")+IntToStr(i+1).c_str()+") "+vct[i]);
		if (vct[i] == "(") nOpen++;
		if (vct[i] == ")") nClose++;
	}

	DebugTrace(string("  ( : ")+IntToStr(nOpen).c_str());
	DebugTrace(string("  ) : ")+IntToStr(nClose).c_str());
	if (nOpen > nClose)
		throw SyntaxError("� ��������� ������� ������������� ������ \'(\'", line);

	if (nOpen < nClose)
		throw SyntaxError("� ��������� ������� ������������� ������ \')\'", line);

	nOpen = 0, nClose = 0;
	for (unsigned i = 0; i < vct.size(); i++) {
		if (vct[i] == "[") nOpen++;
		if (vct[i] == "]") nClose++;
	}
	DebugTrace(string("  [ : ")+IntToStr(nOpen).c_str());
	DebugTrace(string("  ] : ")+IntToStr(nClose).c_str());

	if (nOpen > nClose)
		throw SyntaxError("� ��������� ������� ������������� ������ \'[\'", line);

	if (nOpen < nClose)
		throw SyntaxError("� ��������� ������� ������������� ������ \']\'", line);
	DebugTrace(string("�������� ���������"));
	DebugTrace(string("@VerifyBrackets exit"));
}

//---------------------------------------------------------------------------

bool BracketsPresents(deque<string>& vct){
	for (unsigned i = 0; i < vct.size(); i++) {
		if (vct[i] == "(") return 1;
	}
	return 0;
}

//---------------------------------------------------------------------------

bool NotAlgParamBracket (deque<string>& v, int pos) {
	if (!pos) return 1;
	if (Emulator.IsAlgPresent(v[pos-1]))
		return 0;
	return 1;
}

//---------------------------------------------------------------------------

void FindMostPriorityBracket(const deque<string>& vct,
					deque<string>& vBracket, int& MostHighBracketPos) {

	DebugTrace(string("@FindMostPriorityBracket"));
	DebugTrace(string("����� �������� ��������� ������..."));
	int max_priority = 0;
	int priority = 0;
	int pos = 0;
	for (unsigned i = 0; i < vct.size(); i++) {
		if (vct[i] == "(" && NotAlgParamBracket(vct, i) )
			priority++;
		if (priority > max_priority) {
			max_priority = priority;
			pos = i;
		}
		if (vct[i] == ")") priority--;
	}

	DebugTrace(string("���������� �������: ")+IntToStr(pos).c_str());
	MostHighBracketPos = pos;
	vBracket.clear();
	priority = 1;
	DebugTrace(string("��������� ������ ������:"));
	for (int i = pos+1; 1; i++) {
		if (vct[i] == "(")
			priority++;
		if (vct[i] == ")")
			priority--;
		if (!priority)
			break;

		DebugTrace(string("  ")+IntToStr(vBracket.size()+1).c_str()+") "+vct[i]);
		vBracket.push_back(vct[i]);
	}
	DebugTrace(string("����� ��������"));
	DebugTrace(string("@FindMostPriorityBracket exit"));
}

//---------------------------------------------------------------------------
// ������� ���������� ������������������� ������������� �������:
// ����� ���������, �������������� ��������� �������� � ������

TVariable ReedAlgorithmObj(
	const deque<string>& vBracket, // ���� ���������
	TVarHeader& hdr,               // ���������� ���������
	TVarHeader& buffer,            // �����
	int& pos,                      // �������� pos �� ������ �������
	int line                       // � ������, ������������ ������ ��� throw
) {
	TAlgorithm* alg = Emulator.GetAlgorithmPointer(vBracket[pos]);
	pos++;
	if ((pos == vBracket.size()) || (vBracket[pos] != "("))
		throw SyntaxError("�� ������ ������ ���������� ��������� "+alg->GetName()+", ��������� \'(\'", line);
	pos++;

	string str;
	int n = 1; // ( - )
	int m = 0; // [ - ]
	deque<string> qque;
	deque< TVariable* > qpvar;

	while (1) {
		if (pos == vBracket.size()) {
			for (unsigned i = 0; i < qpvar.size(); i++) {
				delete qpvar[i];
			}
			throw SyntaxError("�� ������ ����� ������ ���������� ��������� "+alg->GetName()+", ��������� \')\'", line);
		}
		str = vBracket[pos];
		pos++;
		if (str == "(")
			n++;
		if (str == ")")
			n--;
		if (str == "[")
			m++;
		if (str == "]")
			m--;
		if (!m && str == ",") { // �������� m, �.�. ������� ���-�� � � �������������� ������
			qpvar.push_back(new TVariable(ss_niSupportState));
			*qpvar[qpvar.size()-1] = CheckFullExpression(hdr, buffer, qque, line);
			qque.clear();
			continue; // ������� �� ��������������
		}

		if (n)
			qque.push_back(str);
		else {
			if(qpvar.size() || !qque.empty()) { // ���� � ������ �� �����
				qpvar.push_back(new TVariable(ss_niSupportState));
				*qpvar[qpvar.size()-1] = CheckFullExpression(hdr, buffer, qque, line);
				qque.clear();
			}
			break;
		}
	}

	try {
		alg->CheckCall(qpvar, line);

	}
	catch(SyntaxError e) {
		for (unsigned i = 0; i < qpvar.size(); i++) {
			delete qpvar[i];
		}
		throw e; // ��������� �����������
	}

	for (unsigned i = 0; i < qpvar.size(); i++) {
		delete qpvar[i];
	}

	TVariable result(ss_niSupportState);
	result.varType = alg->GetResultType();
	result.varState = vsExpression;
	result.bStaticInit = 1;
	return result;
}

//---------------------------------------------------------------------------
// ���������� ��������� ���� �������, ���������� ����������� ��������� ��� ��� �������� ����������
TVariable ReedCustomObj(           // ���������� ���, ������� ������������� � ���������
	const deque<string>& vBracket, // ���� ���������
	TVarHeader& hdr,               // ���������� ���������
	TVarHeader& buffer,            // �����
	int& pos,                      // �������� pos �� ������ �������
	int line                       // � ������, ������������ ������ ��� throw
){
	DebugTrace(string("@ReedCustomObj"));
	DebugTrace(string("������������� ������� \'")+vBracket[pos]+"\'");

	// ������?
	if (hdr.IsPresent(vBracket[pos]) && hdr.GetVariable(vBracket[pos]).varType == vtArray) {
		DebugTrace(string("����������������: ���������� ���� ������"));
		int xpos = pos;
		pos++;
		if ((pos == vBracket.size()) || (vBracket[pos] != "["))
			throw SyntaxError("������� ���������� ������������ �������� ��� ��������, ����������� ������ ��������������", line);
		pos++;

		string str;
		int n = 1;
		deque<string> que;
		while (1) {
			if (pos == vBracket.size())
				throw SyntaxError("������ �������� � ������� �� ������, ��������� \']\'", line);
			str = vBracket[pos];
			pos++;
			if (str == "]")
				n--;
			if (str == "[")
				n++;
			if (n)
				que.push_back(str);
			else
				break;
		}

		TVariable varI(ss_niSupportState);
		varI = CheckFullExpression(hdr, buffer, que, line);

		return hdr.GetVariable(vBracket[xpos]).CheckIndexing(varI, line);
	}

	// �������?
	if (hdr.IsPresent(vBracket[pos]) && hdr.GetVariable(vBracket[pos]).varType == vtMatrix) {
		DebugTrace(string("����������������: ���������� ���� �������"));
		int xpos = pos;
		pos++;
		if ((pos == vBracket.size()) || (vBracket[pos] != "["))
			throw SyntaxError("������� ���������� ������������ �������� ��� ��������, ����������� ������ ��������������", line);
		pos++;

		string str;
		deque<string> que;
		while (1) {
			if (pos == vBracket.size())
				throw SyntaxError("������ ������ � ������� �� ������, ��������� \',\'", line);
			str = vBracket[pos];
			pos++;

			if (str != ",")
				que.push_back(str);
			else
				break;
		}

		TVariable varI(ss_niSupportState);
		varI = CheckFullExpression(hdr, buffer, que, line);


		que.clear();
		int n = 1;
		while (1) {
			if (pos == vBracket.size())
				throw SyntaxError("������ ������� � ������� �� ������, ��������� \']\'", line);
			str = vBracket[pos];
			pos++;
			if (str == "]")
				n--;
			if (str == "[")
				n++;
			if (n)
				que.push_back(str);
			else
				break;
		}

		TVariable varJ(ss_niSupportState);
		varJ = CheckFullExpression(hdr, buffer, que, line);

		return hdr.GetVariable(vBracket[xpos]).CheckIndexing(varI, varJ, line);
	}


	// ��������?
	if ( Emulator.IsAlgPresent(vBracket[pos]) ) {
		DebugTrace("����������������: ����� ����������");
		return ReedAlgorithmObj(vBracket, hdr, buffer, pos, line);
	}

	// ������(�������) ����������?
	if (hdr.IsPresent(vBracket[pos])) {
		DebugTrace(string("����������������: ���������� �������� ����"));
		pos++;
		DebugTrace(string("@ReedCustomObj exit"));
		return hdr.GetVariable(vBracket[pos-1]); // ��������� ��� ����
	}

	// ���������?
	if (buffer.IsPresent(vBracket[pos])) {
		DebugTrace(string("����������������: �������� ����������"));
		pos++;
		DebugTrace(string("@ReedCustomObj exit"));
		return buffer.GetVariable(vBracket[pos-1]); // ��������� ��� ����
	}


	DebugTrace(string("������ �� �������"));
	throw SyntaxError(string("������������ �������, ��������� ������(����������,���������,...), ������� \'")+vBracket[pos]+"\'", line);
}

//---------------------------------------------------------------------------
// ��������� ��� ���������� ��������
TVariable CalculateElemExpression
(const TVariable& varTYPE, string strCOP, const TVariable& varOPERAND, int line) {
	DebugTrace(string("@CalculateElemExpression"));
	DebugTrace(string("���������� �������� ��������..."));
	DebugTrace(string("��� 1�� ��������: ")+varTYPE.GetStrType());
	DebugTrace(string("��� 2�� ��������: ")+varOPERAND.GetStrType());
	DebugTrace(string("��������: ")+strCOP);

	TVariable varResult(ss_niSupportState);

	if( strCOP == "=" ) {
		varResult = varTYPE.CheckEqvalCmp(varOPERAND, line);
	}
	else if( strCOP == "<>" ) {
		varResult = varTYPE.CheckNotEqvalCmp(varOPERAND, line);
	}
	else if( strCOP == "<" ) {
		varResult = varTYPE.CheckLessCmp(varOPERAND, line);
	}
	else if( strCOP == ">" ) {
		varResult = varTYPE.CheckGreaterCmp(varOPERAND, line);
	}
	else if( strCOP == "<=" ) {
		varResult = varTYPE.CheckLessOrEqCmp(varOPERAND, line);
	}
	else if( strCOP == ">=" ) {
		varResult = varTYPE.CheckGreaterOrEqCmp(varOPERAND, line);
	}
	else if( strCOP == "+" ) {
		varResult = varTYPE.CheckAdd(varOPERAND, line);
	}
	else if( strCOP == "-" ) {
		varResult = varTYPE.CheckSub(varOPERAND, line);
	}
	else if( strCOP == "*" ) {
		varResult = varTYPE.CheckMul(varOPERAND, line);
	}
	else if( strCOP == "/" ) {
		varResult = varTYPE.CheckDiv(varOPERAND, line);
	}
	else if( strCOP == ":") {
		varResult = varTYPE.CheckMod(varOPERAND, line);
	}
	else if( strCOP == "**" ) {
		varResult = varTYPE.CheckPow(varOPERAND, line);
	}
	else if( strCOP == "�" ) {
		varResult = varTYPE.CheckAND(varOPERAND, line);
	}
	else if( strCOP == "���" ) {
		varResult = varTYPE.CheckOR(varOPERAND, line);
	}
	else {
		throw LogicalError("���������� ������ ���������: ����������� �������� \'"+strCOP+"\'", line);
	}

	DebugTrace(string("��� ����������: ")+varTYPE.GetStrType());
	DebugTrace(string("���������� ���������"));
	DebugTrace(string("@CalculateElemExpression exit"));
	return varResult;
}

//---------------------------------------------------------------------------
// �������� ������������ ����������� ������
// �������� ������������ �����, ���������� ��� ���������� ������, ���������� ��� ���������
string CheckCOP(const deque<string>& vBracket, TVarHeader& hdr, TVarHeader& buffer, int line) {
	DebugTrace(string("@CheckCOP"));
	DebugTrace(string("���������� ���������.."));
	int size = vBracket.size();
	if (!size)
		throw SyntaxError("������� ������ ���������, ��������� ������������ ����������� ���������� � ����������", line);

	DebugTrace(string("���������:"));
	for (unsigned i = 0; i < size; i++) {
		DebugTrace(string(" ")+IntToStr(i+1).c_str()+") "+vBracket[i]);
	}

	// DebugTrace(string(""));
	int pos = 0;
	deque<string> result;
	TVariable varTYPE (ss_niSupportState); // ������� ���
	if (vBracket[0] == "-") {
		if (size == 1)
			throw SyntaxError("�� ������� �������� ��� ������� �������� \'-\'", line);
		pos = 1;

		if (size == pos) // ������� ������
			throw SyntaxError("�� ������� ������� ������� (����������, ���������,...)", line);
		varTYPE = ReedCustomObj(vBracket, hdr, buffer, pos, line); // �������� pos (+=)
		varTYPE = varTYPE.CheckNeg(line);
	}

	else if (vBracket[0] == "��") {
		if (size == 1)
			throw SyntaxError("�� ������� �������� ��� ������� �������� \'��\'", line);
		pos = 1;

		if (size == pos) // ������� ������
			throw SyntaxError("�� ������� ������� ������� (����������, ���������,...)", line);
		varTYPE = ReedCustomObj(vBracket, hdr, buffer, pos, line); // �������� pos (+=)
		varTYPE = varTYPE.CheckNOT(line);
	}

	else {
		if (size == pos) // ������� ������
			throw SyntaxError("�� ������� ������� ������� (����������, ���������,...)", line);
		varTYPE = ReedCustomObj(vBracket, hdr, buffer, pos, line); // �������� pos (+=)
	}


	DebugTrace(string("�������� ��� ���������(varTYPE): ")+varTYPE.GetStrType());

	// �������� ������� ������������������ � ������� ��������� ���
	TVariable varOPERAND (ss_niSupportState);
	string strCOP;
	bool not;
	while (size != pos) { // ������� ���� ��� ����� ���������.

		not = 0;
		strCOP = vBracket[pos]; // �������� ��� ��������.
		pos+=1;                 // �������� pos.
		if (                    // ���������, �������� �� ���.
		// ��� ��� �������� �� ReedCustomObj ��� ���������������
		// ������������� �������������� ������ � ���������.
			strCOP != "="
		&&  strCOP != "<>"
		&&  strCOP != "<"
		&&  strCOP != ">"
		&&  strCOP != "<="
		&&  strCOP != ">="
		&&  strCOP != "+"
		&&  strCOP != "-"
		&&  strCOP != "*"
		&&  strCOP != "/"
		&&  strCOP != ":"
		&&  strCOP != "**"
		&&  strCOP != "�"
		&&  strCOP != "���"
		)
		throw SyntaxError("���������� ������������ � ��� ����� �������; ��������� ��������", line);

		if (size == pos) // ������� ������, � ������ ����� ���������
			throw SyntaxError("��������� �� ����� ������������ ������", line);

		// ����� ����� ���� ��
		if (vBracket[pos]=="��") {
			not = 1;
			pos++;
		}

//		if (size == pos) // ������� ������, � ������ ����� ���������
//			throw SyntaxError("��������� �� ����� ������������ ������", line);
		// �������� ������� �������:
		varOPERAND = ReedCustomObj(vBracket, hdr, buffer, pos, line);

		if (not) {
			varOPERAND = varOPERAND.CheckNOT(line);
		}
		// � ����� �� ��������� ����:
		varTYPE = CalculateElemExpression(varTYPE, strCOP, varOPERAND, line);
	}

	varTYPE.strName = string("ConstBufferVar#");
	varTYPE.strName += (IntToStr(buffer.vctVariables.size())).c_str();
	buffer.vctVariables.push_back(varTYPE);

	DebugTrace(string("���������� ���������, ���������:"));
	DebugTrace(string("��� �������� ����������: ")+varTYPE.GetStrName());
	DebugTrace(string("���: ")+varTYPE.GetStrType());
	DebugTrace(string("@CheckCOP exit"));
	return varTYPE.strName;
}

// �������� ��������� ������ �� ������� �� ��� �������� � ������
void ReplaceBracket(deque<string>& vct, const deque<string>& vBracket,
					const int MostHighBracketPos, const string& str) {
	DebugTrace(string("@ReplaceBracket"));
	DebugTrace(string("������ ��������� �� ��� ����������� ���..."));

	DebugTrace(string("�������� ���������:"));
	for (unsigned i = 0; i < vct.size(); i++) {
		DebugTrace(string("  ")+IntToStr(i+1).c_str()+") "+vct[i]);
	}
	DebugTrace(string("������� ������ ���������: ")+IntToStr(MostHighBracketPos+1).c_str());

	deque<string> dq;
	for (int i = 0; i < MostHighBracketPos; i++) {
		dq.push_back(vct[i]);
	}
	dq.push_back(str);

	int offset = MostHighBracketPos+vBracket.size()+2; // 2 - � ������ ������
	DebugTrace(string("������� ����� ���������: ")+IntToStr(offset).c_str());

	for (unsigned i = offset; i < vct.size(); i++) {
		dq.push_back(vct[i]);
	}
	vct = dq;
	DebugTrace(string("��������� ������:"));
	for (unsigned i = 0; i < vct.size(); i++) {
		DebugTrace(string("  ")+IntToStr(i+1).c_str()+") "+vct[i]);
	}
	DebugTrace(string("@ReplaceBracket exit"));
}

/////////////////////////////////////////////////////////////////////////////
// �������� ������� ���������� ���������, ���������� ��� ����������
TVariable CheckFullExpression (TVarHeader& hdr, TVarHeader& buffer,
							 deque<string>& vct/*Expression*/, int line) {
	DebugTrace(string("@CheckFullExpression"));
	DebugTrace(string("====== �������� ���������� ���������... ======"));
	DebugTrace(string("�������� ���������"));
	for (unsigned i = 0; i < vct.size(); i++) {
		DebugTrace(string("  ")+IntToStr(i+1).c_str()+") "+vct[i]);
	}

	string str;
	TranslateConstToVar(vct, buffer, line);
	VerifyBrakets(vct, line);
	vct.push_front("(");
	vct.push_back(")");

	int MostHighBracketPos = 0;
	deque<string> vBracket;
	while (BracketsPresents(vct)) {
		FindMostPriorityBracket(vct,vBracket,MostHighBracketPos);
		str = CheckCOP(vBracket, hdr, buffer, line);
		ReplaceBracket(vct,vBracket,MostHighBracketPos, str);    
	}

	DebugTrace(string("��������� ���������:"));
	DebugTrace(string("  ��� �������� ����������: ")+vct[0]);
	DebugTrace(string("  ���: ")+buffer.GetVariable(vct[0]).GetStrType());
	DebugTrace(string("====== �������� ���������� ��������� ���������, ������ �� ������� ======"));
	DebugTrace(string("@CheckFullExpression exit"));
	return buffer.GetVariable(vct[0]);
}


// !!!!!!!!!!!!!!!!!!!!! DebugTrace(string(""));

///////////////////////// CmdCopyExpression /////////////////////////////////
//---------------------------------------------------------------------------
CmdCopyExpression::CmdCopyExpression(TVarHeader& hdr, LecsemManager& lecsGen) {

	string str = lecsGen.GetNext();
	line = lecsGen.line;
	if (!hdr.IsPresent(str))
		throw SyntaxError(string("���������� ")+str+" �� ���������", lecsGen.line);

	TVariable* var;
	var = hdr.GetVariablePTR(str);
	strVarOriginal = str;         // REPLACED

	if (var->varType == vtArray) {
		if ( lecsGen.GetNext() != "[" )
		throw SyntaxError("������� ���������� ������������ �������� ��� ��������, ����������� ������ ��������������", line);

		deque<string>que;
		int n = 1;
		while (1) {
			str = lecsGen.GetNext();
			if (str == "#")
				throw SyntaxError("������ �������� � ������� �� ������, ��������� \']\'", line);
			if (str == "]")
				n--;
			if (str == "[")
				n++;
			if (n)
				que.push_back(str);
			else
				break;
		}

		TVariable varI(ss_niSupportState);
		queI = que;
		varI = CheckFullExpression(hdr, buf, que, line);
		var = new TVariable(var->CheckIndexing(varI, line));
	}

	if (var->varType == vtMatrix) {
		if ( lecsGen.GetNext() != "[" )
		throw SyntaxError("������� ���������� ������������ �������� ��� ��������, ����������� ������ ��������������", line);

		deque<string>que;
		while (1) {
			str = lecsGen.GetNext();
			if (str == "#")
				throw SyntaxError("������ ������ �������� � ������� �� ������, ��������� \',\'", line);
			if (str != ",")
				que.push_back(str);
			else
				break;
		}
		TVariable varI(ss_niSupportState);
		queI = que;
		varI = CheckFullExpression(hdr, buf, que, line);

		que.clear();
		int n = 1;
		while (1) {
			str = lecsGen.GetNext();
			if (str == "#")
				throw SyntaxError("������ ������� �������� � ������� �� ������, ��������� \']\'", line);
			if (str == "]")
				n--;
			if (str == "[")
				n++;
			if (n)
				que.push_back(str);
			else
				break;
		}

		// buf.vctVariables.clear();
		queJ = que;
		TVariable varJ = CheckFullExpression(hdr, buf, que, line);
		var = new TVariable(var->CheckIndexing(varI, varJ, line));
	}

	str = lecsGen.GetNext();
	if ( str != ":=" )
		throw SyntaxError(string("��������� \':=\', �������: \'")+str+"\'", lecsGen.line);

	if (lecsGen.line != line)
		throw SyntaxError("��������� ������ ��������� ������������� �� ����� ������", lecsGen.line);

	exp = new CmdExpression(hdr, lecsGen);

	if (exp->line != line) {
		delete exp;
		throw SyntaxError("��������� ������ ��������� ������������� �� ����� ������", lecsGen.line);
	}

	// strVarOriginal = var->GetStrName(); // WAS
	var->CheckCopy(*(exp->pvarStaticType), line);
}

//---------------------------------------------------------------------------
CmdCopyExpression::~CmdCopyExpression() {
	delete exp;
}

/////////////////////////// CmdExpression ///////////////////////////////////
//---------------------------------------------------------------------------
int FillExpression (deque<string>& vct, LecsemManager& lecsGen){
	int line;
	vct.clear();
	string str;
	bool bFirst = 1;
	int ln = lecsGen.line;
	int ls = lecsGen.lecs;

	do{
		ln = lecsGen.line;
		ls = lecsGen.lecs;

		str = lecsGen.GetNext();
		if (bFirst)
			line = lecsGen.line;
		bFirst = 0;

		if (lecsGen.line != line || (IsKeyword(str) && str!="����" && str!="������")) {
			lecsGen.line = ln;
			lecsGen.lecs = ls;
			break;
		}

		vct.push_back(str);
	}while(1);
	return line;
}

CmdExpression::CmdExpression(TVarHeader& hdr, LecsemManager& lecsGen) {
	pvarExecuteResult = 0;
	line = FillExpression(vct, lecsGen);
	vctEXCEPTION = vct;
	deque<string> vctX = vct;
	pvarStaticType = new TVariable(CheckFullExpression(hdr, buffer, vctX, line));
	pvarExecuteResult = 0;
}

CmdExpression::~CmdExpression() {
	delete pvarStaticType;
	delete pvarExecuteResult;
}
//---------------------------------------------------------------------------

////////////////////////// CmdAlgExpression /////////////////////////////////

bool IsClearStr(LecsemManager& lecsGen) {

	int ln = lecsGen.line;
	int ls = lecsGen.lecs;
	int line1 = lecsGen.line;
	lecsGen.GetNext();
	int line2 = lecsGen.line;
	lecsGen.line = ln;
	lecsGen.lecs = ls;

	if (line1 == line2)
		return 0;

	return 1;
}
//---------------------------------------------------------------------------

CmdAlgExpression::CmdAlgExpression(TVarHeader& hdrHost, LecsemManager& lecsGen) {
	string str;
	line = FillExpression(vct, lecsGen);

	// � vct - ���� ������

	TVarHeader buffer;
	int pos = 0;
	TVariable var (ss_niSupportState);
	var = ReedAlgorithmObj(vct, hdrHost, buffer, pos, line);
	if (pos != vct.size())
		throw SyntaxError("��� ��������� ������ ���������(��� ���������) ����� ��� ������ ������� ��� ���������", line);
}

/////////////////////////// CmdIfExpression /////////////////////////////////

CmdIfExpression::CmdIfExpression(TVarHeader& hdrHost, LecsemManager& lecsGen) {
	lecsGen.GetNext();
	string str;
	nCurrentCmd = -1;
	line = lecsGen.line;
	int nOldLine = lecsGen.line;
	while (1) {
		str = lecsGen.GetNext();
		if ( str == "���" ) {
			throw SyntaxError("������� ����������� ���� �� �������, ��������� \'��\', ������� \'���\'", lecsGen.line);
		}
		if ( str == "��" ) {
			break;		// EQVAL
		}
		if ( nOldLine != lecsGen.line ) {
			if ( str == "��" ) {
				break;	// EQVAL
			}
			else
				throw SyntaxError(
string("������� ����������� ���� ������ ������������� �� ����� ������, �� ������ ������ �����\
 ������������� ���� �������� ����� \'��\', �� ������ ���� ������� \'")+str+"\'"
 , lecsGen.line);
		}
	   deqCondition.push_back(str);
	}

	deque<string> l_deqCondition = deqCondition;
	TVariable var (ss_niSupportState);
	var = CheckFullExpression (hdrHost, buffer, l_deqCondition, lecsGen.line);

	if ( var.varType != vtBool )
		throw SyntaxError(string("������� ��������� ���� ������ ����� ��� ���, ��������� ��������� ����� ��� ")+var.GetStrType(), lecsGen.line);

	// FILL THE BODY
	while (1) {

		DebugTrace(string("��������� ����: ����� ����� �������..."));

		int ln = lecsGen.line;
		int ls = lecsGen.lecs;

		str = lecsGen.GetNext();
		DebugTrace(string("���� ������������� \'")+str+"\'");

		if (str == "���") {
			DebugTrace(string("������ �� �������"));
			break;
		}
		if (str == "�����") {
			DebugTrace(string("������� �����"));
			break;
		}
		if (str == "���") {
			DebugTrace(string("������� ���"));
			throw SyntaxError("����������� ���� ������ ������������� �������� ������ \'���\', ������� \'���\'", lecsGen.line);
		}

		lecsGen.line = ln;
		lecsGen.lecs = ls;

		vctSubCommands.push_back(ReedCommand(str, hdrHost, lecsGen));
	}

	if (str == "�����") {
		while (1) {
			DebugTrace(string("��������� ����: ����� ����� �������..."));

			int ln = lecsGen.line;
			int ls = lecsGen.lecs;

			str = lecsGen.GetNext();
			DebugTrace(string("���� ������������� \'")+str+"\'");

			if (str == "���") {
				DebugTrace(string("������ �� �������"));
				break;
			}
			if (str == "�����") {
				DebugTrace(string("������� �����"));
				throw SyntaxError("�� ����������� ��������� ������ ����� � ����������� ����", lecsGen.line);
			}
			if (str == "���") {
				DebugTrace(string("������� ���"));
				throw SyntaxError("����������� ���� ������ ������������� �������� ������ \'���\', ������� \'���\'", lecsGen.line);
			}

			lecsGen.line = ln;
			lecsGen.lecs = ls;

			vctElseSubCommands.push_back(ReedCommand(str, hdrHost, lecsGen));
		}
	}
}
//---------------------------------------------------------------------------

CmdWhileExpression::CmdWhileExpression(TVarHeader& hdrHost, LecsemManager& lecsGen) {
	lecsGen.GetNext();
	string str;
	nCurrentCmd = -1;
	line = lecsGen.line;
	int nOldLine = lecsGen.line;
	while (1) {
		str = lecsGen.GetNext();
		if ( str == "���" ) {
			throw SyntaxError("������� ����������� ���� �� �������, ��������� \'��\', ������� \'���\'", lecsGen.line);
		}
		if ( str == "��" ) {
			break;		// EQVAL
		}
		if ( nOldLine != lecsGen.line ) {
			if ( str == "��" ) {
				break;	// EQVAL
			}
			else
				throw SyntaxError(
string("������� ����������� ���� ������ ������������� �� ����� ������, �� ������ ������ �����\
 ������������� ���� �������� ����� \'��\', �� ������ ���� ������� \'")+str+"\'"
 , lecsGen.line);
		}
	   deqCondition.push_back(str);
	}

	deque<string> l_deqCondition = deqCondition;
	TVariable var (ss_niSupportState);
	var = CheckFullExpression (hdrHost, buffer, l_deqCondition, lecsGen.line);

	if ( var.varType != vtBool )
		throw SyntaxError(string("������� ����� ���� ������ ����� ��� ���, ��������� ��������� ����� ��� ")+var.GetStrType(), lecsGen.line);

	// FILL THE BODY
	while (1) {

		DebugTrace(string("���� ����: ����� ����� �������..."));

		int ln = lecsGen.line;
		int ls = lecsGen.lecs;

		str = lecsGen.GetNext();
		DebugTrace(string("���� ������������� \'")+str+"\'");

		if (str == "��") {
			DebugTrace(string("������ �� �������"));
			break;
		}

		if (str == "���") {
			DebugTrace(string("������� ���"));
			throw SyntaxError("����������� ���� ������ ������������� �������� ������ \'��\', ������� \'���\'", lecsGen.line);
		}

		lecsGen.line = ln;
		lecsGen.lecs = ls;

		vctSubCommands.push_back(ReedCommand(str, hdrHost, lecsGen));
	}

}
//---------------------------------------------------------------------------

