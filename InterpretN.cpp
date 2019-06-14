//$$---- EXE CPP ----
//---------------------------------------------------------------------------

#include <vcl.h>
#include "UBanner.h"
#include "UnitMainForm.h"
#pragma hdrstop

//#define DEF_FREE_EXPORT // Заставка при запуске
//---------------------------------------------------------------------------
USEFORM("UnitMainForm.cpp", fmMainView);
USEFORM("UProperties.cpp", fmProperties);
USEFORM("UConsole.cpp", fmConsole);
USEFORM("UBanner.cpp", fmBanner);
USEFORM("UnitAbout.cpp", fmAbout);
USEFORM("UEditorSettings.cpp", fmEditorSettings);
USEFORM("URunParams.cpp", fmRunParams);
USEFORM("UConsoleSettings.cpp", fmConsoleSettings);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	#ifdef DEF_FREE_EXPORT
		Application->ShowMainForm = 0;
	#endif

	try
	{
		if (Screen->DefaultKbLayout == (void*)67699721)
			ActivateKeyboardLayout((void*)HKL_NEXT, KLF_REORDER);

		Application->Initialize();
		Application->Title = "Интерпретатор функционально-алгоритмического языка";
		Application->CreateForm(__classid(TfmMainView), &fmMainView);
		Application->CreateForm(__classid(TfmAbout), &fmAbout);
		Application->CreateForm(__classid(TfmProperties), &fmProperties);
		Application->CreateForm(__classid(TfmConsole), &fmConsole);
		Application->CreateForm(__classid(TfmRunParams), &fmRunParams);
		Application->CreateForm(__classid(TfmBanner), &fmBanner);
		Application->CreateForm(__classid(TfmEditorSettings), &fmEditorSettings);
		Application->CreateForm(__classid(TfmConsoleSettings), &fmConsoleSettings);
		#ifdef DEF_FREE_EXPORT
			fmBanner->Show();
		#endif

		Application->Run();

		ActivateKeyboardLayout(Screen->DefaultKbLayout, KLF_REORDER);
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	catch (...)
	{
		try
		{
			throw Exception("");
		}
		catch (Exception &exception)
		{
			Application->ShowException(&exception);
		}
	}
	return 0;
}
//---------------------------------------------------------------------------
