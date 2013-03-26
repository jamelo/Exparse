#ifndef OPERATOR_H
#define OPERATOR_H

#include 	<vector>
#include 	<cmath>
#include 	<string>
#include	<assert.h>

#include	"function.h"

#error Blah

class ArgumentList;

typedef double (*OperationPointer)(ArgumentList& args);

class Operator
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

		std::string _symbol;
		OperationPointer _func;
		unsigned int _precedence;
		Positioning _position;
		Associativity _assoc;
		std::vector<Handedness> _argHandedness;
		Handedness _retHandedness;

		static unsigned int numDefaultOperators;

	public:
		Operator(std::string opSymbol, OperationPointer func, unsigned int opPrecedence,
				Positioning pos, Associativity assoc = ASSOC_LEFT, Handedness retHandedness = HAND_RVALUE);

		std::string symbol() const 		{ return _symbol; }
		unsigned int precedence() const { return _precedence; }
		Positioning position() const 	{ return _position; }
		Associativity associativity() const { return _assoc; }
		bool returnValueHandedness() const { return _retHandedness; }

		Handedness argumentHandedness(unsigned int index) const
		{
			assert(index < _argHandedness.size());
			return _argHandedness[index];
		}
		//TODO: more accessors

		Operator& setReferenceParameters(const std::vector<unsigned int>& paramIndices);

		static const Operator defaults[];

	friend class FunctionContext;
};

#endif
