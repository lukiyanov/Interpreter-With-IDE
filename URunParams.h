//$$---- Form HDR ----
//---------------------------------------------------------------------------

#ifndef URunParamsH
#define URunParamsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------
class TfmRunParams : public TForm
{
__published:	// IDE-managed Components
	TGroupBox *GroupBox1;
	TLabel *Label1;
	TRadioButton *RadioButton1;
	TRadioButton *RadioButton2;
	TRadioButton *RadioButton3;
	TComboBox *ComboBox1;
	TBitBtn *BitBtn1;
	TBitBtn *BitBtn2;
	TBevel *Bevel2;
	TImage *Image1;
	TBevel *Bevel1;
	TCheckBox *CheckBox1;
	TCheckBox *CheckBox2;
	TBitBtn *BitBtn3;
	TMemo *Memo1;
	void __fastcall BitBtn1Click(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall RadioButton1Click(TObject *Sender);
	void __fastcall CheckBox2Click(TObject *Sender);
	void __fastcall BitBtn2Click(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall BitBtn3Click(TObject *Sender);
private:	// User declarations
	bool bChx1;
	bool bChx2;
	int  nRtn1;
	int  nRtn2;
	int  nRtn3;
	String alg;
	bool OK;

public:		// User declarations
	int __fastcall GetMainAlgorithm();
	__fastcall TfmRunParams(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfmRunParams *fmRunParams;
//---------------------------------------------------------------------------
#endif
