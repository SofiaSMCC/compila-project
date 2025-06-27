#include "imp_value.h"

ImpValue::ImpValue() : type(NONE) {}
ImpValue::ImpValue(ImpType tipo, int valor, bool bol, string str_valor)
        : type(tipo), int_value(valor), bool_value(bol), string_value(str_valor), array_value() {}

ImpValue::~ImpValue() {}

void ImpValue::set_default_value(ImpType tt) {
    type = tt;
    int_value = 0;
    bool_value = false;
    string_value = "";
    array_value.clear();
    if (tt == ARRAY) {
        array_value = vector<ImpValue>();
    }
}

string ImpValue::getTypeName() const {
    switch (type) {
        case INT: return "INT";
        case BOOL: return "BOOL";
        case STRING: return "STRING";
        case ARRAY: return "ARRAY";
        case NONE: return "NONE";
        default: return "UNKNOWN";
    }
}