#ifndef EmulatorH
#define EmulatorH
#include "Algorithm.h"
//---------------------------------------------------------------------------

enum TEmulatorState {esStoped, esRunning, esTracing};

class TEmulator {
  public:
  vector<TAlgorithm*> algs;
  TEmulatorState esState;
  bool bNeedToStop;

  public:
	TEmulator();

	// EVERYWHERE INIT
	void 	FillSession(vector<string>&);

	// RUN/TRACE
	void 	Run(bool bStepOver);
	void 	StepIt();
	void 	Stop();
	void	Continue();

	// TRANSLATION
	vector<string> ToPascal();
	vector<string> ToBasic();
	void 	BuildBlockScheme();

	// OTHER
	bool IsAlgPresent(string strAlgName);
	TAlgorithm* GetAlgorithmPointer(string strAlgName);
  private:
	void CloseSession();
};

//---------------------------------------------------------------------------
extern TEmulator Emulator;
extern const char szDividers[];
//---------------------------------------------------------------------------
#endif
