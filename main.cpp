#include <iostream>
#include <fstream>
#include <string>
#include "scanner.h"
#include "parser.h"
#include "type_visitor.h"
#include "environment.h"
#include "eval_visitor.h"
#include "codegen.h"
#include "visitor.h"

using namespace std;

int main(int argc, const char* argv[]) {
    if (argc < 2 || argc > 3) {
        cout << "Numero incorrecto de argumentos. Uso: " << argv[0] << " <archivo_de_entrada> [archivo_de_salida]" << endl;
        exit(1);
    }

    ifstream infile(argv[1]);
    if (!infile.is_open()) {
        cout << "No se pudo abrir el archivo: " << argv[1] << endl;
        exit(1);
    }

    string input;
    string line;
    while (getline(infile, line)) {
        input += line + '\n';
    }
    infile.close();

    Scanner scanner(input.c_str());

    string input_copy = input;
    Scanner scanner_test(input_copy.c_str());
    test_scanner(&scanner_test);
    cout << "Scanner exitoso" << endl;
    cout << endl;

    cout << "Iniciando parsing:" << endl;
    Parser parser(&scanner);

    try {
        Program* program = parser.ParseProgram();
        string outputFilename;
        if (argc == 3) {
            outputFilename = argv[2];
        } else {
            string inputFile(argv[1]);
            size_t dotPos = inputFile.find_last_of('.');
            string baseName = (dotPos == string::npos) ? inputFile : inputFile.substr(0, dotPos);
            outputFilename = baseName + ".s";
        }
        ofstream outfile(outputFilename);
        cout << "Parsing exitoso" << endl << endl;
        PrintVisitor printer;
        printer.imprimir(program);
        cout << "Print visitor exitoso" << endl << endl;
        Environment env;
        TypeVisitor typeVisitor(&env);
        typeVisitor.check(program);
        cout << "Type visitor exitoso" << endl << endl;
        EVALVisitor evalVisitor(env);
        evalVisitor.ejecutar(program);
        cout << "Eval visitor exitoso" << endl << endl;
        if (!outfile.is_open()) {
            cerr << "Error al crear el archivo de salida: " << outputFilename << endl;
            return 1;
        }
        cout << "Generando codigo ensamblador en " << outputFilename << endl;
        GenCode codigo(outfile);
        codigo.generar(program);
        outfile.close();
        delete program;
    } catch (const exception& e) {
        cout << "Error durante la ejecución: " << e.what() << endl;
        return 1;
    }
    return 0;
}