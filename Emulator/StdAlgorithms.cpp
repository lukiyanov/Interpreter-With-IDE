//---------------------------------------------------------------------------
#include "StdAlgorithms.h"
#include "UConsole.h"
#include <math.h>
#include "UnitMainForm.h"

//-------------------------------------------------------------------------//
//							 TSDTAlg_OUT								   //
//-------------------------------------------------------------------------//
void TStrdAlg_OUT::CheckCall(const deque<TVariable*>& params, int line) {
	// Нет проверки - мы выводим всё!
}

TVariable TStrdAlg_OUT::Call(const deque<TVariable>& params) {  // В одну строку
	string str;
	fmConsole->Memo1->Lines->Add(String());
	fmMainView->Memo4->Lines->Add(String());
	for (int i = 0; i < params.size(); i++) {
		fmConsole->WriteVar(params[i]);
	}

	TVariable var(ss_niSupportState);
	var.varType = vtVoid;
	var.bDynamicInit = 1;
	var.varState = vsExpression;
	return var;
}

TStrdAlg_OUT::TStrdAlg_OUT() {
	Header.PushAlgName("ВЫВОД", 0);
	bStdAlg = 1;
}

TVarDataType TStrdAlg_OUT::GetResultType() {
	return vtVoid;
}

//-------------------------------------------------------------------------//
//							 TSDTAlg_OutStr								   //
//-------------------------------------------------------------------------//
void TStrdAlg_OutStr::CheckCall(const deque<TVariable*>& params, int line) {
	// Нет проверки - мы выводим всё!
}

TVariable TStrdAlg_OutStr::Call(const deque<TVariable>& params) {  // В одну строку
	string str;
	for (int i = 0; i < params.size(); i++) {
		fmConsole->WriteVar(params[i]);
	}

	TVariable var(ss_niSupportState);
	var.varType = vtVoid;
	var.bDynamicInit = 1;
	var.varState = vsExpression;
	return var;
}

TStrdAlg_OutStr::TStrdAlg_OutStr() {
	Header.PushAlgName("ВЫВОДСТР", 0);
	bStdAlg = 1;
}

TVarDataType TStrdAlg_OutStr::GetResultType() {
	return vtVoid;
}

//-------------------------------------------------------------------------//
//							 TSDTAlg_Sin								   //
//-------------------------------------------------------------------------//
void TStrdAlg_Sin::CheckCall(const deque<TVariable*>& params, int line) {
	if (params.size() != 1) {
		throw SyntaxError(string("Несоответствие количества параметров при вызове функции ")
		+GetName()+": ожидается 1, найдено "+IntToStr(params.size()).c_str(), line);
	}

	TVariable var(ss_niSupportState);
	var.varType = vtDouble;
	var.varState = vsLocal;
	var.CheckCopy(*params[0], line);
}

TVariable TStrdAlg_Sin::Call(const deque<TVariable>& params) {
	TVariable var(ss_niSupportState);
	var.varType = vtDouble;
	var.bDynamicInit = 1;
	var.varState = vsExpression;
	var.Copy(params[0], 0);
	var.Value.dblValue = sin(var.Value.dblValue);
	return var;
}

TStrdAlg_Sin::TStrdAlg_Sin() {
	Header.PushAlgName("SIN", 0);
	TVariable var("a", vtDouble, 0);
	Header.PushVar(var, "a", 0);
	Header.ToResult("a", 0);
	bStdAlg = 1;
}

TVarDataType TStrdAlg_Sin::GetResultType() {
	return vtDouble;
}

//-------------------------------------------------------------------------//
//							 TSDTAlg_Cos								   //
//-------------------------------------------------------------------------//
void TStrdAlg_Cos::CheckCall(const deque<TVariable*>& params, int line) {
	if (params.size() != 1) {
		throw SyntaxError(string("Несоответствие количества параметров при вызове функции ")
		+GetName()+": ожидается 1, найдено "+IntToStr(params.size()).c_str(), line);
	}

	TVariable var(ss_niSupportState);
	var.varType = vtDouble;
	var.varState = vsLocal;
	var.CheckCopy(*params[0], line);
}

TVariable TStrdAlg_Cos::Call(const deque<TVariable>& params) {
	TVariable var(ss_niSupportState);
	var.varType = vtDouble;
	var.bDynamicInit = 1;
	var.varState = vsExpression;
	var.Copy(params[0], 0);
	var.Value.dblValue = cos(var.Value.dblValue);
	return var;
}

TStrdAlg_Cos::TStrdAlg_Cos() {
	Header.PushAlgName("COS", 0);
	TVariable var("a", vtDouble, 0);
	Header.PushVar(var, "a", 0);
	Header.ToResult("a", 0);
	bStdAlg = 1;
}

TVarDataType TStrdAlg_Cos::GetResultType() {
	return vtDouble;
}


//-------------------------------------------------------------------------//
//							 TSDTAlg_Round								   //
//-------------------------------------------------------------------------//
void TStrdAlg_Round::CheckCall(const deque<TVariable*>& params, int line) {
	if (params.size() != 1) {
		throw SyntaxError(string("Несоответствие количества параметров при вызове функции ")
		+GetName()+": ожидается 1, найдено "+IntToStr(params.size()).c_str(), line);
	}

	TVariable var(ss_niSupportState);
	var.varType = vtDouble;
	var.varState = vsLocal;
	var.CheckCopy(*params[0], line);
}

int ROUND( double x ) {
	int z = 1;
	if ( x < 0 ) z = -1;

	x = fabs(x);
	double c;
	double n = modf(x, &c);

	if ( n < 0.5 ) return c*z;
	else         return (c+1)*z;
}

TVariable TStrdAlg_Round::Call(const deque<TVariable>& params) {
	TVariable var(ss_niSupportState);
	var.varType = vtInt;
	var.bDynamicInit = 1;
	var.varState = vsExpression;
	var.Copy(params[0], 0);
	if (params[0].varType == vtDouble)
		var.Value.intValue = ROUND(params[0].Value.dblValue);
	var.varType = vtInt;
	return var;
}

TStrdAlg_Round::TStrdAlg_Round() {
	Header.PushAlgName("ОКРУГЛ", 0);
	TVariable var("a", vtInt, 0);
	Header.PushVar(var, "a", 0);
	Header.ToResult("a", 0);
	bStdAlg = 1;
}

TVarDataType TStrdAlg_Round::GetResultType() {
	return vtInt;
}

//-------------------------------------------------------------------------//
//							 TStrdAlg_Length							   //
//-------------------------------------------------------------------------//
void TStrdAlg_Length::CheckCall(const deque<TVariable*>& params, int line) {
	if (params.size() != 1) {
		throw SyntaxError(string("Несоответствие количества параметров при вызове функции ")
		+GetName()+": ожидается 1, найдено "+IntToStr(params.size()).c_str(), line);
	}

	TVariable var(ss_niSupportState);
	var.varType = ::vtString;
	var.varState = vsLocal;
	var.CheckCopy(*params[0], line);
}

TVariable TStrdAlg_Length::Call(const deque<TVariable>& params) {
	TVariable var(ss_niSupportState);
	var.varType = vtInt;
	var.bDynamicInit = 1;
	var.varState = vsExpression;
	var.Value.intValue = params[0].Value.strValue.size();
	return var;
}

TStrdAlg_Length::TStrdAlg_Length() {
	Header.PushAlgName("ДЛИНА", 0);
	TVariable var("a", vtInt, 0);
	Header.PushVar(var, "a", 0);
	Header.ToResult("a", 0);
	bStdAlg = 1;
}

TVarDataType TStrdAlg_Length::GetResultType() {
	return vtInt;
}

//-------------------------------------------------------------------------//
//							 TStrdAlg_StrGet							   //
//-------------------------------------------------------------------------//
void TStrdAlg_StrGet::CheckCall(const deque<TVariable*>& params, int line) {
	if (params.size() != 2) {
		throw SyntaxError(string("Несоответствие количества параметров при вызове функции ")
		+GetName()+": ожидается 2, найдено "+IntToStr(params.size()).c_str(), line);
	}

	TVariable var(ss_niSupportState);
	var.varType = ::vtString;
	var.varState = vsLocal;
	var.CheckCopy(*params[0], line);

	var.varType = vtInt;
	var.CheckCopy(*params[1], line);
}

TVariable TStrdAlg_StrGet::Call(const deque<TVariable>& params) {
	if (params[1].Value.intValue < 1)
		throw RuntimeError(string("Произошло обращение к символу строки с индексом меньше 1: ")
		+IntToStr(params[1].Value.intValue).c_str());
	if (params[1].Value.intValue > params[0].Value.strValue.size())
		throw RuntimeError(string("Произошло обращение к символу строки с индексом больше максимального: ")
		+IntToStr(params[1].Value.intValue).c_str());

	TVariable var(ss_niSupportState);
	var.varType = ::vtChar;
	var.bDynamicInit = 1;
	var.varState = vsExpression;
	var.Value.chrValue = params[0].Value.strValue[params[1].Value.intValue-1];
	return var;
}

TStrdAlg_StrGet::TStrdAlg_StrGet() {
	Header.PushAlgName("СИМИЗ", 0);
	TVariable var("a", ::vtChar, 0);
	Header.PushVar(var, "a", 0);
	Header.ToResult("a", 0);
	bStdAlg = 1;
}

TVarDataType TStrdAlg_StrGet::GetResultType() {
	return ::vtChar;
}

//-------------------------------------------------------------------------//
//							 TStrdAlg_StrSet							   //
//-------------------------------------------------------------------------//
void TStrdAlg_StrSet::CheckCall(const deque<TVariable*>& params, int line) {
	if (params.size() != 3) {
		throw SyntaxError(string("Несоответствие количества параметров при вызове функции ")
		+GetName()+": ожидается 3, найдено "+IntToStr(params.size()).c_str(), line);
	}

	TVariable var(ss_niSupportState);
	var.varType = ::vtString;
	var.varState = vsLocal;
	var.CheckCopy(*params[0], line);

	var.varType = vtInt;
	var.CheckCopy(*params[1], line);

	var.varType = ::vtChar;
	var.CheckCopy(*params[2], line);
}

TVariable TStrdAlg_StrSet::Call(const deque<TVariable>& params) {
	if (params[1].Value.intValue < 1)
		throw RuntimeError(string("Произошло обращение к символу строки с индексом меньше 1: ")
		+IntToStr(params[1].Value.intValue).c_str());
	if (params[1].Value.intValue > params[0].Value.strValue.size())
		throw RuntimeError(string("Произошло обращение к символу строки с индексом больше максимального: ")
		+IntToStr(params[1].Value.intValue).c_str());

	TVariable var(ss_niSupportState);
	var.varType = ::vtString;
	var.bDynamicInit = 1;
	var.varState = vsExpression;
	string str = params[0].Value.strValue;
	str[params[1].Value.intValue-1] = params[2].Value.chrValue;
	var.Value.strValue = str;
	return var;
}

TStrdAlg_StrSet::TStrdAlg_StrSet() {
	Header.PushAlgName("СТРС", 0);
	TVariable var("a", ::vtString, 0);
	Header.PushVar(var, "a", 0);
	Header.ToResult("a", 0);
	bStdAlg = 1;
}

TVarDataType TStrdAlg_StrSet::GetResultType() {
	return ::vtString;
}

/*
//-------------------------------------------------------------------------//
//							 TStrdAlg_GetRows							   //
//-------------------------------------------------------------------------//
void TStrdAlg_GetRows::CheckCall(const deque<TVariable*>& params, int line) {
	if (params.size() != 1) {
		throw SyntaxError(string("Несоответствие количества параметров при вызове функции ")
		+GetName()+": ожидается 1, найдено "+IntToStr(params.size()).c_str(), line);
	}

	if (params[0]->varType != vtArray && params[0]->varType == vtMatrix)
		throw SyntaxError(string("В качестве аргумента функции ожидается массив или матрица, найдено ")+params[0]->GetStrType(), line);
}

TVariable TStrdAlg_GetRows::Call(const deque<TVariable>& params) {
	TVariable var(ss_niSupportState);
	var.varType = vtInt;
	var.bDynamicInit = 1;
	var.varState = vsExpression;

	if (params[0].varType == vtArray)
		var.Value.intValue = 1;
	if (params[0].varType == vtMatrix)
		var.Value.intValue = params[0].Value.mtxValue.mtx.size();

	return var;
}

TStrdAlg_GetRows::TStrdAlg_GetRows() {
	Header.PushAlgName("СТРОК", 0);
	TVariable var("a", vtInt, 0);
	Header.PushVar(var, "a", 0);
	Header.ToResult("a", 0);
	bStdAlg = 1;
}

TVarDataType TStrdAlg_GetRows::GetResultType() {
	return vtInt;
}

//-------------------------------------------------------------------------//
//							 TStrdAlg_GetCols							   //
//-------------------------------------------------------------------------//
void TStrdAlg_GetCols::CheckCall(const deque<TVariable*>& params, int line) {
	if (params.size() != 1) {
		throw SyntaxError(string("Несоответствие количества параметров при вызове функции ")
		+GetName()+": ожидается 1, найдено "+IntToStr(params.size()).c_str(), line);
	}

	if (params[0]->varType != vtArray && params[0]->varType == vtMatrix)
		throw SyntaxError(string("В качестве аргумента функции ожидается массив или матрица, найдено ")+params[0]->GetStrType(), line);
}

TVariable TStrdAlg_GetCols::Call(const deque<TVariable>& params) {
	TVariable var(ss_niSupportState);
	var.varType = vtInt;
	var.bDynamicInit = 1;
	var.varState = vsExpression;

	if (params[0].varType == vtArray)
		var.Value.intValue = params[0].Value.arrValue.arr.size();
	if (params[0].varType == vtMatrix)
		var.Value.intValue = params[0].Value.mtxValue.mtx[0].size();

	return var;
}

TStrdAlg_GetCols::TStrdAlg_GetCols() {
	Header.PushAlgName("СТОЛБЦОВ", 0);
	TVariable var("a", vtInt, 0);
	Header.PushVar(var, "a", 0);
	Header.ToResult("a", 0);
	bStdAlg = 1;
}

TVarDataType TStrdAlg_GetCols::GetResultType() {
	return vtInt;
} */

//-------------------------------------------------------------------------//
//							 	FILL									   //
//-------------------------------------------------------------------------//
TStrdAlg_OUT		m_StrdAlg_OUT;
TStrdAlg_OutStr		m_StrdAlg_OutStr;
TStrdAlg_Sin		m_StrdAlg_Sin;
TStrdAlg_Cos		m_StrdAlg_Cos;
TStrdAlg_Round      m_StrdAlg_Round;
TStrdAlg_Length 	m_StrdAlg_Length;
TStrdAlg_StrGet 	m_StrdAlg_StrGet;
TStrdAlg_StrSet 	m_StrdAlg_StrSet;
//TStrdAlg_GetRows	m_StrdAlg_GetRows;
//TStrdAlg_GetCols	m_StrdAlg_GetCols;
vector<TAlgorithm*> vctStdAlgs;

void STDAlgInit() {
	vctStdAlgs.push_back(&m_StrdAlg_OUT);
	vctStdAlgs.push_back(&m_StrdAlg_OutStr);
	vctStdAlgs.push_back(&m_StrdAlg_Sin);
	vctStdAlgs.push_back(&m_StrdAlg_Cos);
	vctStdAlgs.push_back(&m_StrdAlg_Round);
	vctStdAlgs.push_back(&m_StrdAlg_Length);
	vctStdAlgs.push_back(&m_StrdAlg_StrGet);
	vctStdAlgs.push_back(&m_StrdAlg_StrSet);
//	vctStdAlgs.push_back(&m_StrdAlg_GetRows);
//	vctStdAlgs.push_back(&m_StrdAlg_GetCols);
}

//---------------------------------------------------------------------------

