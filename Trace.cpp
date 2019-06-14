//---------------------------------------------------------------------------
#pragma hdrstop
#include "Trace.h"
#include <fstream>
#include "URunParams.h"
#pragma package(smart_init)
using namespace std;

TStringGrid* grid;
TfmMainView* form;
ofstream* err_fout = 0;

//---------------------------------------------------------------------------

 void DebugTrace(const string& str, dtStyle style) {
// text    - extended only  ttt
// always  - always         ttt
// error   - always         !!!>> ОШИБКА: [line] ttt <<<!!!

	switch (style) {
	case text :
		if (!form->N43->Checked) break;
	case always :
		grid->RowCount = grid->RowCount + 1;
		grid->Cells[0][grid->RowCount-1] = str.c_str();
		if (fmRunParams->CheckBox1->Checked)
			*err_fout << str << endl;
		break;
	case error :
		grid->RowCount = grid->RowCount + 1;
		grid->Cells[0][grid->RowCount-1] = (string("!!!>>> ОШИБКА! ")+str+" <<<!!!").c_str();
		if (fmRunParams->CheckBox1->Checked)
			*err_fout << str << endl;
		break;
	}
	grid->Row = grid->RowCount-1;
}

//---------------------------------------------------------------------------

void TraceInit(TfmMainView* f) {
	if (!err_fout) {
		err_fout = new ofstream((GetPath()+"\\debug output.txt").c_str());
	}

	grid = f->StringGrid1;
	form = f;
	grid->RowCount = 1;
}

//---------------------------------------------------------------------------
