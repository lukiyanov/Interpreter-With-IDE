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
const char szLegalChars[] = " \t0987654321,._+-*/><=:;()[]abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZАБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯабвгдеёжзийклмнопрстуфхцчшщъыьэюя";
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
	DebugTrace(">> Удаление комментариев...", always);
	vector<string> result;
	string str;
	bool bIsString;
	for (int i = 0; i < vct.size(); i++) {
		str.clear();
		bIsString = 0;
		for (int j = 0; j < vct[i].size(); j++) {
			if (vct[i][j]=='\'') bIsString = !bIsString;	// Осторожно: сторки
			if (vct[i][j]=='|' && !bIsString) break;
			str+=vct[i][j];
		}
		result.push_back(str);
	}

	DebugTrace(">> Завершено. Полученный алгоритм:");
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
// Разделитель?
bool IsDividerSym(char ch) {
	for (int i = 0; szDividers[i]!='\0'; i++)
		if (szDividers[i]==ch) return 1;
	return 0;
}

// Допустимый символ (строки не в счет)?
bool IsLegalSym(char ch) {
	for (int i = 0; szLegalChars[i]!='\0'; i++)
		if (szLegalChars[i]==ch) return 1;
	return 0;
}

// Эти два символа - одна лексема?
// Для неразрывных лексем типа :=, <>, >=, <=, **
bool IsDblLecs(char first, char last) {
	for (int i = 0; i < nDblLecsQwo; i++) {
			if (aszDblLecs[i][0]==first && aszDblLecs[i][1]==last)
			return 1;
	}
	return 0;
}

// Разбиение строки на лексемы
vector<string> GetLecsems(string line, int lineNum){
	vector<string> vstr;
	bool bIsString = 0;
	string lecsem;
	for (int i = 0; i < line.size(); i++) {
		if (bIsString) {            // Если ранее была открыта строка...
			lecsem+=line[i];       		// Необходимо и если line[i] == '\'' и если нет
			if (line[i]=='\'') {    	// Если закрытие строки...
				bIsString = 0;          	// Сбрасываем признак строки
				vstr.push_back(lecsem); 	// Добавляем готовую лексему
				lecsem.clear();         	// Очищаем буфер лексемы
			}
			continue;                   // Дальнейшую обработку не продолжать,
										// так мы избавились от else{...}
		}
									// ПРИМЕЧАНИЕ: если мы здесь, то строка не открыта
		if (line[i]=='\'') {        // Если открывается строка...
			bIsString = 1;          // Устанавливаем признак строки

			if (!lecsem.empty()) {  //	Если до строки мы что-то сунули в буфер...
				vstr.push_back(lecsem); 	// Добавляем лексему
				lecsem.clear();         	// Очищаем буфер лексемы
			}

			lecsem+=line[i];        // Кладем в буфер '\''
			continue;                   // Дальнейшую обработку не продолжать,
										// так мы избавились от еще одного else{...}
		}
									// ПРИМЕЧАНИЕ: если мы здесь, то дело происходит не в строке

		if (!IsLegalSym(line[i]))   // Что-то непонятное юзер наколбасил...
			throw SyntaxError(string("Найден недопустимый символ: \'")+line[i]+'\'',lineNum);

		if (IsDividerSym(line[i])){ // Если встретился разделитель

			// Обработка двойных лексем BEGIN
			if ( (!lecsem.empty()) && IsDblLecs(lecsem[0],line[i])) { // При нормальной работе, разделитель может быть в lecsem только один
				vstr.push_back(lecsem+line[i]);
				lecsem.clear();
				continue;
			}
			// Обработка двойных лексем END


			if (!lecsem.empty())
				vstr.push_back(lecsem); // Закладываем в вектор то, что у нас было в буфере,
										// если он не пуст
			lecsem.clear();
			if (line[i]!=' ') {     // Пробелы не добавляем в vstr
				lecsem = line[i];   	// Другие разделители-пожалуйста
			}
			continue;               // Избавились от ветвления
		}

		if ((!lecsem.empty()) && IsDividerSym(lecsem[0])) { // Если после разделителя идет не разделитель
			vstr.push_back(lecsem);
			lecsem.clear();
			// НЕ НАДО continue!
		}

		lecsem+=line[i];			// Если не разделитель, добавляем очередной символ в буфер
	}

	if (!lecsem.empty())     		// На случай, если последний символ строки - не разделитель
		vstr.push_back(lecsem);

	if (bIsString)					// Если строка не была закрыта...
		throw SyntaxError   			// Возбуждаем исключение
		(string("Найдена незакрытая строка (")+lecsem+"), поставьте символ \' в конце строки",lineNum);
									// Это наш ответ
	return vstr;
}

vector< vector<string> > DivToLecsems(const vector<string>& vct) {
	DebugTrace(">> Разбиение на лексемы...", always);

	vector< vector<string> > vvstr;
	for (int i = 0; i < vct.size(); i++) {
		vvstr.push_back(GetLecsems(vct[i],i));
	}


	DebugTrace(">> Завершено. Результат разбиения:");
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
	DebugTrace("_______________________________________ НОВАЯ СЕССИЯ ОТЛАДКИ _______________________________________");
	DebugTrace("***>> Подготовка к эмуляции/трансляции, идет заполнение структур... <<***", always);
	UpperCase(vct);
	TabsToSpaces(vct);
	vct = DeleteComment(vct);
	LecsemManager lecsGen(DivToLecsems(vct));
	DebugTrace(">> Генератор лексем инициализирован и готов к работе");

	if (!lecsGen.IsLecsPresents())
		throw LogicalError("Не найдено ни одного лексического элемента в конечном алгоритме, процесс завершен",0);

	#ifndef ALLOW_MEMORY_LOOSE
	// Если очищать память сразу после завершения Run(),
	// то нельзя будет просматривать значения переменных ПОСЛЕ запуска.
	// Удаление фрагмента ведет к утечке памяти при ((каждом FillSession())-1).
	// Фрагмент первое время вел себя не очень стабильно с
	// виртуальным деструктором класса Algorithm.
	for (int i = 0; i < algs.size(); i++)
		if (!algs[i]->bStdAlg)
			delete algs[i];
	#endif

	algs.clear();

	// FILL STD
	for (int i = 0; i < vctStdAlgs.size(); i++) {
		algs.push_back(vctStdAlgs[i]);
	}

	DebugTrace(">> Идет поиск алгоритмов...");
	DebugTrace(">> Идет проверка вложенности алгоритмов...");
	lecsGen.line = -1;
	bool bLastLex = 1; // 0-АЛГ 1-КОН
	string str;
	do{
		str = lecsGen.GetNext();
		if (str == "КОН") {
			if (bLastLex)
				throw SyntaxError("Найдено два \'КОН\' подряд", lecsGen.line);
			bLastLex = 1;
		}
		if (str == "АЛГ") {
			if (!bLastLex)
				throw SyntaxError("Вложенные алгоритмы не допускаются, закройте алгоритм ключевым словом \'КОН\' прежде чем начать новый", lecsGen.line);
			bLastLex = 0;
		}
	}while(str != "#");
	DebugTrace(">> Проверка вложенности алгоритмов завершена, вложенных алгоритмов не найдено");

	// FILL USER
	DebugTrace(">> Идет проверка заголовков алгоритмов...");
	lecsGen.line = -1;
	while (lecsGen.IsLecsPresents()) {
		algs.push_back(new TAlgorithm(lecsGen));
	}
	DebugTrace(">> Проверка заголовков алгоритмов завершена");
	DebugTrace(">> Поиск алгоритмов завершен");

	DebugTrace(">> Идет проверка синтаксиса...", always);
	lecsGen.line = -1;
	int i = vctStdAlgs.size();
	while (lecsGen.IsLecsPresents()) {
		algs[i]->Fill(lecsGen);
		i++;
	}
	DebugTrace(">> Проверка синтаксиса завершена, ошибок не найдено");


	RefreshVarWnd();
}

void TEmulator::Run(bool bStepOver) {
	bNeedToStop = 0;
	DebugTrace("***>> Запуск эмулятора... <<***", always);

	fmConsole->Memo1->Lines->Add("_______________________________________________");
	fmMainView->Memo4->Lines->Add("_______________________________________________");
	if (fmRunParams->CheckBox1->Checked) {
		fmConsole->Memo1->Clear();
		fmConsole->Edit1->Clear();
	}
	fmConsole->Memo1->Lines->Add(String("Выполняется алгоритм ")
	+algs[fmRunParams->GetMainAlgorithm()]->GetName().c_str());
	fmMainView->Memo4->Lines->Add(String("Выполняется алгоритм ")
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

	DebugTrace("***>> Эмулятор успешно инициализирован и запущен. <<***", always);

	if (!bStepOver) {
		ShowTab("Переменные",1);
		alg->RunInit();
		Continue();
	}
	else {
		DebugTrace("-> Эмулятор приостановлен и готов к пошаговому режиму, нажмите ШАГ для выполнения следующей команды или ЗАПУСК для перехода в обычный режим выполнения алгоритма.", always);
		alg->RunInit();
		ShowTab("Переменные",0);
		ShowTab("Переменные",1);
		fmMainView->Invalidate();
	}
}

void TEmulator::StepIt() {
	if (esState == esStoped) {
		MessageBox(Application->Handle, "Шаг отладки недоступен, для его использования начните отладку\n (Запуск->Начать отладку)", "Ошибка запуска отладчика", MB_ICONINFORMATION);
		return;
	}

	if (esState == esRunning) {
		MessageBox(Application->Handle, "Шаг отладки недоступен, т.к алгоритм уже запущен в обычном режиме,\n для использования шага отладки начните отладку\n (Запуск->Начать отладку)", "Ошибка запуска отладчика", MB_ICONINFORMATION);
		return;
	}

	algs[fmRunParams->GetMainAlgorithm()]->Step();

	if (bNeedToStop) {
		DebugTrace("***>> Эмулятор завершил работу, в ходе эмуляции ошибок не возникло. <<***", always);
		esState = esStoped;
		ShowTab("Переменные",0);
		ShowTab("Переменные",1);
	}

}

void TEmulator::Stop() {
	if (esState == esStoped) {
		MessageBox(Application->Handle, "Алгоритм не запущен", "Ошибка работы с отладчиком", MB_ICONINFORMATION);
		return;
	}

	DebugTrace("***>> Выполнение алгоритма прервано пользователем. <<***", always);
	bNeedToStop = 1;

	esState = esStoped;
	if (esState == esTracing) {
		StepIt();
		ShowTab("Переменные",0);
		ShowTab("Переменные",1);
	}
}

void TEmulator::Continue() {
	esState = esRunning;
	while (!Emulator.bNeedToStop) {
		algs[fmRunParams->GetMainAlgorithm()]->Step();
		Application->ProcessMessages();
	}

	DebugTrace("***>> Эмулятор завершил работу, в ходе эмуляции ошибок не возникло. <<***", always);
	esState = esStoped;
}
