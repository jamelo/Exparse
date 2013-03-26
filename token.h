/****************************************************
*	Exparse - Math expression evaluator library		*
* 	Copyright (C) 2013 Jordan Melo				 	*
****************************************************/

#ifndef TOKEN_H
#define TOKEN_H

#include	<string>
#include	<sstream>
#include	"context.h"

class NumberToken
{
	private:
		const double _value;

	public:
		NumberToken(double val) : _value(val) { }
		double value() const { return _value; }
};

class OperatorToken
{
	private:
		const unsigned int _id;

	public:
		OperatorToken(unsigned int opID) : _id(opID) { }
		unsigned int id() const { return _id; }
};

class VariableToken
{
	private:
		const unsigned int _id;

	public:
		VariableToken(unsigned int varID) : _id(varID) { }
		unsigned int id() const { return _id; }
};

class FunctionToken
{
	private:
		const unsigned int _id;
		unsigned int _arity;

	public:
		FunctionToken(unsigned int funcID) : _id(funcID), _arity(0) { }
		unsigned int id() const { return _id; }
		unsigned int arity() const { return _arity; }
		unsigned int setArity(unsigned int a) { return _arity = a; }

		void addArgument() { _arity++; }
};

class ParenthesisToken
{
	public:
		typedef enum ParenType
		{
			ROUND_LEFT,
			ROUND_RIGHT,
			SQUARE_LEFT,
			SQUARE_RIGHT,
			BRACE_LEFT,
			BRACE_RIGHT,
			ANGLE_LEFT,
			ANGLE_RIGHT
		} ParenType;

	private:
		const ParenType _type;

	public:
		ParenthesisToken(ParenType t) : _type(t) { }
		ParenType type() const { return _type; }
};

class DelimiterToken
{
	public:
		typedef enum DelimType
		{
			ARG_DELIM,
			STATEMENT_DELIM
		} DelimType;

	private:
		const DelimType _type;

	public:
		DelimiterToken(DelimType t) : _type(t) { }
		DelimType type() const { return _type; }
};

class Token
{
	public:
		typedef enum TokenType
		{
			NONE = 0,
			NUMBER,
			OPERATOR,
			VARIABLE,
			FUNCTION,
			PARENTHESIS,
			DELIMITER,
			END
		} TokenType;

	private:
		TokenType _type;			/**< The type of token in this container */
		unsigned int _location;		/**< Where this token resides in the original expression string */

		union
		{
			//This union creates a data field with the same size as the largest
			//token type so that it may accommodate any type of token.
			unsigned char _number[sizeof(NumberToken)];
			unsigned char _operator[sizeof(OperatorToken)];
			unsigned char _variable[sizeof(VariableToken)];
			unsigned char _function[sizeof(FunctionToken)];
			unsigned char _parenthesis[sizeof(ParenthesisToken)];
			unsigned char _delimiter[sizeof(DelimiterToken)];

			//This double is provided to give the data member an alignment
			//requirement of 8 bytes.
			double __forTheSakeOfAlignment;
		} _tokenData;

	public:
		Token() : _type(NONE) { }

		Token(NumberToken token, unsigned int tlocation) :
			_type(NUMBER) ,
			_location(tlocation)
		{
			new ((void*) &_tokenData) NumberToken(token);
		}


		Token(OperatorToken token, unsigned int tlocation) :
			_type(OPERATOR),
			_location(tlocation)
		{
			new ((void*) &_tokenData) OperatorToken(token);
		}


		Token(VariableToken token, unsigned int tlocation) :
			_type(VARIABLE),
			_location(tlocation)
		{
			new ((void*) &_tokenData) VariableToken(token);
		}

		Token(FunctionToken token, unsigned int tlocation) :
			_type(FUNCTION),
			_location(tlocation)
		{
			new ((void*) &_tokenData) FunctionToken(token);
		}

		Token(ParenthesisToken token, unsigned int tlocation) :
			_type(PARENTHESIS),
			_location(tlocation)
		{
			new ((void*) &_tokenData) ParenthesisToken(token);
		}

		Token(DelimiterToken token, unsigned int tlocation) :
			_type(DELIMITER),
			_location(tlocation)
		{
			new ((void*) &_tokenData) DelimiterToken(token);
		}

		Token(TokenType ttype)
		{
			assert(ttype == END);
			_type = ttype;
		}

		~Token()
		{
			switch(_type)
			{
				case NONE: 			break;
				case NUMBER: 		((NumberToken*		)&_tokenData)->~NumberToken();		break;
				case OPERATOR: 		((OperatorToken*	)&_tokenData)->~OperatorToken();	break;
				case VARIABLE: 		((VariableToken*	)&_tokenData)->~VariableToken();	break;
				case FUNCTION: 		((FunctionToken*	)&_tokenData)->~FunctionToken();	break;
				case PARENTHESIS: 	((ParenthesisToken*	)&_tokenData)->~ParenthesisToken();	break;
				case DELIMITER: 	((DelimiterToken*	)&_tokenData)->~DelimiterToken();	break;
				case END:			break;
			}
		}

		NumberToken& toNumber() 			{ assert(_type == NUMBER);	 	return *(NumberToken	 *) &_tokenData; }
		OperatorToken& toOperator() 		{ assert(_type == OPERATOR); 	return *(OperatorToken	 *) &_tokenData; }
		VariableToken& toVariable() 		{ assert(_type == VARIABLE); 	return *(VariableToken	 *) &_tokenData; }
		FunctionToken& toFunction() 		{ assert(_type == FUNCTION); 	return *(FunctionToken	 *) &_tokenData; }
		ParenthesisToken& toParenthesis()   { assert(_type == PARENTHESIS); return *(ParenthesisToken*) &_tokenData; }
		DelimiterToken& toDelimiter() 		{ assert(_type == DELIMITER); 	return *(DelimiterToken	 *) &_tokenData; }

		TokenType type() const 			{ return _type; }
		unsigned int location() const 	{ return _location; }

	friend class Tokenizer;
};

#endif
