#ifndef COMPILA_PROJECT_ENVIRONMENT_HH
#define COMPILA_PROJECT_ENVIRONMENT_HH

#include <unordered_map>
#include <list>
#include <vector>
#include <string>
#include <iostream>

using namespace std;
struct VarDeclaration {
    string type;  // Tipo (ej. "int", "bool", "string")
    vector<string> vars;  // Variables declaradas de este tipo

    // Busca el tipo de una variable en esta declaración
    string encontrar_tipo(string var) {
        for (auto& i : vars) {
            if (i == var) {
                return type;
            }
        }
        return "";
    }
};

class Environment {
public:
    vector<unordered_map<string, pair<int,string>>> levels;  // Almacena valores de variables
    vector<unordered_map<string, string>> type_levels;  // Almacena tipos de variables

    // Busca el nivel en el que está una variable
    int search_rib(string var) {
        int idx = levels.size() - 1;
        while (idx >= 0) {
            if (levels[idx].find(var) != levels[idx].end()) {
                return idx;
            }
            idx--;
        }
        return -1;
    }

public:
    Environment() {}

    void clear() {
        levels.clear();
        type_levels.clear();
    }

    // Añadir un nuevo nivel
    void add_level() {
        unordered_map<string, pair<int,string>> l;
        unordered_map<string, string> t;  // Mapa para tipos
        levels.push_back(l);
        type_levels.push_back(t);
    }

    // Añadir una variable con su valor y tipo
    void add_var(string var, int value, string type) {
        if (levels.size() == 0) {
            cout << "Environment sin niveles: no se pueden agregar variables" << endl;
            exit(0);
        }

        levels.back()[var].first = value;
        type_levels.back()[var] = type;
    }
    void add_var(string var, string value, string type) {
        if (levels.size() == 0) {
            cout << "Environment sin niveles: no se pueden agregar variables" << endl;
            exit(0);
        }

        levels.back()[var].second = value;
        type_levels.back()[var] = type;
    }
    // Añadir una variable sin valor inicial
    void add_var(string var, string type) {
        if(type=="int" or type=="bool") {
            levels.back()[var].first = 0;
        } // Valor por defecto
        else if(type=="string")
            levels.back()[var].second = "";
        type_levels.back()[var] = type;
    }

    // Remover un nivel
    bool remove_level() {
        if (levels.size() > 0) {
            levels.pop_back();
            type_levels.pop_back();
            return true;
        }
        return false;
    }

    // Actualizar el valor de una variable
    bool update(string x, int v) {
        int idx = search_rib(x);
        if (idx < 0) return false;
        levels[idx][x].first = v;
        return true;
    }
    bool update(string x, string v) {
        int idx = search_rib(x);
        if (idx < 0) return false;
        levels[idx][x].second = v;
        return true;
    }
    // Verificar si una variable está declarada
    bool check(string x) {
        int idx = search_rib(x);
        return (idx >= 0);
    }

    // Obtener el valor de una variable
    pair<int, string> lookup(string x) {
        int idx = search_rib(x);
        if (idx < 0) {
            cout << "Variable no declarada: " << x << endl;
            exit(0);
        }
        return levels[idx][x];
    }

    // Obtener el tipo de una variable
    string lookup_type(string x) {
        int idx = search_rib(x);
        if (idx < 0) {
            cout << "Variable no declarada: " << x << endl;
            exit(0);
        }
        return type_levels[idx][x];
    }

    // Verificar el tipo de una variable antes de asignar un valor
    bool typecheck(string var, string expected_type) {
        string actual_type = lookup_type(var);
        if (actual_type != expected_type) {
            cout << "Error de tipo: se esperaba " << expected_type << " pero se encontró " << actual_type << " para la variable " << var << endl;
            return false;
        }
        return true;
    }

    void debug_print() {
        cout << "Numero de niveles: " << levels.size() << endl;
        for (size_t i = 0; i < levels.size(); ++i) {
            cout << "Nivel " << i << ":" << endl;
            if (levels[i].empty()) {
                cout << "  (vacio)" << endl;
            } else {
                for (const auto& entry : levels[i]) {
                    string tipo =  type_levels[i][entry.first];
                    cout << "  " << entry.first << " (" << tipo << ") = ";
                    if (tipo == "int") {
                        cout << entry.second.first;
                    } else if (tipo == "bool") {
                        cout << (entry.second.first ? "true" : "false");
                    } else if (tipo == "string") {
                        cout << "\"" << entry.second.second << "\"";
                    }
                    cout << endl;
                }
            }
        }
        cout << "--------------" << endl;
    }
};

#endif //COMPILA_PROJECT_ENVIRONMENT_HH
