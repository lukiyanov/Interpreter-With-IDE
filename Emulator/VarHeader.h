#ifndef VarHeaderH
#define VarHeaderH
#include "Errors.h"
#include "Variables.h"
//---------------------------------------------------------------------------

class TVarHeader{
public:
  vector<TVariable> vctVariables;

public:
	TVarHeader();
	void PushVar(TVariable& Type, const string& strName, int line);
	void PushAlgName(const string& strName, int line);
	string GetAlgName();
	bool IsResultPresent();
	bool IsExceptionPresent();
	TVariable GetResult();
	TVariable GetException();

	void ToArgument(const string& strName, int line);
	void ToResult(const string& strName, int line);
	void ToExc(const string& strName);

	int GetPosition(const string& Name);
	bool IsPresent(const string& Name);
	TVariable& GetVariable(const string& Name);
	TVariable* GetVariablePTR(const string& Name);

	void Print();
private:
	void CheckName(const string& strName, int line);
};

//---------------------------------------------------------------------------
#endif
