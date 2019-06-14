//$$---- Form HDR ----
//---------------------------------------------------------------------------

#ifndef UConsoleH
#define UConsoleH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include "Variables.h"
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include <AppEvnts.hpp>
#include <string>
//---------------------------------------------------------------------------
class TfmConsole : public TForm
{
__published:	// IDE-managed Components
	TMemo *Memo1;
	TEdit *Edit1;
	TPanel *Panel1;
	TImage *Image1;
	TApplicationEvents *ApplicationEvents1;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall Memo1Enter(TObject *Sender);
	void __fastcall Edit1KeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall SpeedButton1Click(TObject *Sender);
	void __fastcall ApplicationEvents1Message(tagMSG &Msg, bool &Handled);
private:	// User declarations
	bool __fastcall Read(TVariable& var);
public:		// User declarations
	std::string m_str;
	__fastcall TfmConsole(TComponent* Owner);

	void __fastcall ReadLn(TVariable& var);
	void __fastcall WriteLn(const TVariable& var);
	void __fastcall WriteVar(const TVariable& var);
	void __fastcall WriteExc(const TVariable& var, TAlgorithm* pAlg);
};
//---------------------------------------------------------------------------
extern PACKAGE TfmConsole *fmConsole;
//---------------------------------------------------------------------------
#endif
