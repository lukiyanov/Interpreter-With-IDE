//$$---- Form CPP ----
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UEditorSettings.h"
#include "UnitMainForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cspin"
#pragma resource "*.dfm"
TfmEditorSettings *fmEditorSettings;
//---------------------------------------------------------------------------
__fastcall TfmEditorSettings::TfmEditorSettings(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfmEditorSettings::CheckBox1Click(TObject *Sender)
{
	TNotifyEvent p = fmMainView->Memo2->OnChange;
	fmMainView->Memo2->OnChange = 0;

	if (!fmEditorSettings->CheckBox1->Checked) {
		fmMainView->Memo2->SelStart  = 0;
		fmMainView->Memo2->SelLength = fmMainView->Memo2->Text.Length();
		fmMainView->InitByFont(fmMainView->Memo2->SelAttributes, fmMainView->GetCodeFont());
	}
	else {
		fmMainView->PaintInColors();
	}

	fmMainView->Memo2->OnChange = p;
	CheckBox3->Enabled = CheckBox1->Checked;
//	CheckBox5->Enabled = CheckBox1->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TfmEditorSettings::BitBtn1Click(TObject *Sender)
{
	OK = 1;
	Close();
}
//---------------------------------------------------------------------------

void __fastcall TfmEditorSettings::CheckBox2Click(TObject *Sender)
{
	fmMainView->Bevel1->Visible = CheckBox2->Checked;
	fmMainView->Panel5->Visible = CheckBox2->Checked;
	fmMainView->Panel4->Visible = CheckBox2->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TfmEditorSettings::BitBtn2Click(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------

void __fastcall TfmEditorSettings::FormClose(TObject *Sender,
	  TCloseAction &Action)
{
	if (OK) {
		CheckBox1->Checked = bChx1;
		CheckBox2->Checked = bChx2;
		CheckBox3->Checked = bChx3;
		CheckBox4->Checked = bChx4;
		CheckBox5->Checked = bChx5;
		CSpinEdit1->Value  = nSpin1;
	}
}
//---------------------------------------------------------------------------

void __fastcall TfmEditorSettings::FormShow(TObject *Sender)
{
	bChx1  = CheckBox1->Checked;
	bChx2  = CheckBox2->Checked;
	bChx3  = CheckBox3->Checked;
	bChx4  = CheckBox4->Checked;
	bChx5  = CheckBox5->Checked;
	nSpin1 = CSpinEdit1->Value;
	OK = 0;
	BitBtn2->SetFocus();
	Height = 288;
}
//---------------------------------------------------------------------------

void __fastcall TfmEditorSettings::CheckBox3Click(TObject *Sender)
{
	fmMainView->thrd->Suspended = !CheckBox3->Checked;
//	CheckBox5->Enabled = !CheckBox3->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TfmEditorSettings::BitBtn3Click(TObject *Sender)
{
	if (Height == 288) {
		Height = 418;
		Memo1->Visible = 1;
	}
	else {
		Height = 288;
		Memo1->Visible = 0;
    }
}
//---------------------------------------------------------------------------

void __fastcall TfmEditorSettings::CheckBox4Click(TObject *Sender)
{
	CSpinEdit1->Enabled = CheckBox4->Checked;
}
//---------------------------------------------------------------------------

