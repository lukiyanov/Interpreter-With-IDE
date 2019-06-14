//$$---- Form HDR ----
//---------------------------------------------------------------------------

#ifndef UConsoleSettingsH
#define UConsoleSettingsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------
class TfmConsoleSettings : public TForm
{
__published:	// IDE-managed Components
	TCheckBox *CheckBox1;
	TCheckBox *CheckBox2;
	TGroupBox *GroupBox2;
	TLabel *Label2;
	TLabel *Label3;
	TColorBox *ColorBox1;
	TColorBox *ColorBox2;
	TBitBtn *BitBtn2;
	TBitBtn *BitBtn1;
	TBevel *Bevel2;
	TImage *Image1;
	TBevel *Bevel1;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall BitBtn2Click(TObject *Sender);
	void __fastcall BitBtn1Click(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);

private:	// User declarations
	bool Chx1;
	bool Chx2;
	TColor Cl1;
	TColor Cl2;
	bool OK;

public:		// User declarations
	__fastcall TfmConsoleSettings(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfmConsoleSettings *fmConsoleSettings;
//---------------------------------------------------------------------------
#endif
