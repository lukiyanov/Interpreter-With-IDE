#ifndef VariablesH
#define VariablesH
//---------------------------------------------------------------------------
#include <vector>
#include <string>
using namespace std;
//---------------------------------------------------------------------------

class TAlgorithm;
class TVariable;

// ss - Support state ( not engine code fragment )
enum ss_TNoInit
  { ss_niSupportState};

enum TVarDataType
  { vtVoid, vtInt, vtDouble, vtString, vtChar, vtBool, vtArray, vtMatrix };

enum TVarState
  { vsLocal, vsArgument, vsResult, vsAlgName, vsException, vsConst, vsExpression, vsOther };

struct TValue {
	int 	intValue;
	double 	dblValue;
	string 	strValue;
	char 	chrValue;
	bool 	blnValue;

	struct {
		vector<TVariable> arr;
		int nColCnt;
		TVarDataType varElemType;
	} 		arrValue;

	struct {
		vector<vector<TVariable> > mtx;
		int nRowCnt;
		int nColCnt;
		TVarDataType varElemType;
	}		mtxValue;

};

class TVariable {
public:
  string		 strName;
  TVarDataType varType;
  TVarState	 varState;
  bool		 bStaticInit;
  bool		 bDynamicInit;
  TValue		 Value;

public:  // methods:
 /*
	Никаких конструкторов по умолчанию для неинициализированных объектов.
	Только явный вызов уконструктора!
 */
	explicit TVariable(ss_TNoInit){}
	TVariable(string Name, TVarDataType Type, int Line);
	TVariable(string Name, TVariable Type, int Line);
	string GetStrName();
	string GetStrType();
	string GetStrValue();

private:
/*
Так криво сделано из-за невозможности вызова одного конструктора
из блока {} другого конструктора
*/
	void Init(string Name, TVariable Type, int Line);
	void StdCheck(const TVariable& var, int line);

public:

	TVariable CheckAdd(const TVariable& var, int line);
	TVariable CheckSub(const TVariable& var, int line);
	TVariable CheckMul(const TVariable& var, int line);
	TVariable CheckDiv(const TVariable& var, int line); // 5:2 = 2.5 ВЕЩ |
	TVariable CheckMod(const TVariable& var, int line); // 5:2 = 2   ЦЕЛ |
	TVariable CheckPow(const TVariable& var, int line);

	TVariable CheckEqvalCmp			(const TVariable& var, int line);
	TVariable CheckNotEqvalCmp		(const TVariable& var, int line);
	TVariable CheckGreaterCmp		(const TVariable& var, int line);
	TVariable CheckLessCmp			(const TVariable& var, int line);
	TVariable CheckGreaterOrEqCmp	(const TVariable& var, int line);
	TVariable CheckLessOrEqCmp		(const TVariable& var, int line);

	TVariable CheckAND	(const TVariable& var, int line);
	TVariable CheckOR	(const TVariable& var, int line);

	TVariable CheckNOT	(int line); // НЕ a
	TVariable CheckNeg	(int line); //  - a


	// Контроль за индексами - на стадии Run()
	// 
	TVariable CheckIndexing(const TVariable& varI, int line);
	TVariable CheckIndexing(const TVariable& varI, const TVariable& varJ, int line);

	void CheckCopy(const TVariable& var, int line);

public:
	TVariable Add(const TVariable& var);
	TVariable Sub(const TVariable& var);
	TVariable Mul(const TVariable& var);
	TVariable Div(const TVariable& var); // 5/2 = 2.5 ВЕЩ |
	TVariable Mod(const TVariable& var); // 5:2 = 2   ЦЕЛ |
	TVariable Pow(const TVariable& var);

	TVariable EqvalCmp			(const TVariable& var);
	TVariable NotEqvalCmp		(const TVariable& var);
	TVariable GreaterCmp		(const TVariable& var);
	TVariable LessCmp			(const TVariable& var);
	TVariable GreaterOrEqCmp	(const TVariable& var);
	TVariable LessOrEqCmp		(const TVariable& var);

	TVariable AND	(const TVariable& var);
	TVariable OR	(const TVariable& var);

	TVariable NOT	(); // НЕ a
	TVariable Neg	(); //  - a

	// +Контроль за индексами!
	TVariable* Indexing(const TVariable& varI);
	TVariable* Indexing(const TVariable& varI, const TVariable& varJ);

	void Copy(const TVariable& var, TAlgorithm* alg);
};

bool IsKeyword(const string& word);
//---------------------------------------------------------------------------
extern const char szFigures[];
//---------------------------------------------------------------------------
#endif
