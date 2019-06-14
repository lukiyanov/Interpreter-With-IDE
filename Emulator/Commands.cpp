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


// Основная функция вычисления выражения, возвращает тип результата
TVariable CheckFullExpression (TVarHeader& hdr, TVarHeader& buffer,
	 deque<string>& vct/*Expression*/, int line);

//---------------------------------------------------------------------------
Command* ReedCommand(string str, TVarHeader& hdrHost, LecsemManager& lecsGen) {
	/* DONE 5 : MODIFI, IF ADD Cmd */

	if ( str == "ПОКА" ) {
		DebugTrace("Найден цикл");
		return new CmdWhileExpression(hdrHost, lecsGen);
	}

	if ( str == "ЕСЛИ" ) {
		DebugTrace("Найдено ветвление");
		return new CmdIfExpression(hdrHost, lecsGen);
	}

	if ( Emulator.IsAlgPresent(str) ) {
		DebugTrace(string("Найден одиночный вызов алгоритма ")+str);
		return new CmdAlgExpression(hdrHost, lecsGen);
	}

	DebugTrace("Сложных команд не найдено, уравнение идентифицировано как присваивающее");
	return new CmdCopyExpression(hdrHost, lecsGen);
}

//---------------------------------------------------------------------------
// Считывает и возвращает очередную команду,
// Вызывается из Algorithm::Fill и из конструкторов команд-контейнеров типа ЕСЛИ и ПОКА
Command* FillLecsem(TVarHeader& hdrHost, LecsemManager& lecsGen) {
	DebugTrace(string("@Command* FillLecsem(...)"));
	DebugTrace(string("Поиск новой команды..."));

	int ln = lecsGen.line;
	int ls = lecsGen.lecs;

	string str = lecsGen.GetNext();
	DebugTrace(string("Идет идентификация \'")+str+"\'");
	if (str == "КОН") {
		DebugTrace(string("Команд не найдено"));
		return 0;
	}

	lecsGen.line = ln;
	lecsGen.lecs = ls;

	return ReedCommand(str, hdrHost, lecsGen);
}

// Виртуальный деструктор
Command::~Command(){}

//---------------------------------------------------------------------------
// Здесь идут определения ряда функций, предназначение которых - определить,
// является ли переданная им лексема константой соответствующего типа

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


// Заменяет константы в строке на имена буферных переменных,
// под которыми значения констант заносятся в буфер.
// Функция использует выше описанные функции для определения признака константы
void TranslateConstToVar(deque<string>& vct, TVarHeader& buffer, int line){
	DebugTrace(string("@TranslateConstToVar"));
	TVariable var(ss_niSupportState);
	DebugTrace(string(">> Идет поиск констант..."));
	for (unsigned i = 0; i < vct.size(); i++) {
		DebugTrace(string("  Проверка лексемы \'")+vct[i]+"\'");
		if (IsBoolConst(vct[i])) {
			var.bStaticInit = 1;
			var.bDynamicInit = 1;
			var.varType = vtBool;
			var.varState = vsConst;
			var.Value.blnValue = (vct[i] == "ИСТИНА");
			var.strName = string("ConstBufferVar#");
			var.strName += (IntToStr(buffer.vctVariables.size())).c_str(); // Нумеруются с 0
			buffer.vctVariables.push_back(var);
			vct[i] = var.strName;
//DebugTrace(string(""));
			DebugTrace(string("  Найдена константа типа ЛОГ, имя в буфере: ")+var.strName);
		}
		else if (IsIntConst(vct[i])) {
			var.bStaticInit = 1;
			var.bDynamicInit = 1;
			var.varType = vtInt;
			var.varState = vsConst;
			var.Value.intValue = StrToInt(vct[i].c_str());
			var.strName = string("ConstBufferVar#");
			var.strName += (IntToStr(buffer.vctVariables.size())).c_str(); // Нумеруются с 0
			buffer.vctVariables.push_back(var);
			vct[i] = var.strName;
			DebugTrace(string("  Найдена константа типа ЦЕЛ, имя в буфере: ")+var.strName);
		}
		else if (IsDoubleConst(vct[i])) {
			var.bStaticInit = 1;
			var.bDynamicInit = 1;
			var.varType = vtDouble;
			var.varState = vsConst;
			var.Value.dblValue = StrToFloat(AlgFloatToBrlnd(vct[i]).c_str());
			var.strName = string("ConstBufferVar#");
			var.strName += (IntToStr(buffer.vctVariables.size())).c_str(); // Нумеруются с 0
			buffer.vctVariables.push_back(var);
			vct[i] = var.strName;
			DebugTrace(string("  Найдена константа типа ВЕЩ, имя в буфере: ")+var.strName);
		}
		else if (IsCharConst(vct[i])) { // ПЕРЕД String
			var.bStaticInit = 1;
			var.bDynamicInit = 1;
			var.varType = ::vtChar;
			var.varState = vsConst;
			var.Value.chrValue = vct[i][1];
			var.strName = string("ConstBufferVar#");
			var.strName += (IntToStr(buffer.vctVariables.size())).c_str(); // Нумеруются с 0
			buffer.vctVariables.push_back(var);
			vct[i] = var.strName;
			DebugTrace(string("  Найдена константа типа СИМ, имя в буфере: ")+var.strName);
		}
		else if (IsStringConst(vct[i])) {
			var.bStaticInit = 1;
			var.bDynamicInit = 1;
			var.varType = ::vtString;
			var.varState = vsConst;
			var.Value.strValue = vct[i].substr(1,vct[i].size()-2);
			var.strName = string("ConstBufferVar#");
			var.strName += (IntToStr(buffer.vctVariables.size())).c_str(); // Нумеруются с 0
			buffer.vctVariables.push_back(var);
			vct[i] = var.strName;
			DebugTrace(string("  Найдена константа типа ЛИТ, имя в буфере: ")+var.strName);
		}

	}

	DebugTrace(string("Выходное выражение:"));
	for (unsigned i = 0; i < vct.size(); i++) {
		DebugTrace(string("  ")+IntToStr(i+1).c_str()+") "+vct[i]);
	}
	DebugTrace(string("Поиск констант завершен"));
	DebugTrace(string("@TranslateConstToVar exit"));
}

//---------------------------------------------------------------------------

// Проверка корректности расстановки скобок
void VerifyBrakets(deque<string>& vct, int line) {
	DebugTrace(string("@VerifyBrackets"));
	DebugTrace(string("Проверка корректности расстановки скобок..."));
	int nOpen = 0, nClose = 0;
	DebugTrace("Выражение:");
	for (unsigned i = 0; i < vct.size(); i++) {
		DebugTrace(string("  ")+IntToStr(i+1).c_str()+") "+vct[i]);
		if (vct[i] == "(") nOpen++;
		if (vct[i] == ")") nClose++;
	}

	DebugTrace(string("  ( : ")+IntToStr(nOpen).c_str());
	DebugTrace(string("  ) : ")+IntToStr(nClose).c_str());
	if (nOpen > nClose)
		throw SyntaxError("В выражении избыток открывающихся скобок \'(\'", line);

	if (nOpen < nClose)
		throw SyntaxError("В выражении избыток закрывающихся скобок \')\'", line);

	nOpen = 0, nClose = 0;
	for (unsigned i = 0; i < vct.size(); i++) {
		if (vct[i] == "[") nOpen++;
		if (vct[i] == "]") nClose++;
	}
	DebugTrace(string("  [ : ")+IntToStr(nOpen).c_str());
	DebugTrace(string("  ] : ")+IntToStr(nClose).c_str());

	if (nOpen > nClose)
		throw SyntaxError("В выражении избыток открывающихся скобок \'[\'", line);

	if (nOpen < nClose)
		throw SyntaxError("В выражении избыток закрывающихся скобок \']\'", line);
	DebugTrace(string("Проверка завершена"));
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
	DebugTrace(string("Поиск наиболее вложенной скобки..."));
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

	DebugTrace(string("Наибольшая глубина: ")+IntToStr(pos).c_str());
	MostHighBracketPos = pos;
	vBracket.clear();
	priority = 1;
	DebugTrace(string("Выражение внутри скобок:"));
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
	DebugTrace(string("Поиск завершен"));
	DebugTrace(string("@FindMostPriorityBracket exit"));
}

//---------------------------------------------------------------------------
// Функции считывания последовательностей определенного формата:
// Вызов алгоритма, индексирование элементов массивов и матриц

TVariable ReedAlgorithmObj(
	const deque<string>& vBracket, // Само выражение
	TVarHeader& hdr,               // Переменные алгоритма
	TVarHeader& buffer,            // Буфер
	int& pos,                      // Сдвигает pos на размер объекта
	int line                       // № строка, используется только для throw
) {
	TAlgorithm* alg = Emulator.GetAlgorithmPointer(vBracket[pos]);
	pos++;
	if ((pos == vBracket.size()) || (vBracket[pos] != "("))
		throw SyntaxError("Не найден список параметров алгоритма "+alg->GetName()+", ожидается \'(\'", line);
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
			throw SyntaxError("Не найден конец списка параметров алгоритма "+alg->GetName()+", ожидается \')\'", line);
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
		if (!m && str == ",") { // проверка m, т.к. запятая исп-ся и в индексировании матриц
			qpvar.push_back(new TVariable(ss_niSupportState));
			*qpvar[qpvar.size()-1] = CheckFullExpression(hdr, buffer, qque, line);
			qque.clear();
			continue; // Запятая не обрабатывается
		}

		if (n)
			qque.push_back(str);
		else {
			if(qpvar.size() || !qque.empty()) { // Если в скобка не пусто
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
		throw e; // Повторное возбуждение
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
// Использует описанные выше функции, возвращает вычисленный результат как имя буферной переменной
TVariable ReedCustomObj(           // Возвращает тип, признак инициализации и состояние
	const deque<string>& vBracket, // Само выражение
	TVarHeader& hdr,               // Переменные алгоритма
	TVarHeader& buffer,            // Буфер
	int& pos,                      // Сдвигает pos на размер объекта
	int line                       // № строка, используется только для throw
){
	DebugTrace(string("@ReedCustomObj"));
	DebugTrace(string("Идентификация объекта \'")+vBracket[pos]+"\'");

	// Массив?
	if (hdr.IsPresent(vBracket[pos]) && hdr.GetVariable(vBracket[pos]).varType == vtArray) {
		DebugTrace(string("Идентифицировано: переменная типа массив"));
		int xpos = pos;
		pos++;
		if ((pos == vBracket.size()) || (vBracket[pos] != "["))
			throw SyntaxError("Попытка выполнения недопустимой операции над массивом, допускается только индексирование", line);
		pos++;

		string str;
		int n = 1;
		deque<string> que;
		while (1) {
			if (pos == vBracket.size())
				throw SyntaxError("Индекс элемента в массиве не указан, ожидается \']\'", line);
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

	// Матрица?
	if (hdr.IsPresent(vBracket[pos]) && hdr.GetVariable(vBracket[pos]).varType == vtMatrix) {
		DebugTrace(string("Идентифицировано: переменная типа матрица"));
		int xpos = pos;
		pos++;
		if ((pos == vBracket.size()) || (vBracket[pos] != "["))
			throw SyntaxError("Попытка выполнения недопустимой операции над матрицей, допускается только индексирование", line);
		pos++;

		string str;
		deque<string> que;
		while (1) {
			if (pos == vBracket.size())
				throw SyntaxError("Индекс строки в матрице не указан, ожидается \',\'", line);
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
				throw SyntaxError("Индекс столбца в матрице не указан, ожидается \']\'", line);
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


	// Алгоритм?
	if ( Emulator.IsAlgPresent(vBracket[pos]) ) {
		DebugTrace("Идентифицировано: вызов алгорнитма");
		return ReedAlgorithmObj(vBracket, hdr, buffer, pos, line);
	}

	// Другая(обычная) переменная?
	if (hdr.IsPresent(vBracket[pos])) {
		DebugTrace(string("Идентифицировано: переменная простого типа"));
		pos++;
		DebugTrace(string("@ReedCustomObj exit"));
		return hdr.GetVariable(vBracket[pos-1]); // Оставляем как есть
	}

	// Константа?
	if (buffer.IsPresent(vBracket[pos])) {
		DebugTrace(string("Идентифицировано: буферная переменная"));
		pos++;
		DebugTrace(string("@ReedCustomObj exit"));
		return buffer.GetVariable(vBracket[pos-1]); // Оставляем как есть
	}


	DebugTrace(string("Объект не опознан"));
	throw SyntaxError(string("Недопустимая лексема, ожидается объект(переменная,константа,...), найдено \'")+vBracket[pos]+"\'", line);
}

//---------------------------------------------------------------------------
// Вычислить тип результата операции
TVariable CalculateElemExpression
(const TVariable& varTYPE, string strCOP, const TVariable& varOPERAND, int line) {
	DebugTrace(string("@CalculateElemExpression"));
	DebugTrace(string("Вычисление бинарной операции..."));
	DebugTrace(string("Тип 1го операнда: ")+varTYPE.GetStrType());
	DebugTrace(string("Тип 2го операнда: ")+varOPERAND.GetStrType());
	DebugTrace(string("Операция: ")+strCOP);

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
	else if( strCOP == "И" ) {
		varResult = varTYPE.CheckAND(varOPERAND, line);
	}
	else if( strCOP == "ИЛИ" ) {
		varResult = varTYPE.CheckOR(varOPERAND, line);
	}
	else {
		throw LogicalError("ЭКСТРЕННАЯ ОЩИБКА ЭМУЛЯТОРА: НЕИЗВЕСТНАЯ ОПЕРАЦИЯ \'"+strCOP+"\'", line);
	}

	DebugTrace(string("Тип результата: ")+varTYPE.GetStrType());
	DebugTrace(string("Вычисление завершено"));
	DebugTrace(string("@CalculateElemExpression exit"));
	return varResult;
}

//---------------------------------------------------------------------------
// Проверка корректности расстановки лексем
// Проверка соответствия типов, возврашает имя переменной буфера, содержащий тип выражения
string CheckCOP(const deque<string>& vBracket, TVarHeader& hdr, TVarHeader& buffer, int line) {
	DebugTrace(string("@CheckCOP"));
	DebugTrace(string("Вычисление выражения.."));
	int size = vBracket.size();
	if (!size)
		throw SyntaxError("Найдено пустое выражение, проверьте правильность расстановки операторов и переменных", line);

	DebugTrace(string("Выражение:"));
	for (unsigned i = 0; i < size; i++) {
		DebugTrace(string(" ")+IntToStr(i+1).c_str()+") "+vBracket[i]);
	}

	// DebugTrace(string(""));
	int pos = 0;
	deque<string> result;
	TVariable varTYPE (ss_niSupportState); // Текущий тип
	if (vBracket[0] == "-") {
		if (size == 1)
			throw SyntaxError("Не найдено операнда для унарной операции \'-\'", line);
		pos = 1;

		if (size == pos) // Ожидаем объект
			throw SyntaxError("Не найдено первого объекта (переменная, константа,...)", line);
		varTYPE = ReedCustomObj(vBracket, hdr, buffer, pos, line); // Сдвигает pos (+=)
		varTYPE = varTYPE.CheckNeg(line);
	}

	else if (vBracket[0] == "НЕ") {
		if (size == 1)
			throw SyntaxError("Не найдено операнда для унарной операции \'НЕ\'", line);
		pos = 1;

		if (size == pos) // Ожидаем объект
			throw SyntaxError("Не найдено первого объекта (переменная, константа,...)", line);
		varTYPE = ReedCustomObj(vBracket, hdr, buffer, pos, line); // Сдвигает pos (+=)
		varTYPE = varTYPE.CheckNOT(line);
	}

	else {
		if (size == pos) // Ожидаем объект
			throw SyntaxError("Не найдено первого объекта (переменная, константа,...)", line);
		varTYPE = ReedCustomObj(vBracket, hdr, buffer, pos, line); // Сдвигает pos (+=)
	}


	DebugTrace(string("Исходный тип выражения(varTYPE): ")+varTYPE.GetStrType());

	// Получаем простую последовательность и попутно проверяем тип
	TVariable varOPERAND (ss_niSupportState);
	string strCOP;
	bool not;
	while (size != pos) { // Ожидаем знак или конец выражения.

		not = 0;
		strCOP = vBracket[pos]; // Получаем код операции.
		pos+=1;                 // Сдвигаем pos.
		if (                    // Проверяем, операция ли это.
		// Все эти проверки ДО ReedCustomObj для своевременности
		// возникновения синтаксической ошибки в эмуляторе.
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
		&&  strCOP != "И"
		&&  strCOP != "ИЛИ"
		)
		throw SyntaxError("Обнаружена недопустимая в это месте лексема; ожидается операция", line);

		if (size == pos) // Ожидаем объект, а найден конец выражения
			throw SyntaxError("Выражение не может оканчиваться знаком", line);

		// Здесь может быть НЕ
		if (vBracket[pos]=="НЕ") {
			not = 1;
			pos++;
		}

//		if (size == pos) // Ожидаем объект, а найден конец выражения
//			throw SyntaxError("Выражение не может оканчиваться знаком", line);
		// Получаем текущий операнд:
		varOPERAND = ReedCustomObj(vBracket, hdr, buffer, pos, line);

		if (not) {
			varOPERAND = varOPERAND.CheckNOT(line);
		}
		// И сразу же проверяем типы:
		varTYPE = CalculateElemExpression(varTYPE, strCOP, varOPERAND, line);
	}

	varTYPE.strName = string("ConstBufferVar#");
	varTYPE.strName += (IntToStr(buffer.vctVariables.size())).c_str();
	buffer.vctVariables.push_back(varTYPE);

	DebugTrace(string("Вычисление завершено, результат:"));
	DebugTrace(string("Имя буферной переменной: ")+varTYPE.GetStrName());
	DebugTrace(string("Тип: ")+varTYPE.GetStrType());
	DebugTrace(string("@CheckCOP exit"));
	return varTYPE.strName;
}

// Заменяет выражение вместе со скобкой на его значение в буфере
void ReplaceBracket(deque<string>& vct, const deque<string>& vBracket,
					const int MostHighBracketPos, const string& str) {
	DebugTrace(string("@ReplaceBracket"));
	DebugTrace(string("Замена выражения на его вычисленный тип..."));

	DebugTrace(string("Исходное выражение:"));
	for (unsigned i = 0; i < vct.size(); i++) {
		DebugTrace(string("  ")+IntToStr(i+1).c_str()+") "+vct[i]);
	}
	DebugTrace(string("Позиция начала выражения: ")+IntToStr(MostHighBracketPos+1).c_str());

	deque<string> dq;
	for (int i = 0; i < MostHighBracketPos; i++) {
		dq.push_back(vct[i]);
	}
	dq.push_back(str);

	int offset = MostHighBracketPos+vBracket.size()+2; // 2 - с учетом скобок
	DebugTrace(string("Позиция конца выражения: ")+IntToStr(offset).c_str());

	for (unsigned i = offset; i < vct.size(); i++) {
		dq.push_back(vct[i]);
	}
	vct = dq;
	DebugTrace(string("Результат замены:"));
	for (unsigned i = 0; i < vct.size(); i++) {
		DebugTrace(string("  ")+IntToStr(i+1).c_str()+") "+vct[i]);
	}
	DebugTrace(string("@ReplaceBracket exit"));
}

/////////////////////////////////////////////////////////////////////////////
// Основная функция вычисления выражения, возвращает тип результата
TVariable CheckFullExpression (TVarHeader& hdr, TVarHeader& buffer,
							 deque<string>& vct/*Expression*/, int line) {
	DebugTrace(string("@CheckFullExpression"));
	DebugTrace(string("====== Проверка синтаксиса выражения... ======"));
	DebugTrace(string("Исходное выражение"));
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

	DebugTrace(string("Результат выражения:"));
	DebugTrace(string("  Имя буферной переменной: ")+vct[0]);
	DebugTrace(string("  Тип: ")+buffer.GetVariable(vct[0]).GetStrType());
	DebugTrace(string("====== Проверка синтаксиса выражения завершена, ошибок не найдено ======"));
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
		throw SyntaxError(string("Переменная ")+str+" не объявлена", lecsGen.line);

	TVariable* var;
	var = hdr.GetVariablePTR(str);
	strVarOriginal = str;         // REPLACED

	if (var->varType == vtArray) {
		if ( lecsGen.GetNext() != "[" )
		throw SyntaxError("Попытка выполнения недопустимой операции над массивом, допускается только индексирование", line);

		deque<string>que;
		int n = 1;
		while (1) {
			str = lecsGen.GetNext();
			if (str == "#")
				throw SyntaxError("Индекс элемента в массиве не указан, ожидается \']\'", line);
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
		throw SyntaxError("Попытка выполнения недопустимой операции над матрицей, допускается только индексирование", line);

		deque<string>que;
		while (1) {
			str = lecsGen.GetNext();
			if (str == "#")
				throw SyntaxError("Индекс строки элемента в матрице не указан, ожидается \',\'", line);
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
				throw SyntaxError("Индекс столбца элемента в матрице не указан, ожидается \']\'", line);
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
		throw SyntaxError(string("Ожидается \':=\', найдено: \'")+str+"\'", lecsGen.line);

	if (lecsGen.line != line)
		throw SyntaxError("Выражение должно полностью располагаться на одной строке", lecsGen.line);

	exp = new CmdExpression(hdr, lecsGen);

	if (exp->line != line) {
		delete exp;
		throw SyntaxError("Выражение должно полностью располагаться на одной строке", lecsGen.line);
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

		if (lecsGen.line != line || (IsKeyword(str) && str!="ЛОЖЬ" && str!="ИСТИНА")) {
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

	// в vct - наша строка

	TVarHeader buffer;
	int pos = 0;
	TVariable var (ss_niSupportState);
	var = ReedAlgorithmObj(vct, hdrHost, buffer, pos, line);
	if (pos != vct.size())
		throw SyntaxError("При одиночном вызове алгоритма(как процедуры) после его вызова найдены еще операторы", line);
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
		if ( str == "КОН" ) {
			throw SyntaxError("Условие конструкции ЕСЛИ не закрыто, ожидается \'ТО\', найдено \'КОН\'", lecsGen.line);
		}
		if ( str == "ТО" ) {
			break;		// EQVAL
		}
		if ( nOldLine != lecsGen.line ) {
			if ( str == "ТО" ) {
				break;	// EQVAL
			}
			else
				throw SyntaxError(
string("Условие конструкции ЕСЛИ должно располагаться на одной строке, на другой строке может\
 располагаться лишь ключевое слово \'ТО\', но вместо него найдено \'")+str+"\'"
 , lecsGen.line);
		}
	   deqCondition.push_back(str);
	}

	deque<string> l_deqCondition = deqCondition;
	TVariable var (ss_niSupportState);
	var = CheckFullExpression (hdrHost, buffer, l_deqCondition, lecsGen.line);

	if ( var.varType != vtBool )
		throw SyntaxError(string("Условие ветвления ЕСЛИ должно иметь тип ЛОГ, найденное выражение имеет тип ")+var.GetStrType(), lecsGen.line);

	// FILL THE BODY
	while (1) {

		DebugTrace(string("Ветвление ЕСЛИ: поиск новой команды..."));

		int ln = lecsGen.line;
		int ls = lecsGen.lecs;

		str = lecsGen.GetNext();
		DebugTrace(string("Идет идентификация \'")+str+"\'");

		if (str == "ВСЕ") {
			DebugTrace(string("Команд не найдено"));
			break;
		}
		if (str == "ИНАЧЕ") {
			DebugTrace(string("Найдено ИНАЧЕ"));
			break;
		}
		if (str == "КОН") {
			DebugTrace(string("Найдено КОН"));
			throw SyntaxError("Конструкция ЕСЛИ должна заканчиваться ключевым словом \'ВСЕ\', найдено \'КОН\'", lecsGen.line);
		}

		lecsGen.line = ln;
		lecsGen.lecs = ls;

		vctSubCommands.push_back(ReedCommand(str, hdrHost, lecsGen));
	}

	if (str == "ИНАЧЕ") {
		while (1) {
			DebugTrace(string("Ветвление ЕСЛИ: поиск новой команды..."));

			int ln = lecsGen.line;
			int ls = lecsGen.lecs;

			str = lecsGen.GetNext();
			DebugTrace(string("Идет идентификация \'")+str+"\'");

			if (str == "ВСЕ") {
				DebugTrace(string("Команд не найдено"));
				break;
			}
			if (str == "ИНАЧЕ") {
				DebugTrace(string("Найдено ИНАЧЕ"));
				throw SyntaxError("Не допускается несколько блоков ИНАЧЕ в конструкции ЕСЛИ", lecsGen.line);
			}
			if (str == "КОН") {
				DebugTrace(string("Найдено КОН"));
				throw SyntaxError("Конструкция ЕСЛИ должна заканчиваться ключевым словом \'ВСЕ\', найдено \'КОН\'", lecsGen.line);
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
		if ( str == "КОН" ) {
			throw SyntaxError("Условие конструкции ПОКА не закрыто, ожидается \'НЦ\', найдено \'КОН\'", lecsGen.line);
		}
		if ( str == "НЦ" ) {
			break;		// EQVAL
		}
		if ( nOldLine != lecsGen.line ) {
			if ( str == "НЦ" ) {
				break;	// EQVAL
			}
			else
				throw SyntaxError(
string("Условие конструкции ПОКА должно располагаться на одной строке, на другой строке может\
 располагаться лишь ключевое слово \'НЦ\', но вместо него найдено \'")+str+"\'"
 , lecsGen.line);
		}
	   deqCondition.push_back(str);
	}

	deque<string> l_deqCondition = deqCondition;
	TVariable var (ss_niSupportState);
	var = CheckFullExpression (hdrHost, buffer, l_deqCondition, lecsGen.line);

	if ( var.varType != vtBool )
		throw SyntaxError(string("Условие цикла ПОКА должно иметь тип ЛОГ, найденное выражение имеет тип ")+var.GetStrType(), lecsGen.line);

	// FILL THE BODY
	while (1) {

		DebugTrace(string("Цикл ПОКА: поиск новой команды..."));

		int ln = lecsGen.line;
		int ls = lecsGen.lecs;

		str = lecsGen.GetNext();
		DebugTrace(string("Идет идентификация \'")+str+"\'");

		if (str == "КЦ") {
			DebugTrace(string("Команд не найдено"));
			break;
		}

		if (str == "КОН") {
			DebugTrace(string("Найдено КОН"));
			throw SyntaxError("Конструкция ПОКА должна заканчиваться ключевым словом \'КЦ\', найдено \'КОН\'", lecsGen.line);
		}

		lecsGen.line = ln;
		lecsGen.lecs = ls;

		vctSubCommands.push_back(ReedCommand(str, hdrHost, lecsGen));
	}

}
//---------------------------------------------------------------------------

