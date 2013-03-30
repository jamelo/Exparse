/****************************************************
*	Exparse - Math expression evaluator library		*
* 	Copyright (C) 2013 Jordan Melo				 	*
****************************************************/

#ifndef CONTEXT_H
#define CONTEXT_H

#include <string>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <cmath>

#include <mutex>

#include "function.h"
#include "variable.h"

#define	RESERVED_VARIABLE_SPACE			64
#define	NULLID							0

class FunctionContext
{
	private:
		struct OperatorComparator
		{
			const std::vector<Operator>& operatorList;

			OperatorComparator(const std::vector<Operator>& ops) :
				operatorList(ops)
			{ }

			bool operator()(unsigned int i1, unsigned int i2) const
			{
				const Operator& a = operatorList[i1];
				const Operator& b = operatorList[i2];

				if (a._symbol.length() > b._symbol.length())
					return true;
				if (a._symbol.length() < b._symbol.length())
					return false;

				if (a._precedence < b._precedence)
					return true;
				if (a._precedence > b._precedence)
					return false;

				if (a._position < b._position)
					return true;
				if (a._position > b._position)
					return false;

				if (a._symbol.compare(b._symbol) > 0)
					return true;

				return false;
			}
		};

		std::vector<Operator> _operators;
		std::vector<Function> _functions;
		std::unordered_map<std::string, unsigned int> _functionIndex;
		std::set<unsigned int, OperatorComparator> operatorOrderedIndex;

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
