#include <Commands.cpp>

//---------------------------------------------------------------------------
typedef deque<string> object;
enum TNodeType {ntOperation = 0, ntObject};
enum TObjType {otArray, otMatrix, otVariable, otBufConst, otAlgorithm};
struct TExpressionNode{
	TNodeType	 ntType;
	TObjType	 objType;
	string		 strOperation;
	object		 objObject;
};

//---------------------------------------------------------------------------
TVariable ExecuteFullExpression (TVarHeader& hdr, TVarHeader& buffer, deque<string> vct);
//---------------------------------------------------------------------------

bool EXPRIsNObject(const string& strCOP) {
	if (
		strCOP == "="
	||  strCOP == "<>"
	||  strCOP == "<"
	||  strCOP == ">"
	||  strCOP == "<="
	||  strCOP == ">="
	||  strCOP == "+"
	||  strCOP == "-"
	||  strCOP == "*"
	||  strCOP == "/"
	||  strCOP == ":"
	||  strCOP == "**"
	||  strCOP == "И"
	||  strCOP == "ИЛИ"
	) return 1;

	return 0;
}

//---------------------------------------------------------------------------

TExpressionNode EXPRGetObject(const deque<string>& vBracket, TVarHeader& hdr, TVarHeader& buffer,
	 vector<TExpressionNode>& vExpression, int& pos) {

	TExpressionNode node;
	node.ntType = ntObject;

	// МАССИВ
	if (hdr.IsPresent(vBracket[pos]) && hdr.GetVariable(vBracket[pos]).varType == vtArray) {
		int xpos = pos;
		node.objObject.push_back(vBracket[pos]);
		pos+=2;

		string str;
		node.objObject.push_back("[");
		int n = 1;
		while (1) {
			str = vBracket[pos];
			pos++;
			if (str == "]")
				n--;
			if (str == "[")
				n++;
			if (n)
				node.objObject.push_back(str);
			else
				break;
		}
		node.objObject.push_back("]");
		node.objType = otArray;
		return node;
	}

	// МАТРИЦА
	if (hdr.IsPresent(vBracket[pos]) && hdr.GetVariable(vBracket[pos]).varType == vtMatrix) {
		int xpos = pos;
		node.objObject.push_back(vBracket[pos]);
		pos+=2;

		string str;
		node.objObject.push_back("[");
		int n = 1;
		while (1) {
			str = vBracket[pos];
			pos++;
			if (str == "]")
				n--;
			if (str == "[")
				n++;
			if (n)
				node.objObject.push_back(str);
			else
				break;
		}
		node.objObject.push_back("]");
		node.objType = otMatrix;
		return node;
	}

	// Алгоритм?
	if ( Emulator.IsAlgPresent(vBracket[pos]) ) {
		int xpos = pos;
		node.objObject.push_back(vBracket[pos]);
		pos+=2;

		string str;
		node.objObject.push_back("(");
		int n = 1;
		while (1) {
			str = vBracket[pos];
			pos++;
			if (str == ")")
				n--;
			if (str == "(")
				n++;
			if (n)
				node.objObject.push_back(str);
			else
				break;
		}
		node.objObject.push_back(")");
		node.objType = otAlgorithm;
		return node;
	}

		// Другая(обычная) переменная?
	if (hdr.IsPresent(vBracket[pos])) {
		node.objObject.push_back(vBracket[pos]);
		pos++;
		node.objType = otVariable;
		return node;
	}

	// Константа?
	if (buffer.IsPresent(vBracket[pos])) {
		node.objObject.push_back(vBracket[pos]);
		pos++;
		node.objType = otBufConst;
		return node;
	}


	throw RuntimeError(string("КРИТИЧЕСКАЯ ОШИБКА ЭМУЛЯТОРА: EXPRGetObject() не опознал объект: ")+vBracket[pos]);
}

//---------------------------------------------------------------------------
void TranslateVSTRToExpression(const deque<string>& vBracket, TVarHeader& hdr,
TVarHeader& buffer, vector<TExpressionNode>& vExpression) {

	int pos = 0;
	TExpressionNode node;
	node.ntType = ntOperation;

	if (vBracket[0] == "-" || vBracket[0] == "НЕ") {
		node.strOperation = vBracket[0];
		vExpression.push_back(node);
		pos++;
	}

	do {
		if (EXPRIsNObject(vBracket[pos])) {
			node.strOperation = vBracket[pos];
			vExpression.push_back(node);
			pos++;
		}
		else
			vExpression.push_back(EXPRGetObject(vBracket, hdr, buffer, vExpression, pos));
	}while(pos < vBracket.size());
}

//-------------------------------------------------------------------------//
// 							  EXECUTE OBJECT							   //
//-------------------------------------------------------------------------//

//---------------------------------------------------------------------------

TVariable ExecuteArrObject(const TExpressionNode& nd, TVarHeader& hdr, TVarHeader& buffer) {
	TVariable varI(ss_niSupportState);
	object obj;
	for (int i = 2; i < nd.objObject.size()-1; i++)     // 0:NAME, 1:[, 2:..., (size-1):]
		obj.push_back(nd.objObject[i]);
	varI = ExecuteFullExpression(hdr, buffer, obj);

	return *hdr.GetVariable(nd.objObject[0]).Indexing(varI);
}

//---------------------------------------------------------------------------

TVariable ExecuteMtxObject(const TExpressionNode& nd, TVarHeader& hdr, TVarHeader& buffer) {
	TVariable varI(ss_niSupportState);
	TVariable varJ(ss_niSupportState);

	int ppos = 0;
	for (int i = 0; i < nd.objObject.size()-1; i++)
		if (nd.objObject[i] == ",") ppos = i;

	object obj;
	for (int i = 2; i < ppos; i++)
		obj.push_back(nd.objObject[i]);
	varI = ExecuteFullExpression(hdr, buffer, obj);

	obj.clear();
	for (int i = ppos+1; i < nd.objObject.size()-1; i++)
		obj.push_back(nd.objObject[i]);
	varJ = ExecuteFullExpression(hdr, buffer, obj);


	return *hdr.GetVariable(nd.objObject[0]).Indexing(varI, varJ);
}

//---------------------------------------------------------------------------

TVariable ExecuteAlgObject(const TExpressionNode& nd, TVarHeader& hdr, TVarHeader& buffer) {
	/* АЛГ1([PARAM [,PARAM][, PARAM]...]) */
	/*
		0:			'АЛГ1'
		1:			'('
		2..n-1:		'PARAM'

		n:			','
		n+1..m-1 	'PARAM'

		m: 			')'
	*/

	deque<TVariable> queParams;
	TVariable var(ss_niSupportState);

/* DONE 1 -oAZE : ExecuteAlgObject, вычисление параметров */
	bool bLoop = 1;
	int pos = 2;
	int nBC = 0; // вложенность скобок
	deque<string> queParam;
	if (nd.objObject[2]!=")") { // Если параметры вообще есть
		while (bLoop) {
			// Добываем новый параметр
			queParam.clear();
			for (; 1; pos++) {
				/* TODO -oAZE : ТРАБЛА: АЛГ1(АЛГ2(1,2,2),АЛГ3())
							или Алгоритм2(Алгоритм2(б)) */
				if (nd.objObject[pos]=="(")
					nBC++;
				if (nBC) {
					queParam.push_back(nd.objObject[pos]);
					if (nd.objObject[pos]==")")
						nBC--;
					continue;
				}

				if (nd.objObject[pos]==",") { pos++; break; }
				if (nd.objObject[pos]==")") { pos++; bLoop = 0; break; }
				queParam.push_back(nd.objObject[pos]);
			}
			queParams.push_back(ExecuteFullExpression(hdr, buffer, queParam));
		}
	}

	return Emulator.GetAlgorithmPointer(nd.objObject[0])->Call(queParams);
}

//---------------------------------------------------------------------------
// Возвращает конечный вычисленный образ(instance) объекта
TVariable ExecuteObject(const TExpressionNode& nd, TVarHeader& hdr, TVarHeader& buffer) {
	if (!nd.ntType)
		throw RuntimeError("КРИТИЧЕСКАЯ ОШИБКА ЭМУЛЯТОРА: в ExecuteObject попало что-то, мало похожее на объект");

	switch (nd.objType) {
	case otArray:
		return ExecuteArrObject(nd, hdr, buffer);

	case otMatrix:
		return ExecuteMtxObject(nd, hdr, buffer);

	case otVariable:

		return hdr.GetVariable(nd.objObject[0]);

	case otBufConst:

		return buffer.GetVariable(nd.objObject[0]);

	case otAlgorithm:
		return ExecuteAlgObject(nd, hdr, buffer);
	}
}

//---------------------------------------------------------------------------

int FindMostPriorityCOP(vector<TExpressionNode>& vExpression, vector<TExpressionNode>& vElemExpr) {
	vElemExpr.clear();
	for (int i = 0; i < vExpression.size(); i++) {
		if (!vExpression[i].ntType) {
			if( vExpression[i].strOperation == "="
			||  vExpression[i].strOperation == "<>"
			||  vExpression[i].strOperation == "<"
			||  vExpression[i].strOperation == ">"
			||  vExpression[i].strOperation == "<="
			||  vExpression[i].strOperation == ">="

			||  vExpression[i].strOperation == "*"
			||  vExpression[i].strOperation == "**"
			||  vExpression[i].strOperation == "/"
			||  vExpression[i].strOperation == ":"

			||  vExpression[i].strOperation == "И"
			||  vExpression[i].strOperation == "ИЛИ"
			) {
				vElemExpr.push_back(vExpression[i-1]);
				vElemExpr.push_back(vExpression[i]);
				vElemExpr.push_back(vExpression[i+1]);
				return i-1;
			}
		}
	}

	for (int i = 0; i < vExpression.size(); i++) {
		if (!vExpression[i].ntType) {
			if( vExpression[i].strOperation == "+"
			||  vExpression[i].strOperation == "-"
			) {
				vElemExpr.push_back(vExpression[i-1]);
				vElemExpr.push_back(vExpression[i]);
				vElemExpr.push_back(vExpression[i+1]);
				return i-1;
			}
		}
	}

	throw RuntimeError("ПОДЛЕЖАЩАЯ ВЫПОЛНЕНИЮ ОПЕРАЦИЯ НЕ НАЙДЕНА");
}

//---------------------------------------------------------------------------

TVariable ExecuteElemCOP(vector<TExpressionNode>& vElemExpr, int nElemPos,
  TVarHeader& hdr, TVarHeader& buffer) {
	string strCOP = vElemExpr[1].strOperation;
	TVariable var(ss_niSupportState);

	if (strCOP == "=") var = ExecuteObject(vElemExpr[0], hdr, buffer).EqvalCmp(ExecuteObject(vElemExpr[2], hdr, buffer));
	else if (strCOP == "<>") var = ExecuteObject(vElemExpr[0], hdr, buffer).NotEqvalCmp(ExecuteObject(vElemExpr[2], hdr, buffer));
	else if (strCOP == "<") var = ExecuteObject(vElemExpr[0], hdr, buffer).LessCmp(ExecuteObject(vElemExpr[2], hdr, buffer));
	else if (strCOP == ">") var = ExecuteObject(vElemExpr[0], hdr, buffer).GreaterCmp(ExecuteObject(vElemExpr[2], hdr, buffer));
	else if (strCOP == "<=") var = ExecuteObject(vElemExpr[0], hdr, buffer).LessOrEqCmp(ExecuteObject(vElemExpr[2], hdr, buffer));
	else if (strCOP == ">=") var = ExecuteObject(vElemExpr[0], hdr, buffer).GreaterOrEqCmp(ExecuteObject(vElemExpr[2], hdr, buffer));

	else if (strCOP == "И") var = ExecuteObject(vElemExpr[0], hdr, buffer).AND(ExecuteObject(vElemExpr[2], hdr, buffer));
	else if (strCOP == "ИЛИ") var = ExecuteObject(vElemExpr[0], hdr, buffer).OR(ExecuteObject(vElemExpr[2], hdr, buffer));

	else if (strCOP == "*") var = ExecuteObject(vElemExpr[0], hdr, buffer).Mul(ExecuteObject(vElemExpr[2], hdr, buffer));
	else if (strCOP == "**") var = ExecuteObject(vElemExpr[0], hdr, buffer).Pow(ExecuteObject(vElemExpr[2], hdr, buffer));
	else if (strCOP == "/") var = ExecuteObject(vElemExpr[0], hdr, buffer).Div(ExecuteObject(vElemExpr[2], hdr, buffer));
	else if (strCOP == ":") var = ExecuteObject(vElemExpr[0], hdr, buffer).Mod(ExecuteObject(vElemExpr[2], hdr, buffer));

	else if (strCOP == "+") var = ExecuteObject(vElemExpr[0], hdr, buffer).Add(ExecuteObject(vElemExpr[2], hdr, buffer));
	else if (strCOP == "-") var = ExecuteObject(vElemExpr[0], hdr, buffer).Sub(ExecuteObject(vElemExpr[2], hdr, buffer));
	else throw RuntimeError(string("ОПЕРАЦИЯ НЕ ОПОЗНАНА: ")+strCOP);

	return var;
}

//---------------------------------------------------------------------------
void ReplaceCOP(vector<TExpressionNode>& vExpression, vector<TExpressionNode>& vElemExpr,
 int pos, TVariable& varRESULT, TVarHeader& buffer) {
	vExpression.erase(&vExpression[pos], &vExpression[pos+(vElemExpr.size())]);

	varRESULT.strName = string("ConstBufferVar#");
	varRESULT.strName += (IntToStr(buffer.vctVariables.size())).c_str(); // Нумеруются с 0
	buffer.vctVariables.push_back(varRESULT);
	buffer.vctVariables.push_back(varRESULT);

	TExpressionNode node;
	node.ntType = ntObject;
	node.objType = otBufConst;
	node.objObject.push_back(varRESULT.strName);

	vExpression.insert(&vExpression[pos], node);
}

//---------------------------------------------------------------------------
string ExecuteCOP(const deque<string>& vBracket, TVarHeader& hdr, TVarHeader& buffer) {
	DebugTrace(string("@ExecuteCOP"));

	DebugTrace(string("Выражение:"));
	for (unsigned i = 0; i < vBracket.size(); i++) {
		DebugTrace(string(" ")+IntToStr(i+1).c_str()+") "+vBracket[i]);
	}

	vector<TExpressionNode> vExpression;
	vector<TExpressionNode> vElemExpr;
	TranslateVSTRToExpression(vBracket, hdr, buffer, vExpression);

	DebugTrace(string("Разложено:"));
	for (unsigned i = 0; i < vExpression.size(); i++) {
		if (vExpression[i].ntType) { // object
			for (unsigned j = 0; j < vExpression[i].objObject.size(); j++) {
				DebugTrace(string(" ")+IntToStr(i+1).c_str()+") OBJ: "+vExpression[i].objObject[j]);
			}
		}
		else 						 // operation
			DebugTrace(string(" ")+IntToStr(i+1).c_str()+") COP: "+vExpression[i].strOperation);
	}


	int pos = 0;
	TVariable varRESULT (ss_niSupportState); // Текущий тип

	if (!vExpression[0].ntType && vExpression[0].strOperation == "-") {
		pos = 1;
		varRESULT = ExecuteObject(vExpression[1], hdr, buffer);
		varRESULT = varRESULT.Neg();
		vElemExpr.push_back(vExpression[0]);
		vElemExpr.push_back(vExpression[1]);
		ReplaceCOP(vExpression, vElemExpr, 0, varRESULT, buffer);
	}

	else if (!vExpression[0].ntType && vExpression[0].strOperation == "НЕ") {
		pos = 1;
		varRESULT = ExecuteObject(vExpression[1], hdr, buffer);
		varRESULT = varRESULT.NOT();
		vElemExpr.push_back(vExpression[0]);
		vElemExpr.push_back(vExpression[1]);
		ReplaceCOP(vExpression, vElemExpr, 0, varRESULT, buffer);
	}

	else {
		varRESULT = ExecuteObject(vExpression[0], hdr, buffer);
	}


	DebugTrace(string("Исходный тип выражения(varRESULT): ")+varRESULT.GetStrType());

	int nElemPos = -1; // Позиция начала текущей команды,
	int m_ss_nIterOverflowCounter = 100;

	while(vExpression.size() > 1) {
		DebugTrace(string("Шаг вычисления..."));

		DebugTrace("Содержимое очереди:");
		for (unsigned i = 0; i < vExpression.size(); i++) {
			if (vExpression[i].ntType) { // object
				for (unsigned j = 0; j < vExpression[i].objObject.size(); j++) {
					DebugTrace(string(" ")+IntToStr(i+1).c_str()+") OBJ: "+vExpression[i].objObject[j]);
				}
			}
			else 						 // operation
				DebugTrace(string(" ")+IntToStr(i+1).c_str()+") COP: "+vExpression[i].strOperation);
		}

		DebugTrace(string("Поиск наиболее приоритетной операции..."));
		nElemPos = FindMostPriorityCOP(vExpression, vElemExpr);
		DebugTrace(string("Поиск завершен, результат:"));
		for (unsigned i = 0; i < vElemExpr.size(); i++) {
			if (vElemExpr[i].ntType) { // object
				for (unsigned j = 0; j < vElemExpr[i].objObject.size(); j++) {
					DebugTrace(string(" ")+IntToStr(i+1).c_str()+") OBJ: "+vElemExpr[i].objObject[j]);
				}
			}
			else 						 // operation
				DebugTrace(string(" ")+IntToStr(i+1).c_str()+") COP: "+vElemExpr[i].strOperation);
		}

		DebugTrace(string("Шаг выполнения, текущее смещение: ")+IntToStr(nElemPos).c_str());

		varRESULT = ExecuteElemCOP(vElemExpr, nElemPos, hdr, buffer);
		ReplaceCOP(vExpression, vElemExpr, nElemPos, varRESULT, buffer);

		m_ss_nIterOverflowCounter--;
		if(!m_ss_nIterOverflowCounter)
			throw RuntimeError("ПРЕДУПРЕЖДЕНИЕ(ВОЗМОЖНАЯ ОШИБКА) ИНТЕРПРЕТАТОРА: ПРЕВЫШЕН ОТЛАДОЧНЫЙ ПОРОГ ИТЕРАЦИОННОСТИ ОПЕРАЦИИ");
	}

	DebugTrace(string("@ExecuteCOP exit"));

	varRESULT.strName = string("ConstBufferVar#");
	varRESULT.strName += (IntToStr(buffer.vctVariables.size())).c_str(); // Нумеруются с 0
	buffer.vctVariables.push_back(varRESULT);

	return varRESULT.strName;
}

//---------------------------------------------------------------------------
// Выполнение линейного алгоритма и возврат имени буферной переменной, содержащей ответ.
TVariable ExecuteFullExpression (TVarHeader& hdr, TVarHeader& buffer, deque<string> vct /*Expression*/) {
	string str;
	TranslateConstToVar(vct, buffer, 0);
	vct.push_front("(");
	vct.push_back(")");

	int MostHighBracketPos = 0;
	deque<string> vBracket;
	while (BracketsPresents(vct)) {
		FindMostPriorityBracket(vct,vBracket,MostHighBracketPos);
		str = ExecuteCOP(vBracket, hdr, buffer);
		ReplaceBracket(vct,vBracket,MostHighBracketPos, str);
	}

	return buffer.GetVariable(vct[0]);
}

//---------------------------------------------------------------------------
bool CmdCopyExpression::Execute(TVarHeader& hdrHost, TAlgorithm* pAlg) {
	TVariable* var = hdrHost.GetVariablePTR(strVarOriginal);

	if (var->varType == vtArray) {
		DebugTrace("Содержимое очереди queI:");
		for (unsigned i = 0; i < queI.size(); i++) {
			DebugTrace(queI[i]);
		}

		DebugTrace("Содержимое буфера:");
		DebugTrace("_________________________________________________________");
		buf.Print();
		DebugTrace("_________________________________________________________");

		TVariable varX(ss_niSupportState);
		varX = ExecuteFullExpression(hdrHost, buf, queI);
		var = var->Indexing(varX);
	}

	else if (var->varType == vtMatrix) {
		TVariable varXI(ss_niSupportState);
		TVariable varXJ(ss_niSupportState);
		varXI = ExecuteFullExpression(hdrHost, buf, queI);
		varXJ = ExecuteFullExpression(hdrHost, buf, queJ);
		var = var->Indexing(varXI, varXJ);
	}

	exp->Execute(hdrHost, pAlg);
	var->Copy(*exp->pvarExecuteResult, pAlg);

	return 1;  // Always 1 - it is not a container
}

//---------------------------------------------------------------------------
bool CmdExpression::Execute(TVarHeader& hdrHost, TAlgorithm* pAlg) {
	TVarHeader buf = buffer;
	deque<string> deq = vct;
	if (pvarExecuteResult)
		delete pvarExecuteResult;
	pvarExecuteResult = new TVariable(ExecuteFullExpression(hdrHost, buf, deq));
	return 1;
}

//---------------------------------------------------------------------------
/* DONE 5 -oAZE : Я сунул сюда pAlg для TVariable::Copy */
bool CmdAlgExpression::Execute(TVarHeader& hdrHost, TAlgorithm* pAlg) {
	TExpressionNode node;
	node.ntType = ntObject;
	node.objType = otMatrix;
	node.objObject = vct;
	TVarHeader buf = buffer;
	ExecuteAlgObject(node, hdrHost, buf);
	return 1;
}
//---------------------------------------------------------------------------

bool CmdIfExpression::Execute(TVarHeader& hdrHost, TAlgorithm* pAlg) {

	if (nCurrentCmd == -1) {
		deque<string> deq = deqCondition;
		TVarHeader buf = buffer;
		bTruth = ExecuteFullExpression(hdrHost, buf, deq).Value.blnValue;
		nCurrentCmd = 0;
		if ( (!bTruth) && (!vctElseSubCommands.size()) ) {
            nCurrentCmd = -1;
			return 1;
		}
		return 0;
	}
	else {
		if (bTruth) {
			if (vctSubCommands[nCurrentCmd]->Execute(hdrHost, pAlg)) {
				// Сдвигаем указатель на сл. команду
				nCurrentCmd++;
				if (nCurrentCmd >= vctSubCommands.size()) { // Сдвигать некуда?
					nCurrentCmd = -1;
					return 1;
				}
				else return 0;
			}
			else return 0;
		}
		else if (vctElseSubCommands.size() > 0) {
			if (vctElseSubCommands[nCurrentCmd]->Execute(hdrHost, pAlg)) {
				// Сдвигаем указатель на сл. команду
				nCurrentCmd++;
				if (nCurrentCmd == vctElseSubCommands.size()) { // Сдвигать некуда?
					nCurrentCmd = -1;
					return 1;
				}
				else return 0;
			}
			else return 0;
		}
		else { nCurrentCmd = -1; return 1; }
	}
	// (return 1) -> (nCurrentCmd := -1) !
}
//---------------------------------------------------------------------------

int  CmdIfExpression::GetLine() {
	if (nCurrentCmd == -1) {
		return line;
	}
	else {
		if(bTruth)
			return vctSubCommands[nCurrentCmd]->GetLine();
		else if (vctElseSubCommands.size() > 0)
			return vctElseSubCommands[nCurrentCmd]->GetLine();
		else
			return line;
	}
}
//---------------------------------------------------------------------------

bool CmdWhileExpression::Execute(TVarHeader& hdrHost, TAlgorithm* pAlg) {

	if (nCurrentCmd == -1) {
		deque<string> deq = deqCondition;
		TVarHeader buf = buffer;
		bool bTruth = ExecuteFullExpression(hdrHost, buf, deq).Value.blnValue;
		if (!bTruth)
			return 1;
		nCurrentCmd = 0;
		return 0;
	}
	else {
		if (vctSubCommands[nCurrentCmd]->Execute(hdrHost, pAlg)) {
		// Сдвигаем указатель на сл. команду
			nCurrentCmd++;
			if (nCurrentCmd >= vctSubCommands.size()) { // Сдвигать некуда?
				nCurrentCmd = -1;
				return 0;
			}
			else return 0;
		}
		else return 0;
	}
	// (return 1) -> (nCurrentCmd := -1) !
}
//---------------------------------------------------------------------------

int  CmdWhileExpression::GetLine() {
	if (nCurrentCmd == -1)
		return line;
	else
		return vctSubCommands[nCurrentCmd]->GetLine();
}
//---------------------------------------------------------------------------

