/****************************************************
*	Exparse - Math expression evaluator library		*
* 	Copyright (C) 2013 Jordan Melo				 	*
****************************************************/

#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>
#include <sstream>
#include <queue>
#include <stack>

#include "function.h"
#include "argument_list.h"
#include "context.h"
#include "variable.h"
#include "tokenizer_exception.h"
#include "token.h"
#include "exputil.h"

#define		WHITESPACE_CHARS	" \t\f\r\n"

class Tokenizer
{
	private:
		std::string expressionString;
		unsigned int location;
		Token lastToken;
		const FunctionContext& functionContext;
		VariableContext& variableContext;
		std::queue<Token> _tokenQueue;		/* For implementation of lexer */
		std::stack<Token> _functionStack;	/* For implementation of lexer */

	public:
		Tokenizer(std::string expression, const FunctionContext& fc, VariableContext& vc) :
			expressionString(expression),
			location(0),
			lastToken(),
			functionContext(fc),
			variableContext(vc)
		{ }

		~Tokenizer() { }

		bool hasNext()
		{
			return false;
		}

		std::string parse()
		{
			Token t;
			std::stringstream output;

			do
			{
				t = nextToken();

				if (t.type() == Token::END)
					break;

				output << Exparse::util::tokenToString(t, functionContext, variableContext) << " ";
			} while (1);

			return output.str();
		}

		Token nextToken()
		{
			Token t;

			//Check for preprocessed tokens
			if (_tokenQueue.size() > 0)
			{
				t = _tokenQueue.front();
				_tokenQueue.pop();
				return t;
			}

			do
			{
				t = parseToken(expressionString, location);

				switch (lastToken.type())
				{
					case Token::PARENTHESIS:
						//Check for operator if right paren
						if (lastToken.toParenthesis().type() == ParenthesisToken::ROUND_RIGHT)
						{
							if (t.type() == Token::END)	break;
							if (t.type() == Token::PARENTHESIS && consumeParenthesis(t, ParenthesisToken::ROUND_RIGHT)) break;
							if (t.type() == Token::DELIMITER && consumeDelimiter(t)) break;
							if (t.type() == Token::OPERATOR && consumeOperator(t, Operator::POS_INFIX | Operator::POS_POSTFIX)) break;

							throw UnexpectedTokenException(t);
						}

						//Fall through if last token is left round parenthesis

					case Token::NONE:
					case Token::DELIMITER:
						if (t.type() == Token::END)	break;
						if (t.type() == Token::PARENTHESIS && consumeParenthesis(t, ParenthesisToken::ROUND_LEFT)) break;
						if (t.type() == Token::PARENTHESIS && consumeParenthesis(t, ParenthesisToken::ROUND_RIGHT)) break;
						if (t.type() == Token::NUMBER) break;
						if (t.type() == Token::FUNCTION && consumeFunction(t)) break;
						if (t.type() == Token::VARIABLE) break;
						if (t.type() == Token::OPERATOR && consumeOperator(t, Operator::POS_PREFIX)) break;

						throw UnexpectedTokenException(t);

					case Token::NUMBER:
					case Token::VARIABLE:
						if (t.type() == Token::END)	break;
						if (t.type() == Token::PARENTHESIS && consumeParenthesis(t, ParenthesisToken::ROUND_RIGHT)) break;
						if (t.type() == Token::DELIMITER && consumeDelimiter(t)) break;
						if (t.type() == Token::OPERATOR && consumeOperator(t, Operator::POS_INFIX | Operator::POS_POSTFIX)) break;

						throw UnexpectedTokenException(t);

					case Token::OPERATOR:
					{
						Operator::Positioning lastType = functionContext.lookupOperator(lastToken.toOperator().id())->position();

						if (lastType == Operator::POS_INFIX || lastType == Operator::POS_PREFIX)
						{
							if (t.type() == Token::PARENTHESIS && consumeParenthesis(t, ParenthesisToken::ROUND_LEFT)) break;
							if (t.type() == Token::NUMBER) break;
							if (t.type() == Token::FUNCTION && consumeFunction(t)) break;
							if (t.type() == Token::VARIABLE) break;
							if (t.type() == Token::OPERATOR && consumeOperator(t, Operator::POS_PREFIX)) break;
						}
						else if (lastType == Operator::POS_POSTFIX)
						{
							if (t.type() == Token::END)	break;
							if (t.type() == Token::DELIMITER && consumeDelimiter(t)) break;
							if (t.type() == Token::PARENTHESIS && consumeParenthesis(t, ParenthesisToken::ROUND_RIGHT)) break;
							if (t.type() == Token::OPERATOR && consumeOperator(t, Operator::POS_INFIX | Operator::POS_POSTFIX)) break;
						}

						throw UnexpectedTokenException(t);
					}

					case Token::FUNCTION:
						if (t.type() == Token::PARENTHESIS && consumeParenthesis(t, ParenthesisToken::ROUND_LEFT)) break;

						throw UnexpectedTokenException(t);

					case Token::END:
						assert(false);
						break;
				}

				lastToken = t;
				_tokenQueue.push(t);
			} while (!_functionStack.empty());

			t = _tokenQueue.front();
			_tokenQueue.pop();

			assert( !(t.type() == Token::END && _functionStack.size() > 0));

			return t;
		}

	private:
		bool isWhitespace(char c)
		{
			static std::string whitespace(WHITESPACE_CHARS);

			for (unsigned int i = 0; i < whitespace.length(); i++)
				if (c == whitespace[i]) return true;

			return false;
		}

		bool isDigit(char c)
		{
			return (c >= '0' && c <= '9');
		}

		bool isIdentifierChar(char c)
		{
			return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_';
		}

		void consumeWhitespace(const std::string& expr, unsigned int& index)
		{
			while (index < expr.length())
			{
				if (isWhitespace(expr[index]))
					index++;
				else
					return;
			}
		}

		Token parseDelimiter(const std::string& expr, unsigned int& index)
		{
			Token t;

			switch (expr[index])
			{
				case ';':
					t = Token(DelimiterToken(DelimiterToken::STATEMENT_DELIM), index);
					index++;
					break;
				case ',':
					t = Token(DelimiterToken(DelimiterToken::ARG_DELIM), index);
					index++;
					break;
			}

			return t;
		}

		Token parseParenthesis(const std::string& expr, unsigned int& index)
		{
			Token t;

			switch (expr[index])
			{
				case '(':
					t = Token(ParenthesisToken(ParenthesisToken::ROUND_LEFT), index);
					index++;
					break;
				case ')':
					t = Token(ParenthesisToken(ParenthesisToken::ROUND_RIGHT), index);
					index++;
					break;
			}

			return t;
		}

		Token parseNumber(const std::string& expr, unsigned int& index)
		{
			unsigned int i = index;
			bool wholePart = false;
			bool fractionalPart = false;
			bool decimalPoint = false;

			assert(i < expr.length());

			//Consume digits that make up whole part
			if (isDigit(expr[i]))
			{
				i++;
				while (isDigit(expr[i])) { i++; }
				wholePart = true;
			}

			//Consume decimal point
			if (expr[i] == '.')
			{
				i++;
				decimalPoint = true;
			}

			//Consume fractional part only if decimal point exists
			if (decimalPoint && isDigit(expr[i]))
			{
				i++;
				while (isDigit(expr[i])) { i++; }
				fractionalPart = true;
			}

			//The number must have either a whole part or fractional part or both
			//If neither, return nothing
			if (!fractionalPart && !wholePart) return Token();

			if (expr[i] == 'E' || expr[i] == 'e')
			{
				//if (!fractionalPart)
				//	throw MalformedNumberException(expr.substr(index, i - index), index);

				i++;

				if (expr[i] == '+' || expr[i] == '-') i++;

				if (isDigit(expr[i]))
				{
					i++;
					while (isDigit(expr[i])) { i++; }
				}
				else
					throw MalformedNumberException(expr.substr(index, i - index), index);
			}

			//Extract double from string stream, store in token
			std::istringstream tokenStream(expr.substr(index, i - index));
			double numberValue;
			tokenStream >> numberValue;

			if (!tokenStream)
				throw MalformedNumberException(expr.substr(index, i - index), index);

			Token token(NumberToken(numberValue), index);
			index = i;

			return token;
		}

		/** \brief This function extracts an identifier string from the expression.
		 *
		 * \param[in] 		expr	The expression string to parse.
		 * \param[in/out] 	index	The location in the string at which to start the search.
		 *							Output will be the location in which the search ended.
		 * \param[out]				Whether or not the function determined the identifier to be
		 *							a function.
		 *
		 * \return		The extracted identifier in string form.
		 *
		 */

		Token parseIdentifier(const std::string& expr, unsigned int& index)
		{
			unsigned int i = index;
			unsigned int i2;
			std::string tokenString;

			assert(i < expr.length());

			if (isIdentifierChar(expr[i]))
			{
				i++;

				while (isIdentifierChar(expr[i]) || isDigit(expr[i])) i++;
			}
			else return Token();

			tokenString = expr.substr(index, i - index);

			//Save current position. The remaining code is for lookahead to determine if function or variable
			i2 = i;

			while (isWhitespace(expr[i2])) i2++;

			Token t;

			if (expr[i2] == '(')
			{
				//Identifier is function
				unsigned int functionID = functionContext.getFunctionID(tokenString);

				if (!functionID) throw UnknownFunctionException(tokenString, index); //Function not found

				t = Token(FunctionToken(functionID), index);
				index = i;
			}
			else
			{
				//Identifier is variable
				unsigned int variableID = variableContext.getId(tokenString);
				t = Token(VariableToken(variableID), index);
				index = i;
			}

			return t;
		}

		Token parseOperator(const std::string& expr, unsigned int& index, int types)
		{
			unsigned int i = index;
			unsigned int operatorID = functionContext.parseOperator(expr, i, types);

			if (operatorID == NULLID) return Token();

			Token t(OperatorToken(operatorID), index);
			index = i;

			return t;
		};

		Token parseToken(const std::string& expr, unsigned int& index)
		{
			Token t;

			consumeWhitespace(expr, index);

			//Parse end
			if (index == expr.length())
			return Token(Token::END);

			t = parseParenthesis(expr, index);
			if (t.type() != Token::NONE) return t;

			t = parseDelimiter(expr, index);
			if (t.type() != Token::NONE) return t;

			t = parseNumber(expr, index);
			if (t.type() != Token::NONE) return t;

			t = parseIdentifier(expr, index);
			if (t.type() != Token::NONE) return t;

			int opTypeToFind = Operator::POS_INFIX | Operator::POS_POSTFIX | Operator::POS_PREFIX;

			switch (lastToken.type())
			{
				case Token::PARENTHESIS:
					if (lastToken.toParenthesis().type() == ParenthesisToken::ROUND_RIGHT)
					{
						opTypeToFind = Operator::POS_INFIX | Operator::POS_POSTFIX;
						break;
					}
					//Fall through if last token is left round parenthesis
				case Token::NONE:
				case Token::DELIMITER:
					opTypeToFind = Operator::POS_PREFIX; break;
				case Token::NUMBER:
				case Token::VARIABLE:
					opTypeToFind = Operator::POS_INFIX | Operator::POS_POSTFIX; break;
				case Token::OPERATOR:
				{
					Operator::Positioning lastType = functionContext.lookupOperator(lastToken.toOperator().id())->position();

					if (lastType == Operator::POS_INFIX || lastType == Operator::POS_PREFIX)
					{
						opTypeToFind = Operator::POS_PREFIX;
						break;
					}
					else if (lastType == Operator::POS_POSTFIX)
					{
						opTypeToFind = Operator::POS_INFIX | Operator::POS_POSTFIX;
						break;
					}
				}

				default: break;
			}

			t = parseOperator(expr, index, opTypeToFind);
			if (t.type() != Token::NONE) return t;

			return Token(index);
		}

		bool consumeDelimiter(Token& t)
		{
			if (t.toDelimiter().type() == DelimiterToken::STATEMENT_DELIM && _functionStack.size() > 0)
				throw UnexpectedTokenException(t);

			if (t.toDelimiter().type() == DelimiterToken::ARG_DELIM)
			{
				if (_functionStack.size() > 0 && _functionStack.top().type() == Token::FUNCTION)
					_functionStack.top().toFunction().addArgument();
				else if (_functionStack.size() != 0)
					throw UnexpectedTokenException(t);
			}

			return true;
		}

		bool consumeParenthesis(Token& t, ParenthesisToken::ParenType requiredType)
		{
			if (t.toParenthesis().type() != requiredType) return false;

			switch (requiredType)
			{
				case ParenthesisToken::ROUND_RIGHT:
					if (_functionStack.size() > 0)
					{
						if (_functionStack.top().type() != Token::FUNCTION)
						{
							_functionStack.pop();
							break;
						}

						Token& funcToken = _functionStack.top();

						if (lastToken.type() != Token::PARENTHESIS ||
								lastToken.toParenthesis().type() != ParenthesisToken::ROUND_LEFT)
							funcToken.toFunction().addArgument();

						if (!validateNumArgs(funcToken)) throw InvalidNumArgumentsException(funcToken, functionContext);

						_functionStack.pop();
					}
					else
						throw UnmatchedParenthesisException(t);
					break;
				case ParenthesisToken::ROUND_LEFT:
					if (lastToken.type() != Token::FUNCTION)
						_functionStack.push(t);
					break;
				default: break;
			}

			return true;
		}

		bool consumeOperator(Token& t, int requiredTypes)
		{
			const Operator* op = functionContext.lookupOperator(t.toOperator().id());
			if ((op->position() & requiredTypes) == 0) throw UnexpectedTokenException(t);
			return true;
		}

		bool consumeFunction(Token& t)
		{
			_functionStack.push(t);
			return true;
		}

		bool validateNumArgs(Token& t)
		{
			const FunctionToken& ft = t.toFunction();
			const Function* func = functionContext.lookupFunction(ft.id());

			if (ft.arity() == func->arity()) return true;
			if (ft.arity() > func->arity() && func->isVariadic()) return true;

			return false;
		}

};

#endif
