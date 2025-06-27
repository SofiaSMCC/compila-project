#ifndef COMPILA_PROJECT_PARSER_H
#define COMPILA_PROJECT_PARSER_H

#include "scanner.h"
#include "exp.h"

class Parser {
private:
    Scanner* scanner;
    Token *current, *previous;
    bool match(Token::Type ttype);
    bool check(Token::Type ttype);
    bool advance();
    bool isAtEnd();
    bool checkTypeToken();
    Exp* parseAExp();
    Exp* parseBExp();
    Exp* parseCExp();
    Exp* parseExpression();
    Exp* parseTerm();
    Exp* parseFactor();
public:
    Parser(Scanner* scanner);
    Body* ParseBody();
    Program* ParseProgram();
    StatementList* ParseStatementList();
    Stm* ParseStatement();
    InitValue* ParseInitValue();
    Var* ParseVar();
    VarDec* ParseVarDec();
    VarDecList* ParseVarDecList();
    FunDecList* ParseFunDecList();
    FunDec* ParseFunDec();
    ~Parser();
};

#endif //COMPILA_PROJECT_PARSER_H
