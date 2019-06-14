#ifndef StdAlgorithmsH
#define StdAlgorithmsH
//---------------------------------------------------------------------------
#include <vector>
#include "Algorithm.h"
using std::vector;
//---------------------------------------------------------------------------

class TStrdAlg_OUT :public TAlgorithm {     // WriteLn �����(�,�,...)
public:
	TStrdAlg_OUT();
	void 	  CheckCall	(const deque<TVariable*>& params, int line);
	TVariable Call(const deque<TVariable>& params);
	TVarDataType GetResultType();
};

//---------------------------------------------------------------------------

class TStrdAlg_OutStr :public TAlgorithm {     // Write �����(�,�,...)
public:
	TStrdAlg_OutStr();
	void 	  CheckCall	(const deque<TVariable*>& params, int line);
	TVariable Call(const deque<TVariable>& params);
	TVarDataType GetResultType();
};

//---------------------------------------------------------------------------
class TStrdAlg_Sin :public TAlgorithm {     // SIN (a)
public:
	TStrdAlg_Sin();
	void 	  CheckCall	(const deque<TVariable*>& params, int line);
	TVariable Call(const deque<TVariable>& params);
	TVarDataType GetResultType();
};

//---------------------------------------------------------------------------
class TStrdAlg_Cos :public TAlgorithm {     // COS (a)
public:
	TStrdAlg_Cos();
	void 	  CheckCall	(const deque<TVariable*>& params, int line);
	TVariable Call(const deque<TVariable>& params);
	TVarDataType GetResultType();
};

//---------------------------------------------------------------------------
class TStrdAlg_Round :public TAlgorithm {   // ��� -> ���        ������(�����)
public:
	TStrdAlg_Round();
	void 	  CheckCall	(const deque<TVariable*>& params, int line);
	TVariable Call(const deque<TVariable>& params);
	TVarDataType GetResultType();
};

//---------------------------------------------------------------------------
class TStrdAlg_Length :public TAlgorithm {  // ���������� ����� ������  �����(���)
public:
	TStrdAlg_Length();
	void 	  CheckCall	(const deque<TVariable*>& params, int line);
	TVariable Call(const deque<TVariable>& params);
	TVarDataType GetResultType();
};

//---------------------------------------------------------------------------
class TStrdAlg_StrGet :public TAlgorithm {  // ���������� ������ � ������ �����(���, ���)
public:
	TStrdAlg_StrGet();
	void 	  CheckCall	(const deque<TVariable*>& params, int line);
	TVariable Call(const deque<TVariable>& params);
	TVarDataType GetResultType();
};

//---------------------------------------------------------------------------
class TStrdAlg_StrSet :public TAlgorithm {  // ���������� ������ � ������������� ��������� ����(���, ���, ���)
public:
	TStrdAlg_StrSet();
	void 	  CheckCall	(const deque<TVariable*>& params, int line);
	TVariable Call(const deque<TVariable>& params);
	TVarDataType GetResultType();
};

/*
//---------------------------------------------------------------------------
class TStrdAlg_GetRows :public TAlgorithm { // �-�� ��������, ��� ���� = 1
public:
	TStrdAlg_GetRows();
	void 	  CheckCall	(const deque<TVariable*>& params, int line);
	TVariable Call(const deque<TVariable>& params);
	TVarDataType GetResultType();
};

//---------------------------------------------------------------------------
class TStrdAlg_GetCols :public TAlgorithm { // �-�� ����
public:
	TStrdAlg_GetCols();
	void 	  CheckCall	(const deque<TVariable*>& params, int line);
	TVariable Call(const deque<TVariable>& params);
	TVarDataType GetResultType();
};                                      */
		

extern vector<TAlgorithm*> vctStdAlgs;
void STDAlgInit();
int ROUND( double x );

//---------------------------------------------------------------------------
#endif
