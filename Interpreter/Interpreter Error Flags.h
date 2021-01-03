#pragma once

namespace InterpreterErrorFlags
{
	/// Interpreter state flags


	/// Normal flag
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
}