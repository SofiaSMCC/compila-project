#ifndef COMPILA_PROJECT_VISITOR_H
#define COMPILA_PROJECT_VISITOR_H

#include "exp.h"
#include <list>

class BinaryExp;
class NumberExp;
class BoolExp;
class StringLiteral;
class IdentifierExp;
class LValue;
class ArrayAccessExp;
class AssignStatement;
class PrintStatement;
class IFExp;
class IfStatement;
class WhileStatement;
class DoWhileStatement;
class ForStatement;
class FunDec;
class FCallExp;
class FunDecList;
class ReturnStatement;
class Var;
class VarDec;
class VarDecList;
class StatementList;
class Body;
class Program;
class InitValue;
class FCallStatement;


class Visitor {
public:
    virtual ImpValue visit(BinaryExp* exp) = 0;
    virtual ImpValue visit(NumberExp* exp) = 0;
    virtual ImpValue visit(BoolExp* exp) = 0;
    virtual ImpValue visit(StringLiteral* exp) = 0;
    virtual ImpValue visit(IdentifierExp* exp) = 0;
    virtual ImpValue visit(LValue* exp) = 0;
    virtual ImpValue visit(IFExp* exp) = 0;
    virtual ImpValue visit(FCallExp* exp) = 0;
    virtual ImpValue visit(ArrayAccessExp* exp) = 0;
    virtual ImpValue visit(InitValue* iv) = 0;

    virtual void visit(AssignStatement* stm) = 0;
    virtual void visit(FCallStatement* stm) = 0;
    virtual void visit(PrintStatement* stm) = 0;
    virtual void visit(IfStatement* stm) = 0;
    virtual void visit(WhileStatement* stm) = 0;
    virtual void visit(DoWhileStatement* stm) = 0;
    virtual void visit(ForStatement* stm) = 0;
    virtual void visit(VarDec* stm) = 0;
    virtual void visit(VarDecList* stm) = 0;
    virtual void visit(Var* stm) = 0;
    virtual void visit(StatementList* stm) = 0;
    virtual void visit(ReturnStatement* stm) = 0;

    virtual void visit(FunDec* fd) = 0;
    virtual void visit(FunDecList* fdl) = 0;

    virtual void visit(Body* b) = 0;
    virtual void visit(Program* p) = 0;
};

class PrintVisitor : public Visitor {
    int indent = 0;
    void printIndent() const {
        for (int i = 0; i < indent; ++i) std::cout << "  ";
    }

public:
    void imprimir(Program* program);
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

#endif //COMPILA_PROJECT_VISITOR_H
