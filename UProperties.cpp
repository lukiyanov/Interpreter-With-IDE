//$$---- Form CPP ----
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UProperties.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfmProperties *fmProperties;
//---------------------------------------------------------------------------
__fastcall TfmProperties::TfmProperties(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfmProperties::FormResize(TObject *Sender)
{
	Label1->Width = Width - 16;	
}
//---------------------------------------------------------------------------
