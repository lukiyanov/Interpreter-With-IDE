#ifndef LecsemManagerH
#define LecsemManagerH
//---------------------------------------------------------------------------
#include <vector>
#include <string>
using namespace std;
//---------------------------------------------------------------------------

// "#"- END OF LECSEMS
struct LecsemManager {
	vector< vector<string> > vvstrLecsems;
	int line, lecs;

	LecsemManager(const vector< vector<string> > &vvstr);
	string GetNext(); // get & lecs++
	string GetPrev(); // get only
	bool IsLecsPresents();
};

//---------------------------------------------------------------------------
#endif
