//---------------------------------------------------------------------------
#pragma hdrstop

#include "Variables.h"
#include "Errors.h"
#include "Emulator.h"
#include <Sysutils.hpp>
#include <math.h>
#include "URunParams.h"
#include "UConsole.h"
#include "StdAlgorithms.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------

const string astrKeywords[] = {
	"АЛГ",
	"АРГ",
	"РЕЗ",
	"НАЧ",
	"КОН",
	"НЕТ",

	"ЛОГ",
	"ЦЕЛ",
	"ВЕЩ",
	"ЛИТ",
	"СИМ",
	"МАСС",
	"МАТР",
	"ТАБ",

	"ИСТИНА",
	"ЛОЖЬ",

	"И",
	"ИЛИ",
	"НЕ",

	"ЕСЛИ",
	"ТО",
	"ИНАЧЕ",
	"ВСЕ",

	"ПОКА",
	"НЦ",
	"КЦ"
};

const char szFigures[] = "0123456789";

//---------------------------------------------------------------------------
bool IsKeyword(const string& word) {
	for (int i = 0; i < sizeof(astrKeywords)/sizeof(string); i++)
		if (word == astrKeywords[i]) return 1;
	return 0;
}

bool IsFigureFirst(const string& word) {
	for (int i = 0; szFigures[i] != '\0'; i++)
		if (word[0] == szFigures[i]) return 1;
	return 0;
}

bool IsDivider(const string& word) {
	for (int i = 0; szDividers[i] != '\0'; i++)
		if (word[0] == szDividers[i]) return 1;
	return 0;
}

//---------------------------------------------------------------------------
TVariable::TVariable(string Name, TVarDataType Type, int Line) {
	TVariable var(ss_niSupportState);
	var.varType = Type;
	Init(Name, var, Line);
}

//---------------------------------------------------------------------------
TVariable::TVariable(string Name, TVariable Type, int Line){
	Init(Name, Type, Line);
}

//---------------------------------------------------------------------------
string TVariable::GetStrName() {
	return strName;
}

string GetType(TVarDataType varType){
	string str("TODO!");
	switch (varType) {
	case vtInt:
		str = "ЦЕЛ";
		break;
	case vtDouble:
		str = "ВЕЩ";
		break;
	case ::vtString:
		str = "ЛИТ";
		break;
	case ::vtChar:
		str = "СИМ";
		break;
	case vtBool:
		str = "ЛОГ";
		break;
	}
	return str;
}

string TVariable::GetStrType() {
	string str("TODO!");
	switch (varType) {
	case vtInt:
		str = "ЦЕЛ";
		break;
	case vtDouble:
		str = "ВЕЩ";
		break;
	case ::vtString:
		str = "ЛИТ";
		break;
	case ::vtChar:
		str = "СИМ";
		break;
	case vtBool:
		str = "ЛОГ";
		break;
	case vtArray:
		str = "МАСС ";
		str+= GetType(Value.arrValue.varElemType);
		str+=" [";
		str+= IntToStr(Value.arrValue.nColCnt).c_str();
		str+=']';
		break;
	case vtMatrix:
		str = "МАТР ";
		str+= GetType(Value.mtxValue.varElemType);
		str+=" [";
		str+= IntToStr(Value.mtxValue.nRowCnt).c_str();
		str+=',';
		str+= IntToStr(Value.mtxValue.nColCnt).c_str();
		str+=']';
		break;
	case vtVoid:
    	str = "НЕТ";
		break;
	}

	return str;
}

string TVariable::GetStrValue() {
	string str("TODO!");
	switch (varType) {
	case vtInt:
		str = IntToStr(Value.intValue).c_str();
		break;
	case vtDouble:
		str = FloatToStr(Value.dblValue).c_str();
		break;
	case ::vtString:
		str = string("\'")+Value.strValue+'\'';
		break;
	case ::vtChar:
		str = string("\'")+Value.chrValue+'\'';
		break;
	case vtBool:
		if (Value.blnValue)
			str = "ИСТИНА";
		else
			str = "ЛОЖЬ";
		break;
	case vtArray:
		str = "{";
		for (int i = 1; i < Value.arrValue.nColCnt; i++) {
			str += Value.arrValue.arr[i-1].GetStrValue();
			str += ", ";
		}
		str += Value.arrValue.arr[Value.arrValue.nColCnt-1].GetStrValue();
		str+='}';
		break;
	case vtMatrix:
		str = "{ ";

		for (int i = 1; i < Value.mtxValue.nRowCnt; i++) {
			str += "{";
			for (int j = 1; j < Value.mtxValue.nColCnt; j++) {
				str += Value.mtxValue.mtx[i-1][j-1].GetStrValue();
				str += ", ";
			}
			str += Value.mtxValue.mtx[i-1][Value.mtxValue.nColCnt-1].GetStrValue();
			str+="}, ";
		}

		str += "{";
		for (int i = 1; i < Value.mtxValue.nColCnt; i++) {
			str += Value.mtxValue.mtx[Value.mtxValue.nRowCnt-1][i-1].GetStrValue();
			str += ", ";
		}
		str += Value.mtxValue.mtx[Value.mtxValue.nRowCnt-1][Value.mtxValue.nColCnt-1].GetStrValue();
		str+="} ";

		str+=" }";
		break;
	}

	return str;
}

//---------------------------------------------------------------------------
void TVariable::Init(string Name, TVariable Type, int Line) {
	if (Name.empty()) // <- На всякий случай
		throw SyntaxError("Имя переменной при описании не задано",Line);
	if (IsKeyword(Name))
		throw SyntaxError(string("Имя переменной совпадает с ключевым словом \'")+Name+'\'',Line);
	if (IsFigureFirst(Name))
		throw SyntaxError(string("Имя переменной \'")+Name+"\' начинается с цифры",Line);
	if (IsDivider(Name))
		throw SyntaxError(string("Имя переменной \'")+Name
		+"\' задано неверно; имя должно состоять из букв, цифр и знаков \'_\', при этом имя переменной не может начинаться с цифры",Line);

	if (Type.varType == vtInt) {
		Type.Value.intValue = 0;
	}
	else if (Type.varType == vtDouble) {
		Type.Value.dblValue = 0.0;
	}
	else if (Type.varType == ::vtString) {
		Type.Value.strValue = ""; // Вообще говоря, не требуется
	}
	else if (Type.varType == ::vtChar) {
		Type.Value.chrValue = ' ';
	}

	else if (Type.varType == vtBool) {
		Type.Value.blnValue = 0;
	}

	else if (Type.varType == vtArray) {
		Type.bStaticInit = 1;
		for (int i = 1; i <= Type.Value.arrValue.nColCnt; i++)
			Type.Value.arrValue.arr.push_back(TVariable("$ARR_ELEM",Type.Value.arrValue.varElemType,Line));
	}


	else if (Type.varType == vtMatrix) {
		Type.bStaticInit = 1;
		for (int i = 1; i <= Type.Value.mtxValue.nRowCnt; i++){
			vector<TVariable> vct;
			for (int j = 1; j <= Type.Value.mtxValue.nColCnt; j++)
				vct.push_back(TVariable("$MTX_ELEM",Type.Value.mtxValue.varElemType,Line));
			Type.Value.mtxValue.mtx.push_back(vct);
		}
	}


	*this = Type;
	strName       =  Name;
	varState      =  vsLocal;
	bStaticInit   =  Type.bStaticInit;
	bDynamicInit  =  Type.bDynamicInit;
}

//___________________________________________________________________________//
//																			 //
//  							 ПРОВЕРКА   								 //
//___________________________________________________________________________//


void TVariable::StdCheck(const TVariable& var, int line) {
	if (var.varState == vsAlgName)
		throw SyntaxError("Имя алгоритма не может участвовать в выражениях", line);
	if (var.varState == vsResult)
		throw SyntaxError("Результат алгоритма не может участвовать в выражениях", line);
	if (var.varState == vsException)
		throw SyntaxError("Признак исключения алгоритма не может участвовать в выражениях", line);

	if (var.varType == vtArray)
		throw SyntaxError("Массивы не могут участвовать в выражениях, используйте операцию индексирования", line);
	if (var.varType == vtMatrix)
		throw SyntaxError("Матрицы не могут участвовать в выражениях, используйте операцию индексирования", line);
	if (!var.bStaticInit)
		throw SyntaxError("Неинициализированные переменные не могут участвовать в выражении", line);
}

///////////////////////////////////////////////////////////////////////////////
//						   	 ПРИСВАИВАНИЕ									 //
///////////////////////////////////////////////////////////////////////////////

void TVariable::CheckCopy(const TVariable& var, int line) {
	if (varState == vsAlgName)
		throw SyntaxError("Имя алгоритма не может участвовать в выражениях", line);

	if (varType == vtArray)
		throw SyntaxError("Массивы не могут участвовать в выражениях, используйте операцию индексирования", line);
	if (varType == vtMatrix)
		throw SyntaxError("Матрицы не могут участвовать в выражениях, используйте операцию индексирования", line);

	StdCheck(var, line);


	if (!(
		 (varType == vtBool 	 && var.varType  == vtBool)
	  || (varType == vtInt  	 && var.varType  == vtInt)
	  || ((varType == vtInt || varType == vtDouble) && (var.varType == vtInt || var.varType == vtDouble) )
	  || (varType == ::vtChar	 && var.varType  == ::vtChar)
	  || (varType == ::vtString	 && (var.varType == ::vtChar || var.varType == ::vtString))
	)) throw SyntaxError(string("Несоответствие типов: ")+GetStrType()+" и "+var.GetStrType(),line);

	bStaticInit = 1;
}

///////////////////////////////////////////////////////////////////////////////
//							   ИНДЕКСИРОВАНИЕ								   //
///////////////////////////////////////////////////////////////////////////////

TVariable TVariable::CheckIndexing(const TVariable& varI, int line) {
	StdCheck(varI, line);
	if (varType != vtArray && varType != ::vtString)
		throw SyntaxError(string("Операция индексирования неприменима к переменной типа ")+GetStrType(), line);

	if (varI.varType != vtInt)
		throw SyntaxError(string("Индекс ")+GetStrName()+" указан неверно: ожидается выражение типа ЦЕЛ, найдено "+varI.GetStrType(), line);

	if (varType == vtArray) {
		Value.arrValue.arr[0].bStaticInit = bStaticInit;
		return TVariable(Value.arrValue.arr[0]);
	}
	if (varType == ::vtString) {
		TVariable var (ss_niSupportState);
		var.varType = ::vtChar;
		var.varState = vsLocal;
		var.bStaticInit = 1;
		return var;
	}
}

TVariable TVariable::CheckIndexing(const TVariable& varI, const TVariable& varJ, int line) {
	StdCheck(varI, line);
	StdCheck(varJ, line);
	if (varI.varType != vtInt)
		throw SyntaxError(string("Индекс строки элемента в матрице ")+GetStrName()+" указан неверно: ожидается выражение типа ЦЕЛ, найдено "+varI.GetStrType(), line);
	if (varJ.varType != vtInt)
		throw SyntaxError(string("Индекс столбца элемента в матрице ")+GetStrName()+" указан неверно: ожидается выражение типа ЦЕЛ, найдено "+varI.GetStrType(), line);

	Value.mtxValue.mtx[0][0].bStaticInit = bStaticInit;
	return TVariable(Value.mtxValue.mtx[0][0]);
}

///////////////////////////////////////////////////////////////////////////////
//							   ВЫЧИСЛЕНИЯ									   //
///////////////////////////////////////////////////////////////////////////////

TVariable TVariable::CheckAdd(const TVariable& var, int line) {
	TVariable res(*this);
	StdCheck(*this, line);
	StdCheck(var, line);
	if (!(

	// Строки
	   ( varType == ::vtString && var.varType == ::vtChar  )
	|| ( varType == ::vtString && var.varType == ::vtString  )

	// Числа
	|| ( 	  (varType == vtInt || varType == vtDouble)
		   && (var.varType == vtInt || var.varType == vtDouble)
	   )
	))
	throw SyntaxError(string("Несоответствие типов: невозможно добавить(+) к значению типа ")
	+GetStrType()+" значение типа "+var.GetStrType(), line);

	res.bStaticInit = 1;
	res.varState = vsExpression;
	if ( var.varType == vtDouble )
		res.varType = vtDouble;

	return res;
}

TVariable TVariable::CheckSub(const TVariable& var, int line) {
	TVariable res(*this);
	StdCheck(*this, line);
	StdCheck(var, line);
	if (!(

	// Числа
		  (varType == vtInt || varType == vtDouble)
		   && (var.varType == vtInt || var.varType == vtDouble)
	))
	throw SyntaxError(string("Несоответствие типов: невозможно вычесть(-) из значения типа ")
	+GetStrType()+" значение типа "+var.GetStrType(), line);

	res.bStaticInit = 1;
	res.varState = vsExpression;
	if ( var.varType == vtDouble )
		res.varType = vtDouble;

	return res;
}


TVariable TVariable::CheckMul(const TVariable& var, int line) {
	TVariable res(*this);
	StdCheck(*this, line);
	StdCheck(var, line);
	if (!(

	// Числа
		  (varType == vtInt || varType == vtDouble)
		   && (var.varType == vtInt || var.varType == vtDouble)
	))
	throw SyntaxError(string("Несоответствие типов: невозможно умножить(*) значение типа ")
	+GetStrType()+" на значение типа "+var.GetStrType(), line);

	res.bStaticInit = 1;
	res.varState = vsExpression;
	if ( var.varType == vtDouble )
		res.varType = vtDouble;

	return res;
}


TVariable TVariable::CheckDiv(const TVariable& var, int line) {
	TVariable res(*this);
	StdCheck(*this, line);
	StdCheck(var, line);
	if (!(

	// Числа
		  (varType == vtInt || varType == vtDouble)
		   && (var.varType == vtInt || var.varType == vtDouble)
	))
	throw SyntaxError(string("Несоответствие типов: невозможно  разделить(/) значение типа ")
	+GetStrType()+" на значение типа "+var.GetStrType(), line);

	res.bStaticInit = 1;
	res.varState = vsExpression;
	res.varType = vtDouble;

	return res;
}

TVariable TVariable::CheckMod(const TVariable& var, int line) {
	TVariable res(*this);
	StdCheck(*this, line);
	StdCheck(var, line);
	if (!(

	// Числа
		  (varType == vtInt || varType == vtDouble)
		   && (var.varType == vtInt || var.varType == vtDouble)
	))
	throw SyntaxError(string("Несоответствие типов: невозможно  разделить нацело(:) значение типа ")
	+GetStrType()+" на значение типа "+var.GetStrType(), line);

	res.bStaticInit = 1;
	res.varState = vsExpression;
	res.varType = vtInt;

	return res;
}

TVariable TVariable::CheckPow(const TVariable& var, int line) {
	TVariable res(*this);
	StdCheck(*this, line);
	StdCheck(var, line);
	if (!(

	// Числа
		  (varType == vtInt || varType == vtDouble)
		   && (var.varType == vtInt || var.varType == vtDouble)
	))
	throw SyntaxError(string("Несоответствие типов: невозможно значение типа ")
	+GetStrType()+" возвести в степень(**), указанную значением типа "+var.GetStrType(), line);

	res.bStaticInit = 1;
	res.varState = vsExpression;
	res.varType = vtInt;

	return res;
}


///////////////////////////////////////////////////////////////////////////////
//							  	СРАВНЕНИЯ 									  //
///////////////////////////////////////////////////////////////////////////////

TVariable TVariable::CheckEqvalCmp(const TVariable& var, int line) {
	TVariable res(*this);
	StdCheck(*this, line);
	StdCheck(var, line);
	if (!(

	// Строки
	   ( 	  (varType == ::vtChar || varType == ::vtString)
		   && (var.varType == ::vtChar || var.varType == ::vtString)
	   )

	// Числа
	|| ( 	  (varType == vtInt || varType == vtDouble)
		   && (var.varType == vtInt || var.varType == vtDouble)
	   )

	// Биты
	|| (
		varType == vtBool && var.varType == vtBool
	   )
	))
	throw SyntaxError(string("Несоответствие типов: невозможно сравнить(=) значению типа ")
	+GetStrType()+" со значением типа "+var.GetStrType(), line);

	res.bStaticInit = 1;
	res.varState = vsExpression;
	res.varType = vtBool;

	return res;
}

TVariable TVariable::CheckNotEqvalCmp(const TVariable& var, int line) {
	TVariable res(*this);
	StdCheck(*this, line);
	StdCheck(var, line);
	if (!(

	// Строки
	   ( 	  (varType == ::vtChar || varType == ::vtString)
		   && (var.varType == ::vtChar || var.varType == ::vtString)
	   )

	// Числа
	|| ( 	  (varType == vtInt || varType == vtDouble)
		   && (var.varType == vtInt || var.varType == vtDouble)
	   )

	// Биты
	|| (
		varType == vtBool && var.varType == vtBool
	   )
	))
	throw SyntaxError(string("Несоответствие типов: невозможно сравнить(<>) значению типа ")
	+GetStrType()+" со значением типа "+var.GetStrType(), line);

	res.bStaticInit = 1;
	res.varState = vsExpression;
	res.varType = vtBool;

	return res;
}

TVariable TVariable::CheckGreaterCmp(const TVariable& var, int line) {
	TVariable res(*this);
	StdCheck(*this, line);
	StdCheck(var, line);
	if (!(

	// Строки
	   ( 	  (varType == ::vtChar || varType == ::vtString)
		   && (var.varType == ::vtChar || var.varType == ::vtString)
	   )

	// Числа
	|| ( 	  (varType == vtInt || varType == vtDouble)
		   && (var.varType == vtInt || var.varType == vtDouble)
	   )

	// Биты
	|| (
		varType == vtBool && var.varType == vtBool
	   )
	))
	throw SyntaxError(string("Несоответствие типов: невозможно сравнить(>) значению типа ")
	+GetStrType()+" со значением типа "+var.GetStrType(), line);

	res.bStaticInit = 1;
	res.varState = vsExpression;
	res.varType = vtBool;

	return res;
}

TVariable TVariable::CheckLessCmp(const TVariable& var, int line) {
	TVariable res(*this);
	StdCheck(*this, line);
	StdCheck(var, line);
	if (!(

	// Строки
	   ( 	  (varType == ::vtChar || varType == ::vtString)
		   && (var.varType == ::vtChar || var.varType == ::vtString)
	   )

	// Числа
	|| ( 	  (varType == vtInt || varType == vtDouble)
		   && (var.varType == vtInt || var.varType == vtDouble)
	   )

	// Биты
	|| (
		varType == vtBool && var.varType == vtBool
	   )
	))
	throw SyntaxError(string("Несоответствие типов: невозможно сравнить(<) значению типа ")
	+GetStrType()+" со значением типа "+var.GetStrType(), line);

	res.bStaticInit = 1;
	res.varState = vsExpression;
	res.varType = vtBool;

	return res;
}

TVariable TVariable::CheckGreaterOrEqCmp(const TVariable& var, int line) {
	TVariable res(*this);
	StdCheck(*this, line);
	StdCheck(var, line);
	if (!(

	// Строки
	   ( 	  (varType == ::vtChar || varType == ::vtString)
		   && (var.varType == ::vtChar || var.varType == ::vtString)
	   )

	// Числа
	|| ( 	  (varType == vtInt || varType == vtDouble)
		   && (var.varType == vtInt || var.varType == vtDouble)
	   )

	// Биты
	|| (
		varType == vtBool && var.varType == vtBool
	   )
	))
	throw SyntaxError(string("Несоответствие типов: невозможно сравнить(>=) значению типа ")
	+GetStrType()+" со значением типа "+var.GetStrType(), line);

	res.bStaticInit = 1;
	res.varState = vsExpression;
	res.varType = vtBool;

	return res;
}

TVariable TVariable::CheckLessOrEqCmp(const TVariable& var, int line) {
	TVariable res(*this);
	StdCheck(*this, line);
	StdCheck(var, line);
	if (!(

	// Строки
	   ( 	  (varType == ::vtChar || varType == ::vtString)
		   && (var.varType == ::vtChar || var.varType == ::vtString)
	   )

	// Числа
	|| ( 	  (varType == vtInt || varType == vtDouble)
		   && (var.varType == vtInt || var.varType == vtDouble)
	   )

	// Биты
	|| (
		varType == vtBool && var.varType == vtBool
	   )
	))
	throw SyntaxError(string("Несоответствие типов: невозможно сравнить(<=) значению типа ")
	+GetStrType()+" со значением типа "+var.GetStrType(), line);

	res.bStaticInit = 1;
	res.varState = vsExpression;
	res.varType = vtBool;

	return res;
}


///////////////////////////////////////////////////////////////////////////////
//							  	ЛОГИЧЕСКИЕ								   //
///////////////////////////////////////////////////////////////////////////////

TVariable TVariable::CheckAND(const TVariable& var, int line) {
	TVariable res(*this);
	StdCheck(*this, line);
	StdCheck(var, line);
	if (!(

	// Биты
	 (
		varType == vtBool && var.varType == vtBool
	   )
	))
	throw SyntaxError(string("Несоответствие типов: невозможно произвести конъюнкцию(И) значения типа ")
	+GetStrType()+" и значения типа "+var.GetStrType(), line);

	res.bStaticInit = 1;
	res.varState = vsExpression;
	res.varType = vtBool;

	return res;
}


TVariable TVariable::CheckOR(const TVariable& var, int line) {
	TVariable res(*this);
	StdCheck(*this, line);
	StdCheck(var, line);
	if (!(

	// Биты
	 (
		varType == vtBool && var.varType == vtBool
	   )
	))
	throw SyntaxError(string("Несоответствие типов: невозможно произвести дизъюнкцию(ИЛИ) значения типа ")
	+GetStrType()+" и значения типа "+var.GetStrType(), line);

	res.bStaticInit = 1;
	res.varState = vsExpression;
	res.varType = vtBool;

	return res;
}

///////////////////////////////////////////////////////////////////////////////
//							   УНАРНЫЕ	  	   								//
///////////////////////////////////////////////////////////////////////////////

TVariable TVariable::CheckNeg(int line) {
	TVariable res(*this);
	StdCheck(*this, line);
	if (varType != vtInt && varType != vtDouble)
	throw SyntaxError(string("Несоответствие типов: невозможно сменить знак значения типа ")
	+GetStrType()+" на противоположный ", line);

	res.bStaticInit = 1;
	res.varState = vsExpression;

	return res;
}

TVariable TVariable::CheckNOT(int line) {
	TVariable res(*this);
	StdCheck(*this, line);
	if (varType != vtBool)
	throw SyntaxError(string("Несоответствие типов: невозможно произвести отрицание(НЕ) значения типа ")
	+GetStrType(), line);

	res.bStaticInit = 1;
	res.varState = vsExpression;

	return res;
}

//___________________________________________________________________________//
//																			 //
//  								ВЫПОЛНЕНИЕ							   //
//___________________________________________________________________________//


///////////////////////////////////////////////////////////////////////////////
//							   ИНДЕКСИРОВАНИЕ								   //
///////////////////////////////////////////////////////////////////////////////

TVariable* TVariable::Indexing(const TVariable& varI) {
	// СМЕЩЕНИЕ! 0 <-> 1
	switch (varType) {
		case vtArray:
			if ( varI.Value.intValue > Value.arrValue.nColCnt || varI.Value.intValue <= 0 )
				throw RuntimeError(string("Выход за границы массива: ")+strName+"["+IntToStr(varI.Value.intValue).c_str()+"]");
			return &Value.arrValue.arr[varI.Value.intValue-1];
			break;
	default:
		throw RuntimeError("ОШИБКА ЭМУЛЯТОРА: Операция индексирования применена не к месту (Indexing(var))");
	}
}

TVariable* TVariable::Indexing(const TVariable& varI, const TVariable& varJ) {
	switch (varType) {
		case vtMatrix:
			if(( varI.Value.intValue > Value.mtxValue.nRowCnt || varI.Value.intValue <= 0 )
			|| ( varJ.Value.intValue > Value.mtxValue.nColCnt || varJ.Value.intValue <= 0 ))
				throw RuntimeError(string("Выход за границы матрицы: ")+strName+"["+IntToStr(varI.Value.intValue).c_str()+"]["+IntToStr(varJ.Value.intValue).c_str()+"]");
			return &Value.mtxValue.mtx[varI.Value.intValue-1][varJ.Value.intValue-1];
			break;
	default:
		throw RuntimeError("ОШИБКА ЭМУЛЯТОРА: Операция индексирования применена не к месту (Indexing(var,var))");
	}
}

///////////////////////////////////////////////////////////////////////////////
//						   	 ПРИСВАИВАНИЕ									 //
///////////////////////////////////////////////////////////////////////////////

void TVariable::Copy(const TVariable& var, TAlgorithm* pAlg) {
	bDynamicInit = 1;
	Value = var.Value;

	if(varType == vtInt) {
		if ( var.varType == vtInt )
			Value.intValue = var.Value.intValue;
		else /* var.varType == vtDouble*/
			Value.intValue = ROUND(var.Value.dblValue);
	}
	else if(varType == vtDouble) {
		if ( var.varType == vtDouble )
			Value.dblValue = var.Value.dblValue;
		else /* var.varType == vtInt*/
			Value.dblValue = var.Value.intValue;
	}

	if(varType == ::vtString) {
		if ( var.varType == ::vtString )
			Value.strValue = var.Value.strValue;
		else /* var.varType == ::vtChar*/
			Value.strValue = (string()+var.Value.chrValue);
	}
	else if(varType == ::vtChar) { // Допускается только chr := chr
			Value.chrValue = var.Value.chrValue;
	}


	if (varState == vsResult ) {
		pAlg->pCurrentCommand = -1;
	}

	if (varState == vsException) {
		fmConsole->WriteExc(*this, pAlg);
		pAlg->pCurrentCommand = -1;
		Emulator.bNeedToStop = 1;
	}
}

///////////////////////////////////////////////////////////////////////////////
//							   УНАРНЫЕ	  	   								//
///////////////////////////////////////////////////////////////////////////////

TVariable TVariable::NOT () {
	TVariable res(*this);
	res.varState = vsExpression;
	res.Value.blnValue = !Value.blnValue;
	return res;
}

TVariable TVariable::Neg () {
	TVariable res(*this);
	res.varState = vsExpression;
	res.Value.intValue = - Value.intValue;
	res.Value.dblValue = - Value.dblValue;
	return res;
}

///////////////////////////////////////////////////////////////////////////////
//							   ВЫЧИСЛЕНИЯ									   //
///////////////////////////////////////////////////////////////////////////////

TVariable TVariable::Add(const TVariable& var) {
	TVariable res(*this);
	res.varState = vsExpression;

	if(varType == ::vtString){
		if ( var.varType == ::vtChar  )
			res.Value.strValue = Value.strValue + var.Value.chrValue;
		else /* var.varType == ::vtString*/
			res.Value.strValue = Value.strValue + var.Value.strValue;
	}

	else if(varType == vtInt){
		if ( var.varType == vtInt  )
			res.Value.intValue = Value.intValue + var.Value.intValue;
		else { /* var.varType == vtDouble*/
			res.varType = vtDouble;
			res.Value.dblValue = Value.intValue + var.Value.dblValue;
		}
	}

	else if(varType == vtDouble){
		if ( var.varType == vtDouble  )
			res.Value.dblValue = Value.dblValue + var.Value.dblValue;
		else /* var.varType == vtInt*/
			res.Value.dblValue = Value.dblValue + var.Value.intValue;
	}

	return res;
}

TVariable TVariable::Sub(const TVariable& var) {
	TVariable res(*this);
	res.varState = vsExpression;

	if(varType == vtInt){
		if ( var.varType == vtInt  )
			res.Value.intValue = Value.intValue - var.Value.intValue;
		else { /* var.varType == vtDouble*/
			res.varType = vtDouble;
			res.Value.dblValue = Value.intValue - var.Value.dblValue;
		}
	}

	else if(varType == vtDouble){
		if ( var.varType == vtDouble  )
			res.Value.dblValue = Value.dblValue - var.Value.dblValue;
		else /* var.varType == vtInt*/
			res.Value.dblValue = Value.dblValue - var.Value.intValue;
	}

	return res;
}

TVariable TVariable::Mul(const TVariable& var) {
	TVariable res(*this);
	res.varState = vsExpression;

	if(varType == vtInt){
		if ( var.varType == vtInt  )
			res.Value.intValue = Value.intValue * var.Value.intValue;
		else { /* var.varType == vtDouble*/
			res.varType = vtDouble;
			res.Value.dblValue = Value.intValue * var.Value.dblValue;
		}
	}

	else if(varType == vtDouble){
		if ( var.varType == vtDouble  )
			res.Value.dblValue = Value.dblValue * var.Value.dblValue;
		else /* var.varType == vtInt*/
			res.Value.dblValue = Value.dblValue * var.Value.intValue;
	}

	return res;
}

TVariable TVariable::Div(const TVariable& var) { // 5/2 = 2.5 ВЕЩ |
	TVariable res(*this);
	res.varState = vsExpression;

	if (var.varType == vtInt){
		if (!var.Value.intValue)
			throw RuntimeError("Попытка деления на ноль (/)");
	}
	else /* var.varType == vtDouble*/
		if (!var.Value.dblValue)
			throw RuntimeError("Попытка деления на ноль (/)");

	if(varType == vtInt){
			res.varType = vtDouble;
		if ( var.varType == vtInt )
			res.Value.dblValue = double(Value.intValue) / var.Value.intValue;
		else { /* var.varType == vtDouble*/
			res.Value.dblValue = Value.intValue / var.Value.dblValue;
		}
	}

	else if(varType == vtDouble){
		if ( var.varType == vtDouble )
			res.Value.dblValue = Value.dblValue / var.Value.dblValue;
		else /* var.varType == vtInt*/
			res.Value.dblValue = Value.dblValue / var.Value.intValue;
	}

	return res;
}

TVariable TVariable::Mod(const TVariable& var) { // 5:2 = 2   ЦЕЛ |
	TVariable res(*this);
	res.varState = vsExpression;
	res.varType = vtInt;

	if (var.varType == vtInt) {
		if (!var.Value.intValue)
			throw RuntimeError("Попытка деления на ноль (:)");
    }
	else /* var.varType == vtDouble*/
		if (!var.Value.dblValue)
			throw RuntimeError("Попытка деления на ноль (:)");

	if(varType == vtInt){
		if ( var.varType == vtInt )
			res.Value.intValue = Value.intValue / var.Value.intValue;
		else  /* var.varType == vtDouble*/
			res.Value.intValue = Value.intValue / var.Value.dblValue;
	}

	else if(varType == vtDouble){
		if ( var.varType == vtDouble )
			res.Value.intValue = Value.dblValue / var.Value.dblValue;
		else /* var.varType == vtInt*/ 
			res.Value.intValue = Value.dblValue / var.Value.intValue;
	}

	return res;
}

TVariable TVariable::Pow(const TVariable& var) {
	TVariable res(*this);
	res.varState = vsExpression;

	if(varType == vtInt){
		if ( var.varType == vtInt )
			res.Value.intValue = pow( Value.intValue, var.Value.intValue);
		else { /* var.varType == vtDouble*/
			res.varType = vtDouble;
			res.Value.dblValue = pow( Value.intValue, var.Value.dblValue);
		}
	}

	else if(varType == vtDouble){
		if ( var.varType == vtDouble )
			res.Value.dblValue = pow( Value.dblValue, var.Value.dblValue);
		else /* var.varType == vtInt*/
			res.Value.dblValue = pow( Value.dblValue, var.Value.intValue);
	}

	return res;
}

///////////////////////////////////////////////////////////////////////////////
//							  	СРАВНЕНИЯ 									  //
///////////////////////////////////////////////////////////////////////////////

TVariable TVariable::EqvalCmp (const TVariable& var) {
	TVariable res(*this);
	res.bDynamicInit = 1;
	res.varType = vtBool;
	res.varState = vsExpression;

	if(varType == vtInt) {
		if ( var.varType == vtInt )
			res.Value.blnValue = (Value.intValue == var.Value.intValue);
		else /* var.varType == vtDouble*/
			res.Value.blnValue = (Value.intValue == var.Value.dblValue);
	}
	else if(varType == vtDouble) {
		if ( var.varType == vtDouble )
			res.Value.blnValue = (Value.dblValue == var.Value.dblValue);
		else /* var.varType == vtInt*/
			res.Value.blnValue = (Value.dblValue == var.Value.intValue);
	}

	if(varType == ::vtString) {
		if ( var.varType == ::vtString )
			res.Value.blnValue = (Value.strValue == var.Value.strValue);
		else /* var.varType == ::vtChar*/
			res.Value.blnValue = (Value.strValue == (string()+var.Value.chrValue));
	}
	else if(varType == ::vtChar) {
		if ( var.varType == ::vtChar )
			res.Value.blnValue = (Value.chrValue == var.Value.chrValue);
		else /* var.varType == ::vtString*/
			res.Value.blnValue = ((string()+Value.chrValue) == var.Value.strValue);
	}

	if(varType == vtBool) {
		res.Value.blnValue = (Value.blnValue == var.Value.blnValue);
	}

	return res;
}

TVariable TVariable::NotEqvalCmp (const TVariable& var) {
	TVariable res(*this);
	res.bDynamicInit = 1;
	res.varType = vtBool;
	res.varState = vsExpression;

	if(varType == vtInt) {
		if ( var.varType == vtInt )
			res.Value.blnValue = (Value.intValue != var.Value.intValue);
		else /* var.varType == vtDouble*/
			res.Value.blnValue = (Value.intValue != var.Value.dblValue);
	}
	else if(varType == vtDouble) {
		if ( var.varType == vtDouble )
			res.Value.blnValue = (Value.dblValue != var.Value.dblValue);
		else /* var.varType == vtInt*/
			res.Value.blnValue = (Value.dblValue != var.Value.intValue);
	}

	if(varType == ::vtString) {
		if ( var.varType == ::vtString )
			res.Value.blnValue = (Value.strValue != var.Value.strValue);
		else /* var.varType == ::vtChar*/
			res.Value.blnValue = (Value.strValue != (string()+var.Value.chrValue));
	}
	else if(varType == ::vtChar) {
		if ( var.varType == ::vtChar )
			res.Value.blnValue = (Value.chrValue != var.Value.chrValue);
		else /* var.varType == ::vtString*/
			res.Value.blnValue = ((string()+Value.chrValue) != var.Value.strValue);
	}

	if(varType == vtBool) {
		res.Value.blnValue = (Value.blnValue != var.Value.blnValue);
	}

	return res;
}


TVariable TVariable::GreaterCmp	(const TVariable& var) {
	TVariable res(*this);
	res.bDynamicInit = 1;
	res.varType = vtBool;
	res.varState = vsExpression;

	if(varType == vtInt) {
		if ( var.varType == vtInt )
			res.Value.blnValue = (Value.intValue > var.Value.intValue);
		else /* var.varType == vtDouble*/
			res.Value.blnValue = (Value.intValue > var.Value.dblValue);
	}
	else if(varType == vtDouble) {
		if ( var.varType == vtDouble )
			res.Value.blnValue = (Value.dblValue > var.Value.dblValue);
		else /* var.varType == vtInt*/
			res.Value.blnValue = (Value.dblValue > var.Value.intValue);
	}

	if(varType == ::vtString) {
		if ( var.varType == ::vtString )
			res.Value.blnValue = (Value.strValue > var.Value.strValue);
		else /* var.varType == ::vtChar*/
			res.Value.blnValue = (Value.strValue > (string()+var.Value.chrValue));
	}
	else if(varType == ::vtChar) {
		if ( var.varType == ::vtChar )
			res.Value.blnValue = (Value.chrValue > var.Value.chrValue);
		else /* var.varType == ::vtString*/
			res.Value.blnValue = ((string()+Value.chrValue) > var.Value.strValue);
	}

	if(varType == vtBool) {
		res.Value.blnValue = (Value.blnValue > var.Value.blnValue);
	}

	return res;
}


TVariable TVariable::LessCmp (const TVariable& var) {
	TVariable res(*this);
	res.bDynamicInit = 1;
	res.varType = vtBool;
	res.varState = vsExpression;

	if(varType == vtInt) {
		if ( var.varType == vtInt )
			res.Value.blnValue = (Value.intValue < var.Value.intValue);
		else /* var.varType == vtDouble*/
			res.Value.blnValue = (Value.intValue < var.Value.dblValue);
	}
	else if(varType == vtDouble) {
		if ( var.varType == vtDouble )
			res.Value.blnValue = (Value.dblValue < var.Value.dblValue);
		else /* var.varType == vtInt*/
			res.Value.blnValue = (Value.dblValue < var.Value.intValue);
	}

	if(varType == ::vtString) {
		if ( var.varType == ::vtString )
			res.Value.blnValue = (Value.strValue < var.Value.strValue);
		else /* var.varType == ::vtChar*/
			res.Value.blnValue = (Value.strValue < (string()+var.Value.chrValue));
	}
	else if(varType == ::vtChar) {
		if ( var.varType == ::vtChar )
			res.Value.blnValue = (Value.chrValue < var.Value.chrValue);
		else /* var.varType == ::vtString*/
			res.Value.blnValue = ((string()+Value.chrValue) < var.Value.strValue);
	}

	if(varType == vtBool) {
		res.Value.blnValue = (Value.blnValue < var.Value.blnValue);
	}

	return res;
}


TVariable TVariable::GreaterOrEqCmp	(const TVariable& var) {
	TVariable res(*this);
	res.bDynamicInit = 1;
	res.varType = vtBool;
	res.varState = vsExpression;

	if(varType == vtInt) {
		if ( var.varType == vtInt )
			res.Value.blnValue = (Value.intValue >= var.Value.intValue);
		else /* var.varType == vtDouble*/
			res.Value.blnValue = (Value.intValue >= var.Value.dblValue);
	}
	else if(varType == vtDouble) {
		if ( var.varType == vtDouble )
			res.Value.blnValue = (Value.dblValue >= var.Value.dblValue);
		else /* var.varType == vtInt*/
			res.Value.blnValue = (Value.dblValue >= var.Value.intValue);
	}

	if(varType == ::vtString) {
		if ( var.varType == ::vtString )
			res.Value.blnValue = (Value.strValue >= var.Value.strValue);
		else /* var.varType == ::vtChar*/
			res.Value.blnValue = (Value.strValue >= (string()+var.Value.chrValue));
	}
	else if(varType == ::vtChar) {
		if ( var.varType == ::vtChar )
			res.Value.blnValue = (Value.chrValue >= var.Value.chrValue);
		else /* var.varType == ::vtString*/
			res.Value.blnValue = ((string()+Value.chrValue) >= var.Value.strValue);
	}

	if(varType == vtBool) {
		res.Value.blnValue = (Value.blnValue >= var.Value.blnValue);
	}

	return res;
}


TVariable TVariable::LessOrEqCmp (const TVariable& var) {
	TVariable res(*this);
	res.bDynamicInit = 1;
	res.varType = vtBool;
	res.varState = vsExpression;

	if(varType == vtInt) {
		if ( var.varType == vtInt )
			res.Value.blnValue = (Value.intValue <= var.Value.intValue);
		else /* var.varType == vtDouble*/
			res.Value.blnValue = (Value.intValue <= var.Value.dblValue);
	}
	else if(varType == vtDouble) {
		if ( var.varType == vtDouble )
			res.Value.blnValue = (Value.dblValue <= var.Value.dblValue);
		else /* var.varType == vtInt*/
			res.Value.blnValue = (Value.dblValue <= var.Value.intValue);
	}

	if(varType == ::vtString) {
		if ( var.varType == ::vtString )
			res.Value.blnValue = (Value.strValue <= var.Value.strValue);
		else /* var.varType == ::vtChar*/
			res.Value.blnValue = (Value.strValue <= (string()+var.Value.chrValue));
	}
	else if(varType == ::vtChar) {
		if ( var.varType == ::vtChar )
			res.Value.blnValue = (Value.chrValue <= var.Value.chrValue);
		else /* var.varType == ::vtString*/
			res.Value.blnValue = ((string()+Value.chrValue) <= var.Value.strValue);
	}

	if(varType == vtBool) {
		res.Value.blnValue = (Value.blnValue <= var.Value.blnValue);
	}

	return res;
}



///////////////////////////////////////////////////////////////////////////////
//							  	ЛОГИЧЕСКИЕ								   //
///////////////////////////////////////////////////////////////////////////////

TVariable TVariable::AND (const TVariable& var) {
	TVariable res(*this);
	res.bDynamicInit = 1;
	res.varType = vtBool;
	res.varState = vsExpression;
	res.Value.blnValue = Value.blnValue && var.Value.blnValue;
	return res;
}

TVariable TVariable::OR (const TVariable& var) {
	TVariable res(*this);
	res.bDynamicInit = 1;
	res.varType = vtBool;
	res.varState = vsExpression;
	res.Value.blnValue = Value.blnValue || var.Value.blnValue;
	return res;
}

