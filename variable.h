/****************************************************
*	Exparse - Math expression evaluator library		*
* 	Copyright (C) 2013 Jordan Melo				 	*
****************************************************/

#ifndef VARIABLE_H
#define VARIABLE_H

#include <string>
#include <assert.h>

class Variable
{
	private:
		std::string _name;
		double _value;
		bool _constant;

	public:
		Variable(std::string varName, double val = 0.0, bool isConstant = false) :
			_name(varName),
			_value(val),
			_constant(isConstant)
		{ }

		std::string name() const
		{
			return _name;
		}

		double value() const
		{
			return _value;
		}

		double& getReference()
		{
			return _value;
		}
};

#endif
