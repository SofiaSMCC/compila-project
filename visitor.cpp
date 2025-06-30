#include <iostream>
#include "exp.h"
#include "visitor.h"
using namespace std;

//////////////////////// Exp //////////////////////////

ImpValue BinaryExp::accept(Visitor *visitor) {
    return visitor->visit(this);
}

ImpValue NumberExp::accept(Visitor *visitor) {
    return visitor->visit(this);
}

ImpValue BoolExp::accept(Visitor *visitor) {
    return visitor->visit(this);
}

ImpValue StringLiteral::accept(Visitor *visitor) {
    return visitor->visit(this);
}

ImpValue IdentifierExp::accept(Visitor *visitor) {
    return visitor->visit(this);
}

ImpValue LValue::accept(Visitor *visitor) {
    return visitor->visit(this);
}

ImpValue ArrayAccessExp::accept(Visitor *visitor) {
    return visitor->visit(this);
}

ImpValue IFExp::accept(Visitor *visitor) {
    return visitor->visit(this);
}

ImpValue FCallExp::accept(Visitor *visitor) {
    return visitor->visit(this);
}

ImpValue InitValue::accept(Visitor* visitor) {
    return visitor->visit(this);
}

///////////////////////////////////////////////////////

int Program::accept(Visitor *visitor) {
    visitor->visit(this);
    return 0;
}

int FCallStatement::accept(Visitor *visitor) {
    visitor->visit(this);
    return 0;
}

int AssignStatement::accept(Visitor *visitor) {
    visitor->visit(this);
    return 0;
}

int PrintStatement::accept(Visitor *visitor) {
    visitor->visit(this);
    return 0;
}

int IfStatement::accept(Visitor *visitor) {
    visitor->visit(this);
    return 0;
}

int WhileStatement::accept(Visitor *visitor) {
    visitor->visit(this);
    return 0;
}

int DoWhileStatement::accept(Visitor *visitor) {
    visitor->visit(this);
    return 0;
}

int ForStatement::accept(Visitor *visitor) {
    visitor->visit(this);
    return 0;
}

int FunDec::accept(Visitor *visitor) {
    visitor->visit(this);
    return 0;
}

int FunDecList::accept(Visitor *visitor) {
    visitor->visit(this);
    return 0;
}

int ReturnStatement::accept(Visitor *visitor) {
    visitor->visit(this);
    return 0;
}

int Var::accept(Visitor *visitor) {
    visitor->visit(this);
    return 0;
}

int VarDec::accept(Visitor *visitor) {
    visitor->visit(this);
    return 0;
}

int VarDecList::accept(Visitor *visitor) {
    visitor->visit(this);
    return 0;
}

int StatementList::accept(Visitor *visitor) {
    visitor->visit(this);
    return 0;
}

int Body::accept(Visitor *visitor) {
    visitor->visit(this);
    return 0;
}

///////////////////////Print//////////////////////////////

// Exp

ImpValue PrintVisitor::visit(BinaryExp* exp) {
    if (exp->op == NOT_OP && exp->right == nullptr) {
        cout << "!";
        cout << "(";
        exp->left->accept(this);
        cout << ")";
        return ImpValue();
    }
    // Operadores unarios disfrazados
    if (exp->op == INC_OP) {
        exp->left->accept(this);
        cout << "++";
        return ImpValue();
    } else if (exp->op == DEC_OP) {
        exp->left->accept(this);
        cout << "--";
        return ImpValue();
    }

    // Operadores binarios normales
    cout << "(";
    exp->left->accept(this);
    cout << " " << Exp::binopToChar(exp->op) << " ";
    exp->right->accept(this);
    cout << ")";
    return ImpValue();
}


ImpValue PrintVisitor::visit(NumberExp* exp) {
    cout << exp->value;
    return ImpValue();
}

ImpValue PrintVisitor::visit(BoolExp* exp) {
    cout << (exp->value ? "true" : "false");
    return ImpValue();
}

ImpValue PrintVisitor::visit(StringLiteral* exp) {
    cout << "\"" << exp->value << "\"";
    return ImpValue();
}

ImpValue PrintVisitor::visit(IdentifierExp* exp) {
    cout << exp->name;
    return ImpValue();
}

ImpValue PrintVisitor::visit(LValue* exp) {
    cout << exp->id;
    for (auto idx : exp->indices) {
        cout << "[";
        idx->accept(this);
        cout << "]";
    }
    return ImpValue();
}

ImpValue PrintVisitor::visit(IFExp* exp) {
    printIndent();
    cout << "if (";
    exp->condi->accept(this);
    cout << ") ";
    exp->body->accept(this);
    return ImpValue();
}

ImpValue PrintVisitor::visit(FCallExp* exp) {
    cout << exp->nombre << "(";
    bool first = true;
    for (auto arg : exp->argumentos) {
        if (!first) cout << ", ";
        arg->accept(this);
        first = false;
    }
    cout << ")";
    return ImpValue();
}

ImpValue PrintVisitor::visit(ArrayAccessExp* exp) {
    cout << exp->arrayName;
    for (auto index : exp->indices) {
        cout << "[";
        index->accept(this);
        cout << "]";
    }
    return ImpValue();
}

// Stm

void PrintVisitor::visit(Program *program) {
    program->func->accept(this);
}

void PrintVisitor::imprimir(Program* program){
    cout << "imprimir" << endl;
    program->func->accept(this);
};

void PrintVisitor::visit(AssignStatement* stm) {
    //cout << "void PrintVisitor::visit(AssignStatement* stm) {" << endl;
    printIndent();
    stm->lvalue->accept(this);
    cout << " = ";
    stm->rhs->accept(this);
    cout << ";" << endl;
}

void PrintVisitor::visit(PrintStatement* stm) {
    printIndent();
    cout << "printf(";
    if (stm->format.front() != '"') cout << "\"";
    cout << stm->format;
    if (stm->format.back() != '"') cout << "\"";

    for (size_t i = 0; i < stm->args.size(); ++i) {
        cout << ", ";
        stm->args[i]->accept(this);
    }

    cout << ");" << endl;
}


void PrintVisitor::visit(IfStatement* stm) {
    size_t n = stm->sent_if.size();
    for (size_t i = 0; i < n; ++i) {
        IFExp* branch = stm->sent_if[i];

        if (i == 0) {
            // Primer if
            printIndent();
            cout << "if (";
            branch->condi->accept(this);
            cout << ") {" << endl;
        } else if (branch->condi != nullptr) {
            // else if
            printIndent();
            cout << "else if (";
            branch->condi->accept(this);
            cout << ") {" << endl;
        } else {
            printIndent();
            cout << "else {" << endl;
        }

        indent++;
        branch->body->accept(this);
        indent--;
        printIndent();
        cout << "}" << endl;
    }
}

void PrintVisitor::visit(WhileStatement* stm) {
    printIndent();
    cout << "while (";
    stm->condition->accept(this);
    cout << ") ";
    stm->b->accept(this);
}

void PrintVisitor::visit(DoWhileStatement* stm) {
    printIndent();
    cout << "do {";
    printIndent();
    stm->b->accept(this);
    cout << "} while (";
    stm->condition->accept(this);
    cout << ");" << endl;
}

void PrintVisitor::visit(ForStatement* stm) {
    printIndent();
    cout << "for ("<<stm->type <<" "<< stm->id << " = ";
    stm->start->accept(this);
    cout << "; ";
    stm->condition->accept(this);
    cout << "; ";
    stm->step->accept(this);
    cout << ") {";
    indent++;
    stm->b->accept(this);
    indent--;
    printIndent();
    cout << "}" << endl;
}

void PrintVisitor::visit(VarDec* stm) {
    printIndent();
    cout << stm->type << " ";
    bool first = true;
    for (auto v : stm->vars) {
        if (!first) cout << ", ";
        v->accept(this);
        first = false;
    }
    cout << ";";
}

void PrintVisitor::visit(VarDecList* stm) {
    for(auto i: stm->vardecs){
        i->accept(this);
        cout << endl;
    }
}

void PrintVisitor::visit(Var* stm) {
    cout << stm->id;
    for (auto d : stm->dimList) {
        cout << "[";
        d->accept(this);
        cout << "]";
    }
    if (stm->iv) {
        cout << " = ";
        stm->iv->accept(this);
    }
}

void PrintVisitor::visit(StatementList* stm) {
    for(auto i: stm->stms){
        i->accept(this);
    }
}

void PrintVisitor::visit(ReturnStatement* stm) {
    printIndent();
    cout << "return ";
     if (stm->e) {stm->e->accept(this);}
    cout << ";" << endl;
}

// Fun

void PrintVisitor::visit(FunDec* fd) {
    printIndent();
    cout << fd->tipo << " " << fd->nombre << "(";
    for (size_t i = 0; i < fd->parametros.size(); ++i) {
        if (i > 0) cout << ", ";
        cout << fd->tipos[i] << " " << fd->parametros[i];
    }
    cout << ") {" << endl;
    indent++;
    fd->cuerpo->accept(this);
    indent--;
    printIndent();
    cout << "}" << endl;
}

void PrintVisitor::visit(FunDecList* fdl) {
    for(auto i: fdl->Fundecs){
        i->accept(this);
        cout << endl;
    }
}

void PrintVisitor::visit(Body* b) {
    if (b->vardecs) b->vardecs->accept(this);
    cout << endl;
    if (b->slist) b->slist->accept(this);
}

ImpValue PrintVisitor::visit(InitValue* iv) {
    if (iv->isList) {
        std::cout << "{";
        for (size_t i = 0; i < iv->list.size(); ++i) {
            if (i > 0) std::cout << ", ";
            if (iv->list[i]) iv->list[i]->accept(this);
        }
        std::cout << "}";
    } else if (iv->value) {
        iv->value->accept(this);
    }
    return ImpValue();
}

void PrintVisitor::visit(FCallStatement* stm) {
    printIndent();
    stm->call->accept(this);
    cout << ";" << endl;
}

