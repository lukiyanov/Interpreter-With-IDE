//---------------------------------------------------------------------------
#ifndef SearchH
#define SearchH
#include <vcl.h>
//---------------------------------------------------------------------------
class CSearch {
	String	strTextToSearch;
public:
	void SearchInit();
	void SearchAgain();
	String GetSearchText()
		{return strTextToSearch;}
};
//---------------------------------------------------------------------------
extern CSearch mSearch;
//---------------------------------------------------------------------------
#endif
