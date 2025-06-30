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
AssignStatement::AssignStatement(LValue* lValue, Exp* e): lvalue(lValue), rhs(e) {}
AssignStatement::~AssignStatement() {
    delete rhs;
}
PrintStatement::PrintStatement(string format, Exp* e): format(format), e(e) {}

PrintStatement::PrintStatement(string format, vector<Exp*> args) : format(format), args(args) {}

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

ForStatement::ForStatement(string id,string type, Exp* start, Exp* condition, Exp* step, Body* b):id(id),start(start),condition(condition),step(step),b(b),type(type){}
ForStatement::~ForStatement(){delete start; delete condition;delete step;delete b;}
VarDec::VarDec(string type, list<Var*> vars): type(type), vars(vars) {}
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

void FunDecList::add(FunDec *fundec) {
    Fundecs.push_back(fundec);
}

ReturnStatement::ReturnStatement(Exp* e):e(e){}
ReturnStatement::~ReturnStatement(){delete e;}

InitValue::InitValue(Exp* val) {
    isList = false;
    value = val;
}

InitValue::InitValue(vector<InitValue*> list_) {
    isList = true;
    list = list_;
    value = nullptr;
}

LValue::LValue(const string& id, vector<Exp*> indices): id(id), indices(indices) {}
LValue::~LValue() {
    for (auto e : indices) delete e;
}

DoWhileStatement::DoWhileStatement(Exp* condition, Body* b): condition(condition), b(b) {}
DoWhileStatement::~DoWhileStatement() {
    delete condition;
    delete b;
}

VarDecList::VarDecList(list<VarDec*> vardecs): vardecs(vardecs) {}
void VarDecList::add(VarDec* v) {
    vardecs.push_back(v);
}
VarDecList::~VarDecList() {
    for (auto v: vardecs) {
        delete v;
    }
}

ArrayAccessExp::ArrayAccessExp(const string& name, const vector<Exp*>& idx) : arrayName(name), indices(idx) {}


StatementList::StatementList(): stms() {}
void StatementList::add(Stm* s) {
    stms.push_back(s);
}
StatementList::~StatementList() {
    for (auto s: stms) {
        delete s;
    }
}

InitValue::~InitValue() {
    delete value;
    for (auto s: list) {
        delete s;
    }
}

Body::Body(VarDecList* v, StatementList* s): vardecs(v), slist(s) {}
Body::~Body() {
    delete vardecs;
    delete slist;
}

Var::Var(string id, list<NumberExp *> dimList, InitValue *iv): id(id), dimList(dimList), iv(iv){}
Var::~Var() {
    for (auto s: dimList) {
        delete s;
    }
    delete iv;
}

Program::Program(VarDecList* vdl, FunDecList *func): vdl(vdl), func(func) {}
Program::~Program(){
    delete vdl;
    delete func;
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
        case LET_OP: c = "<="; break;
        case GT_OP: c = ">"; break;
        case GET_OP: c = ">="; break;
        case AND_OP: c = "&&"; break;
        case OR_OP: c = "||"; break;
        case NOT_OP: c = "!"; break;
        case DIFF_OP: c = "!="; break;
        case EQ_OP: c = "=="; break;
        case INC_OP: c = "++"; break;
        case DEC_OP: c = "--"; break;
        default: c = "$";
    }
    return c;
}