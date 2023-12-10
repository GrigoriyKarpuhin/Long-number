#include "LN.h"
#include "return_codes.h"

#include <fstream>
#include <iostream>
#include <stack>
#include <string>

using namespace std;

void doOper(stack< LN >& numbers, char operation)
{
	if (numbers.empty())
	{
		throw runtime_error("Error: Insufficient operands for operation");
	}
	LN operand1 = numbers.top();
	numbers.pop();

	LN operand2 = 0;
	LN result;

	switch (operation)
	{
	case '+':
		if (numbers.empty())
		{
			throw runtime_error("Error: Insufficient operands for operation");
		}
		operand2 = numbers.top();
		numbers.pop();
		result = operand1 + operand2;
		break;
	case '-':
		if (numbers.empty())
		{
			throw runtime_error("Error: Insufficient operands for operation");
		}
		operand2 = numbers.top();
		numbers.pop();
		result = operand1 - operand2;
		break;
	case '*':
		if (numbers.empty())
		{
			throw runtime_error("Error: Insufficient operands for operation");
		}
		operand2 = numbers.top();
		numbers.pop();
		result = operand1 * operand2;
		break;
	case '/':
		if (numbers.empty())
		{
			throw runtime_error("Error: Insufficient operands for operation");
		}
		operand2 = numbers.top();
		numbers.pop();
		result = operand1 / operand2;
		break;
	case '%':
		if (numbers.empty())
		{
			throw runtime_error("Error: Insufficient operands for operation");
		}
		operand2 = numbers.top();
		numbers.pop();
		result = operand1 % operand2;
		break;
	case '_':
		result = -operand1;
		break;
	case '~':
		result = ~operand1;
		break;
	default:
		throw runtime_error("Error: Invalid operation");
	}

	numbers.push(result);
}

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		fprintf(stderr, "Not 3 arguments\n");
		return ERROR_PARAMETER_INVALID;
	}
	ifstream inputFile(argv[1]);

	if (!inputFile.is_open())
	{
		cerr << "Error: Failed to open input file" << endl;
		return ERROR_CANNOT_OPEN_FILE;
	}

	stack< LN > numbers;
	string line;

	try
	{
		while (getline(inputFile, line))
		{
			if (line.empty())
			{
				continue;
			}

			if (line[0] == '+' || (line[0] == '-' && !(line[1])) || line[0] == '*' || line[0] == '/' ||
				line[0] == '%' || line[0] == '_' || line[0] == '~')
			{
				char operation = line[0];
				doOper(numbers, operation);
			}
			else if (line[0] == '=')
			{
				if (line[1] != '=')
				{
					throw runtime_error("Error: Invalid operation");
				}
				if (numbers.empty())
				{
					throw runtime_error("Error: No operands in stack");
				}
				LN operator1 = numbers.top();
				numbers.pop();
				LN operator2 = numbers.top();
				numbers.pop();
				numbers.emplace(operator1 == operator2);
			}
			else if (line[0] == '!')
			{
				if (line[1] != '=')
				{
					throw runtime_error("Error: Invalid operation");
				}
				if (numbers.empty())
				{
					throw runtime_error("Error: No operands in stack");
				}
				LN operator1 = numbers.top();
				numbers.pop();
				LN operator2 = numbers.top();
				numbers.pop();
				numbers.emplace(operator1 != operator2);
			}
			else if (line[0] == '<')
			{
				bool equality = false;
				if (line[1] != '=')
				{
					equality = true;
				}
				if (numbers.empty())
				{
					throw runtime_error("Error: No operands in stack");
				}
				LN operator1 = numbers.top();
				numbers.pop();
				LN operator2 = numbers.top();
				numbers.pop();
				bool result = equality ? operator1 < operator2 : operator1 <= operator2;
				numbers.emplace(result);
			}
			else if (line[0] == '>')
			{
				bool equality = false;
				if (line[1] != '=')
				{
					equality = true;
				}
				if (numbers.empty())
				{
					throw runtime_error("Error: No operands in stack");
				}
				LN operator1 = numbers.top();
				numbers.pop();
				LN operator2 = numbers.top();
				numbers.pop();
				bool result = equality ? operator1 > operator2 : operator1 >= operator2;
				numbers.emplace(result);
			}
			else
			{
				LN num(line);
				numbers.push(num);
			}
		}
		inputFile.close();

		ofstream outputFile(argv[2]);

		if (!outputFile.is_open())
		{
			cerr << "Error: Failed to open output file" << endl;
			return ERROR_CANNOT_OPEN_FILE;
		}

		while (!numbers.empty())
		{
			LN finalResult = numbers.top();
			numbers.pop();
			char* finalResultString = toString(finalResult);
			outputFile << finalResultString << endl;
		}
		outputFile.close();
	} catch (const exception& e)
	{
		inputFile.close();
		cerr << "Error: " << e.what() << endl;
		return ERROR_DATA_INVALID;
	}

	return 0;
}