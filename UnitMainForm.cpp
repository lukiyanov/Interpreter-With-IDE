//$$---- Form CPP ----
//---------------------------------------------------------------------------

#include <vcl.h>
#include <string>
#include <Registry.hpp>
using namespace std;
#pragma hdrstop

#include "UnitMainForm.h"
#include "UnitAbout.h"
#include "Emulator.h"
#include "Trace.h"
#include "Errors.h"
#include "UProperties.h"
#include "UConsole.h"
#include "URunParams.h"
#include "StdAlgorithms.h"
#include "UEditorSettings.h"
#include "Variables.h"
#include "UConsoleSettings.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//---------------------------------------------------------------------------

TfmMainView *fmMainView;
vector<string> vstrHints;
int nSelHint;

bool bDocChanged = 0;
string strWay, strName;

String FilterToName(String);
void ErrorAutoReport();

//---------------------------------------------------------------------------
__fastcall TfmMainView::TfmMainView(TComponent* Owner)
	: TForm(Owner), bNeedToRedraw(0)
{
	fontCode 	= new TFont;
	fontKeyword = new TFont;
	fontNumber 	= new TFont;
	fontComment = new TFont;
	fontString 	= new TFont;
	fontChar 	= new TFont;

	fontCode	->Name = "Courier New";
	fontKeyword	->Name = "Courier New";
	fontNumber	->Name = "Courier New";
	fontComment	->Name = "Courier New";
	fontString	->Name = "Courier New";
	fontChar	->Name = "Courier New";

	fontCode	->Size = 10;
	fontKeyword	->Size = 10;
	fontNumber	->Size = 10;
	fontComment	->Size = 10;
	fontString	->Size = 10;
	fontChar	->Size = 10;

	fontKeyword	->Color = clNavy;
	fontKeyword	->Style = fontKeyword->Style << fsBold;
	fontNumber	->Color = clBlue;
	fontComment	->Color = clGreen;
	fontString	->Color = clBlue;
	fontChar	->Color = clPurple;

	thrd = new CPaintThread(1);
}

//---------------------------------------------------------------------------
void ErrorAutoReport() {
	fmMainView->StringGrid1DblClick(0);
}
//---------------------------------------------------------------------------
void DrawHint() {
	nSelHint = random(vstrHints.size());
	fmMainView->Label16->Caption = (string("Совет:\n")+vstrHints[nSelHint]).c_str();
}
//---------------------------------------------------------------------------
void ShowTab(String strCaption, bool bVisible){
	for (int i = 0; i < fmMainView->PageControl1->PageCount; i++)
		if (fmMainView->PageControl1->Pages[i]->Caption == strCaption){
			fmMainView->PageControl1->Pages[i]->TabVisible = bVisible;
			if (bVisible)
				fmMainView->PageControl1->ActivePageIndex = i;
		}
}

//---------------------------------------------------------------------------
bool GetTabState(String strCaption){
	for (int i = 0; i < fmMainView->PageControl1->PageCount; i++)
		if (fmMainView->PageControl1->Pages[i]->Caption == strCaption)
			return fmMainView->PageControl1->Pages[i]->TabVisible;
	return 0;
}

//---------------------------------------------------------------------------

string GetPath(){
	string str,s = Application->ExeName.c_str();

	int n = 0;

	for (int i = 0; i <= s.size(); i++)
		if (s[i]=='\\') n = i;

	for (int i = 0; i < n; i++)
		str+=s[i];
	return str;
}
//---------------------------------------------------------------------------


void __fastcall TfmMainView::N11Click(TObject *Sender)
{
	fmAbout->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::ToolButton14Click(TObject *Sender)
{
	N11->Click();
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::N12Click(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::Label1Click(TObject *Sender)
{
	N4->Click();
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::ToolButton1Click(TObject *Sender)
{
	N4->Click();
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::Label2Click(TObject *Sender)
{
	N29->Click();
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::ToolButton2Click(TObject *Sender)
{
	N8->Click();
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::ToolButton3Click(TObject *Sender)
{
	N13->Click();
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::ToolButton11Click(TObject *Sender)
{
	N29->Click();
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::ToolButton12Click(TObject *Sender)
{
	N30->Click();
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::ToolButton13Click(TObject *Sender)
{
	N31->Click();
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::N29Click(TObject *Sender)
{
	string str = GetPath();
	str+="\\Help\\Description\\Description.htm";
	ShellExecute(Handle,"open",str.c_str(), 0 , 0, SW_SHOWMAXIMIZED );
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::N30Click(TObject *Sender)
{
	string str = GetPath();
	str+="\\Help\\Syntax\\Syntax.htm";
	ShellExecute(Handle,"open",str.c_str(), 0 , 0, SW_SHOWMAXIMIZED );
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::Label3Click(TObject *Sender)
{
	N30->Click();
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::ToolButton4Click(TObject *Sender)
{
	N32->Click();
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::ToolButton15Click(TObject *Sender)
{
	N38->Click();
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::ToolButton17Click(TObject *Sender)
{
	N39->Click();
}
//---------------------------------------------------------------------------

String GetPath(String strExeWay){
	string way = strExeWay.c_str();
	string str;
	int last = 0;

	for (int i = 0; i < way.size(); i++) {
		if (way[i]=='\\') {
			last = i;
		}
	}

	for (int i = 0; i <= last; i++) {
		str+=way[i];
	}

	return str.c_str();
}

//---------------------------------------------------------------------------

void __fastcall TfmMainView::FormCreate(TObject *Sender)
{
	StringGrid1->DefaultColWidth = 10000;
	StringGrid3->DefaultColWidth = 10000;
	StringGrid1->Cells[0][0] = "Сообщения отладчика:";

	TraceInit(this);
	STDAlgInit();

Label15->Caption =
"  Программа \"Интерпретатор функционально-алгоритмического языка\" - это система обучения \
студентов младших курсов основам программирования.\n\n\
  Слева от этого текста, под заголовком \"Последние алгоритмы\", находятся названия \
алгоритмов, с которыми вы работали ранее. Для возобновления работы с алгоритмом выберите \
нужный из списка и кликните по надписи Открыть выбранный алгоритм, либо дважды щелкните на \
нем. Если нужного алгоритма нет в списке, вы можете открыть его через меню Файл, либо нажав \
на соответствующую кнопку панели инструментов и найти его вручную.\n\
  Если вы ранее не работали с этой программой, обязательно прочитайте документы из раздела \
\"Справочные материалы\". Там же вы найдете не только полное описание использованного в \
системе языка программирования, но и учебник по программированию с примерами и задачами.\n\
  Ниже отображаются советы, которые могут пригодиться в работе. Если подвести туда курсор \
и нажать правую кнопку мыши, можно прокрутить их в любом направлении.";

	Memo2->Font->Name = "Courier New";
	Memo2->Font->Size = 10;

	PageControl1->ActivePageIndex = 0;
	for (int i = 1; i < PageControl1->PageCount; i++)
		PageControl1->Pages[i]->TabVisible = 0;

	N13->Enabled = 0;
	N14->Enabled = 0;
	ToolButton3->Enabled = 0;

	// Растянем форму!
	Height = Screen->WorkAreaHeight;
	Width  = Screen->WorkAreaWidth;

	TRegistry* reg = new TRegistry;
	reg->RootKey = HKEY_CLASSES_ROOT;
	if (reg->OpenKey(".alg",1)) {
		reg->WriteString("",Application->Title+".DOC");
		reg->CloseKey();
	}

	if (reg->OpenKey(Application->Title+".DOC",1)) {
		reg->WriteString("",String("Документ ")+Application->Title);
		reg->CloseKey();
	}

	if (reg->OpenKey(Application->Title+".DOC"+"\\DefaultIcon",1)) {
		reg->WriteString("",GetPath(Application->ExeName)+"DocSupport.dll" + ", 8");
		reg->CloseKey();
	}

	if (reg->OpenKey(Application->Title+".DOC"+"\\Shell\\Open\\Command",1)) {
		reg->WriteString("",Application->ExeName + " %1");
		reg->CloseKey();
	}

	reg->RootKey = HKEY_CURRENT_USER;
	reg->OpenKey("InterpretN",1);
	reg->WriteString("",Application->ExeName);
	delete reg;

//
	EnableTBTNS();
	N13->Enabled = 0;
	ToolButton3->Enabled = 0;

	// LOAD HINTS:
	randomize();
	TStringList* list = new TStringList;
	for (int i = 1; FileExists(GetPath(Application->ExeName)+"Hints\\"+String("H")+IntToStr(i)+".txt"); i++) {
		list->LoadFromFile(GetPath(Application->ExeName)+"Hints\\"+String("H")+IntToStr(i)+".txt");
		vstrHints.push_back(list->Text.c_str());
	}

	DrawHint();
	Resize();
//

	if (!(ParamCount() > 0)) return;
	// Load file
	const int bufSize = 1024;
	char buf[bufSize];
	GetLongPathName(ParamStr(1).c_str(), buf, bufSize);
	strWay = buf;
	strName = FilterToName(strWay.c_str()).c_str();
	bDocChanged = 0;
	TNotifyEvent p = Memo2->OnChange;

	try {
		Memo2->OnChange = 0;
		Memo2->Lines->LoadFromFile(strWay.c_str());
	} __finally {
		Memo2->OnChange = p;
	}

	ShowTab("Алгоритм",1);

	Caption = (strName + " - Интерпретатор функционально-алгоритмического языка").c_str();

	EnableTBTNS();
	N13->Enabled = 0;
	ToolButton3->Enabled = 0;

	prjManager.OpenProject(strWay);
	ListBox1->Items->Clear();
	for (int i = 0; i < prjManager.GetShortNames().size(); i++) {
		ListBox1->Items->Add(prjManager.GetShortNames()[i].c_str());
	}
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::N1Click(TObject *Sender)
{
	N14->Enabled = GetTabState("Алгоритм");

	if (PageControl1->ActivePage->Caption == "Стартовая страница")
		N42->Enabled = 0;
	else
		N42->Enabled = 1;
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::N42Click(TObject *Sender)
{
	if (PageControl1->ActivePage->Caption == "Алгоритм") {
		N15->Click();
		return;
	}

	ShowTab( PageControl1->ActivePage->Caption, 0);
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::N38Click(TObject *Sender)
{
	ShowTab( "Переменные", 1);
}

//---------------------------------------------------------------------------
void __fastcall TfmMainView::InitByFont(TTextAttributes *t, TFont *f) {
	t->Name  = f->Name;
	t->Size  = f->Size;
	t->Style = f->Style;
	t->Color = f->Color;
}

//---------------------------------------------------------------------------
void __fastcall TfmMainView::Memo2Change(TObject *Sender)
{
	bDocChanged = 1;
	N13->Enabled = 1;
	ToolButton3->Enabled = 1;
	PaintInColors();
	ehHistoryManager.AddState(Memo2);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// Document functions BEGIN
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------

String FilterToName(String strDocWay) {
	// Иначе вместо программы - один большой глюк!
	string str = strDocWay.c_str();
	string strExt;

	for (int i = str.size()-1; i >= 0; i--) {
		if (str[i] == '\\') break;
		strExt = str[i] + strExt;
	}

	str = strExt;
	strExt.clear();

	for (int i = 0; i < str.size(); i++) {
		if (str[i] == '.') break;
		strExt += str[i];
	}

	return String()+strExt.c_str();
}

//---------------------------------------------------------------------------

void __fastcall TfmMainView::DocumentNew(){

	if(!DocumentClose())
		return;

	TNotifyEvent p = Memo2->OnChange;
	Memo2->OnChange = 0;
	Memo2->Clear();
	Memo2->Lines->Add("АЛГ Алгоритм1 ()");
	Memo2->Lines->Add("\tАРГ НЕТ");
	Memo2->Lines->Add("\tРЕЗ НЕТ");
	Memo2->Lines->Add("НАЧ ( ЛИТ Искл )");
	Memo2->Lines->Add("\t| Вставьте сюда код алгоритма");
	Memo2->Lines->Add("КОН");
	ShowTab("Алгоритм",1);
	Memo2->OnChange = p;

	strWay = "";
	strName = "Безымянный";
	bDocChanged = 0;

	Caption = (strName + " - Интерпретатор функционально-алгоритмического языка").c_str();

	EnableTBTNS();
	N13->Enabled = 0;
	ToolButton3->Enabled = 0;

	prjManager.OpenProject(strWay);
	fmMainView->PaintInColors();
	ListBox1->Items->Clear();
	for (int i = 0; i < prjManager.GetShortNames().size(); i++) {
		ListBox1->Items->Add(prjManager.GetShortNames()[i].c_str());
	}
	ehHistoryManager.Clear();
}

//---------------------------------------------------------------------------

void __fastcall TfmMainView::DocumentOpen(){
	if(!DocumentClose())
		return;

	if (!OpenDialog1->Execute()) return;

	strWay = OpenDialog1->FileName.c_str();
	strName = FilterToName(OpenDialog1->FileName).c_str();
	bDocChanged = 0;
	TNotifyEvent p = Memo2->OnChange;

	try {
		Memo2->OnChange = 0;
		Memo2->Lines->LoadFromFile(strWay.c_str());
	} __finally {
		Memo2->OnChange = p;
		fmMainView->PaintInColors();
	}

	ShowTab("Алгоритм",1);

	Caption = (strName + " - Интерпретатор функционально-алгоритмического языка").c_str();

	EnableTBTNS();
	N13->Enabled = 0;
	ToolButton3->Enabled = 0;

	prjManager.OpenProject(strWay);
	ListBox1->Items->Clear();
	for (int i = 0; i < prjManager.GetShortNames().size(); i++) {
		ListBox1->Items->Add(prjManager.GetShortNames()[i].c_str());
	}
	ehHistoryManager.Clear();
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::DocumentOpenID(String str){

	if(!DocumentClose())
		return;

	strWay = str.c_str();
	strName = FilterToName(str).c_str();
	bDocChanged = 0;
	TNotifyEvent p = Memo2->OnChange;

	try {
		Memo2->OnChange = 0;
		Memo2->Lines->LoadFromFile(strWay.c_str());
	} __finally {
		Memo2->OnChange = p;
		fmMainView->PaintInColors();
	}

	ShowTab("Алгоритм",1);

	Caption = (strName + " - Интерпретатор функционально-алгоритмического языка").c_str();

	EnableTBTNS();
	N13->Enabled = 0;
	ToolButton3->Enabled = 0;

	prjManager.OpenProject(strWay);
	ListBox1->Items->Clear();
	for (int i = 0; i < prjManager.GetShortNames().size(); i++) {
		ListBox1->Items->Add(prjManager.GetShortNames()[i].c_str());
	}
	ehHistoryManager.Clear();
}

//---------------------------------------------------------------------------

bool __fastcall TfmMainView::DocumentSave(){
	if (strWay == "")
		return DocumentSaveAs();

	Memo2->Lines->SaveToFile(strWay.c_str());
	bDocChanged = 0;

	EnableTBTNS();
	N13->Enabled = 0;
	ToolButton3->Enabled = 0;

	prjManager.OpenProject(strWay);
	ListBox1->Items->Clear();
	for (int i = 0; i < prjManager.GetShortNames().size(); i++) {
		ListBox1->Items->Add(prjManager.GetShortNames()[i].c_str());
	}

	return 1;
}

//---------------------------------------------------------------------------

bool __fastcall TfmMainView::DocumentSaveAs(){
	if (!SaveDialog1->Execute()) return 0;

	strWay = SaveDialog1->FileName.c_str();

	if (
		 strWay.size()<=4
	  || strWay[strWay.size()-1]!= 'g'
	  || strWay[strWay.size()-2]!= 'l'
	  || strWay[strWay.size()-3]!= 'a'
	  || strWay[strWay.size()-4]!= '.'
	) strWay += ".alg";

	strName = FilterToName(strWay.c_str()).c_str();

	Memo2->Lines->SaveToFile(strWay.c_str());
	bDocChanged = 0;

	Caption = (strName + " - Интерпретатор функционально-алгоритмического языка").c_str();

	EnableTBTNS();
	N13->Enabled = 0;
	ToolButton3->Enabled = 0;


	prjManager.OpenProject(strWay);
	ListBox1->Items->Clear();
	for (int i = 0; i < prjManager.GetShortNames().size(); i++) {
		ListBox1->Items->Add(prjManager.GetShortNames()[i].c_str());
	}

	return 1;
}

//---------------------------------------------------------------------------

bool __fastcall TfmMainView::DocumentClose(){
	if (Emulator.esState != esStoped) {
		if (ID_OK != MessageBox(Handle,
		"Идет выполнение алгоритма.\nПрервать?",
		Application->Title.c_str(),
		MB_ICONQUESTION | MB_OKCANCEL | MB_DEFBUTTON2)) {
			return 0;
		}
	}

	if (Emulator.esState != esStoped)
		Emulator.Stop();

	bool b = 0;

	if (bDocChanged) {
		switch (MessageBox(Handle,"Алгоритм был изменен. Сохранить изменения?",Application->Title.c_str(), MB_ICONEXCLAMATION | MB_YESNOCANCEL)) {
		case ID_YES:
			if(!DocumentSave())return 0; // 1-OK 0-FAILED
			break;
		case ID_NO:
			break;
		default:
			return 0;
		}
	}

	bDocChanged = 0;
	Caption = "Интерпретатор функционально-алгоритмического языка";
	EnableTBTNS();
	N13->Enabled = 0;
	ToolButton3->Enabled = 0;
	return 1;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Document functions END
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------

void __fastcall TfmMainView::EnableTBTNS(){
	bool bEnable = GetTabState("Алгоритм");
	ToolButton4 ->Enabled = bEnable;
	ToolButton5 ->Enabled = bEnable;
	ToolButton6 ->Enabled = bEnable;
	ToolButton7 ->Enabled = bEnable;
//	ToolButton8 ->Enabled = bEnable;
	ToolButton9 ->Enabled = bEnable;
//	ToolButton10->Enabled = bEnable;

	N15->Enabled = bEnable;
	N32->Enabled = bEnable;
	N33->Enabled = bEnable;
	N34->Enabled = bEnable;
	N35->Enabled = bEnable;
//	Basic1->Enabled = bEnable;
	Pascal1->Enabled = bEnable;
//	N40->Enabled = bEnable;
}

//---------------------------------------------------------------------------

void __fastcall TfmMainView::EnableMenuItems(){

	bool b = 0;

	if 		(PageControl1->ActivePage->Caption == "Алгоритм")
		b = 1;

	N18->Enabled = b;
	N19->Enabled = b;
	N20->Enabled = b;
	N21->Enabled = b;
	N22->Enabled = b;
	N26->Enabled = b;
	NUndo->Enabled = b && ehHistoryManager.CanUndo();
	NRedo->Enabled = b && ehHistoryManager.CanRedo();
}

//---------------------------------------------------------------------------

void __fastcall TfmMainView::TabSheet1Show(TObject *Sender)
{
	DrawHint();
	EnableTBTNS();
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::TabSheetShow(TObject *Sender)
{
	EnableTBTNS();
	if (GetTabState("Алгоритм"))
		PaintInColors();
	RefreshVarWnd();
}
//---------------------------------------------------------------------------


void __fastcall TfmMainView::N4Click(TObject *Sender)
{
	DocumentNew();
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::N8Click(TObject *Sender)
{
	DocumentOpen();
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::N13Click(TObject *Sender)
{
	DocumentSave();	
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::N14Click(TObject *Sender)
{
	DocumentSaveAs();	
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::N15Click(TObject *Sender)
{
	if (DocumentClose())
		ShowTab("Алгоритм",0);
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::FormShow(TObject *Sender)
{
	EnableTBTNS();

	ListBox1->Items->Clear();
	for (int i = 0; i < prjManager.GetShortNames().size(); i++) {
		ListBox1->Items->Add(prjManager.GetShortNames()[i].c_str());
	}
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::TabSheet1Enter(TObject *Sender)
{
	EnableTBTNS();
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::N2Click(TObject *Sender)
{
	EnableMenuItems();
	N18->Enabled = Memo2->SelLength;
	N19->Enabled = Memo2->SelLength;
	N20->Enabled = GetTabState("Алгоритм");
	N21->Enabled = Memo2->SelLength;
}
//---------------------------------------------------------------------------



void __fastcall TfmMainView::FormCloseQuery(TObject *Sender, bool &CanClose)
{
	if (GetTabState("Алгоритм"))
		CanClose = DocumentClose();
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::NUndoClick(TObject *Sender)
{
	ehHistoryManager.Undo(Memo2);
	PaintInColors();
}
//---------------------------------------------------------------------------


void __fastcall TfmMainView::N18Click(TObject *Sender)
{
	Memo2->CutToClipboard();
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::N19Click(TObject *Sender)
{
	Memo2->CopyToClipboard();	
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::N20Click(TObject *Sender)
{
	Memo2->PasteFromClipboard();	
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::N21Click(TObject *Sender)
{
	Memo2->ClearSelection();	
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::N22Click(TObject *Sender)
{
	Memo2->SelectAll();
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::N32Click(TObject *Sender)
{
	try {
// RUN
		SaveBackup();
		if (Emulator.esState == esRunning) {
			MessageBox(Handle, "алгоритм уже запущен", "Ошибка запуска эмулятора", MB_ICONINFORMATION);
			return;
		}
		if (Emulator.esState == esTracing) {
			Emulator.Continue();
			return;
		}

		vector<string> vct;
		for (int i = 0; i < Memo2->Lines->Count; i++) {
			vct.push_back(Memo2->Lines->Strings[i].c_str());
		}

		TraceInit(this);
		Emulator.FillSession(vct);
		Emulator.Run(0);
	}

	catch (SyntaxError err) {
		DebugTrace(
		string("Синтаксическая ошибка на строке [")
		+IntToStr(err.line+1).c_str()+"]: "
		+err.message
		, error
		);

		MessageBox(Handle,
		"В исходном коде алгоритма найдена синтаксическая ошибка.\nПодробности смотрите в окне сообщений отладчика.",
		"Синтаксическая ошибка",
		MB_ICONWARNING);
		ErrorAutoReport();
		Emulator.bNeedToStop = 1;
		Emulator.esState = esStoped;
	}

	catch (LogicalError err) {
		DebugTrace(
		string("Логическая ошибка на строке [")
		+IntToStr(err.line+1).c_str()+"]: "
		+err.message
		, error
		);

		MessageBox(Handle,
		"В исходном коде алгоритма найдена логическая ошибка.\nПодробности смотрите в окне сообщений отладчика.",
		"Логическая ошибка",
		MB_ICONWARNING);
		ErrorAutoReport();
		Emulator.bNeedToStop = 1;
		Emulator.esState = esStoped;
	}

	catch (RuntimeError err) {
		DebugTrace(
		string("В процессе выполнения алгоритма возникла ошибка: ")
		+err.message, error);

		MessageBox(Handle,
		"Во время выполнения алгоритма произошла ошибка.\nПодробности смотрите в окне сообщений отладчика.",
		"Ошибка времени выполнения",
		MB_ICONWARNING);
		Emulator.bNeedToStop = 1;
		Emulator.esState = esStoped;
	}
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::Label5Click(TObject *Sender)
{
	int c = ListBox1->ItemIndex;
	if (c == -1) return;
	DocumentOpenID(prjManager.GetFullNames()[c].c_str());
}
//---------------------------------------------------------------------------


void __fastcall TfmMainView::ListBox1DblClick(TObject *Sender)
{
	Label5Click(this);
}
//---------------------------------------------------------------------------


// basepch0 HAS CHANGED!!!
void __fastcall TfmMainView::StringGrid1DblClick(TObject *Sender)
{ // КЛАСС String нестабилен!!!!!!!
	const string strLogicalError("!!!>>> ОШИБКА! Логическая ошибка на строке [");
	const string strSyntaxError ("!!!>>> ОШИБКА! Синтаксическая ошибка на строке [");
	const string strTracePoint("-> Позиция трассировки: [");

	string strID;

	if ( StringGrid1->Cells[0][StringGrid1->Row].SubString(0,strSyntaxError.size()) == strSyntaxError.c_str() )
		strID = strSyntaxError;
	if ( StringGrid1->Cells[0][StringGrid1->Row].SubString(0,strLogicalError.size()) == strLogicalError.c_str() )
		strID = strLogicalError;
	if ( StringGrid1->Cells[0][StringGrid1->Row].SubString(0,strTracePoint.size()) == strTracePoint.c_str() )
		strID = strTracePoint;

	if (strID.empty()) return;

	string pos;
	string strX = StringGrid1->Cells[0][StringGrid1->Row].c_str();
	for (int i = strID.size(); strX[i] != ']'; i++)
		pos += strX[i];
	PageControl1->ActivePage = TabSheet2;
	Memo2->SetFocus();
	TPoint p;
	p.x = 0;
	p.y = StrToInt(pos.c_str())-1;
	Memo2->CaretPos = p;

	// Подсветка
	TNotifyEvent px = Memo2->OnChange;
	Memo2->OnChange = 0;
	int selStart = Memo2->SelStart;
	int selLength = Memo2->SelLength;

	int nCharsPass = 0;
	for (int i = 0; i < p.y; i++)
		nCharsPass+=(Memo2->Lines->Strings[i].Length()+2);
	Memo2->SelStart = nCharsPass;
	Memo2->SelLength = Memo2->Lines->Strings[p.y].Length();
	Memo2->SelAttributes->Color = clRed;
	bNeedToRedraw = 1; // Для нормальной работы при отключенной подсветке

	Memo2->SelStart = selStart;
	Memo2->SelLength = selLength;
	Memo2->OnChange = px;
}
//---------------------------------------------------------------------------
void __fastcall TfmMainView::TabSheet1Hide(TObject *Sender)
{
   	EnableTBTNS();	
}

//---------------------------------------------------------------------------
void __fastcall TfmMainView::N31Click(TObject *Sender)
{
	string str = GetPath();
	str+="\\Help\\Manual\\Index.htm";
	ShellExecute(Handle,"open",str.c_str(), 0 , 0, SW_SHOWMAXIMIZED );
}

//---------------------------------------------------------------------------
void RefreshVarWnd() {
	DebugTrace("Отображение переменных...");

	fmMainView->StringGrid2->RowCount = 1;
	fmMainView->StringGrid2->Cells[0][0] = "Название алгоритма";
	fmMainView->StringGrid2->Cells[1][0] = "Название переменной";
	fmMainView->StringGrid2->Cells[2][0] = "Тип переменной";
	fmMainView->StringGrid2->Cells[3][0] = "Инициализация";
	fmMainView->StringGrid2->Cells[4][0] = "Значение переменной";

	int n = 1;
	for (int i = 0; i < Emulator.algs.size(); i++) {
		if (Emulator.algs[i]->bStdAlg) continue;
		for (int j = 1; j < Emulator.algs[i]->Header.vctVariables.size(); j++) {
			// j = 1 <= vct[0].varState == vsAlgName
			DebugTrace(string("Адрес переменной: ")+IntToStr((int)&(Emulator.algs[i]->Header.vctVariables[j])).c_str());
			fmMainView->StringGrid2->RowCount = fmMainView->StringGrid2->RowCount + 1;
			fmMainView->StringGrid2->Cells[0][n] = Emulator.algs[i]->GetName().c_str();
			fmMainView->StringGrid2->Cells[1][n] = Emulator.algs[i]->Header.vctVariables[j].strName.c_str();
			fmMainView->StringGrid2->Cells[2][n] = Emulator.algs[i]->Header.vctVariables[j].GetStrType().c_str();
			if (Emulator.algs[i]->Header.vctVariables[j].bDynamicInit)
				fmMainView->StringGrid2->Cells[3][n] = "ДА";
			else
				fmMainView->StringGrid2->Cells[3][n] = "НЕТ";
			fmMainView->StringGrid2->Cells[4][n] = Emulator.algs[i]->Header.vctVariables[j].GetStrValue().c_str();
			n++;
		}
	}

	if (fmMainView->StringGrid2->RowCount > 1)
		fmMainView->StringGrid2->FixedRows = 1;

	DebugTrace("Отображение переменных завершено");
}

//---------------------------------------------------------------------------
void __fastcall TfmMainView::N47Click(TObject *Sender)
{
	DrawHint();
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::N45Click(TObject *Sender)
{
	if (nSelHint < vstrHints.size()-1) {
		Label16->Caption = (string("Совет:\n")+vstrHints[++nSelHint]).c_str();
	}
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::N46Click(TObject *Sender)
{
	if (nSelHint > 0) { // На случай, если подсказки удалят
		Label16->Caption = (string("Совет:\n")+vstrHints[--nSelHint]).c_str();
	}
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::N26Click(TObject *Sender)
{
	// Выделено там уже чего-нибудь
	if (Memo2->SelLength)
		FindDialog1->FindTextA = fmMainView->Memo2->SelText;

	FindDialog1->Execute(this);
}
//---------------------------------------------------------------------------


void __fastcall TfmMainView::StringGrid2DblClick(TObject *Sender)
{
	if (!StringGrid2->Row) return;
	String str =
	"Имя алгоритма: "  +StringGrid2->Cells[0][StringGrid2->Row]+'\n'+
	"Имя переменной: "	+StringGrid2->Cells[1][StringGrid2->Row]+'\n'+
	"Тип переменной: "  +StringGrid2->Cells[2][StringGrid2->Row]+'\n'+
	"Признак инициализации: "  +StringGrid2->Cells[3][StringGrid2->Row]+'\n';

	fmProperties->Label1->Caption = str;

	fmProperties->Memo1->Clear();


	string sx = StringGrid2->Cells[4][StringGrid2->Row].c_str();
	int nBaseCnt=0;
	String sss;
	for (int i = 0; i < sx.size() ; i++) {
		if (nBaseCnt > 150) {
			if (sx[i] == ' ') {
				fmProperties->Memo1->Lines->Add(sss);
				sss = "";
				nBaseCnt = 0;
				Application->ProcessMessages();
				continue;
			}
			if (nBaseCnt > 180) {
				sss += sx[i];
				fmProperties->Memo1->Lines->Add(sss);
				sss = "";
				nBaseCnt = 0;
				Application->ProcessMessages();
				continue;
			}
		}
		sss += sx[i];
		nBaseCnt++;
	}
	fmProperties->Memo1->Lines->Add(sss);

	fmProperties->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::N39Click(TObject *Sender)
{
	fmConsole->ShowModal();	
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::ToolButton5Click(TObject *Sender)
{
	N33->Click();
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::ToolButton7Click(TObject *Sender)
{
	N35->Click();
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::ToolButton6Click(TObject *Sender)
{
	N34->Click();
}//---------------------------------------------------------------------------

void __fastcall TfmMainView::N33Click(TObject *Sender)
{ // TRACE RUN
	try {
		SaveBackup();

		if (Emulator.esState == esRunning || Emulator.esState == esTracing) {
			MessageBox(Handle, "Алгоритм уже эмулируется", "Ошибка запуска эмулятора", MB_ICONINFORMATION);
			return;
		}

		vector<string> vct;
		for (int i = 0; i < Memo2->Lines->Count; i++) {
			vct.push_back(Memo2->Lines->Strings[i].c_str());
		}

		TraceInit(this);
		Emulator.FillSession(vct);
		Emulator.Run(1);
	}

	catch (SyntaxError err) {
		DebugTrace(
		string("Синтаксическая ошибка на строке [")
		+IntToStr(err.line+1).c_str()+"]: "
		+err.message
		, error
		);

		MessageBox(Handle,
		"В исходном коде алгоритма найдена синтаксическая ошибка.\nПодробности смотрите в окне сообщений отладчика.",
		"Синтаксическая ошибка",
		MB_ICONWARNING);
		ErrorAutoReport();
		Emulator.bNeedToStop = 1;
		Emulator.esState = esStoped;
	}

	catch (LogicalError err) {
		DebugTrace(
		string("Логическая ошибка на строке [")
		+IntToStr(err.line+1).c_str()+"]: "
		+err.message
		, error
		);

		MessageBox(Handle,
		"В исходном коде алгоритма найдена логическая ошибка.\nПодробности смотрите в окне сообщений отладчика.",
		"Логическая ошибка",
		MB_ICONWARNING);
		ErrorAutoReport();
		Emulator.bNeedToStop = 1;
		Emulator.esState = esStoped;
	}

	catch (RuntimeError err) {
		DebugTrace(
		string("В процессе выполнения алгоритма возникла ошибка: ")
		+err.message, error);

		MessageBox(Handle,
		"Во время выполнения алгоритма произошла ошибка.\nПодробности смотрите в окне сообщений отладчика.",
		"Ошибка времени выполнения",
		MB_ICONWARNING);
		Emulator.bNeedToStop = 1;
		Emulator.esState = esStoped;
	}
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::N34Click(TObject *Sender)
{ // TRACE STEP

	try {
		Emulator.StepIt();
	}

	catch (RuntimeError err) {
		DebugTrace(
		string("В процессе выполнения алгоритма возникла ошибка: ")
		+err.message, error);

		MessageBox(Handle,
		"Во время выполнения алгоритма произошла ошибка.\nПодробности смотрите в окне сообщений отладчика.",
		"Ошибка времени выполнения",
		MB_ICONWARNING);
		Emulator.bNeedToStop = 1;
		Emulator.esState = esStoped;
	}
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::N35Click(TObject *Sender)
{
	// Завершить отладку

	try {
		Emulator.Stop();
	}

	catch (RuntimeError err) {
		DebugTrace(
		string("В процессе выполнения алгоритма возникла ошибка: ")
		+err.message, error);

		MessageBox(Handle,
		"Во время выполнения алгоритма произошла ошибка.\nПодробности смотрите в окне сообщений отладчика.",
		"Ошибка времени выполнения",
		MB_ICONWARNING);
		Emulator.bNeedToStop = 1;
		Emulator.esState = esStoped;
	}
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::N27Click(TObject *Sender)
{
	fmConsoleSettings->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::TabSheet5Show(TObject *Sender)
{
	EnableTBTNS();
	RefreshVarWnd();

	if (Emulator.esState == esTracing) {
		Panel2->Width = 430;
		Splitter1->Show();
		StringGrid3->Row = Emulator.algs[fmRunParams->GetMainAlgorithm()]->GetLine();
	}
	else {
		Splitter1->Hide();
		Panel2->Width = 0;
	}
}
//---------------------------------------------------------------------------


void __fastcall TfmMainView::StringGrid3Click(TObject *Sender)
{
	StringGrid3->Row = Emulator.algs[fmRunParams->GetMainAlgorithm()]->GetLine();
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::N44Click(TObject *Sender)
{
	N44->Checked = 1;
	N44->Default = 1;
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::N43Click(TObject *Sender)
{
	N43->Checked = 1;
	N43->Default = 1;
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::SaveBackup() {
	Memo2->Lines->SaveToFile((GetPath()+"\\backup.alg").c_str());
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::ListBox1KeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
	if ( Key == 13 )
		Label5Click(this);
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::N48Click(TObject *Sender)
{
	PageControl1->SelectNextPage(1,1);
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::N49Click(TObject *Sender)
{
	PageControl1->SelectNextPage(0,1);
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::Label6Click(TObject *Sender)
{
	string str = GetPath();
	str+="\\Help\\Manual\\doc1.htm";
	ShellExecute(Handle,"open",str.c_str(), 0 , 0, SW_SHOWMAXIMIZED );
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::Label9Click(TObject *Sender)
{
	string str = GetPath();
	str+="\\Help\\Manual\\doc2.htm";
	ShellExecute(Handle,"open",str.c_str(), 0 , 0, SW_SHOWMAXIMIZED );
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::Label10Click(TObject *Sender)
{
	string str = GetPath();
	str+="\\Help\\Manual\\doc3.htm";
	ShellExecute(Handle,"open",str.c_str(), 0 , 0, SW_SHOWMAXIMIZED );
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::Label11Click(TObject *Sender)
{
	string str = GetPath();
	str+="\\Help\\Manual\\doc4.htm";
	ShellExecute(Handle,"open",str.c_str(), 0 , 0, SW_SHOWMAXIMIZED );
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::Label7Click(TObject *Sender)
{
	string str = GetPath();
	str+="\\Help\\Manual\\doc5.htm";
	ShellExecute(Handle,"open",str.c_str(), 0 , 0, SW_SHOWMAXIMIZED );
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::Label8Click(TObject *Sender)
{
	string str = GetPath();
	str+="\\Help\\Manual\\doc6.htm";
	ShellExecute(Handle,"open",str.c_str(), 0 , 0, SW_SHOWMAXIMIZED );
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::Label4Click(TObject *Sender)
{
	string str = GetPath();
	str+="\\Help\\Version\\Version.htm";
	ShellExecute(Handle,"open",str.c_str(), 0 , 0, SW_SHOWMAXIMIZED );
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::Memo2KeyPress(TObject *Sender, char &Key)
{
	if (Key == '\r') {
		// Этот фрагмент выполняется до добавления символа перевода стороки
		int ss;
		static TPoint p;
		ss = Memo2->CaretPos.x;

		String str;
		String Str = Memo2->Lines->Strings[Memo2->CaretPos.y];
		for (int i = 1;
		i <= Str.Length(); i++) {
			if(Str[i]=='\t')
				str += '\t';
			else break;
		}

		Memo2->Lines->Strings[Memo2->CaretPos.y]=Memo2->Lines->Strings[Memo2->CaretPos.y].Insert(str,Memo2->CaretPos.x+1);

		p.x = ss;
		p.y = Memo2->CaretPos.y;
		Memo2->CaretPos = p;

		for (int i = 1; i <= str.Length(); i++) {
			PostMessage(Memo2->Handle, WM_KEYDOWN, 39, 0); //39
			PostMessage(Memo2->Handle, WM_KEYUP, 39, 0);
		}
	}
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/*void __fastcall TfmMainView::ApplicationEvents1Message(tagMSG &Msg,
	  bool &Handled)
{
	static int ss;
	if (Msg.message == WM_KEYDOWN) {
		if (Msg.wParam == 9 && Msg.lParam == 983041 && Memo2->Focused()) {
			ss = Memo2->CaretPos.x;
			Memo2->Lines->Strings[Memo2->CaretPos.y]=Memo2->Lines->Strings[Memo2->CaretPos.y].Insert("\t",Memo2->CaretPos.x+1);

			static TPoint p;
			p.x = ss + 1;
			p.y = Memo2->CaretPos.y;
			Memo2->CaretPos = p;

			Handled = 1;
		}
	}
}*/

// --------------------------- Подсветка --------------------------------
inline bool IsDivider(char ch) {
	for (int i = 0; szDividers[i] != '\0'; i++)
		if (szDividers[i] == ch)
			return 1;
	return 0;
}

//---------------------------------------------------------------------------
inline bool IsNumber(String str) {
	// Щас посмотрим - число это или нет...
	try {
		StrToFloat(str);
		// Если всё в норме, значит это - число
		return 1;
	}
	// Всё-таки не число
	catch(...){return 0;}
}

// --------------------------- Подсветка --------------------------------
void __fastcall TfmMainView::PaintInColors() {
	if (fmEditorSettings->CheckBox1->Checked) {
		if (fmEditorSettings->CheckBox3->Checked)
			bNeedToDraw = 1;
		else
			PaintInColorsExecute();
	}
	else
	if (bNeedToRedraw) {
		TNotifyEvent p = fmMainView->Memo2->OnChange;
		fmMainView->Memo2->OnChange = 0;
		int selStart = Memo2->SelStart;
		int selLength = Memo2->SelLength;

		fmMainView->Memo2->SelStart  = 0;
		fmMainView->Memo2->SelLength = fmMainView->Memo2->Text.Length();
		fmMainView->InitByFont(fmMainView->Memo2->SelAttributes, fmMainView->GetCodeFont());

		Memo2->SelStart = selStart;
		Memo2->SelLength = selLength;
		fmMainView->Memo2->OnChange = p;
		bNeedToRedraw = 0;
	}
}
// --------------------------- Подсветка --------------------------------
void __fastcall TfmMainView::PaintInColorsExecute() {
	// Медленно!!!

	int beg = 0, end = Memo2->Lines->Count;
	bool bProgressive = fmEditorSettings->CheckBox5->Checked && !Memo2->SelLength;

	if (bProgressive) {
		if (Memo2->CaretPos.y) // if (Memo2->CaretPos.y > 0)
			beg = Memo2->CaretPos.y - 1;
		else
			beg = Memo2->CaretPos.y;

		if (Memo2->CaretPos.y < (Memo2->Lines->Count-1))
			end = Memo2->CaretPos.y + 1;
		else
			end = Memo2->CaretPos.y;
	}

	TNotifyEvent p = Memo2->OnChange;
	Memo2->OnChange = 0;
	Memo2->Lines->BeginUpdate();
	// Айда раскрашивать!

	int selStart = Memo2->SelStart;
	int selLength = Memo2->SelLength;


	Memo2->SelStart  = 0;
	Memo2->SelLength = Memo2->Text.Length();
	InitByFont(Memo2->SelAttributes, fontCode);

	String str;
	int nLastLecsemBeg;
	int nCharsLast = 0;
	bool bIsString;
	int i,j;

	for (i = beg; i < end; i++) {
		str = Memo2->Lines->Strings[i];
		nLastLecsemBeg = -1;
		bIsString = 0;
		for (j = 1; j <= str.Length(); j++) {

			// Строки
			if (str[j] == '\'')
			{
				if (!bIsString) { 	// Начало строки
					bIsString = 1;
					nLastLecsemBeg = j;
				}
				else {              // Конец строки
					bIsString = 0;
					// Раскрашиваем...
					Memo2->SelStart  = nCharsLast + nLastLecsemBeg - 1;
					Memo2->SelLength = j - nLastLecsemBeg + 1;
					if (Memo2->SelLength > 3) {
						InitByFont(Memo2->SelAttributes, fontString);
					}
					else
						InitByFont(Memo2->SelAttributes, fontChar);
					nLastLecsemBeg = -1;
					// DONE!
				}
				continue;
			}
			if (bIsString) continue;

			// Комментарии
			if (str[j] == '|'){
				// Раскрашиваем...
				Memo2->SelStart  = j + nCharsLast - 1;
				Memo2->SelLength = str.Length() - j + 1;
				InitByFont(Memo2->SelAttributes, fontComment);
				nLastLecsemBeg = -1; // Можно удалить эту строку
				break;	// переходим к следующей строке
			}

			// Другая бесовщина:

			// Конец лексемы
			// Или это последний символ? (-> тоже конец лексемы)
			if (IsDivider(str[j]) && nLastLecsemBeg != -1) {

				const String lexem = Memo2->Text.SubString(nCharsLast + nLastLecsemBeg, j - nLastLecsemBeg);

				// KEYWORD?
				if (IsKeyword(lexem.UpperCase().c_str())) {
					Memo2->SelStart  = nCharsLast + nLastLecsemBeg-1;
					Memo2->SelLength = j - nLastLecsemBeg;
					InitByFont(Memo2->SelAttributes, fontKeyword);
				}

				// NUMBER?
				if (IsNumber(lexem)) {
					Memo2->SelStart  = nCharsLast + nLastLecsemBeg-1;
					Memo2->SelLength = j - nLastLecsemBeg;
					InitByFont(Memo2->SelAttributes, fontNumber);
				}

				nLastLecsemBeg = -1;
				continue;
			}

			// Новая лексема
			if ( nLastLecsemBeg == -1 && !IsDivider(str[j]) )
				nLastLecsemBeg = j;
		}

		if (nLastLecsemBeg != -1) {
			const String lexem = Memo2->Text.SubString(nCharsLast + nLastLecsemBeg, str.Length() - nLastLecsemBeg + 1);

			// KEYWORD?
			if (IsKeyword(lexem.UpperCase().c_str())) {
				Memo2->SelStart  = nCharsLast + nLastLecsemBeg-1;
				Memo2->SelLength = str.Length() - nLastLecsemBeg + 1;
				InitByFont(Memo2->SelAttributes, fontKeyword);
			}

			// NUMBER?
			if (IsNumber(lexem)) {
				Memo2->SelStart  = nCharsLast + nLastLecsemBeg-1;
				Memo2->SelLength = str.Length() - nLastLecsemBeg + 1;
				InitByFont(Memo2->SelAttributes, fontNumber);
			}
		}

		nCharsLast+=str.Length()+2;
	}

	Memo2->SelStart = selStart;
	Memo2->SelLength = selLength;

	Memo2->Lines->EndUpdate();
	Memo2->OnChange = p;
}

//---------------------------------------------------------------------------
void __fastcall TfmMainView::N10Click(TObject *Sender)
{
	fmEditorSettings->Show();
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::N50Click(TObject *Sender)
{
	if (Emulator.esState != esStoped) {
		MessageBox(Handle, "Настройка параметров запуска невозможна, т.к алгоритм запущен,\n для изменения настроек запуска, остановите выполнение алгоритма", "Параметры", MB_ICONINFORMATION);
		return;
	}

	vector<string> vct;
	for (int i = 0; i < Memo2->Lines->Count; i++) {
		vct.push_back(Memo2->Lines->Strings[i].c_str());
	}

	try {
		TraceInit(this);
		Emulator.FillSession(vct);
	} catch (...) {
	}

	fmRunParams->ShowModal();}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::FormClose(TObject *Sender, TCloseAction &Action)
{
	thrd->loop = 0;
	delete thrd;
}
//---------------------------------------------------------------------------
void __fastcall TfmMainView::NHistoryClick(TObject *Sender)
{
	ShowTab("История ввода-вывода", 1);
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::ToolButton22Click(TObject *Sender)
{
	StringGrid1->RowCount = 1;	
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::ToolButton23Click(TObject *Sender)
{
	StringGrid1DblClick(Sender);	
}
//---------------------------------------------------------------------------


void __fastcall TfmMainView::ApplicationEvents1Message(tagMSG &Msg,
      bool &Handled)
{
	if(Msg.message == WM_CHAR && (int)GetKeyboardLayout(0)!=67699721){
		switch (Msg.wParam) {
		case '\"':
			Msg.wParam = '\'';
			break;
		case '/':
			Msg.wParam = '|';
			break;
		default:
			;
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TfmMainView::NRedoClick(TObject *Sender)
{
	ehHistoryManager.Redo(Memo2);
	PaintInColors();
}
//---------------------------------------------------------------------------



void __fastcall TfmMainView::FindDialog1Find(TObject *Sender)
{
	// Копия для поиска
	string text; // Текст
	string sstr; // Подстрока

	// Последствия флажка "С учетом регистра"
	if (fmMainView->FindDialog1->Options.Contains(frMatchCase)) {
		text = fmMainView->Memo2->Lines->Text.c_str();
		sstr = fmMainView->FindDialog1->FindTextA.c_str();
	}
	else {
		text = fmMainView->Memo2->Lines->Text.UpperCase().c_str();
		sstr = fmMainView->FindDialog1->FindTextA.UpperCase().c_str();
	}

	// Ищем...
	int		nLastPos;
	if (fmMainView->FindDialog1->Options.Contains(frDown))
		nLastPos = text.find(sstr, fmMainView->Memo2->SelStart+1);
	else
		nLastPos = text.rfind(sstr, fmMainView->Memo2->SelStart-1);


	if (nLastPos != string::npos) {
		fmMainView->Memo2->SelStart = nLastPos;
		fmMainView->Memo2->SelLength = fmMainView->FindDialog1->FindTextA.Length();
		ShowWindow("Алгоритм", 1);
		fmMainView->Memo2->SetFocus();
	}
	else
		MessageBox(fmMainView->FindDialog1->Handle,
		(String("Фрагмент \"")+fmMainView->FindDialog1->FindTextA+"\" не найден").c_str(), "Поиск", MB_ICONINFORMATION);
}
//---------------------------------------------------------------------------


