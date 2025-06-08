#ifndef COMPILA_PROJECT_SCANNER_H
#define COMPILA_PROJECT_SCANNER_H

# include "iostream"
#include "token.h"
using namespace std;

class Scanner {
private:
    string input;
    int first, current;
public:
    Scanner(const char* in_s);
    Token* nextToken();
    void reset();
    ~Scanner();
};

void test_scanner(Scanner* scanner);

#endif //COMPILA_PROJECT_SCANNER_H
