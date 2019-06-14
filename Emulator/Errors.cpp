//---------------------------------------------------------------------------
#pragma hdrstop
#include "Errors.h"
#include "Emulator.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
SyntaxError::SyntaxError(const string& msg, const int ln)
{message = msg; line = ln; Emulator.esState = esStoped;}

LogicalError::LogicalError(const string& msg, const int ln)
{message = msg; line = ln; Emulator.esState = esStoped;}

RuntimeError::RuntimeError(const string& msg)
{message = msg; Emulator.esState = esStoped;}
//---------------------------------------------------------------------------

