//$$---- Form CPP ----
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UnitAbout.h"
#include "UnitMainForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfmAbout *fmAbout;
const int m_nHighVersion 	= 1;
const int m_nLowVersion 	= 1;
const int m_nBuildVersion	= 57;

//---------------------------------------------------------------------------
__fastcall TfmAbout::TfmAbout(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfmAbout::BitBtn1Click(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------
void __fastcall TfmAbout::FormShow(TObject *Sender)
{
	Label1->Caption = Application->Title;
	Label2->Caption = String() + "Версия " + IntToStr(m_nHighVersion) + "." + IntToStr(m_nLowVersion)+ "." + IntToStr(m_nBuildVersion);
	Label3->Caption = String() + "Дата компиляции: " + __DATE__ + ", время компиляции: " + __TIME__;
	Label4->Caption = String() + "Автор проекта: Лукьянов А.В.";
	Label5->Caption = String() + "Руководитель проекта: Служаева И.В.";
	Label7->Caption = String() + "Документация: Лукьянов А.В.";
}
//---------------------------------------------------------------------------
void __fastcall TfmAbout::BitBtn2Click(TObject *Sender)
{
	fmMainView->Label4Click(this);	
}
//---------------------------------------------------------------------------

