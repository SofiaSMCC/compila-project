#ifndef COMPILA_PROJECT_IMP_VALUE_H
#define COMPILA_PROJECT_IMP_VALUE_H

#include <iostream>
#include <vector>
using namespace std;

class ImpValue {
public:
    string type;
    int int_value;
    bool bool_value;
    string string_value;
    vector<ImpValue> array_value;

    ImpValue();
    ImpValue(string tipo, int valor, bool bol, string str_valor);
    ~ImpValue();

    static string get_basic_type(string s);
};

#endif //COMPILA_PROJECT_IMP_VALUE_H