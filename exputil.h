/****************************************************
*	Exparse - Math expression evaluator library		*
* 	Copyright (C) 2013 Jordan Melo				 	*
****************************************************/

#ifndef	EXPARSE_UTIL_H
#define	EXPARSE_UTIL_H

#include	<string>
#include	"token.h"

namespace Exparse
{
	namespace util
	{
		std::string lookupTokenTypeString(Token::TokenType type);
		std::string tokenToString(Token& t, const FunctionContext& fc, VariableContext& vc);
	}
}

#endif
