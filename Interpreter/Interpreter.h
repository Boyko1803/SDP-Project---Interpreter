#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <stack>
#include <vector>
#include <string>
#include <map>
#include <cstring>
#include <fstream>
#include <iostream>

#include "Number.h"

struct stringCompare {
	bool operator()(const std::string& first, const std::string& second) const 
	{
		return (first.compare(second) < 0);
	}
};

class Interpreter
{
private:
	struct Instruction
	{
		/// General types:
		const static std::string defaultType;
		const static std::string sequenceType;
		const static std::string ifStatementType;
		const static std::string whileStatementType;
		const static std::string readType;
		const static std::string printType;
		const static std::string returnType;
		const static std::string booleanType;						/// Has char data
		const static std::string arithmeticType;					/// Has char data
		const static std::string basicBooleanType;					/// Has bool data
		const static std::string numberType;						/// Has Number data
		const static std::string variableNameType;					/// Has string data
		const static std::string functionNameType;					/// Has string data
		const static std::string variableDefinitionType;
		const static std::string functionDefinitionType;
		const static std::string recursiveFunctionDefinitionType;
		const static std::string functionCallType;

		/// Runtime types:
		const static std::string undefinedVariableType;				/// Has string data
		const static std::string undefinedFunctionType;				/// Has string data
		const static std::string variableReferenceType;				/// Has Instruction* data
		const static std::string functionReferenceType;				/// Has Instruction* data
		const static std::string functionReferenceCallType;


		using DEFINITIONS = std::map<std::string, Instruction*, stringCompare>;
		using DEFINED = std::map<std::string, bool, stringCompare>;
		using REDEFINED = std::stack<std::pair<std::string, Instruction*>>;

		void deleteData();
		void copyData(const Instruction&);

		static bool convertNumber(const std::string&, Number&);
		static void undoRedefining(DEFINITIONS&, REDEFINED&, int);
		static Instruction* createClosure(const Instruction& instruction, const std::string& freeVariable, DEFINITIONS& definitions, DEFINED& alreadyDefined, REDEFINED& redefinedObj, int& redefined);

		std::string type;
		std::vector<Instruction> parameters;
		void* data;

		Instruction(std::string = Interpreter::Instruction::defaultType);
		Instruction(const Instruction&);
		Instruction& operator=(const Instruction&);
		~Instruction();

		void print(std::ostream& outputStream = std::cout) const;
		void execute(char& state, std::string& undefinedObject, DEFINITIONS& definitions, DEFINED& alreadyDefined, REDEFINED& redefinedObj, int& redefined, std::ostream& os, std::istream& is, bool& returnFlag, Number& returnValue);
	};

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
	const static char invalidInputFlag = 31;
	const static char undefinedVariableFlag = 32;
	const static char undefinedFunctionFlag = 33;
	const static char lackOfReturnValue = 34;

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
};