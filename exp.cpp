#include <iostream>
#include "exp.h"
using namespace std;
IFExp::IFExp(Exp* cond, Body* body_): condi(cond),body(body_){}
BinaryExp::BinaryExp(Exp* l, Exp* r, BinaryOp op):left(l),right(r),op(op) {
    if (op == PLUS_OP || op == MINUS_OP || op == MUL_OP || op == DIV_OP) {
        type = "int";
    } else {
        type = "bool";
    }
}
NumberExp::NumberExp(int v):value(v) {}
BoolExp::BoolExp(bool v):value(v) {}
IdentifierExp::IdentifierExp(const string& n):name(n) {}
StringLiteral::StringLiteral(const string& value):value(value){}
StringLiteral::~StringLiteral(){}
Exp::~Exp() {}
BinaryExp::~BinaryExp() { delete left; delete right; }
IFExp::~IFExp() {delete condi, delete body; }
NumberExp::~NumberExp() { }
BoolExp::~BoolExp() { }
IdentifierExp::~IdentifierExp() { }
AssignStatement::AssignStatement(string id, Exp* e): id(id), rhs(e) {}
AssignStatement::~AssignStatement() {
    delete rhs;
}
PrintStatement::PrintStatement(Exp* e): e(e) {}
PrintStatement::~PrintStatement() {
    delete e;
}

IfStatement::IfStatement(vector<IFExp*> sent): sent_if(sent){}
IfStatement::~IfStatement() {
    for(auto s:sent_if)
        delete s;

}
WhileStatement::WhileStatement(Exp* c, Body* t): condition(c), b(t) {}
WhileStatement::~WhileStatement() {
    delete condition;
    delete b;
}

ForStatement::ForStatement(string id, Exp* start, Exp* end, Exp* step, Body* b):id(id),start(start),condition(condition),step(step),b(b){}
ForStatement::~ForStatement(){delete start; delete condition;delete step;delete b;}
VarDec::VarDec(string type, list<string> ids): type(type), vars(ids) {}
VarDec::~VarDec(){}

FunDec::FunDec(string nombre,string tipo,vector<string> parametros,vector<string> tipos,Body* cuerpo):nombre(nombre),tipo(tipo),parametros(parametros),tipos(tipos),cuerpo(cuerpo){}
FunDec::~FunDec() {
    delete cuerpo;
}

FCallExp::FCallExp(string nombre,vector<Exp*> argumentos):nombre(nombre),argumentos(argumentos){}
FCallExp::~FCallExp() {
    for(auto ar:argumentos)
        delete ar;
}

FunDecList::FunDecList(list<FunDec*> Fundecs):Fundecs(Fundecs){}
FunDecList::~FunDecList() {
    for(auto fun:Fundecs)
        delete fun;
}
ReturnStatement::ReturnStatement(Exp* e):e(e){}
ReturnStatement::~ReturnStatement(){delete e;}



VarDecList::VarDecList(list<VarDec*> vardecs): vardecs(vardecs) {}
void VarDecList::add(VarDec* v) {
    vardecs.push_back(v);
}
VarDecList::~VarDecList() {
    for (auto v: vardecs) {
        delete v;
    }
}

StatementList::StatementList(): stms() {}
void StatementList::add(Stm* s) {
    stms.push_back(s);
}
StatementList::~StatementList() {
    for (auto s: stms) {
        delete s;
    }
}

Body::Body(VarDecList* v, StatementList* s): vardecs(v), slist(s) {}
Body::~Body() {
    delete vardecs;
    delete slist;
}


Stm::~Stm() {}
string Exp::binopToChar(BinaryOp op) {
    string  c;
    switch(op) {
        case PLUS_OP: c = "+"; break;
        case MINUS_OP: c = "-"; break;
        case MUL_OP: c = "*"; break;
        case DIV_OP: c = "/"; break;
        case LT_OP: c = "<"; break;
        case LE_OP: c = "<="; break;
        case EQ_OP: c = "=="; break;
        default: c = "$";
    }
    return c;
}