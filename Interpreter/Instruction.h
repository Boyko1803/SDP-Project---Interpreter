#pragma once

#include <stack>
#include <vector>
#include <string>
#include <map>
#include <cstring>
#include <iostream>

#include "Number.h"
#include "Interpreter Error Flags.h"

struct stringCompare {
	bool operator()(const std::string& first, const std::string& second) const
	{
		return (first.compare(second) < 0);
	}
};

class Instruction;

using DEFINITIONS = std::map<std::string, Instruction*, stringCompare>;
using DEFINED = std::map<std::string, bool, stringCompare>;
using REDEFINED = std::stack<std::pair<std::string, Instruction*>>;

class Interpreter;

class Instruction
{
private:
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
	const static std::string functionActionType;

	void deleteData();
	void copyData(const Instruction&);

	static bool convertNumber(const std::string&, Number&);
	static void undoRedefining(DEFINITIONS&, REDEFINED&, int);

public:
	std::string type;
	std::vector<Instruction> parameters;
	void* data;

	Instruction(std::string = defaultType);
	Instruction(const Instruction&);
	Instruction& operator=(const Instruction&);
	~Instruction();

	void print(std::ostream& outputStream = std::cout) const;
	void execute(char& state, std::string& undefinedObject, DEFINITIONS& definitions, DEFINED& alreadyDefined, REDEFINED& redefinedObj, int& redefined, std::ostream& os, std::istream& is, bool& returnFlag, Number& returnValue);

	friend class Interpreter;
};