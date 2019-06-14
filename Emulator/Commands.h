#ifndef CommandsH
#define CommandsH
#include <deque>
#include "LecsemManager.h"
#include "VarHeader.h"
//---------------------------------------------------------------------------

struct Command {
	int line;
	vector<Command*> vctSubCommands;
public:
	virtual ~Command();
	virtual bool Execute(TVarHeader& hdrHost, TAlgorithm* pAlg) = 0;
	virtual vector<string> ToPascal() = 0;
	virtual int GetLine() {return line;}  // Контейнеры переопределяют.
};

Command* FillLecsem(TVarHeader& hdrHost, LecsemManager& lecsGen);

//---------------------------------------------------------------------------

struct CmdExpression : Command {
	TVarHeader buffer;
	deque<string> vct;
	TVariable* pvarStaticType;
	deque<string> vctEXCEPTION;
	TVariable* pvarExecuteResult;
public:
	~CmdExpression();
	CmdExpression(TVarHeader& hdrHost, LecsemManager& lecsGen);
	bool Execute(TVarHeader& hdrHost, TAlgorithm* pAlg);
	vector<string> ToPascal();
};
//---------------------------------------------------------------------------

struct CmdCopyExpression : Command {
	CmdExpression* exp;
	string strVarOriginal;
	deque<string> queI;
	deque<string> queJ;
	TVarHeader buf;
public:
	CmdCopyExpression(TVarHeader& hdrHost, LecsemManager& lecsGen);
	~CmdCopyExpression();
	bool Execute(TVarHeader& hdrHost, TAlgorithm* pAlg);
	vector<string> ToPascal();
};
//---------------------------------------------------------------------------

struct CmdAlgExpression : Command {
	TVarHeader buffer;
	deque<string> vct;
public:
	CmdAlgExpression(TVarHeader& hdrHost, LecsemManager& lecsGen);
	bool Execute(TVarHeader& hdrHost, TAlgorithm* pAlg);
	vector<string> ToPascal();
};
//---------------------------------------------------------------------------

struct CmdIfExpression : Command {
	TVarHeader 			buffer;
	// int 				Command::line;
	// vector<Command*> Command::vctSubCommands;
	vector<Command*> vctElseSubCommands;
	deque<string> deqCondition;
	int  nCurrentCmd; // Execute, -1 = Condition
	bool bTruth;      // Execute, ТО - ИНАЧЕ
public:
	CmdIfExpression(TVarHeader& hdrHost, LecsemManager& lecsGen);
	bool Execute(TVarHeader& hdrHost, TAlgorithm* pAlg);
	int GetLine();
	vector<string> ToPascal();
};
//---------------------------------------------------------------------------

struct CmdWhileExpression : Command {
	TVarHeader 			buffer;
	// int 				Command::line;
	// vector<Command*> Command::vctSubCommands;
	deque<string> deqCondition;
	int  nCurrentCmd; // Execute, -1 = Condition
public:
	CmdWhileExpression(TVarHeader& hdrHost, LecsemManager& lecsGen);
	bool Execute(TVarHeader& hdrHost, TAlgorithm* pAlg);
	int GetLine();
	vector<string> ToPascal();
};
//---------------------------------------------------------------------------
#endif
