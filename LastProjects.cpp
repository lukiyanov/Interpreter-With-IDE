#pragma hdrstop
#include "LastProjects.h"
#include <Registry.hpp>
#include "UnitMainForm.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
string FilterToName(string str) {
	string strExt;

	for (int i = str.size()-1; i >= 0; i--) {
		if (str[i] == '\\') break;
		strExt = str[i] + strExt;
	}

	str = strExt;
	strExt.clear();

	for (int i = 0; i < str.size(); i++) {
		if (str[i] == '.') break;
		strExt += str[i];
	}

	return strExt;
}

void Trim(deque<string>& q) {
	deque<string> p;

	for (int i = 0; i < q.size(); i++)
		if (q[i] != "") p.push_back(q[i]);

	q = p;
}

TLastProjects::TLastProjects():	nMaxProjectsQwo(8){
	TRegistry *reg = new TRegistry();
	reg->OpenKey("InterpretN", 1);

	for (int i = 0; reg->ValueExists(String("LastProject #")+IntToStr(i)) && qstrProjects.size() < nMaxProjectsQwo; i++)
		if (FileExists(reg->ReadString(String("LastProject #")+IntToStr(i))))
			qstrProjects.push_back(reg->ReadString(String("LastProject #")+IntToStr(i)).c_str());

	reg->CloseKey();
	delete reg;
}

TLastProjects::~TLastProjects() {
	TRegistry *reg = new TRegistry();
	reg->OpenKey("InterpretN", 1);

	for (int i = 0; reg->ValueExists(String("LastProject #")+IntToStr(i)); i++)
		reg->DeleteValue(String("LastProject #")+IntToStr(i));

	for (int i = 0, n = 0; i < qstrProjects.size() && n < nMaxProjectsQwo; i++)
		if (FileExists(qstrProjects[i].c_str())) {
			reg->WriteString(String("LastProject #")+IntToStr(n), qstrProjects[i].c_str());
        	n++;
		}

	reg->CloseKey();
	delete reg;
}

void TLastProjects::OpenProject(string str) {
	for (int i = 0; i < qstrProjects.size(); i++) {
		if (qstrProjects[i] == str) {
			qstrProjects.erase(qstrProjects.begin()+i);
			qstrProjects.insert(qstrProjects.begin(), str);
			Trim(qstrProjects);
			return;
		}
	}

	if (qstrProjects.size() == nMaxProjectsQwo)
		qstrProjects.erase(qstrProjects.end()-1);
	qstrProjects.insert(qstrProjects.begin(), str);
	Trim(qstrProjects);
}

deque <string> TLastProjects::GetFullNames() {
	return qstrProjects;
}

deque <string> TLastProjects::GetShortNames() {
	deque<string> q;

	for (int i = 0; i < qstrProjects.size(); i++) {
		q.push_back(FilterToName(qstrProjects[i]));
	}

	return q;
}

