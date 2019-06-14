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
  TVarHeader Header;            // ��������� ��������� �� ����� ��� �����������
  vector<Command*> vctBody;     // ���� ���������
  int pCurrentCommand;		    // ������� ������� (��� �� �����������), ��� ��������� = -1
  bool bStdAlg;                 // ������� ������������ ���������, ��� sin(x),cos(x),...
  vector<TVariable> vctParams;  // ������ ���������� ��������� �� ������� ������� ���
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
	TAlgorithm(){} // ��� STD - ����������
};

//---------------------------------------------------------------------------
#endif
