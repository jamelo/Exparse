/****************************************************
*	Exparse - Math expression evaluator library		*
* 	Copyright (C) 2013 Jordan Melo				 	*
****************************************************/

#include	"context.h"

FunctionContext::FunctionContext()
{
	unsigned int numOps = Operator::numDefaultOperators / sizeof(Operator);
	unsigned int numFuncs = Function::numDefaultFunctions / sizeof(Function);

	_operators.reserve(numOps);
	_functions.reserve(numFuncs);
	_operatorIndex.reserve(numOps);
	_functionIndex.reserve(numFuncs);

	for (unsigned int i = 0; i < numOps; i++)
		registerOperator(Operator::defaults[i]);

	for (unsigned int i = 0; i < numFuncs; i++)
		registerFunction(Function::defaults[i]);
}

void FunctionContext::registerOperator(const Operator& o)
{
	//TODO: make thread-safe

	//TODO: sort operators by length (longest to shortest) and then by precedence (highest to lowest)

	auto it = _operatorIndex.find(o.symbol());
	if (it != _operatorIndex.end())
		assert(_operators[it->second].position() != o.position());

	unsigned int index = _operators.size();
	std::pair<std::string, unsigned int> kvp1;
	std::pair<unsigned int, unsigned int> kvp2;

	_operators.push_back(o);
	kvp1 = std::pair<std::string, unsigned int>(o.symbol(), index);
	kvp2 = std::pair<unsigned int, unsigned int>(o.precedence(), index);

	_operatorIndex.insert(kvp1);
	_operatorPrecedenceIndex.insert(kvp2);
}

void FunctionContext::registerFunction(const Function& f)
{
	//TODO: make thread-safe

	assert(_functionIndex.find(f.symbol()) == _functionIndex.end());

	unsigned int index = _functions.size();
	std::pair<std::string, unsigned int> kvp;

	_functions.push_back(f);
	kvp = std::pair<std::string, unsigned int>(f.symbol(), index);
	_functionIndex.insert(kvp);
}

unsigned int FunctionContext::getFunctionID(std::string name) const
{
	std::unordered_map<std::string, unsigned int>::const_iterator it = _functionIndex.find(name);

	if (it == _functionIndex.end())
		return NULLID;
	else
		return it->second + 1;
}


unsigned int FunctionContext::parseOperator(const std::string& expr, unsigned int& index, int positions) const
{
	//TODO: make thread-safe?
	for (auto it = _operatorPrecedenceIndex.begin(); it != _operatorPrecedenceIndex.end(); it++)
	{
		const Operator* op = &_operators[it->second];
		std::string opsymbol = op->symbol();
		std::string inputChunk = expr.substr(index, opsymbol.length());

		if (opsymbol.compare(inputChunk) == 0 && (op->position() & positions))
		{
			index += opsymbol.size();
			return it->second + 1;
		}
	}

	return NULLID;
}

const Operator* FunctionContext::lookupOperator(unsigned int id) const
{
	assert(id > 0 && id <= _operators.size());
	return &_operators[id - 1];
}

const Function* FunctionContext::lookupFunction(unsigned int id) const
{
	assert(id > 0 && id <= _functions.size());
	return &_functions[id - 1];
}

unsigned int VariableContext::registerVariable(const Variable& var)
{
	//TODO: make thread-safe

	assert(_variableIndex.find(var.name()) == _variableIndex.end());

	unsigned int index = _variables.size();
	std::pair<std::string, unsigned int> kvp;

	_variables.push_back(var);
	kvp = std::pair<std::string, unsigned int>(var.name(), index);
	_variableIndex.insert(kvp);

	return index + 1;
}

VariableContext::VariableContext()
{
	_variables.reserve(RESERVED_VARIABLE_SPACE);
	_variableIndex.reserve(RESERVED_VARIABLE_SPACE);

	registerVariable(Variable("pi", std::acos(-1.0), true));
	registerVariable(Variable("e", std::exp(1.0), true));
	registerVariable(Variable("phi", 0.5 + std::sqrt(5.0) * 0.5, true));
}

unsigned int VariableContext::getId(const std::string& name)
{
	std::unordered_map<std::string, unsigned int>::iterator it;

	it = _variableIndex.find(name);

	if (it == _variableIndex.end())
		//Create the variable since it doesn't already exist
		return registerVariable(Variable(name, 0.0));
	else
		return it->second + 1;
}

Variable* VariableContext::lookupVariable(unsigned int id)
{
	assert(id > 0 && id <= _variables.size());
	return &_variables[id - 1];
}
