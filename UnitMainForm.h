//$$---- Form HDR ----
//---------------------------------------------------------------------------

#ifndef UnitMainFormH
#define UnitMainFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include <ToolWin.hpp>
#include <XPMan.hpp>
#include <Graphics.hpp>
#include <ImgList.hpp>
#include <Grids.hpp>
#include <Dialogs.hpp>
#include <Buttons.hpp>
#include "LastProjects.h"
#include <AppEvnts.hpp>
#include <AppEvnts.hpp>
#include "UThread.h"
#include "EditHistory.h"
//---------------------------------------------------------------------------
class TfmMainView : public TForm
{
__published:	// IDE-managed Components
	TMainMenu *MainMenu1;
	TMenuItem *N1;
	TMenuItem *N2;
	TMenuItem *N3;
	TMenuItem *N5;
	TMenuItem *N6;
	TMenuItem *N7;
	TControlBar *ControlBar1;
	TToolBar *ToolBar1;
	TPageControl *PageControl1;
	TTabSheet *TabSheet1;
	TXPManifest *XPManifest1;
	TScrollBox *ScrollBox1;
	TImage *Image1;
	TToolBar *ToolBar2;
	TToolBar *ToolBar3;
	TToolButton *ToolButton1;
	TImageList *ImageList1;
	TToolButton *ToolButton2;
	TToolButton *ToolButton3;
	TToolButton *ToolButton4;
	TToolButton *ToolButton5;
	TToolButton *ToolButton6;
	TToolButton *ToolButton7;
	TToolBar *ToolBar4;
	TToolButton *ToolButton11;
	TToolButton *ToolButton12;
	TToolButton *ToolButton13;
	TToolButton *ToolButton9;
	TToolButton *ToolButton8;
	TToolButton *ToolButton10;
	TPanel *Panel1;
	TSplitter *Splitter3;
	TStatusBar *StatusBar1;
	TToolButton *ToolButton14;
	TMenuItem *N11;
	TMenuItem *N12;
	TMenuItem *N4;
	TMenuItem *N8;
	TMenuItem *N13;
	TMenuItem *N14;
	TMenuItem *N15;
	TMenuItem *N16;
	TMenuItem *N17;
	TMenuItem *N18;
	TMenuItem *N19;
	TMenuItem *N20;
	TMenuItem *N21;
	TMenuItem *N22;
	TMenuItem *N23;
	TMenuItem *NUndo;
	TMenuItem *N26;
	TMenuItem *N28;
	TMenuItem *N29;
	TMenuItem *N30;
	TMenuItem *N31;
	TMenuItem *N32;
	TMenuItem *N33;
	TMenuItem *N34;
	TMenuItem *N35;
	TMenuItem *N36;
	TMenuItem *N37;
	TMenuItem *N38;
	TMenuItem *N39;
	TMenuItem *Pascal1;
	TMenuItem *Basic1;
	TMenuItem *N40;
	TToolButton *ToolButton15;
	TToolButton *ToolButton16;
	TToolButton *ToolButton17;
	TOpenDialog *OpenDialog1;
	TSaveDialog *SaveDialog1;
	TTabSheet *TabSheet2;
	TTabSheet *TabSheet3;
	TMemo *Memo3;
	TTabSheet *TabSheet5;
	TTabSheet *TabSheet6;
	TMemo *Memo4;
	TStringGrid *StringGrid2;
	TMenuItem *N41;
	TMenuItem *N42;
	TImageList *ImageList2;
	TLabel *Label12;
	TImage *Image3;
	TListBox *ListBox1;
	TLabel *Label5;
	TLabel *Label1;
	TLabel *Label13;
	TImage *Image4;
	TLabel *Label2;
	TLabel *Label3;
	TLabel *Label6;
	TLabel *Label9;
	TLabel *Label10;
	TLabel *Label11;
	TLabel *Label7;
	TLabel *Label8;
	TLabel *Label4;
	TLabel *Label14;
	TMenuItem *N25;
	TMenuItem *N43;
	TMenuItem *N44;
	TLabel *Label15;
	TLabel *Label16;
	TPopupMenu *PopupMenu2;
	TMenuItem *N45;
	TMenuItem *N46;
	TMenuItem *N47;
	TPanel *Panel2;
	TSplitter *Splitter1;
	TStringGrid *StringGrid3;
	TMenuItem *N48;
	TMenuItem *N49;
	TMenuItem *N9;
	TMenuItem *N10;
	TMenuItem *N50;
	TMenuItem *N51;
	TPanel *Panel3;
	TRichEdit *Memo2;
	TPanel *Panel4;
	TBevel *Bevel1;
	TPanel *Panel5;
	TMenuItem *NHistory;
	TImageList *ImageList3;
	TApplicationEvents *ApplicationEvents1;
	TToolBar *ToolBar6;
	TToolButton *ToolButton23;
	TToolButton *ToolButton22;
	TStringGrid *StringGrid1;
	TMenuItem *NRedo;
	TFindDialog *FindDialog1;
	void __fastcall N11Click(TObject *Sender);
	void __fastcall ToolButton14Click(TObject *Sender);
	void __fastcall N12Click(TObject *Sender);
	void __fastcall Label1Click(TObject *Sender);
	void __fastcall ToolButton1Click(TObject *Sender);
	void __fastcall Label2Click(TObject *Sender);
	void __fastcall ToolButton2Click(TObject *Sender);
	void __fastcall ToolButton3Click(TObject *Sender);
	void __fastcall ToolButton11Click(TObject *Sender);
	void __fastcall ToolButton12Click(TObject *Sender);
	void __fastcall ToolButton13Click(TObject *Sender);
	void __fastcall N29Click(TObject *Sender);
	void __fastcall N30Click(TObject *Sender);
	void __fastcall Label3Click(TObject *Sender);
	void __fastcall ToolButton4Click(TObject *Sender);
	void __fastcall ToolButton15Click(TObject *Sender);
	void __fastcall ToolButton17Click(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall N1Click(TObject *Sender);
	void __fastcall N42Click(TObject *Sender);
	void __fastcall N38Click(TObject *Sender);
	void __fastcall Memo2Change(TObject *Sender);
	void __fastcall TabSheetShow(TObject *Sender);
	void __fastcall TabSheet1Show(TObject *Sender);
	void __fastcall N4Click(TObject *Sender);
	void __fastcall N8Click(TObject *Sender);
	void __fastcall N13Click(TObject *Sender);
	void __fastcall N14Click(TObject *Sender);
	void __fastcall N15Click(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall TabSheet1Enter(TObject *Sender);
	void __fastcall N2Click(TObject *Sender);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall NUndoClick(TObject *Sender);
	void __fastcall N18Click(TObject *Sender);
	void __fastcall N19Click(TObject *Sender);
	void __fastcall N20Click(TObject *Sender);
	void __fastcall N21Click(TObject *Sender);
	void __fastcall N22Click(TObject *Sender);
	void __fastcall N32Click(TObject *Sender);
	void __fastcall Label5Click(TObject *Sender);
	void __fastcall ListBox1DblClick(TObject *Sender);
	void __fastcall StringGrid1DblClick(TObject *Sender);
	void __fastcall TabSheet1Hide(TObject *Sender);
	void __fastcall N31Click(TObject *Sender);
	void __fastcall N47Click(TObject *Sender);
	void __fastcall N45Click(TObject *Sender);
	void __fastcall N46Click(TObject *Sender);
	void __fastcall N26Click(TObject *Sender);
	void __fastcall StringGrid2DblClick(TObject *Sender);
	void __fastcall N39Click(TObject *Sender);
	void __fastcall ToolButton5Click(TObject *Sender);
	void __fastcall N33Click(TObject *Sender);
	void __fastcall N34Click(TObject *Sender);
	void __fastcall N35Click(TObject *Sender);
	void __fastcall ToolButton7Click(TObject *Sender);
	void __fastcall ToolButton6Click(TObject *Sender);
	void __fastcall N27Click(TObject *Sender);
	void __fastcall TabSheet5Show(TObject *Sender);
	void __fastcall StringGrid3Click(TObject *Sender);
	void __fastcall N44Click(TObject *Sender);
	void __fastcall N43Click(TObject *Sender);
	void __fastcall ListBox1KeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall N48Click(TObject *Sender);
	void __fastcall N49Click(TObject *Sender);
	void __fastcall Label6Click(TObject *Sender);
	void __fastcall Label9Click(TObject *Sender);
	void __fastcall Label10Click(TObject *Sender);
	void __fastcall Label11Click(TObject *Sender);
	void __fastcall Label7Click(TObject *Sender);
	void __fastcall Label8Click(TObject *Sender);
	void __fastcall Label4Click(TObject *Sender);
	void __fastcall Memo2KeyPress(TObject *Sender, char &Key);
	void __fastcall N10Click(TObject *Sender);
	void __fastcall N50Click(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall NHistoryClick(TObject *Sender);
	void __fastcall ToolButton22Click(TObject *Sender);
	void __fastcall ToolButton23Click(TObject *Sender);
	void __fastcall ApplicationEvents1Message(tagMSG &Msg, bool &Handled);
	void __fastcall NRedoClick(TObject *Sender);
	void __fastcall FindDialog1Find(TObject *Sender);

private:	// User declarations
	TLastProjects prjManager;
	TFont* fontCode;
	TFont* fontKeyword;
	TFont* fontNumber;
	TFont* fontComment;
	TFont* fontString;
	TFont* fontChar;
	bool bNeedToRedraw;

public:		// User declarations
	CPaintThread* thrd;
	volatile bool bNeedToDraw;

public:
	__fastcall TfmMainView(TComponent* Owner);
	void __fastcall PaintInColors();
	void __fastcall PaintInColorsExecute();
	void __fastcall InitByFont(TTextAttributes *t, TFont *f);
	TFont* GetCodeFont(){return fontCode;}

	// Document functions
	void __fastcall DocumentNew();
	void __fastcall DocumentOpen();
	void __fastcall DocumentOpenID(String str);
	bool __fastcall DocumentSave();
	bool __fastcall DocumentSaveAs();
	bool __fastcall DocumentClose();

	void __fastcall EnableTBTNS();
	void __fastcall EnableMenuItems();
	void __fastcall SaveBackup();
};
//---------------------------------------------------------------------------
void ShowTab(String strCaption, bool bVisible);
bool GetTabState(String strCaption);
void RefreshVarWnd();
string GetPath();
//---------------------------------------------------------------------------
extern PACKAGE TfmMainView *fmMainView;
//---------------------------------------------------------------------------
#endif
