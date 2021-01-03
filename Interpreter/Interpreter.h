#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <fstream>

#include "Instruction.h"
#include "Interpreter Error Flags.h"

class Interpreter
{
private:
	char* address;
	char stateFlag;
	int currentLine;
	std::string undefinedObjectName;
	std::ifstream file;

	Instruction mainSequence;
	
	static void removeSpaces(const std::string&, int&, int&);

	void handleLackOfEndLine(const std::string&);
	void handleErrorFlag(std::ostream&);

	void checkSequence(Instruction&, bool possibleReturn = false, const std::string& expectedEndLine = "");
	Instruction checkIf(bool possibleReturn);
	Instruction checkWhile(bool possibleReturn);
	Instruction checkRecdef();
	Instruction checkLine(bool possibleReturn, const std::string&, int, int);
	Instruction checkCond(const std::string&, int, int);
	Instruction checkExpr(const std::string&, int, int);
	Instruction checkTerm(const std::string&, int, int);
	Instruction checkFactor(const std::string&, int, int);
	Instruction checkFun(const std::string&, int, int);
	Instruction checkVar(const std::string&, int, int);
	Instruction checkNum(const std::string&, int, int);
public:
	Interpreter();
	~Interpreter();

	void Run(const std::string&, std::istream& = std::cin, std::ostream& = std::cout);

	friend class Instruction;
};