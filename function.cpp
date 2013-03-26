/****************************************************
*	Exparse - Math expression evaluator library		*
* 	Copyright (C) 2013 Jordan Melo				 	*
****************************************************/

#include	"function.h"
#include	"argument_list.h"

Function::Function(std::string funcName, FunctionPointer func, unsigned int numArgs,
				Handedness retHandedness, bool variadic,
				const std::vector<Handedness>& argHandedness) :
	_symbol(funcName),
	_func(func),
	_arity(numArgs),
	_variadic(variadic),
	_retHandedness(retHandedness),
	_argHandedness(_arity + 1, HAND_RVALUE)
{
	std::copy(argHandedness.begin(), argHandedness.end(), _argHandedness.begin());
	//TODO: error handling for inputs
}

namespace DefaultFunction
{
	Value deref(ArgumentList& args)		{ return args.dereference(0);			}
	Value cos(ArgumentList& args)		{ return std::cos(args[0]);				}
	Value sin(ArgumentList& args)		{ return std::sin(args[0]);				}
	Value tan(ArgumentList& args)		{ return std::tan(args[0]);				}
	Value acos(ArgumentList& args)		{ return std::acos(args[0]);			}
	Value asin(ArgumentList& args)		{ return std::asin(args[0]);			}
	Value atan(ArgumentList& args)		{ return std::atan(args[0]);			}
	Value atan2(ArgumentList& args)		{ return std::atan2(args[0], args[1]);	}
	Value cosh(ArgumentList& args)		{ return std::cosh(args[0]);			}
	Value sinh(ArgumentList& args)		{ return std::sinh(args[0]);			}
	Value tanh(ArgumentList& args)		{ return std::tanh(args[0]);			}
	Value exp(ArgumentList& args)		{ return std::exp(args[0]);				}
	Value log(ArgumentList& args)		{ return std::log(args[0]);				}
	Value log10(ArgumentList& args)		{ return std::log10(args[0]);			}
	Value pow(ArgumentList& args)		{ return std::pow(args[0], args[1]);	}
	Value sqrt(ArgumentList& args)		{ return std::sqrt(args[0]);			}
	Value ceil(ArgumentList& args)		{ return std::ceil(args[0]);			}
	Value abs(ArgumentList& args)		{ return std::abs(args[0]);				}
	Value floor(ArgumentList& args)		{ return std::floor(args[0]);			}
	Value mod(ArgumentList& args) 		{ return std::fmod(args[0], args[1]); 	}
}

const Function Function::defaults[] =
{
	Function("_deref", &DefaultFunction::deref, 1, HAND_RVALUE, false, std::vector<Handedness>(1, HAND_LVALUE)),
	Function("cos", &DefaultFunction::cos, 1),
	Function("sin", &DefaultFunction::sin, 1),
	Function("tan", &DefaultFunction::tan, 1),
	Function("acos", &DefaultFunction::acos, 1),
	Function("asin", &DefaultFunction::asin, 1),
	Function("atan", &DefaultFunction::atan, 1),
	Function("atan2", &DefaultFunction::atan2, 2),
	Function("cosh", &DefaultFunction::cosh, 1),
	Function("sinh", &DefaultFunction::sinh, 1),
	Function("tanh", &DefaultFunction::tanh, 1),
	Function("exp", &DefaultFunction::exp, 1),
	Function("log", &DefaultFunction::log, 1),
	Function("log10", &DefaultFunction::log10, 1),
	Function("pow", &DefaultFunction::pow, 2),
	Function("sqrt", &DefaultFunction::sqrt, 1),
	Function("ceil", &DefaultFunction::ceil, 1),
	Function("abs", &DefaultFunction::abs, 1),
	Function("floor", &DefaultFunction::floor, 1),
	Function("mod", &DefaultFunction::mod, 2)
};

unsigned int Function::numDefaultFunctions = sizeof(defaults);
