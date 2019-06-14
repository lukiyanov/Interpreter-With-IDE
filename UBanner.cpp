//$$---- Form CPP ----
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UBanner.h"
#include "UnitMainForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfmBanner *fmBanner;
//---------------------------------------------------------------------------
__fastcall TfmBanner::TfmBanner(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfmBanner::FormShow(TObject *Sender)
{
	Timer1->Enabled = 1;
	SetWindowPos(Handle, HWND_TOPMOST, Left, Top, Width, Height, SWP_SHOWWINDOW);
}
//---------------------------------------------------------------------------
void __fastcall TfmBanner::Timer1Timer(TObject *Sender)
{
	Timer1->Enabled = 0;
	fmBanner->Close();
	fmMainView->Show();
}
//---------------------------------------------------------------------------
