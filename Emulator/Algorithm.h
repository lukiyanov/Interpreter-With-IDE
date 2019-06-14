#ifndef AlgorithmH
#define AlgorithmH
//---------------------------------------------------------------------------
#include "Variables.h"
#include "LecsemManager.h"
#include "Errors.h"
#include "VarHeader.h"
#include "Commands.h"
#include <vector>
#include <string>
#include <deque>
using namespace std;
//---------------------------------------------------------------------------

class TAlgorithm {
public:
  TVarHeader Header;            // Заголовок алгоритма со всеми его переменными
  vector<Command*> vctBody;     // Тело алгоритма
  int pCurrentCommand;		    // Текущая команда (ещё не выполненная), для последней = -1
  bool bStdAlg;                 // Признак стандартнсти алгоритма, для sin(x),cos(x),...
  vector<TVariable> vctParams;  // Список аргументов алгоритма по порядку раздела АРГ
public:
	TAlgorithm(LecsemManager& lecsGen);
	virtual ~TAlgorithm();

	void Fill(LecsemManager& lecsGen);
	string GetName(){return Header.GetAlgName();};
	vector<TVariable> GetParams();
	TVarDataType GetResultType();

	virtual void 	  CheckCall	(const deque<TVariable*>& params, int line);
	virtual TVariable Call		(const deque<TVariable>& params);

public:
	void Step();
	void RunInit();
	void ShowPos();
	int  GetLine();
private:
	void CheckToSimpleType(const string&, TVariable&, int);
	bool FindType(LecsemManager&, TVariable&);

protected:
	TAlgorithm(){} // Для STD - алгоритмов
};

//---------------------------------------------------------------------------
#endif
