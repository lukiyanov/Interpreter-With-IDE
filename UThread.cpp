//$$---- Thread CPP ----
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UThread.h"
#include "UnitMainForm.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(&UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall CPaintThread::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------

__fastcall CPaintThread::CPaintThread(bool CreateSuspended)
	: TThread(CreateSuspended)
{
	loop = 1;
}
//---------------------------------------------------------------------------
void __fastcall CPaintThread::Execute()
{
	while (loop) {
		if (fmMainView->bNeedToDraw) {
			fmMainView->bNeedToDraw = 0;
			fmMainView->PaintInColorsExecute();
		}
		Sleep(1000);
	}
}
//---------------------------------------------------------------------------

