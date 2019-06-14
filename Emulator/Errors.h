#ifndef ErrorsH
#define ErrorsH
#include <string>
using namespace std;
//---------------------------------------------------------------------------
struct AlgorithmError {
	string message;
	int    line;
};

/*
	line - непосредственный номер строки,
	соответствует расположению строки в векторе,
	начинается с 0.
*/

struct SyntaxError : AlgorithmError {
	SyntaxError(const string& msg, const int ln);
};

struct LogicalError : AlgorithmError {
	LogicalError(const string& msg, const int ln);
};

struct RuntimeError : AlgorithmError {
	RuntimeError(const string& msg);
};

//---------------------------------------------------------------------------
#endif
