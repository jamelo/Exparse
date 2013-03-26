/****************************************************
*	Exparse - Math expression evaluator library		*
* 	Copyright (C) 2013 Jordan Melo				 	*
****************************************************/

#include <iostream>
#include <string>
#include <assert.h>

#include "expression_parser.h"
#include "tokenizer.h"

using namespace std;

int main()
{
	FunctionContext fc;
	VariableContext vc;

	cout.precision(8);

    while (1)
	{
		char input[512];
		cout << "> ";
		cin.getline(input, 512);
		string strinput(input);

		try
		{
			ExpressionParser ep(strinput, vc, fc);
			cout << "   = " << ep.evaluate() << endl;
		}
		catch (TokenizerException& tex)
		{
			cout << tex.what() << endl;
		}

		cout << endl;
	}

    return 0;
}
