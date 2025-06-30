#ifndef COMPILA_PROJECT_TOKEN_H
#define COMPILA_PROJECT_TOKEN_H


#include <string>
using namespace std;

class Token {
public:
    enum Type {
        ID, COMA, ASSIGN, PC,
        PI, PD,
        LLI, LLD,
        CI, CD,
        EQUAL, LT, DIFF, LET, GT, GET,
        FOR, WHILE, IF, ELSEIF, ELSE, DO, PRINTF, RETURN,
        AND, OR, PLUS, MINUS, MUL, DIV, NOT, DPLUS, DMINUS,
        TRUE, FALSE, INT, STRING, NUM, BOOLEAN,
        ERR, END,
        VOID
    };

    // LLI, LLD - llave izquierda, llave derecha
    // CI, CD - corchete izquierdo, corchete derecho
    // EQUAL - ==
    // LT - <
    // DIFF - !=
    // LET - <=
    // GT - >
    // GET - >=
    // DPLUS - "--"
    // DMINUS - "++"
    // NOT - !

    Type type;
    string text;

    Token(Type type);
    Token(Type type, char c);
    Token(Type type, const string& source, int first, int last);

    friend ostream& operator<<(ostream& outs, const Token& tok);
    friend ostream& operator<<(ostream& outs, const Token* tok);
};

#endif //COMPILA_PROJECT_TOKEN_H
