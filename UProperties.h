//$$---- Form HDR ----
//---------------------------------------------------------------------------

#ifndef UPropertiesH
#define UPropertiesH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfmProperties : public TForm
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TMemo *Memo1;
	TLabel *Label2;
	TLabel *Label1;
	void __fastcall FormResize(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfmProperties(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfmProperties *fmProperties;
//---------------------------------------------------------------------------
#endif
