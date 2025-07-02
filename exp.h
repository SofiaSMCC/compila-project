#ifndef EXP_H
#define EXP_H
#include <vector>
#include <string>
#include <unordered_map>
#include "imp_value.h"
#include "visitor.h"
#include <list>
using namespace std;

enum BinaryOp { PLUS_OP, MINUS_OP, MUL_OP, DIV_OP,
                LT_OP, LET_OP, GT_OP, GET_OP, EQ_OP, AND_OP, OR_OP, NOT_OP, DIFF_OP,
                INC_OP, DEC_OP};

class Visitor;
class Body;

class Exp {
public:
    virtual ImpValue accept(Visitor* visitor) = 0;
    virtual ~Exp() = 0;
    static string binopToChar(BinaryOp op);
};

class BinaryExp : public Exp {
public:
    Exp *left, *right;
    string type;
    BinaryOp op;
    BinaryExp(Exp* l, Exp* r, BinaryOp op);
    ImpValue accept(Visitor* visitor);
    ~BinaryExp();
};

class NumberExp : public Exp {
public:
    int value;
    NumberExp(int v);
    ImpValue accept(Visitor* visitor);
    ~NumberExp();
};

class BoolExp : public Exp {
public:
    int value;
    BoolExp(bool v);
    ImpValue accept(Visitor* visitor);
    ~BoolExp();
};

class StringLiteral : public Exp {
public:
    string value;
    StringLiteral(const string& value);
    ImpValue accept(Visitor* visitor);
    //string acceptString(Visitor* visitor);
    ~StringLiteral();
};

class IdentifierExp : public Exp {
public:
    string name;
    IdentifierExp(const string& n);
    ImpValue accept(Visitor* visitor);
    ~IdentifierExp();
};

class LValue : public Exp {
public:
    string id;
    vector<Exp*> indices;
    LValue(const string& id, vector<Exp*> indices);
    ~LValue();
    ImpValue accept(Visitor* visitor) override;
};

class ArrayAccessExp : public Exp {
public:
    string arrayName;
    vector<Exp*> indices;
    ArrayAccessExp(const string& name, const vector<Exp*>& idx);
    ImpValue accept(Visitor* visitor) override;
    ~ArrayAccessExp() override {}
};

class Stm {
public:
    virtual int accept(Visitor* visitor) = 0;
    virtual ~Stm() = 0;
};

class AssignStatement : public Stm {
public:
    LValue* lvalue;
    Exp* rhs;
    AssignStatement(LValue* lvalue, Exp* e);
    int accept(Visitor* visitor);
    ~AssignStatement();
};

class InitValue {
public:
    bool isList;
    Exp* value;
    vector<InitValue*> list;
    InitValue(Exp* val);
    InitValue(vector<InitValue*> list);
    virtual ImpValue accept(Visitor* visitor);
    ~InitValue();
};


class PrintStatement : public Stm {
public:
    string format;
    Exp* e;
    vector<Exp*> args;
    PrintStatement(string format, Exp* e);
    PrintStatement(string format, Exp* e, vector<Exp* >args);

    int accept(Visitor* visitor);
    ~PrintStatement();
};

inline bool isString(Exp* e) {
    return dynamic_cast<StringLiteral*>(e) != nullptr;
}

class IFExp : public Exp {
public:
    Exp *condi;
    Body* body;
    IFExp(Exp *cond, Body* body_);
    ImpValue accept(Visitor* visitor);
    ~IFExp();
};

class IfStatement : public Stm {
public:
    vector<IFExp*> sent_if;
    IfStatement(vector<IFExp*> sent);
    int accept(Visitor* visitor);
    ~IfStatement();
};

class WhileStatement : public Stm {
public:
    Exp* condition;
    Body* b;
    WhileStatement(Exp* condition, Body* b);
    int accept(Visitor* visitor);
    ~WhileStatement();
};

class DoWhileStatement : public Stm {
public:
    Exp* condition;
    Body* b;
    DoWhileStatement(Exp* condition, Body* b);
    int accept(Visitor* visitor);
    ~DoWhileStatement();
};

class ForStatement : public Stm {
public:
    string id;
    string type;
    Exp* start;
    Exp* condition;
    Exp* step;
    Body* b;
    ForStatement(string id,string type, Exp* start, Exp* condition, Exp* step, Body* b);
    int accept(Visitor* visitor);
    ~ForStatement();
};

class FunDec {
public:
    string nombre;
    string tipo;
    // ParamDecList
    vector<string> parametros;
    vector<string> tipos;
    Body* cuerpo;
    FunDec(string nombre,string tipo,vector<string> parametros,vector<string> tipos,Body* cuerpo);
    ~FunDec();
    int accept(Visitor* visitor);
};

class FCallExp : public Exp {
public:
    string nombre;
    vector<Exp*> argumentos;
    FCallExp(string nombre, vector<Exp*> argumentos);
    ~FCallExp();
    ImpValue accept(Visitor* visitor);
};

class FCallStatement : public Stm {
public:
    FCallExp* call;
    FCallStatement(FCallExp* call) : call(call) {}
    int accept(Visitor* visitor);
    ~FCallStatement() { delete call; }
};

class FunDecList{
public:
    list<FunDec*> Fundecs;
    void add(FunDec* fundec);
    int accept(Visitor* visitor);
    FunDecList();
    FunDecList(list<FunDec*> Fundecs);
    ~FunDecList();
};

class ReturnStatement: public Stm {
public:
    string type;
    Exp* e;
    ReturnStatement(Exp* e);
    ~ReturnStatement();
    int accept(Visitor* visitor);
};

class Var {
public:
    string id;
    list<NumberExp*> dimList;
    InitValue* iv;
    Var(string id, list<NumberExp*> dimList, InitValue* iv);
    int accept(Visitor* visitor);
    ~Var();
};

class VarDec {
public:
    string type;
    list<Var*> vars;
    VarDec(string type, list<Var*> vars);
    int accept(Visitor* visitor);
    ~VarDec();
};

class VarDecList{
public:
    list<VarDec*> vardecs;
    VarDecList(list<VarDec*> vardecs);
    VarDecList(){};
    void add(VarDec* vardec);
    int accept(Visitor* visitor);
    ~VarDecList();
};

class StatementList {
public:
    list<Stm*> stms;
    StatementList();
    void add(Stm* stm);
    int accept(Visitor* visitor);
    ~StatementList();
};

class Body{
public:
    VarDecList* vardecs;
    StatementList* slist;
    Body(VarDecList* vardecs, StatementList* stms);
    int accept(Visitor* visitor);
    ~Body();
};

class Program {
public:
    VarDecList* vdl;
    FunDecList* func;
    Program(VarDecList* vdl, FunDecList* func);
    int accept(Visitor* visitor);
    ~Program();
};

#endif // EXP_H