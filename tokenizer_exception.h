/****************************************************
*	Exparse - Math expression evaluator library		*
* 	Copyright (C) 2013 Jordan Melo				 	*
****************************************************/

#ifndef TOKENIZER_EXCEPTION_H
#define TOKENIZER_EXCEPTION_H

#include	<string>
#include	<sstream>
#include	<exception>

#include	"token.h"
#include	"context.h"
#include	"exputil.h"

class TokenizerException : public std::exception
{
	protected:
		std::string _message;
		unsigned int _exprLocation;

	public:
		virtual ~TokenizerException() throw() { };

		virtual const char* what() const throw()
		{
			std::stringstream stream;
			stream << _message << " at column " << _exprLocation;
			return stream.str().c_str();
		}
};

class UnknownFunctionException : public TokenizerException
{
	public:
		UnknownFunctionException(const std::string& token, unsigned int exprLocation)
		{
			_exprLocation = exprLocation;
			_message = std::string("Unknown function '") + token + std::string("'");
		}
};

class UnknownTokenException : public TokenizerException
{
	public:
		UnknownTokenException(unsigned int exprLocation)
		{
			_exprLocation = exprLocation;
			_message = std::string("Unknown token");
		}
};

class UnexpectedTokenException : public TokenizerException
{
	public:
		UnexpectedTokenException(const Token& t)
		{
			_exprLocation = t.location();
			_message = std::string("Unexpected ") + Exparse::util::lookupTokenTypeString(t.type()) + std::string(" token");
		}
};

class UnexpectedEndException : public TokenizerException
{
	public:
		UnexpectedEndException(unsigned int exprLocation)
		{
			_exprLocation = exprLocation;
			_message = std::string("Unexpected end of statement");
		}
};

class MalformedNumberException : public TokenizerException
{
	public:
		MalformedNumberException(const std::string& token, unsigned int exprLocation)
		{
			_exprLocation = exprLocation;
			_message = std::string("Malformed number '") + token + std::string("'");
		}
};

class MalformedIdentifierException : public TokenizerException
{
	public:
		MalformedIdentifierException(const std::string& token, unsigned int exprLocation)
		{
			_exprLocation = exprLocation;
			_message = std::string("Malformed identifier '") + token + std::string("'");
		}
};

class UnmatchedParenthesisException : public TokenizerException
{
	public:
		UnmatchedParenthesisException(const Token& t)
		{
			_exprLocation = t.location();
			_message = std::string("Unmatched parenthesis");
		}
};

class InvalidArgumentException : public TokenizerException
{
	public:
		InvalidArgumentException(Token& t, unsigned int argumentIndex, const FunctionContext& fc)
		{
			_exprLocation = t.location();
			std::stringstream s;
			const Function* func;

			switch (t.type())
			{
				case Token::OPERATOR: func = fc.lookupOperator(t.toOperator().id()); break;
				case Token::FUNCTION: func = fc.lookupFunction(t.toFunction().id()); break;
				default:			  assert(false);
			}

			s << "Invalid argument: Expecting variable reference as argument ";
			s << argumentIndex + 1 << " for function '" << func->symbol() << "'";

			_message = s.str();
		}
};

class InvalidNumArgumentsException : public TokenizerException
{
	public:
		InvalidNumArgumentsException(Token& t, const FunctionContext& fc)
		{
			FunctionToken& ft = t.toFunction();
			const Function* func = fc.lookupFunction(ft.id());
			std::stringstream s;

			s << "Invalid number of arguments for function '";
			s << func->symbol() << "' (expecting ";

			if (func->isVariadic())
				s << " at least ";

			s << func->arity() << " arguments but got " << ft.arity() << ")";

			_exprLocation = t.location();
			_message = s.str();
		}
};

#endif
