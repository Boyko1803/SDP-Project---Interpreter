#include "Interpreter.h"

void Interpreter::removeSpaces(const std::string& s, int& beginIndex, int& endIndex)
{
	while (beginIndex < endIndex && s[beginIndex] == ' ') beginIndex++;
	while (endIndex > beginIndex && s[endIndex] == ' ') endIndex--;
}

void Interpreter::handleLackOfEndLine(const std::string& expectedEndLine)
{
	if (!expectedEndLine.compare("else"))
	{
		stateFlag = InterpreterErrorFlags::expectedElseFlag;
		return;
	}
	if (!expectedEndLine.compare("endif"))
	{
		stateFlag = InterpreterErrorFlags::expectedEndIfFlag;
		return;
	}
	if (!expectedEndLine.compare("endwhile"))
	{
		stateFlag = InterpreterErrorFlags::expectedEndWhileFlag;
		return;
	}
	if (!expectedEndLine.compare("endrecdef"))
	{
		stateFlag = InterpreterErrorFlags::expectedEndRecdefFlag;
		return;
	}
}

void Interpreter::handleErrorFlag(std::ostream& outputStream)
{
	switch (stateFlag)
	{
	case InterpreterErrorFlags::normalStateFlag:
		outputStream << "The program ended successfully!\n";
		return;
	case InterpreterErrorFlags::alreadyRunFlag:
		outputStream << "This interpreter has already run a program. Create a new instance to run another one!\n";
		break;
	case InterpreterErrorFlags::invalidAddressFlag:
		outputStream << "The given address is invalid!\n";
		stateFlag = InterpreterErrorFlags::normalStateFlag;
		return;
	case InterpreterErrorFlags::invalidLineFlag:
		outputStream << "There is an invalid command at line "<< currentLine <<"!\n";
		break;
	case InterpreterErrorFlags::invalidReturnValueFlag:
		outputStream << "There is an invalid return command at line " << currentLine << "! Returns commands are allowed only inside recursive definitions!\n";
		break;
	case InterpreterErrorFlags::expectedThenFlag:
		outputStream << "Expected \"then\" command at line " << currentLine << "!\n";
		break;
	case InterpreterErrorFlags::expectedElseFlag:
		outputStream << "Expected \"else\" command at line " << currentLine << "!\n";
		break;
	case InterpreterErrorFlags::expectedEndIfFlag:
		outputStream << "Expected \"endif\" command at line " << currentLine << "!\n";
		break;
	case InterpreterErrorFlags::expectedEndWhileFlag:
		outputStream << "Expected \"endwhile\" command at line " << currentLine << "!\n";
		break;
	case InterpreterErrorFlags::expectedEndRecdefFlag:
		outputStream << "Expected \"endrecdef\" command at line " << currentLine << "!\n";
		break;
	case InterpreterErrorFlags::divisionByZeroFlag:
		outputStream << "Division by zero occured!\n";
		break;
	case InterpreterErrorFlags::invalidInputFlag:
		outputStream << "The given input is invalid!\n";
		break;
	case InterpreterErrorFlags::undefinedVariableFlag:
		outputStream << "Variable " << undefinedObjectName << " is indefined!\n";
		break;
	case InterpreterErrorFlags::undefinedFunctionFlag:
		outputStream << "Function " << undefinedObjectName << " is indefined!\n";
		break;
	case InterpreterErrorFlags::lackOfReturnValue:
		outputStream << "Function " << undefinedObjectName << " failed to return a value!\n";
		break;
	}

	stateFlag = InterpreterErrorFlags::alreadyRunFlag;
}

void Interpreter::checkSequence(Instruction& Ins, bool possibleReturn, const std::string& expectedEndLine)
{
	if (file.eof())
	{
		if (expectedEndLine.compare("")) handleLackOfEndLine(expectedEndLine);
		return;
	}

	int beginIndex, endIndex;
	std::string line;

	currentLine++;
	getline(file, line);
	beginIndex = 0;
	endIndex = line.length() - 1;
	removeSpaces(line, beginIndex, endIndex);

	if (!std::string("if").compare(line.substr(beginIndex, endIndex - beginIndex + 1))) Ins.parameters.push_back(checkIf(possibleReturn));
	else if (!std::string("while").compare(line.substr(beginIndex, endIndex - beginIndex + 1))) Ins.parameters.push_back(checkWhile(possibleReturn));
	else if (!std::string("recdef").compare(line.substr(beginIndex, endIndex - beginIndex + 1))) Ins.parameters.push_back(checkRecdef());
	else if (expectedEndLine.compare("") && !expectedEndLine.compare(line.substr(beginIndex, endIndex - beginIndex + 1))) return;
	else Ins.parameters.push_back(checkLine(possibleReturn, line, beginIndex, endIndex));

	if (stateFlag != InterpreterErrorFlags::normalStateFlag) return;
	checkSequence(Ins, possibleReturn, expectedEndLine);
}

Instruction Interpreter::checkIf(bool possibleReturn)
{
	Instruction temp(Instruction::ifStatementType);

	if (file.eof())
	{
		stateFlag = InterpreterErrorFlags::expectedEndIfFlag;
		return temp;
	}

	int beginIndex, endIndex;
	std::string line;
	
	currentLine++;
	getline(file, line);
	beginIndex = 0;
	endIndex = line.length() - 1;
	removeSpaces(line, beginIndex, endIndex);

	temp.parameters.push_back(checkCond(line, beginIndex, endIndex));
	if (stateFlag != InterpreterErrorFlags::normalStateFlag) return temp;

	if (file.eof())
	{
		stateFlag = InterpreterErrorFlags::expectedEndIfFlag;
		return temp;
	}

	currentLine++;
	getline(file, line);
	beginIndex = 0;
	endIndex = line.length() - 1;
	removeSpaces(line, beginIndex, endIndex);

	if (std::string("then").compare(line.substr(beginIndex, endIndex - beginIndex + 1)))
	{
		stateFlag = InterpreterErrorFlags::expectedThenFlag;
		return temp;
	}

	Instruction trueSequence(Instruction::sequenceType), falseSequence(Instruction::sequenceType);

	checkSequence(trueSequence, possibleReturn, std::string("else"));
	temp.parameters.push_back(trueSequence);
	if (stateFlag != InterpreterErrorFlags::normalStateFlag) return temp;

	checkSequence(falseSequence, possibleReturn, std::string("endif"));
	temp.parameters.push_back(falseSequence);

	return temp;
}

Instruction Interpreter::checkWhile(bool possibleReturn)
{
	Instruction temp(Instruction::whileStatementType);

	if (file.eof())
	{
		stateFlag = InterpreterErrorFlags::expectedEndWhileFlag;
		return temp;
	}

	int beginIndex, endIndex;
	std::string line;

	currentLine++;
	getline(file, line);
	beginIndex = 0;
	endIndex = line.length() - 1;
	removeSpaces(line, beginIndex, endIndex);

	temp.parameters.push_back(checkCond(line, beginIndex, endIndex));
	if (stateFlag != InterpreterErrorFlags::normalStateFlag) return temp;

	Instruction sequence(Instruction::sequenceType);
	checkSequence(sequence, possibleReturn, std::string("endwhile"));
	temp.parameters.push_back(sequence);

	return temp;
}

Instruction Interpreter::checkRecdef()
{
	Instruction temp(Instruction::recursiveFunctionDefinitionType);

	if (file.eof())
	{
		stateFlag = InterpreterErrorFlags::expectedEndRecdefFlag;
		return temp;
	}

	int beginIndex, endIndex, leftBracket;
	std::string line;

	currentLine++;
	getline(file, line);
	beginIndex = 0;
	endIndex = line.length() - 1;
	removeSpaces(line, beginIndex, endIndex);

	for (leftBracket = beginIndex; leftBracket <= endIndex; leftBracket++)
	{
		if (line[leftBracket] == '[') break;
	}

	if (leftBracket > endIndex || line[endIndex] != ']')
	{
		stateFlag = InterpreterErrorFlags::invalidLineFlag;
		return temp;
	}

	temp.parameters.push_back(checkFun(line, beginIndex, leftBracket - 1));
	if (stateFlag != InterpreterErrorFlags::normalStateFlag) return temp;

	temp.parameters.push_back(checkVar(line, leftBracket + 1, endIndex - 1));
	if (stateFlag != InterpreterErrorFlags::normalStateFlag) return temp;

	Instruction sequence(Instruction::sequenceType);

	checkSequence(sequence, true, std::string("endrecdef"));
	temp.parameters.push_back(sequence);

	return temp;
}

Instruction Interpreter::checkLine(bool possibleReturn, const std::string& line, int beginIndex, int endIndex)
{
	Instruction temp;

	if (beginIndex > endIndex) return temp;

	int newBegInd, newEndInd;

	if (!std::string("print ").compare(line.substr(beginIndex, 6)))
	{
		temp = Instruction(Instruction::printType);
		newBegInd = beginIndex + 6;
		newEndInd = endIndex;
		removeSpaces(line, newBegInd, newEndInd);
		temp.parameters.push_back(checkExpr(line, newBegInd, newEndInd));
		return temp;
	}

	if (!std::string("read ").compare(line.substr(beginIndex, 5)))
	{
		temp = Instruction(Instruction::readType);
		newBegInd = beginIndex + 5;
		newEndInd = endIndex;
		removeSpaces(line, newBegInd, newEndInd);
		temp.parameters.push_back(checkVar(line, newBegInd, newEndInd));
		return temp;
	}

	if (!std::string("return ").compare(line.substr(beginIndex, 7)))
	{
		if (!possibleReturn)
		{
			stateFlag = InterpreterErrorFlags::invalidReturnValueFlag;
			return temp;
		}
		temp = Instruction(Instruction::returnType);
		newBegInd = beginIndex + 7;
		newEndInd = endIndex;
		removeSpaces(line, newBegInd, newEndInd);
		temp.parameters.push_back(checkExpr(line, newBegInd, newEndInd));
		return temp;
	}

	int equalityIndex, leftBracket;

	for (equalityIndex = beginIndex; equalityIndex <= endIndex; equalityIndex++)
	{
		if (line[equalityIndex] == '=') break;
	}

	if (equalityIndex > endIndex)
	{
		stateFlag = InterpreterErrorFlags::invalidLineFlag;
		return temp;
	}

	newBegInd = beginIndex;
	newEndInd = equalityIndex - 1;
	removeSpaces(line, newBegInd, newEndInd);
	Instruction arg(checkVar(line, newBegInd, newEndInd));

	if (stateFlag == InterpreterErrorFlags::normalStateFlag)
	{
		temp = Instruction(Instruction::variableDefinitionType);
		temp.parameters.push_back(arg);
	}
	else
	{
		stateFlag = InterpreterErrorFlags::normalStateFlag;

		temp = Instruction(Instruction::functionDefinitionType);

		newBegInd = beginIndex;
		newEndInd = equalityIndex - 1;
		removeSpaces(line, newBegInd, newEndInd);

		for (leftBracket = newBegInd; leftBracket < newEndInd; leftBracket++)
		{
			if (line[leftBracket] == '[') break;
		}

		if (leftBracket >= newEndInd || line[newEndInd] != ']')
		{
			stateFlag = InterpreterErrorFlags::invalidLineFlag;
			return temp;
		}

		temp.parameters.push_back(checkFun(line, newBegInd, leftBracket - 1));
		if (stateFlag != InterpreterErrorFlags::normalStateFlag) return temp;

		temp.parameters.push_back(checkVar(line, leftBracket + 1, newEndInd - 1));
		if (stateFlag != InterpreterErrorFlags::normalStateFlag) return temp;
	}
	
	newBegInd = equalityIndex + 1;
	newEndInd = endIndex;
	removeSpaces(line, newBegInd, newEndInd);
	temp.parameters.push_back(checkExpr(line, newBegInd, newEndInd));

	return temp;
}

Instruction Interpreter::checkCond(const std::string& line, int beginIndex, int endIndex)
{
	Instruction temp;

	if (beginIndex > endIndex)
	{
		stateFlag = InterpreterErrorFlags::invalidLineFlag;
		return temp;
	}

	int newBegInd, newEndInd;

	if (!std::string("true").compare(line.substr(beginIndex, endIndex - beginIndex + 1)))
	{
		temp = Instruction(Instruction::basicBooleanType);
		temp.data = new bool(true);
		return temp;
	}
	if (!std::string("false").compare(line.substr(beginIndex, endIndex - beginIndex + 1)))
	{
		temp = Instruction(Instruction::basicBooleanType);
		temp.data = new bool(false);
		return temp;
	}

	if (line[beginIndex] == '!' && line[endIndex] == ')' && line[beginIndex + 1] == '(')
	{
		temp = Instruction(Instruction::booleanType);
		temp.data = new char('!');
		newBegInd = beginIndex + 2;
		newEndInd = endIndex - 1;
		removeSpaces(line, newBegInd, newEndInd);
		temp.parameters.push_back(checkCond(line, newBegInd, newEndInd));
		return temp;
	}

	int operationIndex, bracketBalance = 0;

	if (line[beginIndex] == '(' && line[endIndex] == ')')
	{
		temp = Instruction(Instruction::booleanType);

		beginIndex++;
		endIndex--;
		for (operationIndex = endIndex; operationIndex >= beginIndex; operationIndex--)
		{
			if (line[operationIndex] == ')' || line[operationIndex] == ']') bracketBalance++;
			else if (line[operationIndex] == '(' || line[operationIndex] == '[') bracketBalance--;

			if (bracketBalance == 0 && (line[operationIndex] == '&' || line[operationIndex] == '|' || line[operationIndex] == '<' || line[operationIndex] == '>' || line[operationIndex] == '=')) break;
		}

		if (operationIndex < beginIndex)
		{
			stateFlag = InterpreterErrorFlags::invalidLineFlag;
			return temp;
		}
		temp.data = new char(line[operationIndex]);

		if (line[operationIndex] == '<' || line[operationIndex] == '>' || line[operationIndex] == '=')
		{
			newBegInd = beginIndex;
			if (line[operationIndex] == '<' || line[operationIndex] == '>') newEndInd = operationIndex - 1;
			else if (operationIndex > beginIndex && line[operationIndex] == '=' && line[operationIndex - 1] == '=') newEndInd = operationIndex - 2;
			else
			{
				stateFlag = InterpreterErrorFlags::invalidLineFlag;
				return temp;
			}
			removeSpaces(line, newBegInd, newEndInd);
			temp.parameters.push_back(checkExpr(line, newBegInd, newEndInd));
			if (stateFlag != InterpreterErrorFlags::normalStateFlag) return temp;

			newBegInd = operationIndex + 1;
			newEndInd = endIndex;
			removeSpaces(line, newBegInd, newEndInd);
			temp.parameters.push_back(checkExpr(line, newBegInd, newEndInd));

			return temp;
		}
		else
		{
			newBegInd = beginIndex;
			if (operationIndex > beginIndex && ((line[operationIndex] == '&' && line[operationIndex - 1] == '&') || (line[operationIndex] == '|' && line[operationIndex - 1] == '|'))) newEndInd = operationIndex - 2;
			else
			{
				stateFlag = InterpreterErrorFlags::invalidLineFlag;
				return temp;
			}
			removeSpaces(line, newBegInd, newEndInd);
			temp.parameters.push_back(checkCond(line, newBegInd, newEndInd));
			if (stateFlag != InterpreterErrorFlags::normalStateFlag) return temp;

			newBegInd = operationIndex + 1;
			newEndInd = endIndex;
			removeSpaces(line, newBegInd, newEndInd);
			temp.parameters.push_back(checkCond(line, newBegInd, newEndInd));

			return temp;
		}
	}
	else
	{
		stateFlag = InterpreterErrorFlags::invalidLineFlag;
		return temp;
	}
}

Instruction Interpreter::checkExpr(const std::string& line, int beginIndex, int endIndex)
{
	Instruction temp(Instruction::arithmeticType);

	if (beginIndex > endIndex)
	{
		stateFlag = InterpreterErrorFlags::invalidLineFlag;
		return temp;
	}

	int newBegInd, newEndInd, operationIndex, bracketBalance = 0;

	for (operationIndex = endIndex; operationIndex >= beginIndex; operationIndex--)
	{
		if (line[operationIndex] == ')' || line[operationIndex] == ']') bracketBalance++;
		else if (line[operationIndex] == '(' || line[operationIndex] == '[') bracketBalance--;

		if (bracketBalance == 0 && (line[operationIndex] == '+' || line[operationIndex] == '-')) break;
	}

	if (operationIndex < beginIndex) return checkTerm(line, beginIndex, endIndex);

	temp.data = new char(line[operationIndex]);

	newBegInd = beginIndex;
	newEndInd = operationIndex - 1;
	removeSpaces(line, newBegInd, newEndInd);
	temp.parameters.push_back(checkExpr(line, newBegInd, newEndInd));
	if (stateFlag != InterpreterErrorFlags::normalStateFlag) return temp;

	newBegInd = operationIndex + 1;
	newEndInd = endIndex;
	removeSpaces(line, newBegInd, newEndInd);
	temp.parameters.push_back(checkTerm(line, newBegInd, newEndInd));

	return temp;
}

Instruction Interpreter::checkTerm(const std::string& line, int beginIndex, int endIndex)
{
	Instruction temp(Instruction::arithmeticType);

	if (beginIndex > endIndex)
	{
		stateFlag = InterpreterErrorFlags::invalidLineFlag;
		return temp;
	}

	int newBegInd, newEndInd, operationIndex, bracketBalance = 0;

	for (operationIndex = endIndex; operationIndex >= beginIndex; operationIndex--)
	{
		if (line[operationIndex] == ')' || line[operationIndex] == ']') bracketBalance++;
		else if (line[operationIndex] == '(' || line[operationIndex] == '[') bracketBalance--;

		if (bracketBalance == 0 && (line[operationIndex] == '*' || line[operationIndex] == '/' || line[operationIndex] == '%')) break;
	}

	if (operationIndex < beginIndex)
	{
		return checkFactor(line, beginIndex, endIndex);
	}

	temp.data = new char(line[operationIndex]);

	newBegInd = beginIndex;
	newEndInd = operationIndex - 1;
	removeSpaces(line, newBegInd, newEndInd);
	temp.parameters.push_back(checkTerm(line, newBegInd, newEndInd));
	if (stateFlag != InterpreterErrorFlags::normalStateFlag) return temp;

	newBegInd = operationIndex + 1;
	newEndInd = endIndex;
	removeSpaces(line, newBegInd, newEndInd);
	temp.parameters.push_back(checkFactor(line, newBegInd, newEndInd));

	return temp;
}

Instruction Interpreter::checkFactor(const std::string& line, int beginIndex, int endIndex)
{
	Instruction temp, arg;

	if (beginIndex > endIndex)
	{
		stateFlag = InterpreterErrorFlags::invalidLineFlag;
		return temp;
	}

	int newBegInd, newEndInd;

	if (line[beginIndex] == '(' && line[endIndex] == ')')
	{
		newBegInd = beginIndex + 1;
		newEndInd = endIndex - 1;
		removeSpaces(line, newBegInd, newEndInd);
		return checkExpr(line, newBegInd, newEndInd);
	}

	int leftBracket;

	if (line[endIndex] == ']')
	{
		temp = Instruction(Instruction::functionCallType);

		for (leftBracket = beginIndex; leftBracket <= endIndex; leftBracket++)
		{
			if (line[leftBracket] == '[') break;
		}

		if (leftBracket > endIndex)
		{
			stateFlag = InterpreterErrorFlags::invalidLineFlag;
			return temp;
		}

		newBegInd = beginIndex;
		newEndInd = leftBracket - 1;
		temp.parameters.push_back(checkFun(line, newBegInd, newEndInd));
		if (stateFlag != InterpreterErrorFlags::normalStateFlag) return temp;
		
		newBegInd = leftBracket + 1;
		newEndInd = endIndex - 1;
		removeSpaces(line, newBegInd, newEndInd);
		temp.parameters.push_back(checkExpr(line, newBegInd, newEndInd));

		return temp;
	}

	arg = checkVar(line, beginIndex, endIndex);
	if (stateFlag == InterpreterErrorFlags::normalStateFlag) return arg;
	else
	{
		stateFlag = InterpreterErrorFlags::normalStateFlag;
		return checkNum(line, beginIndex, endIndex);
	}
}

Instruction Interpreter::checkFun(const std::string& line, int beginIndex, int endIndex)
{
	if (beginIndex > endIndex)
	{
		stateFlag = InterpreterErrorFlags::invalidLineFlag;
		return Instruction();
	}

	for (int i = beginIndex; i <= endIndex; i++)
	{
		if (line[i] >= 'A' && line[i] <= 'Z') continue;
		stateFlag = InterpreterErrorFlags::invalidLineFlag;
		return Instruction();
	}

	Instruction temp = Instruction(Instruction::functionNameType);
	temp.data = new std::string(line.substr(beginIndex, endIndex - beginIndex + 1));
	return temp;
}

Instruction Interpreter::checkVar(const std::string& line, int beginIndex, int endIndex)
{
	if (beginIndex > endIndex)
	{
		stateFlag = InterpreterErrorFlags::invalidLineFlag;
		return Instruction();
	}

	for (int i = beginIndex; i <= endIndex; i++)
	{
		if (line[i] >= 'a' && line[i] <= 'z') continue;
		stateFlag = InterpreterErrorFlags::invalidLineFlag;
		return Instruction();
	}

	Instruction temp = Instruction(Instruction::variableNameType);
	temp.data = new std::string(line.substr(beginIndex, endIndex - beginIndex + 1));
	return temp;
}

Instruction Interpreter::checkNum(const std::string& line, int beginIndex, int endIndex)
{
	if (beginIndex > endIndex)
	{
		stateFlag = InterpreterErrorFlags::invalidLineFlag;
		return Instruction();
	}

	for (int i = beginIndex; i <= endIndex; i++)
	{
		if (line[i] >= '0' && line[i] <= '9') continue;
		stateFlag = InterpreterErrorFlags::invalidLineFlag;
		return Instruction();
	}

	Instruction temp = Instruction(Instruction::numberType);
	temp.data = new Number(line.substr(beginIndex, endIndex - beginIndex + 1));
	return temp;
}

Interpreter::Interpreter()
{
	address = nullptr;
	stateFlag = InterpreterErrorFlags::normalStateFlag;
	currentLine = 0;
	mainSequence = Instruction(Instruction::sequenceType);
}

Interpreter::~Interpreter()
{
	if (address != nullptr) delete address;
}

void Interpreter::run(const std::string& fileAddress, std::istream& inputStream, std::ostream& outputStream)
{
	if (stateFlag != InterpreterErrorFlags::normalStateFlag)
	{
		handleErrorFlag(outputStream);
		return;
	}

	if (address != nullptr) delete address;
	address = new char[fileAddress.length() + 1];
	strcpy(address, fileAddress.c_str());

	file.open(address, std::ios::in);

	if (!file)
	{
		stateFlag = InterpreterErrorFlags::invalidAddressFlag;
		handleErrorFlag(outputStream);
		return;
	}

	currentLine = 0;
	
	checkSequence(mainSequence);
	file.close();
	if (stateFlag != InterpreterErrorFlags::normalStateFlag)
	{
		handleErrorFlag(outputStream);
		return;
	}

	DEFINITIONS definitions;
	DEFINED alreadyDefined;
	REDEFINED predefinedObjects;
	int redefined = 0;
	bool ret = false;
	Number result;

	mainSequence.execute(stateFlag, undefinedObjectName, definitions, alreadyDefined, predefinedObjects, redefined, outputStream, inputStream, ret, result);
	Instruction::undoRedefining(definitions, predefinedObjects, redefined);
	handleErrorFlag(outputStream);
}