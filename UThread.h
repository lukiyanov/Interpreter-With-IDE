//$$---- Thread HDR ----
//---------------------------------------------------------------------------

#ifndef UThreadH
#define UThreadH
//---------------------------------------------------------------------------
#include <Classes.hpp>
//---------------------------------------------------------------------------
class CPaintThread : public TThread
{
private:
protected:
	void __fastcall Execute();
public:
//	volatile
	bool loop;
	__fastcall CPaintThread(bool CreateSuspended);
};
//---------------------------------------------------------------------------
#endif
