#ifndef COMPILA_PROJECT_TYPE_VISITOR_H
#define COMPILA_PROJECT_TYPE_VISITOR_H

#include "visitor.h"
#include "environment.h"
#include <string>
#include <iostream>
using namespace std;

class TypeVisitor : public Visitor {
    Environment* env;
public:
    std::unordered_map<string, FunDec*> funciones;
    TypeVisitor(Environment* env_) {
        env = env_;
    }
    void check(Program* program);

    // Exp
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

    // Statements
    void visit(AssignStatement* stm) override;
    void visit(PrintStatement* stm) override;
    void visit(IfStatement* stm) override;
    void visit(WhileStatement* stm) override;
    void visit(DoWhileStatement* stm) override;
    void visit(ForStatement* stm) override;
    void visit(ReturnStatement* stm) override;
    void visit(FCallStatement* stm) override;
    void visit(VarDec* vd) override;
    void visit(VarDecList* vdl) override;
    void visit(Var* var) override;
    void visit(StatementList* stm) override;
    void visit(Body* b) override;
    void visit(FunDec* fd) override;
    void visit(FunDecList* fdl) override;
    void visit(Program* program) override;
};

#endif //COMPILA_PROJECT_TYPE_VISITOR_H
