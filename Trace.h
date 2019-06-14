//---------------------------------------------------------------------------
#ifndef TraceH
#define TraceH
#include <string>
using namespace std;
#include "UnitMainForm.h"
//---------------------------------------------------------------------------
enum dtStyle {text, always, error};
// text    - extended only  ttt
// always  - always         ttt
// error   - always         !!!>> ньхайю: [line] ttt <<<!!!

// Trace to debugger screen
void DebugTrace(const string& str, dtStyle = text);
void TraceInit(TfmMainView* f);
//---------------------------------------------------------------------------
#endif
