/****************************************************
*	Exparse - Math expression evaluator library		*
* 	Copyright (C) 2013 Jordan Melo				 	*
****************************************************/

#include	"exputil.h"

namespace Exparse
{
	namespace util
	{
		std::string lookupTokenTypeString(Token::TokenType type)
		{
			switch (type)
			{
				case Token::NUMBER:			return std::string("NUMBER");
				case Token::VARIABLE:		return std::string("VARIABLE");
				case Token::FUNCTION:		return std::string("FUNCTION");
				case Token::PARENTHESIS:	return std::string("PARENTHESIS");
				case Token::OPERATOR:		return std::string("OPERATOR");
				case Token::END:			return std::string("END");
				case Token::DELIMITER:		return std::string("DELIMITER");
				default: assert(false);
			}

			return std::string();
		}

		std::string tokenToString(Token& t, const FunctionContext& fc, VariableContext& vc)
		{
			switch (t.type())
			{
				case Token::NUMBER:
				{
					std::stringstream numstr;
					numstr << t.toNumber().value();
					return numstr.str();
				}
				case Token::VARIABLE:
					return vc.lookupVariable(t.toVariable().id())->name();
				case Token::FUNCTION:
					return fc.lookupFunction(t.toFunction().id())->symbol();
				case Token::OPERATOR:
					return fc.lookupOperator(t.toOperator().id())->symbol();

				case Token::PARENTHESIS:
				{
					switch (t.toParenthesis().type())
					{
						case ParenthesisToken::ROUND_LEFT: 		return std::string("(");
						case ParenthesisToken::ROUND_RIGHT: 	return std::string(")");
						case ParenthesisToken::SQUARE_LEFT: 	return std::string("[");
						case ParenthesisToken::SQUARE_RIGHT: 	return std::string("]");
						case ParenthesisToken::BRACE_LEFT: 		return std::string("{");
						case ParenthesisToken::BRACE_RIGHT: 	return std::string("}");
						case ParenthesisToken::ANGLE_LEFT: 		return std::string("<");
						case ParenthesisToken::ANGLE_RIGHT: 	return std::string(">");
						default: 								assert(false);
					}
				}

				case Token::DELIMITER:
				{
					switch (t.toDelimiter().type())
					{
						case DelimiterToken::ARG_DELIM: 		return std::string(",");
						case DelimiterToken::STATEMENT_DELIM: 	return std::string(";");
						default:								assert(false);
					}
				}

				default: assert(false);
			}

			return std::string();
		}
	}
}
