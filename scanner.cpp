#include "scanner.h"
#include <cstring>

Scanner::Scanner(const char* s):input(s),first(0), current(0) { }

Token* Scanner::nextToken() {
    Token* token;
    while (current < input.length() && isspace(input[current])) {
        current++;
    }

    if (current < input.length() && input[current] == '#') {
        while (current < input.length() && input[current] != '\n') {
            current++;
        }
        return nextToken();
    }

    if (current >= input.length()) return new Token(Token::END);

    char c  = input[current];
    first = current;

    if (isdigit(c)) {
        current++;
        while (current < input.length() && isdigit(input[current]))
            current++;
        token = new Token(Token::NUM, input, first, current - first);
    } else if (isalpha(c)) {
        current++;
        while (current < input.length() && isalnum(input[current]))
            current++;
        string word = input.substr(first, current - first);

        if (word == "else") {
            size_t temp = current;
            while (temp < input.length() && isspace(input[temp])) temp++;
            if (temp + 1 < input.length() &&
                input[temp] == 'i' && input[temp + 1] == 'f' &&
                (temp + 2 == input.length() || !isalnum(input[temp + 2]))) {
                token = new Token(Token::ELSEIF, "elseif", 0, 6);
                current = temp + 2;
                return token;
            }
        }

        if (word == "for") token = new Token(Token::FOR, word, 0, word.length());
        else if (word == "if") token = new Token(Token::IF, word, 0, word.length());
        else if (word == "void") token = new Token(Token::VOID, word, 0, word.length());
        else if (word == "while") token = new Token(Token::WHILE, word, 0, word.length());
        else if (word == "do") token = new Token(Token::DO, word, 0, word.length());
        else if (word == "printf") token = new Token(Token::PRINTF, word, 0, word.length());
        else if (word == "else") token = new Token(Token::ELSE, word, 0, word.length());
        else if (word == "return") token = new Token(Token::RETURN, word, 0, word.length());
        else if (word == "true") token = new Token(Token::TRUE, word, 0, word.length());
        else if (word == "false") token = new Token(Token::FALSE, word, 0, word.length());
        else if (word == "int") token = new Token(Token::INT, word, 0, word.length());
        else if (word == "bool") token = new Token(Token::BOOLEAN, word, 0, word.length());
        else if (word == "string") token = new Token(Token::STRING, word, 0, word.length());
        else token = new Token(Token::ID, word, 0, word.length());

    } else if (strchr("+-*/(){}=;!,|&<>[]", c)) {

        if (c == '&' && current + 1 < input.length() && input[current + 1] == '&') {
            token = new Token(Token::AND, "&&", 0, 2);
            current += 2;
            return token;
        }

        if (c == '|' && current + 1 < input.length() && input[current + 1] == '|') {
            token = new Token(Token::OR, "||", 0, 2);
            current += 2;
            return token;
        }

        switch(c) {

            case '+':
                if (current + 1 < input.length() && input[current + 1] == '+') {
                    token = new Token(Token::DPLUS, "++", 0, 2);
                    current++;
                } else {
                    token = new Token(Token::PLUS, c);
                }
                break;

            case '-':
                if (current + 1 < input.length() && input[current + 1] == '-') {
                    token = new Token(Token::DMINUS, "--", 0, 2);
                    current++;
                } else {
                    token = new Token(Token::MINUS, c);
                }
                break;

            case '*': token = new Token(Token::MUL, c); break;
            case '/': token = new Token(Token::DIV, c); break;

            case '(': token = new Token(Token::PI, c); break;
            case ')': token = new Token(Token::PD, c); break;
            case '[': token = new Token(Token::CI, c); break;
            case ']': token = new Token(Token::CD, c); break;
            case '{': token = new Token(Token::LLI, c); break;
            case '}': token = new Token(Token::LLD, c); break;

            case '=':
                if (current + 1 < input.length() && input[current + 1] == '=') {
                    token = new Token(Token::EQUAL, "==", 0, 2);
                    current++;
                } else {
                    token = new Token(Token::ASSIGN, c);
                }
                break;

            case '!':
                if (current + 1 < input.length() && input[current + 1] == '=') {
                    token = new Token(Token::DIFF, "!=", 0, 2);
                    current++;
                } else {
                    token = new Token(Token::NOT, c);
                }
                break;

            case '>':
                if (current + 1 < input.length() && input[current + 1] == '=') {
                    token = new Token(Token::GET, ">=", 0, 2);
                    current++;
                } else {
                    token = new Token(Token::GT, c);
                }
                break;

            case '<':
                if (current + 1 < input.length() && input[current + 1] == '=') {
                    token = new Token(Token::LET, "<=", 0, 2);
                    current++;
                } else {
                    token = new Token(Token::LT, c);
                }
                break;

            case ';': token = new Token(Token::PC, c); break;
            case ',': token = new Token(Token::COMA, c); break;

            default:
                cout << "No debería llegar acá" << endl;
                token = new Token(Token::ERR, c);
        }
        current++;
    } else if (c == '"') {
        current++;
        first = current;
        while (current < input.length() && input[current] != '"') {
            current++;
        }

        if (current < input.length()) {
            token = new Token(Token::STRING, input, first, current - first);
            current++;
        } else {
            token = new Token(Token::ERR, "\"", 0, 1);
        }
    }
    else {
        token = new Token(Token::ERR, c);
        current++;
    }
    return token;
}

void Scanner::reset() {
    first = 0;
    current = 0;
}

Scanner::~Scanner() = default;

void test_scanner(Scanner* scanner) {
    Token* current;
    cout << "Iniciando Scanner:" << endl << endl;
    while ((current = scanner->nextToken())->type != Token::END) {
        if (current->type == Token::ERR) {
            cout << "Error en scanner - carácter inválido: " << current->text << endl;
            break;
        } else {
            cout << *current << endl;
        }
        delete current;
    }
    cout << "TOKEN(END)" << endl;
    delete current;
}