#ifndef COMPILA_PROJECT_CODEGEN_HH
#define COMPILA_PROJECT_CODEGEN_HH

#include "visitor.h"
using namespace std;

class GenCode : public Visitor {
private:
    std::ostream& out;
    unordered_map<string, int> memoria;
    int offset = -8;
    int labelcont = 0;
    bool entornoFuncion = false;
    string nombreFuncion;
public:
    GenCode(ostream& out) : out(out) {}
    void generar(Program* program);

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
