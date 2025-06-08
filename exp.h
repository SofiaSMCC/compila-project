#ifndef EXP_H
#define EXP_H
#include <vector>
#include <string>
#include <unordered_map>
#include <list>
//#include "visitor.h"
using namespace std;
enum BinaryOp { PLUS_OP, MINUS_OP, MUL_OP, DIV_OP,LT_OP, LE_OP, EQ_OP, AND_OP, OR_OP, NOT_OP};
class StmList;
class Body;

class ImpValue {
public:
  ImpValue(string tipo,int valor, bool bol, string str_valor){
    type = tipo;
    int_value = valor;
    bool_value = bol;
    string_value=str_valor;
  };
  ImpValue(){
  };
  ~ImpValue(){};
  string type;
  int int_value;
  bool bool_value;
    string string_value;
};

class Exp {
public:
//    virtual ImpValue  accept(Visitor* visitor) = 0;
    virtual ~Exp() = 0;
    static string binopToChar(BinaryOp op);
};

//if(condicion){body}
class IFExp : public Exp {
public:
    Exp *condi;
    Body* body;
    IFExp(Exp *cond, Body* body_);
//    ImpValue accept(Visitor* visitor);
    ~IFExp();
};

// binary Exp
class BinaryExp : public Exp {
public:
    Exp *left, *right;
    string type;
    BinaryOp op;
    BinaryExp(Exp* l, Exp* r, BinaryOp op);
//    ImpValue accept(Visitor* visitor);
    ~BinaryExp();
};

class NumberExp : public Exp {
public:
    int value;
    NumberExp(int v);
//    ImpValue accept(Visitor* visitor);
    ~NumberExp();
};

class BoolExp : public Exp {
public:
    int value;
    BoolExp(bool v);
//    ImpValue accept(Visitor* visitor);
    ~BoolExp();
};

class IdentifierExp : public Exp {
public:
    std::string name;
    IdentifierExp(const std::string& n);
//    ImpValue accept(Visitor* visitor);
    ~IdentifierExp();
};

class StringLiteral : public Exp {
public:
    string value;
    StringLiteral(const string& value);
//    ImpValue accept(Visitor* visitor);
//    string acceptString(Visitor* visitor);
    ~StringLiteral();
};

class Stm {
public:
//    virtual int accept(Visitor* visitor) = 0;
    virtual ~Stm() = 0;
};

class AssignStatement : public Stm {
public:
    std::string id;
    Exp* rhs;
    AssignStatement(std::string id, Exp* e);
//    int accept(Visitor* visitor);
    ~AssignStatement();
};

class PrintStatement : public Stm {
public:
    Exp* e;
    PrintStatement(Exp* e);
//    int accept(Visitor* visitor);
    ~PrintStatement();
};

// sentencia if: (if(){}   else if(){}  else {} )
class IfStatement : public Stm {
public:
    vector<IFExp*> sent_if;
    IfStatement(vector<IFExp*> sent);
//    int accept(Visitor* visitor);
    ~IfStatement();
};

class WhileStatement : public Stm {
public:
    Exp* condition;
    Body* b;
    WhileStatement(Exp* condition, Body* b);
//    int accept(Visitor* visitor);
    ~WhileStatement();
};

/*class DoWhileStatement : public Stm {
public:
    Exp* condition;
    Body* b;
    DoWhileStatement(Exp* condition, Body* b);
    //    int accept(Visitor* visitor);
    ~DoWhileStatement();
};*/

//FOR(int,condition,int) { stmlist}
class ForStatement : public Stm {
public:
    string id;
    Exp* start;
    Exp* condition;
    Exp* step;
    Body* b;
    ForStatement(string id, Exp* start, Exp* end, Exp* step, Body* b);
//    int accept(Visitor* visitor);
    ~ForStatement();
};


class FunDec {
public:
    string nombre;
    string tipo;
    vector<string> parametros;
    vector<string> tipos;
    Body* cuerpo;
    FunDec(string nombre,string tipo,vector<string> parametros,vector<string> tipos,Body* cuerpo);
    ~FunDec();
//    int accept(Visitor* visitor);
};

class FCallExp : public Exp {
public:
    string nombre;
    vector<Exp*> argumentos;
    FCallExp(string nombre, vector<Exp*> argumentos);
    ~FCallExp();
//    int accept(Visitor* visitor);
};

class FunDecList{
public:
    list<FunDec*> Fundecs;
    void add(FunDec* fundec) {
        Fundecs.push_back(fundec);
    };
//    int accept(Visitor* visitor);
    FunDecList(list<FunDec*> Fundecs);
    ~FunDecList();
};

class ReturnStatement: public Stm {
public:
    Exp* e;
    ReturnStatement(Exp* e);
    ~ReturnStatement();
//    int accept(Visitor* visitor);
};

class VarDec {
public:
    string type;
    list<string> vars;
    VarDec(string type, list<string> vars);
 //   int accept(Visitor* visitor);
    ~VarDec();
};

class VarDecList{
public:
    list<VarDec*> vardecs;
    VarDecList(list<VarDec*> vardecs);
    void add(VarDec* vardec);
 //   int accept(Visitor* visitor);
    ~VarDecList();
};

class StatementList {
public:
    list<Stm*> stms;
    StatementList();
    void add(Stm* stm);
//    int accept(Visitor* visitor);
    ~StatementList();
};


class Body{
public:
    VarDecList* vardecs;
    StatementList* slist;
    Body(VarDecList* vardecs, StatementList* stms);
 //   int accept(Visitor* visitor);
    ~Body();
};

class Program {
public:
    Body* body;
    FunDecList* func;
    Program(Body* body);
    ~Program();
};



#endif // EXP_H