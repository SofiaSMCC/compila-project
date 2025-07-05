#include "imp_value.h"

ImpValue::ImpValue() : type("none") {}
ImpValue::ImpValue(string tipo, int valor, bool bol, string str_valor)
        : type(tipo), int_value(valor), bool_value(bol), string_value(str_valor), array_value() {}

ImpValue::~ImpValue() {}

#include <algorithm>
#include <cctype>

string ImpValue::get_basic_type(string s) {

    while(!s.empty() && isspace(s.front())) s.erase(s.begin());
    while(!s.empty() && isspace(s.back())) s.pop_back();

    std::transform(s.begin(), s.end(), s.begin(), ::tolower);

    if (s == "int" || s == "integer"){
        return "int";
    }
    if (s == "char"){
        return "char";
    }
    if (s == "bool" || s == "boolean"){
        return "bool";
    }
    return "none";
}