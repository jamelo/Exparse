/****************************************************
*	Exparse - Math expression evaluator library		*
* 	Copyright (C) 2013 Jordan Melo				 	*
****************************************************/

#ifndef CONTEXT_H
#define CONTEXT_H

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <cmath>

#include "function.h"
#include "variable.h"

#define	RESERVED_VARIABLE_SPACE			64
#define	NULLID							0

class FunctionContext
{
	private:
		std::vector<Operator> _operators;
		std::vector<Function> _functions;
		std::unordered_map<std::string, unsigned int> _operatorIndex;
		std::unordered_map<std::string, unsigned int> _functionIndex;
		//TODO: consider using vector for op precedence index and sort upon insertion should result in faster iteration.
		std::multimap<unsigned int, unsigned int> _operatorPrecedenceIndex;

	public:
		FunctionContext();

		void registerOperator(const Operator& o);
		void registerFunction(const Function& f);

		unsigned int getFunctionID(std::string name) const;

		const Operator* lookupOperator(unsigned int id) const;
		const Function* lookupFunction(unsigned int id) const;

		unsigned int parseOperator(const std::string& expr, unsigned int& index, int positions) const;
};

class VariableContext
{
	private:
		std::vector<Variable> _variables;
		std::unordered_map<std::string, unsigned int> _variableIndex;

		unsigned int registerVariable(const Variable& var);

	public:
		VariableContext();

		unsigned int getId(const std::string& name);
		Variable* lookupVariable(unsigned int id);
};

#endif
