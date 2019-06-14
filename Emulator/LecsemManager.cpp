//---------------------------------------------------------------------------
#pragma hdrstop
#include "LecsemManager.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------

LecsemManager::LecsemManager(const vector< vector<string> >& vvstr):
vvstrLecsems(vvstr), line(-1), lecs(-1){}
//---------------------------------------------------------------------------

string LecsemManager::GetNext() {
	if (line == -1) {
		// »щем самый первый
		for (int i = 0; i < vvstrLecsems.size(); i++) {
			if (vvstrLecsems[i].empty()) continue;
			// ELSE:
			line = i;
			lecs = 0;
			return vvstrLecsems[line][0];
		}

		return "#";
	}


	for (int i = line; i < vvstrLecsems.size(); i++) {
		int j = (i==line)?lecs+1:0;
		if ( j >= vvstrLecsems[i].size() ) continue;
			// ELSE:
			line = i;
			lecs = j;
			return vvstrLecsems[line][lecs];
	}

	return "#";
}

//---------------------------------------------------------------------------
bool LecsemManager::IsLecsPresents() {
	if (line == -1) {
		// »щем самый первый
		for (int i = 0; i < vvstrLecsems.size(); i++) {
			if (vvstrLecsems[i].empty()) continue;
			return 1;
		}

		return 0;
	}


	for (int i = line; i < vvstrLecsems.size(); i++) {
		int j = (i==line)?lecs+1:0;
		if ( j >= vvstrLecsems[i].size() ) continue;
			return 1;
	}

	return 0;
}

//---------------------------------------------------------------------------

string LecsemManager::GetPrev() {
	for (int i = line; i > 0; i--) {
		int j = (i==line)?lecs-1:vvstrLecsems[i].size()-1;
		if ( j < 0 ) continue;
			// ELSE:
			return vvstrLecsems[i][j];
	}

	return "#";
}
//---------------------------------------------------------------------------
