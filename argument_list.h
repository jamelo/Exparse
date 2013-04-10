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
		/**< An iterator that points to the beginning of the argument list on the evaluation stack */
		std::vector<Value>::iterator argListBegin;

		/**< An iterator that points to the end of the argument list on the evaluation stack */
		std::vector<Value>::iterator argListEnd;

        /**< A variable context to be used for variable look-ups within function definitions */
		VariableContext& _vc;

	public:
        /** \brief Constructor
         *
         * \param first		An iterator that points to the beginning of the argument list on the evaluation stack
         * \param last		An iterator that points to the end of the argument list on the evaluation stack
         * \param vc		A variable context to be used for variable look-ups within function definitions
         *
         */
		ArgumentList(std::vector<Value>::iterator first, std::vector<Value>::iterator last, VariableContext& vc) :
			argListBegin(first),
			argListEnd(last),
			_vc(vc)
		{ }

        /** \brief Looks up a variable and returns a reference to the underlying value
         *
         * \param index		The ID of the variable to look up
         * \return 			A reference to the variable's value
         *
         */
		double& dereference(const unsigned int index)
		{
			assert(index < length());
			unsigned int varID = (argListBegin + index)->variableId;

			return _vc.lookupVariable(varID)->getReference();
		}

        /** \brief Looks up a variable and returns its value
         *
         * \param index		The ID of the variable to look up
         * \return 			The variable's value
         *
         */
		double operator[](const unsigned int index) const
		{
			assert(index < length());
			return (argListBegin + index)->numeric;
		}

        /** \brief Returns the number of arguments in the argument list
         *
         * \return The number of arguments contained in the argument list
         *
         */
		unsigned int length() const
		{
			return std::distance(argListBegin, argListEnd);
		}
};


#endif
