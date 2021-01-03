#include "Interpreter.h"

/// Definition of struct Instruction

const std::string Interpreter::Instruction::defaultType = "default";
const std::string Interpreter::Instruction::sequenceType = "sequence";
const std::string Interpreter::Instruction::ifStatementType = "if";
const std::string Interpreter::Instruction::whileStatementType = "while";
const std::string Interpreter::Instruction::readType = "read";
const std::string Interpreter::Instruction::printType = "print";
const std::string Interpreter::Instruction::returnType = "return";
const std::string Interpreter::Instruction::booleanType = "boolean";
const std::string Interpreter::Instruction::arithmeticType = "arithmetic";
const std::string Interpreter::Instruction::basicBooleanType = "basic boolean";
const std::string Interpreter::Instruction::numberType = "number";
const std::string Interpreter::Instruction::variableNameType = "variable name";
const std::string Interpreter::Instruction::functionNameType = "function name";
const std::string Interpreter::Instruction::variableDefinitionType = "variable definition";
const std::string Interpreter::Instruction::functionDefinitionType = "function definition";
const std::string Interpreter::Instruction::recursiveFunctionDefinitionType = "recursive function definition";
const std::string Interpreter::Instruction::functionCallType = "function call";

const std::string Interpreter::Instruction::undefinedVariableType = "undefined variable";
const std::string Interpreter::Instruction::undefinedFunctionType = "undefined function";
const std::string Interpreter::Instruction::variableReferenceType = "variable reference";
const std::string Interpreter::Instruction::functionReferenceType = "function reference";
const std::string Interpreter::Instruction::functionReferenceCallType = "function reference call";

void Interpreter::Instruction::deleteData()
{
	if (data != nullptr)
	{
		if (!basicBooleanType.compare(type)) delete (bool*)(data);
		else if (!booleanType.compare(type)) delete (char*)(data);
		else if (!arithmeticType.compare(type)) delete (char*)(data);
		else if (!numberType.compare(type)) delete (Number*)(data);
		else if (!variableNameType.compare(type)) delete (std::string*)(data);
		else if (!functionNameType.compare(type)) delete (std::string*)(data);
		else if (!undefinedVariableType.compare(type)) delete (std::string*)(data);
		else if (!undefinedFunctionType.compare(type)) delete (std::string*)(data);
		else if (!variableReferenceType.compare(type)) delete (Instruction**)(data);
		else if (!functionReferenceType.compare(type)) delete (Instruction**)(data);
	}
}

void Interpreter::Instruction::copyData(const Interpreter::Instruction& other)
{
	type = other.type;
	parameters = other.parameters;

	if (other.data != nullptr)
	{
		if (!basicBooleanType.compare(other.type)) data = new bool(*((bool*)(other.data)));
		else if (!booleanType.compare(other.type)) data = new char(*((char*)(other.data)));
		else if (!arithmeticType.compare(other.type)) data = new char(*((char*)(other.data)));
		else if (!numberType.compare(other.type)) data = new Number(*((Number*)(other.data)));
		else if (!variableNameType.compare(other.type)) data = new std::string(*((std::string*)(other.data)));
		else if (!functionNameType.compare(other.type)) data = new std::string(*((std::string*)(other.data)));
		else if (!undefinedVariableType.compare(other.type)) data = new std::string(*((std::string*)(other.data)));
		else if (!undefinedFunctionType.compare(other.type)) data = new std::string(*((std::string*)(other.data)));
		else if (!variableReferenceType.compare(other.type)) data = new Instruction*(*((Instruction**)(other.data)));
		else if (!functionReferenceType.compare(other.type)) data = new Instruction*(*((Instruction**)(other.data)));
	}
	else data = nullptr;
}

bool Interpreter::Instruction::convertNumber(const std::string& str, Number& num)
{
	int beginIndex = 0, endIndex = str.length() - 1;
	if (beginIndex > endIndex) return 0;

	while (beginIndex < endIndex && str[beginIndex] == ' ') beginIndex++;
	while (endIndex > beginIndex && str[endIndex] == ' ') endIndex--;

	for (int i = beginIndex; i <= endIndex; i++)
	{
		if (str[i] >= '0' && str[i] <= '9') continue;
		return 0;
	}

	num = Number(str.substr(beginIndex, endIndex - beginIndex + 1));
	return 1;
}

void Interpreter::Instruction::undoRedefining(DEFINITIONS& definitions, REDEFINED& stack, int number)
{
	for (int i = 0; i < number; i++)
	{
		if (stack.empty()) return;
		std::string name = stack.top().first;
		Instruction* ins = stack.top().second;

		if (definitions[name] != nullptr) delete definitions[name];						/// Check for undefined behaviour?
		definitions[name] = ins;

		stack.pop();
	}
}

Interpreter::Instruction* Interpreter::Instruction::createClosure(const Instruction& instruction, const std::string& freeVariable, DEFINITIONS& definitions, DEFINED& alreadyDefined, REDEFINED& redefinedObj, int& redefined)
{
	Instruction *temp, *ins;
	std::string name;
	if (!variableNameType.compare(instruction.type))
	{
		name = *((std::string*)(instruction.data));
		if (!freeVariable.compare(name))
		{
			ins = new Instruction(instruction);

			return ins;
		}
		else
		{
			if (definitions[name] == nullptr)
			{
				redefinedObj.push(make_pair(name, definitions[name]));
				alreadyDefined[name] = true;
				redefined++;
				temp = new Instruction(undefinedVariableType);
				temp->data = new std::string(name);
				definitions[name] = temp;
				
				ins = new Instruction(variableReferenceType);
				ins->data = new Instruction*(temp);
			}
			else
			{
				ins = new Instruction(variableReferenceType);
				ins->data = new Instruction*(definitions[name]);
			}
			return ins;
		}
	}
	else if (!functionNameType.compare(instruction.type))
	{
		name = *((std::string*)(instruction.data));
		if (definitions[name] == nullptr)
		{
			redefinedObj.push(make_pair(name, definitions[name]));
			alreadyDefined[name] = true;
			redefined++;

			temp = new Instruction(undefinedFunctionType);
			temp->data = new std::string(name);
			definitions[name] = temp;

			ins = new Instruction(functionReferenceType);
			ins->data = new Instruction*(temp);
		}
		else
		{
			ins = new Instruction(functionReferenceType);
			ins->data = new Instruction*(definitions[name]);
		}
		return ins;
	}
	else if (!functionCallType.compare(instruction.type))
	{
		ins = new Instruction(functionReferenceCallType);

		temp = createClosure(instruction.parameters[0], freeVariable, definitions, alreadyDefined, redefinedObj, redefined);
		ins->parameters.push_back(*temp);
		delete temp;

		temp = createClosure(instruction.parameters[1], freeVariable, definitions, alreadyDefined, redefinedObj, redefined);
		ins->parameters.push_back(*temp);
		delete temp;

		return ins;
	}
	else if (!readType.compare(instruction.type))
	{
		ins = new Instruction(instruction);

		return ins;
	}
	else if (!variableDefinitionType.compare(instruction.type))
	{
		ins = new Instruction(instruction);

		return ins;
	}
	else if (!functionDefinitionType.compare(instruction.type))
	{
		ins = new Instruction(instruction);

		return ins;
	}
	else if (!recursiveFunctionDefinitionType.compare(instruction.type))
	{
		ins = new Instruction(instruction);

		return ins;
	}
	else if (!functionReferenceType.compare(instruction.type))
	{
		ins = new Instruction(instruction);

		return ins;
	}
	else
	{
		ins = new Instruction(instruction);

		for (int i = 0; i < instruction.parameters.size(); i++)
		{
			temp = createClosure(instruction.parameters[i], freeVariable, definitions, alreadyDefined, redefinedObj, redefined);
			ins->parameters[i] = *temp;
			delete temp;
		}

		return ins;
	}
}

Interpreter::Instruction::Instruction(std::string InsType)
{
	type = InsType;
	data = nullptr;
}

Interpreter::Instruction::Instruction(const Instruction& other)
{
	copyData(other);
}

Interpreter::Instruction& Interpreter::Instruction::operator=(const Instruction& other)
{
	if (this != &other)
	{
		deleteData();
		copyData(other);
	}
	return *this;
}

Interpreter::Instruction::~Instruction()
{
	deleteData();
}

void Interpreter::Instruction::print(std::ostream& outputStream) const
{
	if (!defaultType.compare(type)) {}
	else if (!sequenceType.compare(type))
	{
		for (int i = 0; i < parameters.size(); i++)
		{
			parameters[i].print(outputStream);
			outputStream << '\n';
		}
	}
	else if (!ifStatementType.compare(type))
	{
		outputStream << "if" << '\n';
		parameters[0].print(outputStream);
		outputStream << '\n' << "then" << '\n';
		parameters[1].print(outputStream);
		outputStream << "else" << '\n';
		parameters[2].print(outputStream);
		outputStream << "endif";
	}
	else if (!whileStatementType.compare(type))
	{
		outputStream << "while" << '\n';
		parameters[0].print(outputStream);
		outputStream << '\n';
		parameters[1].print(outputStream);
		outputStream << "endwhile";
	}
	else if (!readType.compare(type))
	{
		outputStream << "read ";
		parameters[0].print(outputStream);
	}
	else if (!printType.compare(type))
	{
		outputStream << "print ";
		parameters[0].print(outputStream);
	}
	else if (!returnType.compare(type))
	{
		outputStream << "return ";
		parameters[0].print(outputStream);
	}
	else if (!booleanType.compare(type))
	{
		if (*((char*)(data)) == '!')
		{
			outputStream << "!( ";
			parameters[0].print(outputStream);
			outputStream << " )";
		}
		else
		{
			outputStream << "( ";
			parameters[0].print(outputStream);
			switch (*((char*)(data)))
			{
			case '&':
				outputStream << " && ";
				break;
			case '|':
				outputStream << " || ";
				break;
			case '<':
				outputStream << " < ";
				break;
			case '>':
				outputStream << " > ";
				break;
			case '=':
				outputStream << " == ";
				break;
			}
			parameters[1].print(outputStream);
			outputStream << " )";
		}
	}
	else if (!arithmeticType.compare(type))
	{
		outputStream << "( ";
		parameters[0].print(outputStream);
		outputStream << ' ' << *((char*)(data)) << ' ';
		parameters[1].print(outputStream);
		outputStream << " )";
	}
	else if (!basicBooleanType.compare(type))
	{
		if (*((bool*)(data))) outputStream << "true";
		else outputStream << "false";
	}
	else if (!numberType.compare(type))
	{
		outputStream << *((Number*)(data));
	}
	else if (!variableNameType.compare(type))
	{
		outputStream << *((std::string*)(data));
	}
	else if (!functionNameType.compare(type))
	{
		outputStream << *((std::string*)(data));
	}
	else if (!variableDefinitionType.compare(type))
	{
		parameters[0].print(outputStream);
		outputStream << " = ";
		parameters[1].print(outputStream);
	}
	else if (!functionDefinitionType.compare(type))
	{
		parameters[0].print(outputStream);
		outputStream << '[';
		parameters[1].print(outputStream);
		outputStream << "] = ";
		parameters[2].print(outputStream);
	}
	else if (!recursiveFunctionDefinitionType.compare(type))
	{
		outputStream << "recdef" << '\n';
		parameters[0].print(outputStream);
		outputStream << '[';
		parameters[1].print(outputStream);
		outputStream << ']' << '\n';
		parameters[2].print(outputStream);
		outputStream << "endrecdef";
	}
	else if (!functionCallType.compare(type))
	{
		parameters[0].print(outputStream);
		outputStream << '[';
		parameters[1].print(outputStream);
		outputStream << ']';
	}
}

void Interpreter::Instruction::execute(char& state, std::string& undefinedObject, DEFINITIONS& definitions, DEFINED& alreadyDefined, REDEFINED& redefinedObj, int& redefined, std::ostream& os, std::istream& is, bool& returnFlag, Number& returnValue)
{
	if (!defaultType.compare(type)) return;
	else if (!sequenceType.compare(type))
	{
		for (int i = 0; i < parameters.size(); i++)
		{
			parameters[i].execute(state, undefinedObject, definitions, alreadyDefined, redefinedObj, redefined, os, is, returnFlag, returnValue);
			if (state != normalStateFlag || returnFlag) return;
		}
	}
	else if (!ifStatementType.compare(type))
	{
		Number cond;
		bool ret = false;
		parameters[0].execute(state, undefinedObject, definitions, alreadyDefined, redefinedObj, redefined, os, is, ret, cond);
		if (state != normalStateFlag) return;
		if (cond)
		{
			parameters[1].execute(state, undefinedObject, definitions, alreadyDefined, redefinedObj, redefined, os, is, returnFlag, returnValue);
		}
		else
		{
			parameters[2].execute(state, undefinedObject, definitions, alreadyDefined, redefinedObj, redefined, os, is, returnFlag, returnValue);
		}
	}
	else if (!whileStatementType.compare(type))
	{
		Number cond;
		bool ret = false;
		parameters[0].execute(state, undefinedObject, definitions, alreadyDefined, redefinedObj, redefined, os, is, ret, cond);
		if (state != normalStateFlag) return;
		while (cond)
		{
			parameters[1].execute(state, undefinedObject, definitions, alreadyDefined, redefinedObj, redefined, os, is, returnFlag, returnValue);
			if (state != normalStateFlag || returnFlag) return;
			cond = Number(0);
			ret = false;
			parameters[0].execute(state, undefinedObject, definitions, alreadyDefined, redefinedObj, redefined, os, is, ret, cond);
			if (state != normalStateFlag) return;
		}
	}
	else if (!readType.compare(type))
	{
		os << "> ";
		bool isNumber;
		Number num;
		std::string input, name = *((std::string*)(parameters[0].data));
		is >> input;
		isNumber = convertNumber(input, num);
		if (isNumber)
		{
			Instruction* ins = new Instruction(numberType);
			ins->data = new Number(num);
			if (alreadyDefined[name])
			{
				*(definitions[name]) = *ins;
				delete ins;
			}
			else
			{
				redefinedObj.push(make_pair(name, definitions[name]));
				definitions[name] = ins;
				alreadyDefined[name] = true;
				redefined++;
			}
		}
		else
		{
			state = invalidInputFlag;
			return;
		}
	}
	else if (!printType.compare(type))
	{
		Number result;
		bool ret = false;
		parameters[0].execute(state, undefinedObject, definitions, alreadyDefined, redefinedObj, redefined, os, is, ret, result);
		if (state != normalStateFlag) return;
		os << result << '\n';
	}
	else if (!returnType.compare(type))
	{
		Number result;
		bool ret = false;
		parameters[0].execute(state, undefinedObject, definitions, alreadyDefined, redefinedObj, redefined, os, is, ret, result);
		if (state != normalStateFlag) return;
		if (ret)
		{
			returnFlag = true;
			returnValue = result;
		}
		else
		{
			state = lackOfReturnValue;
			return;
		}
	}
	else if (!booleanType.compare(type))
	{
		Number first, second;
		bool ret;
		char op = *((char*)(data));
		ret = false;
		parameters[0].execute(state, undefinedObject, definitions, alreadyDefined, redefinedObj, redefined, os, is, ret, first);
		if (state != normalStateFlag) return;

		if (op == '!')
		{
			returnFlag = true;
			if (first) returnValue = Number(0);
			else returnValue = Number(1);
			return;
		}
		else if (op == '|' && first)
		{
			returnFlag = true;
			returnValue = Number(1);
			return;
		}
		else if (op == '&' && !first)
		{
			returnFlag = true;
			returnValue = Number(0);
			return;
		}

		ret = false;
		parameters[1].execute(state, undefinedObject, definitions, alreadyDefined, redefinedObj, redefined, os, is, ret, second);
		if (state != normalStateFlag) return;

		returnFlag = true;
		switch (op)
		{
		case '|':
			if (first || second) returnValue = Number(1);
			else returnValue = Number(0);
			break;
		case '&':
			if (first && second) returnValue = Number(1);
			else returnValue = Number(0);
			break;
		case '<':
			if (first < second) returnValue = Number(1);
			else returnValue = Number(0);
			break;
		case '>':
			if (first > second) returnValue = Number(1);
			else returnValue = Number(0);
			break;
		case '=':
			if (first == second) returnValue = Number(1);
			else returnValue = Number(0);
			break;
		}
	}
	else if (!arithmeticType.compare(type))
	{
		Number first, second;
		bool ret;
		char op = *((char*)(data));

		ret = false;
		parameters[0].execute(state, undefinedObject, definitions, alreadyDefined, redefinedObj, redefined, os, is, ret, first);
		if (state != normalStateFlag) return;

		ret = false;
		parameters[1].execute(state, undefinedObject, definitions, alreadyDefined, redefinedObj, redefined, os, is, ret, second);
		if (state != normalStateFlag) return;

		if ((op == '/' || op == '%') && second == Number(0))
		{
			state = divisionByZeroFlag;
			return;
		}

		returnFlag = true;
		switch (op)
		{
		case '+':
			returnValue = first + second;
			break;
		case '-':
			returnValue = first - second;
			break;
		case '*':
			returnValue = first * second;
			break;
		case '/':
			returnValue = first / second;
			break;
		case '%':
			returnValue = first % second;
			break;
		}
	}
	else if (!basicBooleanType.compare(type))
	{
		bool value = *((bool*)(data));
		returnFlag = true;
		if (value) returnValue = Number(1);
		else returnValue = Number(0);
	}
	else if (!numberType.compare(type))
	{
		returnFlag = true;
		returnValue = *((Number*)(data));
	}
	else if (!variableNameType.compare(type))
	{
		Number result;
		bool ret;
		std::string name = *((std::string*)(data));
		if (definitions[name] == nullptr)
		{
			state = undefinedVariableFlag;
			undefinedObject = name;
			return;
		}
		else
		{
			ret = false;
			definitions[name]->execute(state, undefinedObject, definitions, alreadyDefined, redefinedObj, redefined, os, is, ret, result);
			if (state != normalStateFlag) return;
			returnFlag = true;
			returnValue = result;
		}
	}
	else if (!functionNameType.compare(type)) return;
	else if (!variableDefinitionType.compare(type))
	{
		Number result;
		bool ret;
		std::string variableName = *((std::string*)(parameters[0].data));
		ret = false;
		parameters[1].execute(state, undefinedObject, definitions, alreadyDefined, redefinedObj, redefined, os, is, ret, result);
		if (state != normalStateFlag) return;

		Instruction* ins = new Instruction(numberType);
		ins->data = new Number(result);
		if (alreadyDefined[variableName])
		{
			*(definitions[variableName]) = *ins;
			delete ins;
		}
		else
		{
			redefinedObj.push(make_pair(variableName, definitions[variableName]));
			definitions[variableName] = ins;
			alreadyDefined[variableName] = true;
			redefined++;
		}
	}
	else if (!functionDefinitionType.compare(type))
	{
		Instruction *temp, *ins;
		std::string functionName = *((std::string*)(parameters[0].data));
		std::string variableName = *((std::string*)(parameters[1].data));
		temp = createClosure(parameters[2], variableName, definitions, alreadyDefined, redefinedObj, redefined);

		ins = new Instruction(functionReferenceType);
		ins->parameters.push_back(parameters[1]);
		ins->parameters.push_back(*temp);
		delete temp;

		if (alreadyDefined[functionName])
		{
			*(definitions[functionName]) = *ins;
			delete ins;
		}
		else
		{
			redefinedObj.push(make_pair(functionName, definitions[functionName]));
			definitions[functionName] = ins;
			alreadyDefined[functionName] = true;
			redefined++;
		}
	}
	else if (!recursiveFunctionDefinitionType.compare(type))
	{
		Instruction *temp, *ins;
		std::string functionName = *((std::string*)(parameters[0].data));
		std::string variableName = *((std::string*)(parameters[1].data));
		temp = createClosure(parameters[2], variableName, definitions, alreadyDefined, redefinedObj, redefined);

		ins = new Instruction(functionReferenceType);
		ins->parameters.push_back(parameters[1]);
		ins->parameters.push_back(*temp);
		delete temp;

		if (alreadyDefined[functionName])
		{
			*(definitions[functionName]) = *ins;
			delete ins;
		}
		else
		{
			redefinedObj.push(make_pair(functionName, definitions[functionName]));
			definitions[functionName] = ins;
			alreadyDefined[functionName] = true;
			redefined++;
		}
	}
	else if (!functionCallType.compare(type))
	{
		Number result;
		bool ret;
		std::string variableName, functionName = *((std::string*)(parameters[0].data));
		
		if (definitions[functionName] == nullptr)
		{
			state = undefinedFunctionFlag;
			return;
		}
		else if (!undefinedFunctionType.compare(definitions[functionName]->type))
		{
			state = undefinedFunctionFlag;
			undefinedObject = functionName;
			return;
		}
		else
		{
			ret = false;
			parameters[1].execute(state, undefinedObject, definitions, alreadyDefined, redefinedObj, redefined, os, is, ret, result);
			if (state != normalStateFlag) return;

			Instruction* ins = new Instruction(numberType);
			ins->data = new Number(result);

			int newRedefined = 0;
			DEFINED newDefined;
			variableName = *((std::string*)(definitions[functionName]->parameters[0].data));
			redefinedObj.push(make_pair(variableName, definitions[variableName]));
			definitions[variableName] = ins;
			newDefined[variableName] = true;
			newRedefined++;

			ret = false;
			definitions[functionName]->parameters[1].execute(state, undefinedObject, definitions, newDefined, redefinedObj, newRedefined, os, is, ret, result);
			undoRedefining(definitions, redefinedObj, newRedefined);
			if (state != normalStateFlag) return;

			if (ret)
			{
				returnFlag = true;
				returnValue = result;
			}
			else
			{
				state = lackOfReturnValue;
				undefinedObject = functionName;
				return;
			}
		}
	}
	else if (!undefinedVariableType.compare(type))
	{
		state = undefinedVariableFlag;
		undefinedObject = *((std::string*)(data));
	}
	else if (!undefinedFunctionType.compare(type)) return;
	else if (!variableReferenceType.compare(type))
	{
		Number result;
		bool ret = false;
		(*((Instruction**)(data)))->execute(state, undefinedObject, definitions, alreadyDefined, redefinedObj, redefined, os, is, ret, result);
		if (state != normalStateFlag) return;
		returnFlag = true;
		returnValue = result;
	}
	else if (!functionReferenceType.compare(type)) return;
	else if (!functionReferenceCallType.compare(type))
	{
		Number result;
		bool ret;
		Instruction* functionReference = *((Instruction**)(parameters[0].data));
		std::string variableName;

		if (!undefinedFunctionType.compare(functionReference->type))
		{
			state = undefinedFunctionFlag;
			undefinedObject = *((std::string*)(functionReference->data));
			return;
		}
		else
		{
			ret = false;
			parameters[1].execute(state, undefinedObject, definitions, alreadyDefined, redefinedObj, redefined, os, is, ret, result);
			if (state != normalStateFlag) return;

			Instruction* ins = new Instruction(numberType);
			ins->data = new Number(result);

			int newRedefined = 0;
			DEFINED newDefined;
			variableName = *((std::string*)(functionReference->parameters[0].data));
			redefinedObj.push(make_pair(variableName, definitions[variableName]));
			definitions[variableName] = ins;
			newDefined[variableName] = true;
			newRedefined++;

			ret = false;
			functionReference->parameters[1].execute(state, undefinedObject, definitions, newDefined, redefinedObj, newRedefined, os, is, ret, result);
			undoRedefining(definitions, redefinedObj, newRedefined);
			if (state != normalStateFlag) return;

			if (ret)
			{
				returnFlag = true;
				returnValue = result;
			}
			else
			{
				state = lackOfReturnValue;
				return;
			}
		}
	}
}

/// Definition of class Interpreter

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
		outputStream << "The program ended successfully!\n";
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
	case divisionByZeroFlag:
		outputStream << "Division by zero occured!\n";
		break;
	case invalidInputFlag:
		outputStream << "The given input is invalid!\n";
		break;
	case undefinedVariableFlag:
		outputStream << "Variable " << undefinedObjectName << " is indefined!\n";
		break;
	case undefinedFunctionFlag:
		outputStream << "Function " << undefinedObjectName << " is indefined!\n";
		break;
	case lackOfReturnValue:
		outputStream << "A function failed to return a value!\n";
		break;
	}

	stateFlag = alreadyRunFlag;
}

void Interpreter::checkSequence(Interpreter::Instruction& Ins, bool possibleReturn, const std::string& expectedEndLine)
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

	if (stateFlag != normalStateFlag) return;
	checkSequence(Ins, possibleReturn, expectedEndLine);
}

Interpreter::Instruction Interpreter::checkIf(bool possibleReturn)
{
	Instruction temp(Instruction::ifStatementType);

	if (file.eof())
	{
		stateFlag = expectedEndIfFlag;
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
	if (stateFlag != normalStateFlag) return temp;

	if (file.eof())
	{
		stateFlag = expectedEndIfFlag;
		return temp;
	}

	currentLine++;
	getline(file, line);
	beginIndex = 0;
	endIndex = line.length() - 1;
	removeSpaces(line, beginIndex, endIndex);

	if (std::string("then").compare(line.substr(beginIndex, endIndex - beginIndex + 1)))
	{
		stateFlag = expectedThenFlag;
		return temp;
	}

	Instruction trueSequence(Instruction::sequenceType), falseSequence(Instruction::sequenceType);

	checkSequence(trueSequence, possibleReturn, std::string("else"));
	temp.parameters.push_back(trueSequence);
	if (stateFlag != normalStateFlag) return temp;

	checkSequence(falseSequence, possibleReturn, std::string("endif"));
	temp.parameters.push_back(falseSequence);

	return temp;
}

Interpreter::Instruction Interpreter::checkWhile(bool possibleReturn)
{
	Instruction temp(Instruction::whileStatementType);

	if (file.eof())
	{
		stateFlag = expectedEndWhileFlag;
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
	if (stateFlag != normalStateFlag) return temp;

	Instruction sequence(Instruction::sequenceType);
	checkSequence(sequence, possibleReturn, std::string("endwhile"));
	temp.parameters.push_back(sequence);

	return temp;
}

Interpreter::Instruction Interpreter::checkRecdef()
{
	Instruction temp(Instruction::recursiveFunctionDefinitionType);

	if (file.eof())
	{
		stateFlag = expectedEndRecdefFlag;
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
		stateFlag = invalidLineFlag;
		return temp;
	}

	temp.parameters.push_back(checkFun(line, beginIndex, leftBracket - 1));
	if (stateFlag != normalStateFlag) return temp;

	temp.parameters.push_back(checkVar(line, leftBracket + 1, endIndex - 1));
	if (stateFlag != normalStateFlag) return temp;

	Instruction sequence(Instruction::sequenceType);

	checkSequence(sequence, true, std::string("endrecdef"));
	temp.parameters.push_back(sequence);

	return temp;
}

Interpreter::Instruction Interpreter::checkLine(bool possibleReturn, const std::string& line, int beginIndex, int endIndex)
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
			stateFlag = invalidReturnValueFlag;
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
		stateFlag = invalidLineFlag;
		return temp;
	}

	newBegInd = beginIndex;
	newEndInd = equalityIndex - 1;
	removeSpaces(line, newBegInd, newEndInd);
	Instruction arg(checkVar(line, newBegInd, newEndInd));

	if (stateFlag == normalStateFlag)
	{
		temp = Instruction(Instruction::variableDefinitionType);
		temp.parameters.push_back(arg);
	}
	else
	{
		stateFlag = normalStateFlag;

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
			stateFlag = invalidLineFlag;
			return temp;
		}

		temp.parameters.push_back(checkFun(line, newBegInd, leftBracket - 1));
		if (stateFlag != normalStateFlag) return temp;

		temp.parameters.push_back(checkVar(line, leftBracket + 1, newEndInd - 1));
		if (stateFlag != normalStateFlag) return temp;
	}
	
	newBegInd = equalityIndex + 1;
	newEndInd = endIndex;
	removeSpaces(line, newBegInd, newEndInd);
	temp.parameters.push_back(checkExpr(line, newBegInd, newEndInd));

	return temp;
}

Interpreter::Instruction Interpreter::checkCond(const std::string& line, int beginIndex, int endIndex)
{
	Instruction temp;

	if (beginIndex > endIndex)
	{
		stateFlag = invalidLineFlag;
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
			stateFlag = invalidLineFlag;
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
				stateFlag = invalidLineFlag;
				return temp;
			}
			removeSpaces(line, newBegInd, newEndInd);
			temp.parameters.push_back(checkExpr(line, newBegInd, newEndInd));
			if (stateFlag != normalStateFlag) return temp;

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
				stateFlag = invalidLineFlag;
				return temp;
			}
			removeSpaces(line, newBegInd, newEndInd);
			temp.parameters.push_back(checkCond(line, newBegInd, newEndInd));
			if (stateFlag != normalStateFlag) return temp;

			newBegInd = operationIndex + 1;
			newEndInd = endIndex;
			removeSpaces(line, newBegInd, newEndInd);
			temp.parameters.push_back(checkCond(line, newBegInd, newEndInd));

			return temp;
		}
	}
	else
	{
		stateFlag = invalidLineFlag;
		return temp;
	}
}

Interpreter::Instruction Interpreter::checkExpr(const std::string& line, int beginIndex, int endIndex)
{
	Instruction temp(Instruction::arithmeticType);

	if (beginIndex > endIndex)
	{
		stateFlag = invalidLineFlag;
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
	if (stateFlag != normalStateFlag) return temp;

	newBegInd = operationIndex + 1;
	newEndInd = endIndex;
	removeSpaces(line, newBegInd, newEndInd);
	temp.parameters.push_back(checkTerm(line, newBegInd, newEndInd));

	return temp;
}

Interpreter::Instruction Interpreter::checkTerm(const std::string& line, int beginIndex, int endIndex)
{
	Instruction temp(Instruction::arithmeticType);

	if (beginIndex > endIndex)
	{
		stateFlag = invalidLineFlag;
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
	if (stateFlag != normalStateFlag) return temp;

	newBegInd = operationIndex + 1;
	newEndInd = endIndex;
	removeSpaces(line, newBegInd, newEndInd);
	temp.parameters.push_back(checkFactor(line, newBegInd, newEndInd));

	return temp;
}

Interpreter::Instruction Interpreter::checkFactor(const std::string& line, int beginIndex, int endIndex)
{
	Instruction temp, arg;

	if (beginIndex > endIndex)
	{
		stateFlag = invalidLineFlag;
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
			stateFlag = invalidLineFlag;
			return temp;
		}

		newBegInd = beginIndex;
		newEndInd = leftBracket - 1;
		temp.parameters.push_back(checkFun(line, newBegInd, newEndInd));
		if (stateFlag != normalStateFlag) return temp;
		
		newBegInd = leftBracket + 1;
		newEndInd = endIndex - 1;
		removeSpaces(line, newBegInd, newEndInd);
		temp.parameters.push_back(checkExpr(line, newBegInd, newEndInd));

		return temp;
	}

	arg = checkVar(line, beginIndex, endIndex);
	if (stateFlag == normalStateFlag) return arg;
	else
	{
		stateFlag = normalStateFlag;
		return checkNum(line, beginIndex, endIndex);
	}
}

Interpreter::Instruction Interpreter::checkFun(const std::string& line, int beginIndex, int endIndex)
{
	if (beginIndex > endIndex)
	{
		stateFlag = invalidLineFlag;
		return Instruction();
	}

	for (int i = beginIndex; i <= endIndex; i++)
	{
		if (line[i] >= 'A' && line[i] <= 'Z') continue;
		stateFlag = invalidLineFlag;
		return Instruction();
	}

	Instruction temp = Instruction(Instruction::functionNameType);
	temp.data = new std::string(line.substr(beginIndex, endIndex - beginIndex + 1));
	return temp;
}

Interpreter::Instruction Interpreter::checkVar(const std::string& line, int beginIndex, int endIndex)
{
	if (beginIndex > endIndex)
	{
		stateFlag = invalidLineFlag;
		return Instruction();
	}

	for (int i = beginIndex; i <= endIndex; i++)
	{
		if (line[i] >= 'a' && line[i] <= 'z') continue;
		stateFlag = invalidLineFlag;
		return Instruction();
	}

	Instruction temp = Instruction(Instruction::variableNameType);
	temp.data = new std::string(line.substr(beginIndex, endIndex - beginIndex + 1));
	return temp;
}

Interpreter::Instruction Interpreter::checkNum(const std::string& line, int beginIndex, int endIndex)
{
	if (beginIndex > endIndex)
	{
		stateFlag = invalidLineFlag;
		return Instruction();
	}

	for (int i = beginIndex; i <= endIndex; i++)
	{
		if (line[i] >= '0' && line[i] <= '9') continue;
		stateFlag = invalidLineFlag;
		return Instruction();
	}

	Instruction temp = Instruction(Instruction::numberType);
	temp.data = new Number(line.substr(beginIndex, endIndex - beginIndex + 1));
	return temp;
}

Interpreter::Interpreter()
{
	address = nullptr;
	stateFlag = normalStateFlag;
	currentLine = 0;
	mainSequence = Instruction(Instruction::sequenceType);
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
	
	checkSequence(mainSequence);
	file.close();
	if (stateFlag != normalStateFlag)
	{
		handleErrorFlag(outputStream);
		return;
	}

	Instruction::DEFINITIONS definitions;
	Instruction::DEFINED alreadyDefined;
	Instruction::REDEFINED predefinedObjects;
	int redefined = 0;
	bool ret = false;
	Number result;

	mainSequence.execute(stateFlag, undefinedObjectName, definitions, alreadyDefined, predefinedObjects, redefined, outputStream, inputStream, ret, result);
	Instruction::undoRedefining(definitions, predefinedObjects, redefined);
	handleErrorFlag(outputStream);
}