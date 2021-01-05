#include "Instruction.h"

const std::string Instruction::defaultType = "default";
const std::string Instruction::sequenceType = "sequence";
const std::string Instruction::ifStatementType = "if";
const std::string Instruction::whileStatementType = "while";
const std::string Instruction::readType = "read";
const std::string Instruction::printType = "print";
const std::string Instruction::returnType = "return";
const std::string Instruction::booleanType = "boolean";
const std::string Instruction::arithmeticType = "arithmetic";
const std::string Instruction::basicBooleanType = "basic boolean";
const std::string Instruction::numberType = "number";
const std::string Instruction::variableNameType = "variable name";
const std::string Instruction::functionNameType = "function name";
const std::string Instruction::variableDefinitionType = "variable definition";
const std::string Instruction::functionDefinitionType = "function definition";
const std::string Instruction::recursiveFunctionDefinitionType = "recursive function definition";
const std::string Instruction::functionCallType = "function call";

const std::string Instruction::functionActionType = "function action";

void Instruction::deleteData()
{
	if (data != nullptr)
	{
		if (!basicBooleanType.compare(type)) delete (bool*)(data);
		else if (!booleanType.compare(type)) delete (char*)(data);
		else if (!arithmeticType.compare(type)) delete (char*)(data);
		else if (!numberType.compare(type)) delete (Number*)(data);
		else if (!variableNameType.compare(type)) delete (std::string*)(data);
		else if (!functionNameType.compare(type)) delete (std::string*)(data);
	}
}

void Instruction::copyData(const Instruction& other)
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
	}
	else data = nullptr;
}

bool Instruction::convertNumber(const std::string& str, Number& num)
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

void Instruction::undoRedefining(DEFINITIONS& definitions, REDEFINED& stack, int number)
{
	for (int i = 0; i < number; i++)
	{
		if (stack.empty()) return;
		std::string name = stack.top().first;

		definitions[name] = stack.top().second;

		stack.pop();
	}
}

Instruction::Instruction(std::string InsType)
{
	type = InsType;
	data = nullptr;
}

Instruction::Instruction(const Instruction& other)
{
	copyData(other);
}

Instruction& Instruction::operator=(const Instruction& other)
{
	if (this != &other)
	{
		deleteData();
		copyData(other);
	}
	return *this;
}

Instruction::~Instruction()
{
	deleteData();
}

void Instruction::print(std::ostream& outputStream) const
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

void Instruction::execute(char& state, std::string& undefinedObject, DEFINITIONS& definitions, DEFINED& alreadyDefined, REDEFINED& redefinedObj, int& redefined, std::ostream& os, std::istream& is, bool& returnFlag, Number& returnValue)
{
	if (!defaultType.compare(type)) return;
	else if (!sequenceType.compare(type))
	{
		for (int i = 0; i < parameters.size(); i++)
		{
			parameters[i].execute(state, undefinedObject, definitions, alreadyDefined, redefinedObj, redefined, os, is, returnFlag, returnValue);
			if (state != InterpreterErrorFlags::normalStateFlag || returnFlag) return;
		}
	}
	else if (!ifStatementType.compare(type))
	{
		Number cond;
		bool ret = false;
		parameters[0].execute(state, undefinedObject, definitions, alreadyDefined, redefinedObj, redefined, os, is, ret, cond);
		if (state != InterpreterErrorFlags::normalStateFlag) return;
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
		if (state != InterpreterErrorFlags::normalStateFlag) return;
		while (cond)
		{
			parameters[1].execute(state, undefinedObject, definitions, alreadyDefined, redefinedObj, redefined, os, is, returnFlag, returnValue);
			if (state != InterpreterErrorFlags::normalStateFlag || returnFlag) return;
			cond = Number(0);
			ret = false;
			parameters[0].execute(state, undefinedObject, definitions, alreadyDefined, redefinedObj, redefined, os, is, ret, cond);
			if (state != InterpreterErrorFlags::normalStateFlag) return;
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
			Instruction ins(numberType);
			ins.data = new Number(num);
			if (alreadyDefined[name])
			{
				definitions[name] = ins;
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
			state = InterpreterErrorFlags::invalidInputFlag;
			return;
		}
	}
	else if (!printType.compare(type))
	{
		Number result;
		bool ret = false;
		parameters[0].execute(state, undefinedObject, definitions, alreadyDefined, redefinedObj, redefined, os, is, ret, result);
		if (state != InterpreterErrorFlags::normalStateFlag) return;
		os << result << '\n';
	}
	else if (!returnType.compare(type))
	{
		Number result;
		bool ret = false;
		parameters[0].execute(state, undefinedObject, definitions, alreadyDefined, redefinedObj, redefined, os, is, ret, result);
		if (state != InterpreterErrorFlags::normalStateFlag) return;
		if (ret)
		{
			returnFlag = true;
			returnValue = result;
		}
		else
		{
			state = InterpreterErrorFlags::lackOfReturnValue;
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
		if (state != InterpreterErrorFlags::normalStateFlag) return;

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
		if (state != InterpreterErrorFlags::normalStateFlag) return;

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
		if (state != InterpreterErrorFlags::normalStateFlag) return;

		ret = false;
		parameters[1].execute(state, undefinedObject, definitions, alreadyDefined, redefinedObj, redefined, os, is, ret, second);
		if (state != InterpreterErrorFlags::normalStateFlag) return;

		if ((op == '/' || op == '%') && second == Number(0))
		{
			state = InterpreterErrorFlags::divisionByZeroFlag;
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
		if (!defaultType.compare(definitions[name].type))
		{
			state = InterpreterErrorFlags::undefinedVariableFlag;
			undefinedObject = name;
			return;
		}
		else
		{
			ret = false;
			definitions[name].execute(state, undefinedObject, definitions, alreadyDefined, redefinedObj, redefined, os, is, ret, result);
			if (state != InterpreterErrorFlags::normalStateFlag) return;
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
		if (state != InterpreterErrorFlags::normalStateFlag) return;

		Instruction ins(numberType);
		ins.data = new Number(result);
		if (alreadyDefined[variableName])
		{
			definitions[variableName] = ins;
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
		Instruction ins(functionActionType);
		std::string functionName = *((std::string*)(parameters[0].data));

		ins.parameters.push_back(parameters[1]);
		ins.parameters.push_back(parameters[2]);

		if (alreadyDefined[functionName])
		{
			definitions[functionName] = ins;
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
		Instruction ins(functionActionType);
		std::string functionName = *((std::string*)(parameters[0].data));

		
		ins.parameters.push_back(parameters[1]);
		ins.parameters.push_back(parameters[2]);

		if (alreadyDefined[functionName])
		{
			definitions[functionName] = ins;
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

		if (!defaultType.compare(definitions[functionName].type))
		{
			state = InterpreterErrorFlags::undefinedFunctionFlag;
			undefinedObject = functionName;
			return;
		}
		else
		{
			ret = false;
			parameters[1].execute(state, undefinedObject, definitions, alreadyDefined, redefinedObj, redefined, os, is, ret, result);
			if (state != InterpreterErrorFlags::normalStateFlag) return;

			Instruction ins(numberType);
			ins.data = new Number(result);

			int newRedefined = 0;
			DEFINED newAlreadyDefined;
			variableName = *((std::string*)(definitions[functionName].parameters[0].data));
			redefinedObj.push(make_pair(variableName, definitions[variableName]));
			definitions[variableName] = ins;
			newAlreadyDefined[variableName] = true;
			newRedefined++;

			ret = false;
			definitions[functionName].parameters[1].execute(state, undefinedObject, definitions, newAlreadyDefined, redefinedObj, newRedefined, os, is, ret, result);
			undoRedefining(definitions, redefinedObj, newRedefined);
			if (state != InterpreterErrorFlags::normalStateFlag) return;

			if (ret)
			{
				returnFlag = true;
				returnValue = result;
			}
			else
			{
				state = InterpreterErrorFlags::lackOfReturnValue;
				undefinedObject = functionName;
				return;
			}
		}
	}
	else if (!functionActionType.compare(type)) return;
}