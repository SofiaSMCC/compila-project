#include <iostream>
#include "token.h"

using namespace std;

Token::Token(Type type):type(type) { text = ""; }

Token::Token(Type type, char c):type(type) { text = string(1, c); }

Token::Token(Type type, const string& source, int first, int last):type(type) {
    text = source.substr(first, last);
}

std::ostream& operator << ( std::ostream& outs, const Token & tok )
{
    switch (tok.type) {
        case Token::PLUS: outs << "TOKEN(PLUS)"; break;
        case Token::MINUS: outs << "TOKEN(MINUS)"; break;
        case Token::MUL: outs << "TOKEN(MUL)"; break;
        case Token::DIV: outs << "TOKEN(DIV)"; break;
        case Token::NUM: outs << "TOKEN(NUM)"; break;
        case Token::ERR: outs << "TOKEN(ERR)"; break;
        case Token::PD: outs << "TOKEN(PD)"; break;
        case Token::PI: outs << "TOKEN(PI)"; break;
        case Token::END: outs << "TOKEN(END)"; break;
        case Token::ID: outs << "TOKEN(ID)"; break;
        case Token::ASSIGN: outs << "TOKEN(ASSIGN)"; break;
        case Token::VOID: outs << "TOKEN(VOID)"; break;
        case Token::PC: outs << "TOKEN(PC)"; break;
        case Token::TRUE: outs << "TOKEN(TRUE)"; break;
        case Token::FALSE: outs << "TOKEN(FALSE)"; break;
        case Token::INT: outs << "TOKEN(INT)"; break;
        case Token::STRING: outs << "TOKEN(STRING)"; break;
        case Token::NOT: outs << "TOKEN(NOT)"; break;
        case Token::DPLUS: outs << "TOKEN(DPLUS)"; break;
        case Token::DMINUS: outs << "TOKEN(DMINUS)"; break;
        case Token::OR: outs << "TOKEN(OR)"; break;
        case Token::AND: outs << "TOKEN(AND)"; break;
        case Token::RETURN: outs << "TOKEN(RETURN)"; break;
        case Token::PRINTF: outs << "TOKEN(PRINTF)"; break;
        case Token::DO: outs << "TOKEN(DO)"; break;
        case Token::ELSEIF: outs << "TOKEN(ELSEIF)"; break;
        case Token::ELSE: outs << "TOKEN(ELSE)"; break;
        case Token::IF: outs << "TOKEN(IF)"; break;
        case Token::WHILE: outs << "TOKEN(WHILE)"; break;
        case Token::FOR: outs << "TOKEN(FOR)"; break;
        case Token::GT: outs << "TOKEN(GT)"; break;
        case Token::GET: outs << "TOKEN(GET)"; break;
        case Token::LET: outs << "TOKEN(LET)"; break;
        case Token::DIFF: outs << "TOKEN(DIFF)"; break;
        case Token::LT: outs << "TOKEN(LT)"; break;
        case Token::EQUAL: outs << "TOKEN(EQUAL)"; break;
        case Token::CD: outs << "TOKEN(CD)"; break;
        case Token::CI: outs << "TOKEN(CI)"; break;
        case Token::LLD: outs << "TOKEN(LLD)"; break;
        case Token::LLI: outs << "TOKEN(LLI)"; break;
        case Token::COMA: outs << "TOKEN(COMA)"; break;
        case Token::BOOLEAN: outs << "TOKEN(BOOLEAN)"; break;
        default: outs << "TOKEN(UNKNOWN)"; break;
    }
    return outs;
}

std::ostream& operator << ( std::ostream& outs, const Token* tok ) {
    return outs << *tok;
}