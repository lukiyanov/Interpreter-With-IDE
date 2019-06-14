//---------------------------------------------------------------------------
#pragma hdrstop
#include "VarHeader.h"
#include "Emulator.h"
#include <windows.h>
#include "Trace.h"
#pragma package(smart_init)

//---------------------------------------------------------------------------
TVarHeader::TVarHeader(){

}

//---------------------------------------------------------------------------
void TVarHeader::PushAlgName(const string& strName, int line){
	for (int i = 0; i < Emulator.algs.size(); i++){
		if (Emulator.algs[i]->GetName() == strName)
			throw SyntaxError(string("Алгоритм с именем \'")+strName+"\' уже был описан выше по тексту алгоритма", line);
	}

	vctVariables.push_back(TVariable(strName, ::vtString, line));
	vctVariables[0].varState = vsAlgName;
}

//---------------------------------------------------------------------------
string TVarHeader::GetAlgName(){
	for (int i = 0; i < vctVariables.size(); i++)
		if (vctVariables[i].varState == vsAlgName)
			return vctVariables[i].strName;
}

//---------------------------------------------------------------------------
int TVarHeader::GetPosition(const string& Name) {
	for (int i = 0; i < vctVariables.size(); i++)
		if (vctVariables[i].GetStrName() == Name) return i;
	return -1;
}
//---------------------------------------------------------------------------
void TVarHeader::PushVar(TVariable& Type, const string& strName, int line) {
	CheckName(strName, line);
	if (GetPosition(strName) != -1)
		throw SyntaxError( string("Переменная с именем ")+strName+" алгоритма "+GetAlgName()+" уже была описана ранее", line);
	vctVariables.push_back(TVariable(strName,Type,line));
	/* TODO -oAZE : IS IT WRONG? */
	vctVariables[vctVariables.size()].bStaticInit = 0;
	vctVariables[vctVariables.size()].bDynamicInit = 0;
}
//---------------------------------------------------------------------------
void TVarHeader::ToArgument(const string& strName, int line) {
	CheckName(strName, line);
	int n;
	if ((n = GetPosition(strName)) == -1)
		throw SyntaxError( string("Аргумент \'")+strName+"\' в алгоритме "+GetAlgName()+" не объявлен", line);
	vctVariables[n].varState = vsArgument;
	vctVariables[n].bStaticInit = 1;
}

//---------------------------------------------------------------------------
void TVarHeader::ToResult(const string& strName, int line) {
	CheckName(strName, line);
	int n;
	if ((n = GetPosition(strName)) == -1)
		throw SyntaxError( string("Результат \'")+strName+"\' в алгоритме "+GetAlgName()+" не объявлен", line);
	if (vctVariables[n].varState == vsArgument)
		throw SyntaxError( string("Переменная \'")+strName+"\' уже зарезервирована как аргумент алгоритма "+GetAlgName()+", её невозможно сделать результирующей", line);
	vctVariables[n].varState = vsResult;
}

//---------------------------------------------------------------------------
void TVarHeader::CheckName(const string& strName, int line) {
	if (strName == GetAlgName())
		throw SyntaxError( string("Не допускается использования имени алгоритма в его же заголовке (")+strName+")", line);

	for (int i = 0; i < Emulator.algs.size(); i++){
		if (Emulator.algs[i]->GetName() == strName)
			throw SyntaxError(string("Имя переменной \'")+strName+"\' совпадает с именем одного из алгоритмов", line);
	}
}

//---------------------------------------------------------------------------
void TVarHeader::ToExc(const string& strName) {
	vctVariables[GetPosition(strName)].varState = vsException;
}

//---------------------------------------------------------------------------
bool TVarHeader::IsPresent(const string& Name) {
	for (int i = 0; i < vctVariables.size(); i++)
		if (vctVariables[i].GetStrName() == Name) return 1;
	return 0;
}
//---------------------------------------------------------------------------

TVariable& TVarHeader::GetVariable(const string& Name) {
	for (int i = 0; i < vctVariables.size(); i++)
		if (vctVariables[i].GetStrName() == Name)
			return vctVariables[i];

	throw RuntimeError(string("Запрошена неизвестная переменная \'")
	+Name+"\' алгоритма \'"+GetAlgName()+"\'");
}

TVariable* TVarHeader::GetVariablePTR(const string& Name) {
	for (int i = 0; i < vctVariables.size(); i++)
		if (vctVariables[i].GetStrName() == Name)
			return &vctVariables[i];

	throw RuntimeError(string("Запрошена неизвестная переменная \'")
	+Name+"\' алгоритма \'"+GetAlgName()+"\'");
}

//---------------------------------------------------------------------------

bool TVarHeader::IsResultPresent() {
	for (int i = 0; i < vctVariables.size(); i++)
		if (vctVariables[i].varState == vsResult)
			return 1;
	return 0;
}

//---------------------------------------------------------------------------
TVariable TVarHeader::GetResult() {
	for (int i = 0; i < vctVariables.size(); i++)
		if (vctVariables[i].varState == vsResult)
			return vctVariables[i];

	throw RuntimeError("ОШИБКА ДВИЖКА ИНТЕРПРЕТАТОРА: Запрошенный результат не найден(TVarHeader::GetResult)");
}
//---------------------------------------------------------------------------

bool TVarHeader::IsExceptionPresent() {
	for (int i = 0; i < vctVariables.size(); i++)
		if (vctVariables[i].varState == vsException)
			return 1;
	return 0;

}
//---------------------------------------------------------------------------

TVariable TVarHeader::GetException() {
	for (int i = 0; i < vctVariables.size(); i++)
		if (vctVariables[i].varState == vsException)
			return vctVariables[i];

	throw RuntimeError("ОШИБКА ДВИЖКА ИНТЕРПРЕТАТОРА: Запрошенное исключение не найдено(TVarHeader::GetEsception)");
}
//---------------------------------------------------------------------------

void TVarHeader::Print() {
	for (int i = 0; i < vctVariables.size(); i++) {
		DebugTrace(string()+IntToStr(i).c_str()+") NAME: "+vctVariables[i].strName
		+", TYPE: "+vctVariables[i].GetStrType()+", VALUE: "+vctVariables[i].GetStrValue());
	}
}
//---------------------------------------------------------------------------

