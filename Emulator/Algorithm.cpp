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
	DebugTrace("Поиск нового алгоритма...");
	{ // Находим начало алгоритма:
		string str = lecsGen.GetNext();
		if ( str != "АЛГ"){
			str = "Не найдено начало алгоритма, ожидается \'АЛГ\', найдено "+str;
			throw SyntaxError(str.c_str(), lecsGen.line);
		}
	}

	{ // Находим название алгоритма:
		Header.PushAlgName(lecsGen.GetNext(), lecsGen.line);
		DebugTrace(string("Алгоритм найден: ")+GetName());
	}

	DebugTrace(string("Чтение заголовка алгоритма ")+GetName());

	{ // Находим конец алгоритма:
		int ln = lecsGen.line;
		int ls = lecsGen.lecs;

		bool bEndFound = 0;
		string str;
		do{
			str = lecsGen.GetNext();
			if (str == "КОН") {
				bEndFound = 1;
				break;
			}
		}while(str != "#");

		if (!bEndFound)
			throw SyntaxError((string("Не найден конец алгоритма ")+Header.GetAlgName()+", ожидается \'КОН\'").c_str(), lecsGen.line); // line - последняя строка

		lecsGen.line = ln;
		lecsGen.lecs = ls; 
	}


	{ // Считываем список переменных
		string str = lecsGen.GetNext();
		if ( str != "(") {
			str = "Не найдено начало списка переменных алгоритма "+
			Header.GetAlgName()+", ожидается \'(\', найдено \'"+str+"\'";
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
						throw SyntaxError(string("После описания типа переменных, в заголовке алгоритма " +GetName()+", ожидается перечисление их имен, вместо этого найдено \'")+str+"\'",lecsGen.line);
					if (!bVarLast)
						throw SyntaxError(string("В заголовке алгоритма ")+GetName()+" надена запятая перед символом \'"+str+"\'", lecsGen.line);
					break;
				}

				if (bLecsFound && !bVarLast && str == ",")
					throw SyntaxError(string("Две запятые подряд при описании переменных в алгоритме ")
					+GetName()+", между запятыми ожидается переменная, либо удалите лишнюю", lecsGen.line);

				if (bVarLast && str != ",")
					throw SyntaxError(string("Две переменные подряд при описании переменных в алгоритме ")
					+GetName()+", перед переменной "+str+" ожидается запятая", lecsGen.line);

				if ( !bVarFound && !bVarLast && bLecsFound)
					throw SyntaxError(string("После описания типа переменных в заголовке алгоритма ")
					+GetName()+" найдена запятая, как первая лексема, вместо этого ожидается имя переменной", lecsGen.line);


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
			throw SyntaxError(string("Найден символ \';\' перед символом \')\' в заголовке алгоритма ")
			+GetName()+" при описании переменных; либо удалите символ \';\', либо продолжите объявление переменных, поставив после него имя типа", lecsGen.line);
	}

	{ // Устанавливаем аргументы
		string str = lecsGen.GetNext();
		if ( str != "АРГ") {
			str = "Не найдено начало списка аргументов алгоритма "+
			Header.GetAlgName()+", ожидается \'АРГ\', найдено \'"+str+"\'";
			throw SyntaxError(str.c_str(), lecsGen.line);
		}

		str = lecsGen.GetNext();
		if (str != "НЕТ") {
			Header.ToArgument(str, lecsGen.line);
			vctParams.push_back(Header.GetVariable(str));
			while (1) {
				str = lecsGen.GetNext();
				if (str == "РЕЗ") break;
				if (str != ",")
					throw SyntaxError(string("Ошибка описания аргументов алгоритма ")
					+GetName()+": ожидается запятая для продолжения или \'РЕЗ\' для окончания списка, найдено \'"+str+"\'",lecsGen.line);
				str = lecsGen.GetNext();
				Header.ToArgument(str, lecsGen.line);
				vctParams.push_back(Header.GetVariable(str));
			}
		}
		else {
			str = lecsGen.GetNext();
			if (str != "РЕЗ")
				throw SyntaxError(string("Ошибка описания результата алгоритма ")
				+GetName()+": ожидается \'РЕЗ\', найдено \'"+str+"\'",lecsGen.line);
		}

	}

	{ // Устанавливаем результат
		string str = lecsGen.GetNext();
		if (str != "НЕТ")
			Header.ToResult(str, lecsGen.line);
	}

	{ // НАЧ - ИСКЛ
		string str = lecsGen.GetNext();
		if (str != "НАЧ")
			throw SyntaxError(string("Ошибка описания алгоритма ")
			+GetName()+": ожидается \'НАЧ\', найдено \'"+str+"\'",lecsGen.line);

		str = lecsGen.GetNext();
		if (str != "(")
			throw SyntaxError(string("Ошибка описания алгоритма ")
			+GetName()+": ожидается \'(\', найдено \'"+str+"\'",lecsGen.line);

		str = lecsGen.GetNext();
		if (str != ")" ){
			TVariable var(ss_niSupportState);
			CheckToSimpleType(str, var, lecsGen.line);
			str = lecsGen.GetNext();
			Header.PushVar(var, str, lecsGen.line);
			Header.ToExc(str);
			str = lecsGen.GetNext();
			if (str != ")")
				throw SyntaxError(string("Ошибка описания исключения алгоритма ")
				+GetName()+": ожидается \')\', найдено \'"+str+"\'",lecsGen.line);
		}
	}

	{ // Прокручиваем до конца алгоритма:

		bool bEndFound = 0;
		string str;
		do{
			str = lecsGen.GetNext();
			if (str == "КОН") {
				bEndFound = 1;
				break;
			}
		}while(str != "#");

		if (!bEndFound)
			throw SyntaxError((string("Не найден конец алгоритма ")+Header.GetAlgName()+", ожидается \'КОН\'").c_str(), lecsGen.line); // line - последняя строка
	}

	DebugTrace(string("Заголовок считан"));
}


void TAlgorithm::Fill(LecsemManager& lecsGen) {
	{ // Прокручиваем до начала команд:

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

	DebugTrace(string(">> Заполнение команд..."));
	Command* cmd;
	while (cmd = FillLecsem(Header, lecsGen))
		vctBody.push_back(cmd);
	DebugTrace(string("Заполнение завершено"));
	DebugTrace("___________________________________________________________");
}

//---------------------------------------------------------------------------
void TAlgorithm::CheckToSimpleType(const string& str, TVariable& var, int line) {
	if (str == "ЛОГ")
		var.varType = vtBool;

	else if (str == "ЦЕЛ")
		var.varType = vtInt;

	else if (str == "ВЕЩ")
		var.varType = vtDouble;

	else if (str == "ЛИТ")
		var.varType = ::vtString;

	else if (str == "СИМ")
		var.varType = ::vtChar;

	else throw SyntaxError(string("Ошибка описания исключения алгоритма ")
	+GetName()+": ожидается имя простого типа или \')\', найдено \'"+str+"\'", line);
}

//---------------------------------------------------------------------------
bool TAlgorithm::FindType(LecsemManager& lecsGen, TVariable& var) {
	string str = lecsGen.GetNext();

	if (str == ")")
		return 0;


	if (str == "ЛОГ")
		var.varType = vtBool;

	else if (str == "ЦЕЛ")
		var.varType = vtInt;

	else if (str == "ВЕЩ")
		var.varType = vtDouble;

	else if (str == "ЛИТ")
		var.varType = ::vtString;

	else if (str == "СИМ")
		var.varType = ::vtChar;

	else if (str == "МАСС") {
		var.varType = vtArray;
		str = lecsGen.GetNext();

		if (str == "ЛОГ")
			var.Value.arrValue.varElemType = vtBool;

		else if (str == "ЦЕЛ")
			var.Value.arrValue.varElemType = vtInt;

		else if (str == "ВЕЩ")
			var.Value.arrValue.varElemType = vtDouble;

		else if (str == "ЛИТ")
			var.Value.arrValue.varElemType = ::vtString;

		else if (str == "СИМ")
			var.Value.arrValue.varElemType = ::vtChar;

		else throw SyntaxError(string("Ожидается имя типа элементов массива, найдено \'")+str
		+"\' в заголовке алгоритма "+Header.GetAlgName(), lecsGen.line);

		str = lecsGen.GetNext();
		if (str!="[")
			throw SyntaxError(string("Описание массива в заголовке алгоритма ")+GetName()
			+" задано неверно, после указания типа элементов массива ожидается символ \'[\' и затем указание размерности, вместо этого найдено \'"+str+"\'", lecsGen.line);

		str = lecsGen.GetNext();
		bool bFound;
		for (int i = 0; i < str.size(); i++) {
			bFound = 0;
			for (int j = 0; szFigures[j]!='\0'; j++) {
				if (szFigures[j] == str[i]) bFound = 1;
			}
			if (!bFound)
			throw SyntaxError(string("Описание массива в заголовке алгоритма ")+GetName()
			+" задано неверно, после символа \'[\' ожидается указание размерности (натуральное число до 4096), вместо этого найдено \'"+str+"\'", lecsGen.line);
		}

		if (!StrToInt(str.c_str()))
			throw SyntaxError(string("Описание массива в заголовке алгоритма ")+GetName()
			+" задано неверно, после символа \'[\' ожидается указание размерности (натуральное число до 4096), вместо этого найден ноль", lecsGen.line);

		if (StrToInt(str.c_str()) > 4096)
			throw SyntaxError(string("Описание массива в заголовке алгоритма ")+GetName()
			+" задано неверно, превышено предельное указание размерности (натуральное число до 4096): \'"+str+"\'", lecsGen.line);

		var.Value.arrValue.nColCnt = StrToInt(str.c_str());

		str = lecsGen.GetNext();
		if (str!="]")
			throw SyntaxError(string("Описание массива в заголовке алгоритма ")+GetName()
			+" задано неверно, после указания количества элементов массива ожидается символ \']\', вместо этого найдено \'"+str+"\'", lecsGen.line);

	}
// ОБРАЗЕЦ: throw SyntaxError(string("")+str+""+GetName(), lecsGen.line);
	else if (str == "МАТР") {
		var.varType = vtMatrix;
		str = lecsGen.GetNext();

		if (str == "ЛОГ")
			var.Value.mtxValue.varElemType = vtBool;

		else if (str == "ЦЕЛ")
			var.Value.mtxValue.varElemType = vtInt;

		else if (str == "ВЕЩ")
			var.Value.mtxValue.varElemType = vtDouble;

		else if (str == "ЛИТ")
			var.Value.mtxValue.varElemType = ::vtString;

		else if (str == "СИМ")
			var.Value.mtxValue.varElemType = ::vtChar;

		else throw SyntaxError(string("Ожидается имя типа элементов матрицы, найдено \'")+str
		+"\' в заголовке алгоритма "+Header.GetAlgName(), lecsGen.line);

		str = lecsGen.GetNext();
		if (str!="[")
			throw SyntaxError(string("Описание матрицы в заголовке алгоритма ")+GetName()
			+" задано неверно, после указания типа элементов ожидается символ \'[\' и затем указание размерностей через запятую, вместо этого найдено \'"+str+"\'", lecsGen.line);

		str = lecsGen.GetNext();
		bool bFound;
		for (int i = 0; i < str.size(); i++) {
			bFound = 0;
			for (int j = 0; szFigures[j]!='\0'; j++) {
				if (szFigures[j] == str[i]) bFound = 1;
			}
			if (!bFound)
			throw SyntaxError(string("Описание матрицы в заголовке алгоритма ")+GetName()
			+" задано неверно, после символа \'[\' ожидается указание количества строк (натуральное число), вместо этого найдено \'"+str+"\'", lecsGen.line);
		}

		if (!StrToInt(str.c_str()))
			throw SyntaxError(string("Описание матрицы в заголовке алгоритма ")+GetName()
			+" задано неверно, после символа \'[\' ожидается указание количества строк (натуральное число), вместо этого найден ноль", lecsGen.line);

		if (StrToInt(str.c_str()) > 512)
			throw SyntaxError(string("Описание матрицы в заголовке алгоритма ")+GetName()
			+" задано неверно, превышено предельное указание размерности строк (натуральное число до 512): \'"+str+"\'", lecsGen.line);

		var.Value.mtxValue.nRowCnt = StrToInt(str.c_str());


		str = lecsGen.GetNext();
		if (str!=":")
			throw SyntaxError(string("Описание матрицы в заголовке алгоритма ")+GetName()
			+" задано неверно, после указания количества строк ожидается символ \':\' и затем указание количества столбцов, вместо этого найдено \'"+str+"\'", lecsGen.line);

		str = lecsGen.GetNext();
		for (int i = 0; i < str.size(); i++) {
			bFound = 0;
			for (int j = 0; szFigures[j]!='\0'; j++) {
				if (szFigures[j] == str[i]) bFound = 1;
			}
			if (!bFound)
			throw SyntaxError(string("Описание матрицы в заголовке алгоритма ")+GetName()
			+" задано неверно, после символа \':\' ожидается указание количества столбцов (натуральное число), вместо этого найдено \'"+str+"\'", lecsGen.line);
		}

		if (!StrToInt(str.c_str()))
			throw SyntaxError(string("Описание матрицы в заголовке алгоритма ")+GetName()
			+" задано неверно, после символа \':\' ожидается указание количества столбцов (натуральное число), вместо этого найден ноль", lecsGen.line);

		if (StrToInt(str.c_str()) > 512)
			throw SyntaxError(string("Описание матрицы в заголовке алгоритма ")+GetName()
			+" задано неверно, превышено предельное указание размерности столбцов (натуральное число до 512): \'"+str+"\'", lecsGen.line);

		var.Value.mtxValue.nColCnt = StrToInt(str.c_str());


		str = lecsGen.GetNext();
		if (str!="]")
			throw SyntaxError(string("Описание матрицы в заголовке алгоритма ")+GetName()
			+" задано неверно, после указания количества столбцов ожидается символ \']\', вместо этого найдено \'"+str+"\'", lecsGen.line);

	}

	else throw SyntaxError(string("Ожидается имя типа, найдено \'")+str
	+"\' в заголовке алгоритма "+Header.GetAlgName(), lecsGen.line);

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
	DebugTrace(string("Вызван деструктор алгоритма ")+GetName());
	for (int i = 0; i < vctBody.size(); i++)
		delete vctBody[i];
}




//---------------------------------------------------------------------------
void TAlgorithm::Step() {
	static const TValue value;
																		// Если...
	if (	pCurrentCommand == -1                                       // Алгоритм завершен
			&& Emulator.algs[fmRunParams->GetMainAlgorithm()] == this   // Это основной алгоритм
			&& Header.IsResultPresent()									// При этом здесь есть результат
			&& Header.GetResult().bDynamicInit                          // Результат инициализирован (что и спровацировало завершение алгоритма)
	) {
		fmConsole->WriteLn(Header.GetResult());
	}

	if (pCurrentCommand == -1) // Алгоритм завершен.
		Emulator.bNeedToStop = 1; // Останавливаем выполнение
	else {                                       // Выполнием следующую команду

		if ( vctBody[pCurrentCommand]->Execute(Header, this) ){ // Делаем шаг; если эта команда выполнена...
			if (pCurrentCommand == (vctBody.size()-1)) 		    // Если эта последняя и сдвигать некуда.
				pCurrentCommand = -1;                         	// Устанавливаем пометку останова
			else {
				if (pCurrentCommand != -1) 						// TVariable::Copy может изменить в Execute
					pCurrentCommand++;   						// Иначе сдвигаем на следующую.
			}
		}

	}

	RefreshVarWnd();
	fmMainView->StringGrid3->Row = GetLine();

	ShowPos();
}

//---------------------------------------------------------------------------
void TAlgorithm::RunInit() {
	if (!vctBody.size()) // Алгоритм пуст.
		pCurrentCommand = -1;
	else
		pCurrentCommand = 0; // Устанавливаем на первую.

	ShowPos();
}

//---------------------------------------------------------------------------
void TAlgorithm::ShowPos() {
	if(Emulator.bNeedToStop)
		return;

	if (Emulator.esState == esTracing) {
		if (pCurrentCommand != -1)
			DebugTrace(string("-> Позиция трассировки: [")+IntToStr(GetLine()+1).c_str()
			+"]: "+fmMainView->StringGrid3->Cells[0][GetLine()].c_str(), always);
		else
			DebugTrace("-> Трассировка завершена, следующим шагом алгоритм будет завершен.", always);
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
		throw SyntaxError(string("Несовпадение количества параметров при вызове алгоритма ")+GetName(), line);
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

	if (!vctBody.size()) // Алгоритм пуст.
		pCurrentCommand = -1;
	else
		pCurrentCommand = 0; // Устанавливаем на первую.

	while(1){

		if (pCurrentCommand == -1) // Алгоритм завершен.
			break;
		else                                        // Выполнием следующую команду
			if ( vctBody[pCurrentCommand]->Execute(Header, this) ){ // Делаем шаг; если эта команда выполнена...
				if (pCurrentCommand == (vctBody.size()-1)) 		    // Если эта последняя и сдвигать некуда.
					pCurrentCommand = -1;                         	// Устанавливаем пометку останова
				else {
					if (pCurrentCommand != -1) 						// TVariable::Copy может изменить в Execute
						pCurrentCommand++;   						// Иначе сдвигаем на следующую.
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
	MessageBox(Application->Handle, "Произошло переполнение стека.\nПо-видимому, это следствие бесконечной рекурсии в алгоритме.", "Критическая ошибка в работе программы", MB_ICONSTOP);
	throw RuntimeError("Произошло переполнение стека. По-видимому, это следствие бесконечной рекурсии в алгоритме.");
}
catch (...) {
	MessageBox(Application->Handle, "Произошла недетектируемая фатальная ошибка в работе интерпретатора.", "Критическая ошибка в работе программы", MB_ICONSTOP);
	throw RuntimeError("Произошла фатальная ошибка в работе интерпретатора.");
}
}
//---------------------------------------------------------------------------

