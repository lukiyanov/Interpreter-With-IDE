//---------------------------------------------------------------------------
#ifndef EditHistoryH
#define EditHistoryH

#include <vcl.h>
#include <deque>
using namespace std;
//---------------------------------------------------------------------------

struct CEditState{
	String	strText;
	int		nSelStart;
	int		nSelLength;
	CEditState(
		String	Text,
		int		SelStart,
		int		SelLength
	): strText(Text), nSelStart(SelStart), nSelLength(SelLength){}
};

//---------------------------------------------------------------------------

class CEditHistory{
	deque<CEditState> dquState;
	int nUndoCnt;
public:
	CEditHistory();
	void Undo(TCustomMemo* edit);
	void Redo(TCustomMemo* edit);
	bool CanUndo();
	bool CanRedo();
	void AddState(TCustomMemo* edit);
	void Clear();
};

//---------------------------------------------------------------------------
extern CEditHistory ehHistoryManager;
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
