//$$---- Form HDR ----
//---------------------------------------------------------------------------

#ifndef UBannerH
#define UBannerH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <jpeg.hpp>
//---------------------------------------------------------------------------
class TfmBanner : public TForm
{
__published:	// IDE-managed Components
	TImage *Image1;
	TLabel *Label1;
	TTimer *Timer1;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall Timer1Timer(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfmBanner(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfmBanner *fmBanner;
//---------------------------------------------------------------------------
#endif
