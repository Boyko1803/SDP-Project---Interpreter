#include "Number.h"
#include "Interpreter.h"

using namespace std;

int main()
{
	Interpreter IT;
	string address;

	cout << "Enter program address: ";
	getline(cin, address);

	IT.Run(address);

	return 0;
}