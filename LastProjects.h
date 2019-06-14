#ifndef LastProjectsH
#define LastProjectsH
#include <deque>
#include <string>
using namespace std;
//---------------------------------------------------------------------------
class TLastProjects {
	deque <string> qstrProjects;
	const int nMaxProjectsQwo;
public:
	TLastProjects();
	~TLastProjects();
	void OpenProject(string str);
	deque <string> GetFullNames();
	deque <string> GetShortNames();
};
//---------------------------------------------------------------------------
#endif
