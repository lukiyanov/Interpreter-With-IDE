//$$---- Form CPP ----
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "URunParams.h"
#include "Emulator.h"
#include "StdAlgorithms.h"
#include "UnitMainForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfmRunParams *fmRunParams;
//---------------------------------------------------------------------------

__fastcall TfmRunParams::TfmRunParams(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------

int __fastcall TfmRunParams::GetMainAlgorithm() {
	if (RadioButton1->Checked) {
		return vctStdAlgs.size();
	}
	if (RadioButton2->Checked) {
		return Emulator.algs.size()-1;
	}
	if (RadioButton3->Checked) {
		int n = vctStdAlgs.size();
		for (unsigned i = vctStdAlgs.size(); i < Emulator.algs.size(); i++) {
			if (Emulator.algs[i]->GetName().c_str() == ComboBox1->Text)
				n = i;
		}
		return n;
	}
}
//---------------------------------------------------------------------------

void __fastcall TfmRunParams::BitBtn1Click(TObject *Sender)
{
	OK = 1;
	Close();
}
//---------------------------------------------------------------------------

void __fastcall TfmRunParams::FormShow(TObject *Sender)
{
	Height = 266;
	String str = ComboBox1->Text;
	ComboBox1->Enabled = fmRunParams->RadioButton3->Checked;
	ComboBox1->Clear();
	for (unsigned i = vctStdAlgs.size(); i < Emulator.algs.size(); i++) {
		ComboBox1->Items->Add(Emulator.algs[i]->GetName().c_str());
	}
	ComboBox1->Text = str;
	OK = 0;

	bChx1 = CheckBox1->Checked;
	bChx2 = CheckBox2->Checked;
	nRtn1 = RadioButton1->Checked;
	nRtn2 = RadioButton2->Checked;
	nRtn3 = RadioButton3->Checked;
	alg	  =	ComboBox1->Text;
	BitBtn2->SetFocus();
}
//---------------------------------------------------------------------------

void __fastcall TfmRunParams::RadioButton1Click(TObject *Sender)
{
	fmRunParams->ComboBox1->Enabled = fmRunParams->RadioButton3->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TfmRunParams::CheckBox2Click(TObject *Sender)
{
	if(CheckBox2->Checked)
		fmMainView->N43->Checked = 1;
	else
		fmMainView->N44->Checked = 1;
}
//---------------------------------------------------------------------------

void __fastcall TfmRunParams::BitBtn2Click(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------

void __fastcall TfmRunParams::FormClose(TObject *Sender, TCloseAction &Action)
{
	if (OK) {
		CheckBox1->Checked = bChx1;
		CheckBox2->Checked = bChx2;
		RadioButton1->Checked = nRtn1;
		RadioButton2->Checked = nRtn2;
		RadioButton3->Checked = nRtn3;
		ComboBox1->Text = alg;
	}
}
//---------------------------------------------------------------------------

void __fastcall TfmRunParams::BitBtn3Click(TObject *Sender)
{
	if (Height == 266) {
		Height = 412;
		Memo1->Visible = 1;
	}
	else {
		Height = 266;
		Memo1->Visible = 0;
	}
}
//---------------------------------------------------------------------------

