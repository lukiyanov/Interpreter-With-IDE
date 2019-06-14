//$$---- Form CPP ----
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UConsoleSettings.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfmConsoleSettings *fmConsoleSettings;
//---------------------------------------------------------------------------
__fastcall TfmConsoleSettings::TfmConsoleSettings(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfmConsoleSettings::FormShow(TObject *Sender)
{
	Chx1 = CheckBox1->Checked;
	Chx2 = CheckBox2->Checked;
	Cl1  = ColorBox1->Color;
	Cl2  = ColorBox2->Color;
	OK = 0;
	BitBtn2->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TfmConsoleSettings::BitBtn2Click(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------
void __fastcall TfmConsoleSettings::BitBtn1Click(TObject *Sender)
{
	OK = 1;
	Close();
}
//---------------------------------------------------------------------------
void __fastcall TfmConsoleSettings::FormClose(TObject *Sender,
	  TCloseAction &Action)
{
	if (OK) {
		CheckBox1->Checked = Chx1;
		CheckBox2->Checked = Chx2;
		ColorBox1->Color = Cl1;
		ColorBox2->Color = Cl2;
	}
}
//---------------------------------------------------------------------------
