#ifndef COMPILA_PROJECT_IMP_VALUE_H
#define COMPILA_PROJECT_IMP_VALUE_H

#include <iostream>
#include <vector>
using namespace std;

enum ImpType { INT, BOOL, STRING, ARRAY, NONE };

class ImpValue {
public:
    ImpType type;
    int int_value;
    bool bool_value;
    string string_value;
    vector<ImpValue> array_value;

    ImpValue();
    ImpValue(ImpType tipo, int valor, bool bol, string str_valor);
    ~ImpValue();

    void set_default_value(ImpType tt);
    string getTypeName() const;
};

#endif //COMPILA_PROJECT_IMP_VALUE_H