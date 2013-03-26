/****************************************************
*	Exparse - Math expression evaluator library		*
* 	Copyright (C) 2013 Jordan Melo				 	*
****************************************************/

#include	"function.h"
#include	"argument_list.h"

Operator::Operator(std::string opSymbol, FunctionPointer func, unsigned int opPrecedence,
		Positioning pos, Associativity assoc, Handedness retHandedness,
		const std::vector<Handedness>& argHandedness) :
	Function(opSymbol, func, pos == POS_INFIX ? 2 : 1, retHandedness, false, argHandedness),
	_precedence(opPrecedence),
	_position(pos),
	_assoc(assoc)
{
	//TODO: error handling for inputs, esp. opSymbol
}

namespace DefaultOperator
{
	Value plus(ArgumentList& args)					{ return args[0]; }
	Value minus(ArgumentList& args)					{ return -args[0]; }
	Value addition(ArgumentList& args) 				{ return args[0] + args[1]; }
	Value subtraction(ArgumentList& args) 			{ return args[0] - args[1]; }
	Value multiplication(ArgumentList& args)		{ return args[0] * args[1]; }
	Value division(ArgumentList& args)				{ return args[0] / args[1]; }
	Value modulo(ArgumentList& args)				{ return std::fmod(args[0], args[1]); }
	Value exponentiation(ArgumentList& args)		{ return std::pow(args[0], args[1]); }
	Value preIncrement(ArgumentList& args)			{ return ++args.dereference(0); }
	Value postIncrement(ArgumentList& args)			{ return args.dereference(0)++; }
	Value preDecrement(ArgumentList& args)			{ return --args.dereference(0); }
	Value postDecrement(ArgumentList& args)			{ return args.dereference(0)--; }
	Value assignment(ArgumentList& args)			{ return args.dereference(0) = args[1]; }
	Value addAndAssign(ArgumentList& args)			{ return args.dereference(0) += args[1]; }
	Value subtractAndAssign(ArgumentList& args)		{ return args.dereference(0) -= args[1]; }
	Value multiplyAndAssign(ArgumentList& args)		{ return args.dereference(0) *= args[1]; }
	Value divideAndAssign(ArgumentList& args)		{ return args.dereference(0) /= args[1]; }
	Value moduloAndAssign(ArgumentList& args)		{ return args.dereference(0) = std::fmod(args.dereference(0), args[1]); }
	Value isEqual(ArgumentList& args)				{ return (std::fabs(args[0] - args[1]) / (std::fabs(args[0]) + 1.0)) < 0.00001 ? 1.0 : 0.0; }
	Value isNotEqual(ArgumentList& args)			{ return (std::fabs(args[0] - args[1]) / (std::fabs(args[0]) + 1.0)) >= 0.00001 ? 1.0 : 0.0; }
	Value isLessThan(ArgumentList& args)			{ return args[0] < args[1] ? 1.0 : 0.0; }
	Value isGreaterThan(ArgumentList& args)			{ return args[0] > args[1] ? 1.0 : 0.0; }
	Value isLessOrEqual(ArgumentList& args)			{ return (isEqual(args).numeric + isLessThan(args).numeric) > 0.5 ? 1.0 : 0.0; }
	Value isGreaterOrEqual(ArgumentList& args)		{ return (isEqual(args).numeric + isGreaterThan(args).numeric) > 0.5 ? 1.0 : 0.0; }
	Value booleanAnd(ArgumentList& args)			{ return std::fabs(args[0]) >= 0.5 && std::fabs(args[1]) >= 0.5 ? 1.0 : 0.0; }
	Value booleanOr(ArgumentList& args)				{ return std::fabs(args[0]) >= 0.5 || std::fabs(args[1]) >= 0.5 ? 1.0 : 0.0; }
	Value booleanNot(ArgumentList& args)			{ return std::fabs(args[0]) < 0.5 ? 1.0 : 0.0; }
}

const Operator Operator::defaults[] = {
	Operator("++", &DefaultOperator::postIncrement, 2, POS_POSTFIX, ASSOC_LEFT, HAND_RVALUE, std::vector<Handedness>(1, HAND_LVALUE)),
	Operator("--", &DefaultOperator::postDecrement, 2, POS_POSTFIX, ASSOC_LEFT, HAND_RVALUE, std::vector<Handedness>(1, HAND_LVALUE)),
	Operator("++", &DefaultOperator::preIncrement, 4, POS_PREFIX, ASSOC_RIGHT, HAND_RVALUE, std::vector<Handedness>(1, HAND_LVALUE)),
	Operator("--", &DefaultOperator::preDecrement, 4, POS_PREFIX, ASSOC_RIGHT, HAND_RVALUE, std::vector<Handedness>(1, HAND_LVALUE)),
	Operator("+", &DefaultOperator::plus, 4, POS_PREFIX, ASSOC_RIGHT),
	Operator("-", &DefaultOperator::minus, 4, POS_PREFIX, ASSOC_RIGHT),
	Operator("+", &DefaultOperator::addition, 6, POS_INFIX),
	Operator("-", &DefaultOperator::subtraction, 6, POS_INFIX),
	Operator("*", &DefaultOperator::multiplication, 5, POS_INFIX),
	Operator("/", &DefaultOperator::division, 5, POS_INFIX),
	Operator("%", &DefaultOperator::modulo, 5, POS_INFIX),
	Operator("^", &DefaultOperator::exponentiation, 3, POS_INFIX, ASSOC_RIGHT),
	Operator("=", &DefaultOperator::assignment, 15, POS_INFIX, ASSOC_RIGHT, HAND_RVALUE, std::vector<Handedness>(1, HAND_LVALUE)),
	Operator("+=", &DefaultOperator::addAndAssign, 15, POS_INFIX, ASSOC_RIGHT, HAND_RVALUE, std::vector<Handedness>(1, HAND_LVALUE)),
	Operator("-=", &DefaultOperator::subtractAndAssign, 15, POS_INFIX, ASSOC_RIGHT, HAND_RVALUE, std::vector<Handedness>(1, HAND_LVALUE)),
	Operator("*=", &DefaultOperator::multiplyAndAssign, 15, POS_INFIX, ASSOC_RIGHT, HAND_RVALUE, std::vector<Handedness>(1, HAND_LVALUE)),
	Operator("/=", &DefaultOperator::divideAndAssign, 15, POS_INFIX, ASSOC_RIGHT, HAND_RVALUE, std::vector<Handedness>(1, HAND_LVALUE)),
	Operator("%=", &DefaultOperator::moduloAndAssign, 15, POS_INFIX, ASSOC_RIGHT, HAND_RVALUE, std::vector<Handedness>(1, HAND_LVALUE)),
	Operator("==", &DefaultOperator::isEqual, 9, POS_INFIX),
	Operator("!=", &DefaultOperator::isNotEqual, 9, POS_INFIX),
	Operator("<", &DefaultOperator::isLessThan, 8, POS_INFIX),
	Operator(">", &DefaultOperator::isGreaterThan, 8, POS_INFIX),
	Operator("<=", &DefaultOperator::isLessOrEqual, 8, POS_INFIX),
	Operator(">=", &DefaultOperator::isGreaterOrEqual, 8, POS_INFIX),
	Operator("&&", &DefaultOperator::booleanAnd, 13, POS_INFIX),
	Operator("||", &DefaultOperator::booleanOr, 14, POS_INFIX),
	Operator("!", &DefaultOperator::booleanNot, 4, POS_PREFIX, ASSOC_RIGHT)
};

unsigned int Operator::numDefaultOperators = sizeof(defaults);
