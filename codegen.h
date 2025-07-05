#ifndef COMPILA_PROJECT_CODEGEN_HH
#define COMPILA_PROJECT_CODEGEN_HH
#include "visitor.h"
#include "stack"
#include <map>
using namespace std;

class GenCode : public Visitor {
private:
    ostream &out;
    unordered_map<string, int> memoria;

    // arrays
    unordered_map<string, int> tamaniosArray;     // total elementos
    unordered_map<string, vector<int>> dimsArray; // dimensiones
    unordered_map<string, bool> isArray;

    // strings
    map<string, string> stringLiterals;      // label → contenido
    map<string, string> literalToLabel;      // contenido → label
    int stringLabelCounter = 0;
    bool primeraPasada = true;
    // manejar memoria
    stack<unordered_map<string, int>> scopeStack;
    stack<int> offsetStack;
    string registrarStringLiteral(const std::string& val);
    string escapeString(const std::string& raw);
    int offset = -8;
    int labelcont = 0;
    bool entornoFuncion = false;
    int numeroParametros = 0;
    string nombreFuncion;

public:
    GenCode(ostream& out) : out(out) {}
    string emitStringLiteral(const std::string& str);
    void generar(Program* program);
    void pushScope();
    void popScope();
    void declareVariable(const string& name, int size);
    bool isVariableInCurrentScope(const string& name);

    ImpValue visit(BinaryExp* exp) override;
    ImpValue visit(NumberExp* exp) override;
    ImpValue visit(BoolExp* exp) override;
    ImpValue visit(StringLiteral* exp) override;
    ImpValue visit(IdentifierExp* exp) override;
    ImpValue visit(LValue* exp) override;
    ImpValue visit(IFExp* exp) override;
    ImpValue visit(FCallExp* exp) override;
    ImpValue visit(ArrayAccessExp* exp) override;
    ImpValue visit(InitValue* iv) override;

    void visit(AssignStatement* stm) override;
    void visit(PrintStatement* stm) override;
    void visit(IfStatement* stm) override;
    void visit(WhileStatement* stm) override;
    void visit(DoWhileStatement* stm) override;
    void visit(FCallStatement* stm) override;
    void visit(ForStatement* stm) override;
    void visit(VarDec* stm) override;
    void visit(VarDecList* stm) override;
    void visit(Var* stm) override;
    void visit(StatementList* stm) override;
    void visit(ReturnStatement* stm) override;

    void visit(FunDec* fd) override;
    void visit(FunDecList* fdl) override;

    void visit(Body* b) override;
    void visit(Program* program) override;
};

#endif //COMPILA_PROJECT_CODEGEN_HH
