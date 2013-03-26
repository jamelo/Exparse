/****************************************************
*	Exparse - Math expression evaluator library		*
* 	Copyright (C) 2013 Jordan Melo				 	*
****************************************************/

#ifndef	EXPRESSION_PARSER_H
#define	EXPRESSION_PARSER_H

#include	<string>
#include	<vector>
#include	<stack>
#include	<assert.h>
#include	<algorithm>

#include	"tokenizer.h"
#include	"argument_list.h"

class ExpressionParser
{
	private:
		std::string _expression;
		std::stack<Token> _shuntStack;
		std::vector<Token> _postfixString;
		std::vector<unsigned int> argumentIndexStack;
		std::vector<unsigned int> varDereferencerIndexes; //Keeps track of locations in postfix string to insert variable dereferencer function calls
		VariableContext& _variableContext;
		const FunctionContext& _functionContext;

		bool isPostfixStringBuilt; //For ensuring buildPostfixString() is only ever called once

		//TODO: document high level algorithm

	public:
		ExpressionParser(const std::string& expr, VariableContext& vc, const FunctionContext& fc) :
			_expression(expr),
			_variableContext(vc),
			_functionContext(fc),
			isPostfixStringBuilt(false)
		{
			//_shuntStack.reserve(20);
			//Reserve space for efficiency by reducing number of reallocations
			_postfixString.reserve(50);
			argumentIndexStack.reserve(10);
			varDereferencerIndexes.reserve(10);

			buildPostfixString();
		}

		void printPostfixString()
		{
			for (auto it = _postfixString.begin(); it != _postfixString.end(); ++it)
			{
				std::cout << Exparse::util::tokenToString(*it, _functionContext, _variableContext) << " ";
			}

			std::cout << std::endl;
		}

		double evaluate()
		{
			std::vector<Value> resultStack;

			resultStack.reserve(20);

			for (unsigned int i = 0; i < _postfixString.size(); i++)
			{
				Token t = _postfixString[i];

				switch (t.type())
				{
					case Token::NUMBER:
						resultStack.push_back(Value(t.toNumber().value()));
						break;
					case Token::VARIABLE:
						resultStack.push_back(Value(t.toVariable().id()));
						break;
					case Token::FUNCTION:
					case Token::OPERATOR:
					{
						//TODO: this code seems to be repeated a few times. clean it up?
						const Function* func = nullptr;
						unsigned int funcArity;

						switch (t.type())
						{
							case Token::OPERATOR:
								func = _functionContext.lookupOperator(t.toOperator().id());
								funcArity = func->arity();
								break;

							case Token::FUNCTION:
								func = _functionContext.lookupFunction(t.toFunction().id());
								funcArity = t.toFunction().arity();
								break;

							default: break;
						}

						ArgumentList args(resultStack.end() - funcArity, resultStack.end(), _variableContext);
						Value result = func->execute(args);

						//Pop arguments off the stack since they have been consumed
						resultStack.resize(resultStack.size() - funcArity);

						resultStack.push_back(result);

						break;
					}
					case Token::DELIMITER:
					{
						if (!resultStack.empty())
							resultStack.pop_back();

						assert(resultStack.empty());
						break;
					}
					default: assert(false);
				}
			}

			if (resultStack.empty())
				return 0.0;
			else
			{
				assert(resultStack.size() == 1);
				return resultStack.back().numeric;
			}
		}

	private:
		void buildPostfixString()
		{
			assert(!isPostfixStringBuilt);

			Tokenizer tokenizer(_expression, _functionContext, _variableContext);
			Token t;

			do
			{
				t = tokenizer.nextToken();

				if (t.type() == Token::END)
				{
					popStackUntilEmpty();

					//insert dereferencer for final result if it's an LVALUE, so that the numeric result may be printed
					if (_postfixString.back().type() != Token::DELIMITER &&
							determineHandedness(_postfixString.back()) == HAND_LVALUE)
						varDereferencerIndexes.push_back(_postfixString.size() - 1);

					break;
				}

				switch (t.type())
				{
					case Token::NUMBER:
					case Token::VARIABLE:
						argumentIndexStack.push_back(_postfixString.size());
						_postfixString.push_back(t);
						break;
					case Token::FUNCTION:
						//We're using the function token's arity here to temporarily store the argumentStack size
						//for later calculation of the actual arity from the size of the argumentStack after all arguments
						//are placed

						t.toFunction().setArity(argumentIndexStack.size());
						_shuntStack.push(t);
						break;
					case Token::DELIMITER:
					{
						const DelimiterToken& dt = t.toDelimiter();

						if (dt.type() == DelimiterToken::ARG_DELIM)
							popStackUntilLeftParen();
						else if (dt.type() == DelimiterToken::STATEMENT_DELIM)
						{
							popStackUntilEmpty();
							_postfixString.push_back(t);
						}

						break;
					}
					case Token::OPERATOR:
					{
						const OperatorToken& ot1 = t.toOperator();
						const Operator* op1 = _functionContext.lookupOperator(ot1.id());

						while (_shuntStack.size() > 0 && _shuntStack.top().type() == Token::OPERATOR)
						{
							const OperatorToken& ot2 = _shuntStack.top().toOperator();
							const Operator* op2 = _functionContext.lookupOperator(ot2.id());

							if ((op1->associativity() == Operator::ASSOC_LEFT && op1->precedence() >= op2->precedence()) || op1->precedence() > op2->precedence())
								consumeFunctionOnTopOfShunt();
							else
								break;
						}

						_shuntStack.push(t);
						break;
					}
					case Token::PARENTHESIS:
					{
						const ParenthesisToken& pt = t.toParenthesis();

						if (pt.type() == ParenthesisToken::ROUND_LEFT)
							_shuntStack.push(t);
						else if (pt.type() == ParenthesisToken::ROUND_RIGHT)
						{
							popStackUntilLeftParen();

							//Pop off the left parenthesis
							assert(_shuntStack.size() > 0);
							assert(_shuntStack.top().type() == Token::PARENTHESIS && _shuntStack.top().toParenthesis().type() == ParenthesisToken::ROUND_LEFT);
							_shuntStack.pop();

							if (_shuntStack.size() > 0 && _shuntStack.top().type() == Token::FUNCTION)
							{
								_shuntStack.top().toFunction().setArity(argumentIndexStack.size() - _shuntStack.top().toFunction().arity());
								consumeFunctionOnTopOfShunt();
							}
						}

						break;
					}

					default: assert(false);
				}
			} while (1);

			std::sort(varDereferencerIndexes.begin(), varDereferencerIndexes.end());
			insertVariableDereferencers();

			isPostfixStringBuilt = true;
		}

		void insertVariableDereferencers()
		{
			/*TODO: instead of reiterating through the postfix string and inserting variable calls to _deref
			at the end, consider checking arguments and inserting these calls as arguments are parsed.
			I don't think this would be easy since backtracking is involved*/

			std::vector<Token> newPostfixString;
			newPostfixString.reserve(_postfixString.size() + varDereferencerIndexes.size());

			for (unsigned int i = 0, vdi = 0; i < _postfixString.size(); i++)
			{
				newPostfixString.push_back(_postfixString[i]);

				if (vdi < varDereferencerIndexes.size() && i == varDereferencerIndexes[vdi])
				{
					//Insert call to hidden _deref function that returns the value stored in a variable
					newPostfixString.push_back(Token(FunctionToken(_functionContext.getFunctionID(std::string("_deref"))), -1));
					newPostfixString.back().toFunction().setArity(1);
					vdi++;
				}

			}

			_postfixString.swap(newPostfixString);
		}

		void popStackUntilEmpty()
		{
			//Pop operators off stack until stack is empty
			while (_shuntStack.size() > 0)
			{
				assert(_shuntStack.top().type() == Token::OPERATOR);

				consumeFunctionOnTopOfShunt();
			}
		}

		void popStackUntilLeftParen()
		{
			//Pop operators off stack until left parenthesis
			while (_shuntStack.top().type() != Token::PARENTHESIS || _shuntStack.top().toParenthesis().type() != ParenthesisToken::ROUND_LEFT)
			{
				assert(_shuntStack.size() > 0);
				assert(_shuntStack.top().type() == Token::OPERATOR);

				consumeFunctionOnTopOfShunt();
			}
		}

		void consumeFunctionOnTopOfShunt()
		{
			consumeArguments(_shuntStack.top());
			argumentIndexStack.push_back(_postfixString.size());
			_postfixString.push_back(_shuntStack.top());
			_shuntStack.pop();
		}

		void consumeArguments(Token& t)
		{
			unsigned int funcArity;
			const Function* func = nullptr;

			switch (t.type())
			{
				case Token::OPERATOR:
					func = _functionContext.lookupOperator(t.toOperator().id());
					funcArity = func->arity();
					break;

				case Token::FUNCTION:
					func = _functionContext.lookupFunction(t.toFunction().id());
					funcArity = t.toFunction().arity();
					break;

				default: assert(false);
			}

			assert(func != nullptr);
			assert(argumentIndexStack.size() >= funcArity);

			for (unsigned int i = 0, j = argumentIndexStack.size() - funcArity; i < funcArity; i++, j++)
			{
				unsigned int tokenIndex = argumentIndexStack[j];
				Token& argToken = _postfixString[tokenIndex];
				Handedness argHandedness = determineHandedness(argToken);

				if (argHandedness == HAND_LVALUE && func->argumentHandedness(i) == HAND_RVALUE)
					varDereferencerIndexes.push_back(tokenIndex);
				else if (argHandedness != func->argumentHandedness(i))
					throw InvalidArgumentException(t, i, _functionContext);
			}

			argumentIndexStack.resize(argumentIndexStack.size() - funcArity);
		}

		Handedness determineHandedness(Token& t)
		{
			switch (t.type())
			{
				case Token::OPERATOR:	return _functionContext.lookupOperator(t.toOperator().id())->returnValueHandedness();
				case Token::FUNCTION:	return _functionContext.lookupFunction(t.toFunction().id())->returnValueHandedness();
				case Token::VARIABLE:	return HAND_LVALUE;
				case Token::NUMBER:		return HAND_RVALUE;
				default:				assert(false);
			}

			return HAND_RVALUE;
		}
};

#endif
