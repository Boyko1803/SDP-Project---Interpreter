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
		stateFlag = expectedElseFlag;
		return;
	}
	if (!expectedEndLine.compare("endif"))
	{
		stateFlag = expectedEndIfFlag;
		return;
	}
	if (!expectedEndLine.compare("endwhile"))
	{
		stateFlag = expectedEndWhileFlag;
		return;
	}
	if (!expectedEndLine.compare("endrecdef"))
	{
		stateFlag = expectedEndRecdefFlag;
		return;
	}
}

void Interpreter::handleErrorFlag(std::ostream& outputStream)
{
	switch (stateFlag)
	{
	case normalStateFlag:
		outputStream << "Program running normally!\n";
		return;
	case alreadyRunFlag:
		outputStream << "This interpreter has already run a program. Create a new instance to run another one!\n";
		break;
	case invalidAddressFlag:
		outputStream << "The given address is invalid!\n";
		stateFlag = normalStateFlag;
		return;
	case invalidLineFlag:
		outputStream << "There is an invalid command at line "<< currentLine <<"!\n";
		break;
	case invalidReturnValueFlag:
		outputStream << "There is an invalid return command at line " << currentLine << "! Returns commands are allowed only inside recursive definitions!\n";
		break;
	case expectedThenFlag:
		outputStream << "Expected \"then\" command at line " << currentLine << "!\n";
		break;
	case expectedElseFlag:
		outputStream << "Expected \"else\" command at line " << currentLine << "!\n";
		break;
	case expectedEndIfFlag:
		outputStream << "Expected \"endif\" command at line " << currentLine << "!\n";
		break;
	case expectedEndWhileFlag:
		outputStream << "Expected \"endwhile\" command at line " << currentLine << "!\n";
		break;
	case expectedEndRecdefFlag:
		outputStream << "Expected \"endrecdef\" command at line " << currentLine << "!\n";
		break;
	}

	stateFlag = alreadyRunFlag;
}

void Interpreter::checkSequence(bool possibleReturn, const std::string& expectedEndLine)
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

	if (!std::string("if").compare(line.substr(beginIndex, endIndex - beginIndex + 1))) checkIf(possibleReturn);
	else if (!std::string("while").compare(line.substr(beginIndex, endIndex - beginIndex + 1))) checkWhile(possibleReturn);
	else if (!std::string("recdef").compare(line.substr(beginIndex, endIndex - beginIndex + 1))) checkRecdef();
	else if (expectedEndLine.compare("") && !expectedEndLine.compare(line.substr(beginIndex, endIndex - beginIndex + 1))) return;
	else checkLine(possibleReturn, line, beginIndex, endIndex);

	if (stateFlag != normalStateFlag) return;
	checkSequence(possibleReturn, expectedEndLine);
}

void Interpreter::checkIf(bool possibleReturn)
{
	if (file.eof())
	{
		stateFlag = expectedEndIfFlag;
		return;
	}

	int beginIndex, endIndex;
	std::string line;
	
	currentLine++;
	getline(file, line);
	beginIndex = 0;
	endIndex = line.length() - 1;
	removeSpaces(line, beginIndex, endIndex);

	checkCond(line, beginIndex, endIndex);
	if (stateFlag != normalStateFlag) return;

	if (file.eof())
	{
		stateFlag = expectedEndIfFlag;
		return;
	}

	currentLine++;
	getline(file, line);
	beginIndex = 0;
	endIndex = line.length() - 1;
	removeSpaces(line, beginIndex, endIndex);

	if (std::string("then").compare(line.substr(beginIndex, endIndex - beginIndex + 1)))
	{
		stateFlag = expectedThenFlag;
		return;
	}

	checkSequence(possibleReturn, std::string("else"));
	if (stateFlag != normalStateFlag) return;

	checkSequence(possibleReturn, std::string("endif"));
}

void Interpreter::checkWhile(bool possibleReturn)
{
	if (file.eof())
	{
		stateFlag = expectedEndWhileFlag;
		return;
	}

	int beginIndex, endIndex;
	std::string line;

	currentLine++;
	getline(file, line);
	beginIndex = 0;
	endIndex = line.length() - 1;
	removeSpaces(line, beginIndex, endIndex);

	checkCond(line, beginIndex, endIndex);
	if (stateFlag != normalStateFlag) return;

	checkSequence(possibleReturn, std::string("endwhile"));
}

void Interpreter::checkRecdef()
{
	if (file.eof())
	{
		stateFlag = expectedEndRecdefFlag;
		return;
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
		stateFlag = invalidLineFlag;
		return;
	}

	checkFun(line, beginIndex, leftBracket - 1);
	if (stateFlag != normalStateFlag) return;
	checkVar(line, leftBracket + 1, endIndex - 1);
	if (stateFlag != normalStateFlag) return;

	checkSequence(true, std::string("endrecdef"));
}

void Interpreter::checkLine(bool possibleReturn, const std::string& line, int beginIndex, int endIndex)
{
	if (beginIndex > endIndex) return;

	int newBegInd, newEndInd;

	if (!std::string("print ").compare(line.substr(beginIndex, 6)))
	{
		newBegInd = beginIndex + 6;
		newEndInd = endIndex;
		removeSpaces(line, newBegInd, newEndInd);
		checkExpr(line, newBegInd, newEndInd);
		return;
	}

	if (!std::string("read ").compare(line.substr(beginIndex, 5)))
	{
		newBegInd = beginIndex + 5;
		newEndInd = endIndex;
		removeSpaces(line, newBegInd, newEndInd);
		checkVar(line, newBegInd, newEndInd);
		return;
	}

	if (!std::string("return ").compare(line.substr(beginIndex, 7)))
	{
		if (!possibleReturn)
		{
			stateFlag = invalidReturnValueFlag;
			return;
		}
		newBegInd = beginIndex + 7;
		newEndInd = endIndex;
		removeSpaces(line, newBegInd, newEndInd);
		checkExpr(line, newBegInd, newEndInd);
		return;
	}

	int equalityIndex, leftBracket;

	for (equalityIndex = beginIndex; equalityIndex <= endIndex; equalityIndex++)
	{
		if (line[equalityIndex] == '=') break;
	}

	if (equalityIndex > endIndex)
	{
		stateFlag = invalidLineFlag;
		return;
	}

	newBegInd = beginIndex;
	newEndInd = equalityIndex - 1;
	removeSpaces(line, newBegInd, newEndInd);
	checkVar(line, newBegInd, newEndInd);
	if (stateFlag != normalStateFlag)
	{
		stateFlag = normalStateFlag;

		newBegInd = beginIndex;
		newEndInd = equalityIndex - 1;
		removeSpaces(line, newBegInd, newEndInd);

		for (leftBracket = newBegInd; leftBracket < newEndInd; leftBracket++)
		{
			if (line[leftBracket] == '[') break;
		}

		if (leftBracket >= newEndInd || line[newEndInd] != ']')
		{
			stateFlag = invalidLineFlag;
			return;
		}

		checkFun(line, newBegInd, leftBracket - 1);
		if (stateFlag != normalStateFlag) return;
		checkVar(line, leftBracket + 1, newEndInd - 1);
		if (stateFlag != normalStateFlag) return;
	}
	
	newBegInd = equalityIndex + 1;
	newEndInd = endIndex;
	removeSpaces(line, newBegInd, newEndInd);
	checkExpr(line, newBegInd, newEndInd);
}

void Interpreter::checkCond(const std::string& line, int beginIndex, int endIndex)
{
	if (beginIndex > endIndex)
	{
		stateFlag = invalidLineFlag;
		return;
	}

	int newBegInd, newEndInd;

	if (!std::string("true").compare(line.substr(beginIndex, endIndex - beginIndex + 1))) return;
	if (!std::string("false").compare(line.substr(beginIndex, endIndex - beginIndex + 1))) return;

	if (line[beginIndex] == '!' && line[endIndex] == ')' && line[beginIndex + 1] == '(')
	{
		newBegInd = beginIndex + 2;
		newEndInd = endIndex - 1;
		removeSpaces(line, newBegInd, newEndInd);
		checkCond(line, newBegInd, newEndInd);
		return;
	}

	int operationIndex, bracketBalance = 0;

	if (line[beginIndex] == '(' && line[endIndex] == ')')
	{
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
			stateFlag = invalidLineFlag;
			return;
		}

		if (line[operationIndex] == '<' || line[operationIndex] == '>' || line[operationIndex] == '=')
		{
			newBegInd = operationIndex + 1;
			newEndInd = endIndex;
			removeSpaces(line, newBegInd, newEndInd);
			checkExpr(line, newBegInd, newEndInd);
			if (stateFlag != normalStateFlag) return;

			newBegInd = beginIndex;
			if (line[operationIndex] == '<' || line[operationIndex] == '>') newEndInd = operationIndex - 1;
			else if (operationIndex > beginIndex && line[operationIndex] == '=' && line[operationIndex - 1] == '=') newEndInd = operationIndex - 2;
			else
			{
				stateFlag = invalidLineFlag;
				return;
			}
			removeSpaces(line, newBegInd, newEndInd);
			checkExpr(line, newBegInd, newEndInd);
		}
		else
		{
			newBegInd = operationIndex + 1;
			newEndInd = endIndex;
			removeSpaces(line, newBegInd, newEndInd);
			checkCond(line, newBegInd, newEndInd);
			if (stateFlag != normalStateFlag) return;

			newBegInd = beginIndex;
			if (operationIndex > beginIndex && ((line[operationIndex] == '&' && line[operationIndex - 1] == '&') || (line[operationIndex] == '|' && line[operationIndex - 1] == '|'))) newEndInd = operationIndex - 2;
			else
			{
				stateFlag = invalidLineFlag;
				return;
			}
			removeSpaces(line, newBegInd, newEndInd);
			checkCond(line, newBegInd, newEndInd);
		}
	}
	else stateFlag = invalidLineFlag;
}

void Interpreter::checkExpr(const std::string& line, int beginIndex, int endIndex)
{
	if (beginIndex > endIndex)
	{
		stateFlag = invalidLineFlag;
		return;
	}

	int newBegInd, newEndInd, operationIndex, bracketBalance = 0;

	for (operationIndex = endIndex; operationIndex >= beginIndex; operationIndex--)
	{
		if (line[operationIndex] == ')' || line[operationIndex] == ']') bracketBalance++;
		else if (line[operationIndex] == '(' || line[operationIndex] == '[') bracketBalance--;

		if (bracketBalance == 0 && (line[operationIndex] == '+' || line[operationIndex] == '-')) break;
	}

	if (operationIndex < beginIndex)
	{
		checkTerm(line, beginIndex, endIndex);
		return;
	}

	newBegInd = operationIndex + 1;
	newEndInd = endIndex;
	removeSpaces(line, newBegInd, newEndInd);
	checkTerm(line, newBegInd, newEndInd);
	if (stateFlag != normalStateFlag) return;

	newBegInd = beginIndex;
	newEndInd = operationIndex - 1;
	removeSpaces(line, newBegInd, newEndInd);
	checkExpr(line, newBegInd, newEndInd);
}

void Interpreter::checkTerm(const std::string& line, int beginIndex, int endIndex)
{
	if (beginIndex > endIndex)
	{
		stateFlag = invalidLineFlag;
		return;
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
		checkFactor(line, beginIndex, endIndex);
		return;
	}

	newBegInd = operationIndex + 1;
	newEndInd = endIndex;
	removeSpaces(line, newBegInd, newEndInd);
	checkFactor(line, newBegInd, newEndInd);
	if (stateFlag != normalStateFlag) return;

	newBegInd = beginIndex;
	newEndInd = operationIndex - 1;
	removeSpaces(line, newBegInd, newEndInd);
	checkTerm(line, newBegInd, newEndInd);
}

void Interpreter::checkFactor(const std::string& line, int beginIndex, int endIndex)
{
	if (beginIndex > endIndex)
	{
		stateFlag = invalidLineFlag;
		return;
	}

	int newBegInd, newEndInd;

	if (line[beginIndex] == '(' && line[endIndex] == ')')
	{
		newBegInd = beginIndex + 1;
		newEndInd = endIndex - 1;
		removeSpaces(line, newBegInd, newEndInd);
		checkExpr(line, newBegInd, newEndInd);
		return;
	}

	int leftBracket;

	if (line[endIndex] == ']')
	{
		for (leftBracket = beginIndex; leftBracket <= endIndex; leftBracket++)
		{
			if (line[leftBracket] == '[') break;
		}

		if (leftBracket > endIndex)
		{
			stateFlag = invalidLineFlag;
			return;
		}

		newBegInd = beginIndex;
		newEndInd = leftBracket - 1;
		checkFun(line, newBegInd, newEndInd);
		if (stateFlag != normalStateFlag) return;
		
		newBegInd = leftBracket + 1;
		newEndInd = endIndex - 1;
		removeSpaces(line, newBegInd, newEndInd);
		checkExpr(line, newBegInd, newEndInd);
		return;
	}

	checkVar(line, beginIndex, endIndex);
	if (stateFlag != normalStateFlag)
	{
		stateFlag = normalStateFlag;
		checkNum(line, beginIndex, endIndex);
	}
}

void Interpreter::checkFun(const std::string& line, int beginIndex, int endIndex)
{
	if (beginIndex > endIndex)
	{
		stateFlag = invalidLineFlag;
		return;
	}

	for (int i = beginIndex; i <= endIndex; i++)
	{
		if (line[i] >= 'A' && line[i] <= 'Z') continue;
		stateFlag = invalidLineFlag;
		return;
	}
}

void Interpreter::checkVar(const std::string& line, int beginIndex, int endIndex)
{
	if (beginIndex > endIndex)
	{
		stateFlag = invalidLineFlag;
		return;
	}

	for (int i = beginIndex; i <= endIndex; i++)
	{
		if (line[i] >= 'a' && line[i] <= 'z') continue;
		stateFlag = invalidLineFlag;
		return;
	}
}

void Interpreter::checkNum(const std::string& line, int beginIndex, int endIndex)
{
	if (beginIndex > endIndex)
	{
		stateFlag = invalidLineFlag;
		return;
	}

	for (int i = beginIndex; i <= endIndex; i++)
	{
		if (line[i] >= '0' && line[i] <= '9') continue;
		stateFlag = invalidLineFlag;
		return;
	}
}

Interpreter::Interpreter()
{
	address = nullptr;
	stateFlag = normalStateFlag;
	currentLine = 0;
}

Interpreter::~Interpreter()
{
	if (address != nullptr) delete address;
}

void Interpreter::Run(const std::string& fileAddress, std::istream& inputStream, std::ostream& outputStream)
{
	if (stateFlag != normalStateFlag)
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
		stateFlag = invalidAddressFlag;
		handleErrorFlag(outputStream);
		return;
	}

	currentLine = 0;
	
	checkSequence();
	file.close();
	if (stateFlag != normalStateFlag)
	{
		handleErrorFlag(outputStream);
		return;
	}
}