//$$---- Form HDR ----
//---------------------------------------------------------------------------

#ifndef UEditorSettingsH
#define UEditorSettingsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <Dialogs.hpp>
#include <Graphics.hpp>
#include "cspin.h"
//---------------------------------------------------------------------------
class TfmEditorSettings : public TForm
{

__published:	// IDE-managed Components
	TBevel *Bevel1;
	TBitBtn *BitBtn1;
	TBitBtn *BitBtn2;
	TCheckBox *CheckBox1;
	TBevel *Bevel2;
	TImage *Image1;
	TBevel *Bevel3;
	TLabel *Label1;
	TCheckBox *CheckBox2;
	TCheckBox *CheckBox3;
	TBitBtn *BitBtn3;
	TMemo *Memo1;
	TCheckBox *CheckBox4;
	TCSpinEdit *CSpinEdit1;
	TLabel *Label2;
	TCheckBox *CheckBox5;
	void __fastcall CheckBox1Click(TObject *Sender);
	void __fastcall BitBtn1Click(TObject *Sender);
	void __fastcall CheckBox2Click(TObject *Sender);
	void __fastcall BitBtn2Click(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall CheckBox3Click(TObject *Sender);
	void __fastcall BitBtn3Click(TObject *Sender);
	void __fastcall CheckBox4Click(TObject *Sender);

private:	// User declarations
	bool bChx1;
	bool bChx2;
	bool bChx3;
	bool bChx4;
	bool bChx5;
	int	 nSpin1;
	bool OK;

public:		// User declarations
	__fastcall TfmEditorSettings(TComponent* Owner);

};
//---------------------------------------------------------------------------
extern PACKAGE TfmEditorSettings *fmEditorSettings;
//---------------------------------------------------------------------------
#endif
