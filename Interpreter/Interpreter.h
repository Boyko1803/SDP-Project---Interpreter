#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <string>
#include <cstring>
#include <fstream>
#include <iostream>

class Interpreter
{
private:
	/// State flags
	const static char normalStateFlag = 0;
	/// File error flags
	const static char alreadyRunFlag = 10;
	const static char invalidAddressFlag = 11;
	/// Parsing error flags
	const static char invalidLineFlag = 20;
	const static char invalidReturnValueFlag = 21;
	const static char expectedThenFlag = 22;
	const static char expectedElseFlag = 23;
	const static char expectedEndIfFlag = 24;
	const static char expectedEndWhileFlag = 25;
	const static char expectedEndRecdefFlag = 26;
	/// Execution error flags
	const static char divisionByZeroFlag = 30;
	const static char undefinedVariableFlag = 31;
	const static char undefinedFunctionFlag = 32;
	const static char lackOfReturnValue = 33;

	char* address;
	char stateFlag;
	int currentLine;
	std::ifstream file;
	
	static void removeSpaces(const std::string&, int&, int&);

	void handleLackOfEndLine(const std::string&);
	void handleErrorFlag(std::ostream&);

	void checkSequence(bool possibleReturn = false, const std::string& expectedEndLine = "");
	void checkIf(bool possibleReturn);
	void checkWhile(bool possibleReturn);
	void checkRecdef();
	void checkLine(bool possibleReturn, const std::string&, int, int);
	void checkCond(const std::string&, int, int);
	void checkExpr(const std::string&, int, int);
	void checkTerm(const std::string&, int, int);
	void checkFactor(const std::string&, int, int);
	void checkFun(const std::string&, int, int);
	void checkVar(const std::string&, int, int);
	void checkNum(const std::string&, int, int);
public:
	Interpreter();
	~Interpreter();

	void Run(const std::string&, std::istream& = std::cin, std::ostream& = std::cout);
};
