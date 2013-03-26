/****************************************************
*	Exparse - Math expression evaluator library		*
* 	Copyright (C) 2013 Jordan Melo				 	*
****************************************************/

#ifndef FUNCTION_H
#define FUNCTION_H

#include	<cmath>
#include	<string>
#include	<vector>
#include	<assert.h>

typedef enum Handedness
{
	HAND_RVALUE,
	HAND_LVALUE
} Handedness;

union Value
{
	double numeric;
	unsigned int variableId;

	Value() 									{ }
	Value(unsigned int vid) : variableId(vid) 	{ }
	Value(double val) : numeric(val) 			{ }
};

class ArgumentList;

typedef Value (*FunctionPointer)(ArgumentList& args);

class Function
{
	private:
		std::string _symbol;
		FunctionPointer _func;
		unsigned int _arity;
		bool _variadic;
		Handedness _retHandedness;
		std::vector<Handedness> _argHandedness;

		static unsigned int numDefaultFunctions;

	public:
		Function(std::string funcName, FunctionPointer func, unsigned int arity,
				Handedness retHandedness = HAND_RVALUE, bool variadic = false,
				const std::vector<Handedness>& argHandedness = std::vector<Handedness>());

		std::string symbol() const { return _symbol; }
		unsigned int arity() const { return _arity; }
		bool isVariadic() const { return _variadic; }
		Handedness returnValueHandedness() const { return _retHandedness; }

		Handedness argumentHandedness(unsigned int index) const
		{
			assert(index < _argHandedness.size());
			return _argHandedness[index];
		}

		Value execute(ArgumentList& args) const
		{
			return _func(args);
		}

		static const Function defaults[];

	friend class FunctionContext;

};

class Operator : public Function
{
	public:
		typedef enum Positioning
		{
			//Should be powers of two so we can select which ops to find
			POS_PREFIX = 1,
			POS_POSTFIX = 2,
			POS_INFIX = 4
		} Positioning;

		typedef enum Associativity
		{
			ASSOC_NONE = 0,
			ASSOC_LEFT,
			ASSOC_RIGHT
		} Associativity;

	private:
		unsigned int _precedence;
		Positioning _position;
		Associativity _assoc;

		static unsigned int numDefaultOperators;

	public:
		Operator(std::string opSymbol, FunctionPointer func, unsigned int opPrecedence,
				Positioning pos, Associativity assoc = ASSOC_LEFT, Handedness retHandedness = HAND_RVALUE,
				const std::vector<Handedness>& argHandedness = std::vector<Handedness>());

		unsigned int precedence() const { return _precedence; }
		Positioning position() const 	{ return _position; }
		Associativity associativity() const { return _assoc; }

		Operator& setReferenceParameters(const std::vector<unsigned int>& paramIndices);

		static const Operator defaults[];

	friend class FunctionContext;
};

#endif
