/****************************************************
*	Exparse - Math expression evaluator library		*
* 	Copyright (C) 2013 Jordan Melo				 	*
****************************************************/

#ifndef ARGUMENT_LIST_H
#define ARGUMENT_LIST_H

#include	<vector>
#include	<iterator>

#include	"context.h"
#include	"function.h"

class ArgumentList
{
	private:
		std::vector<Value>::iterator argListBegin;
		std::vector<Value>::iterator argListEnd;
		VariableContext& _vc;

	public:
		ArgumentList(std::vector<Value>::iterator first, std::vector<Value>::iterator last, VariableContext& vc) :
			argListBegin(first),
			argListEnd(last),
			_vc(vc)
		{ }

		double& dereference(const unsigned int index)
		{
			assert(index < length());
			unsigned int varID = (argListBegin + index)->variableId;

			return _vc.lookupVariable(varID)->getReference();
		}

		double operator[](const unsigned int index) const
		{
			assert(index < length());
			return (argListBegin + index)->numeric;
		}

		unsigned int length() const
		{
			return std::distance(argListBegin, argListEnd);
		}
};


#endif
