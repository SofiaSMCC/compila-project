#include "eval_visitor.h"

void EVALVisitor::ejecutar(Program* program) {
    env.add_level();
    program->func->accept(this);
    cout << "\nEstado del environment antes de limpiar:" << endl;
    env.debug_print();
    env.remove_level();
}

// Exp

ImpValue EVALVisitor::visit(BinaryExp *exp) {
    return ImpValue();
}

ImpValue EVALVisitor::visit(NumberExp *exp) {
    int a = exp->value;
    return ImpValue("int", a, false, "");
}

ImpValue EVALVisitor::visit(BoolExp *exp) {
    bool a = exp->value;
    return ImpValue("bool", 0, a, "");
}

ImpValue EVALVisitor::visit(StringLiteral *exp) {
    string a = exp->value;
    return ImpValue("string", 0, false, a);
}

ImpValue EVALVisitor::visit(IdentifierExp *exp) {
    string t = env.lookup_type(exp->name);

    if (t == "int") {
        int n = env.lookup(exp->name).first;
        return ImpValue(t, n, false, "");
    }
    else if (t == "bool") {
        bool v = env.lookup(exp->name).first;
        return ImpValue(t, 0, v, "");
    }
    else if (t == "string") {
        string s = env.lookup(exp->name).second;
        return ImpValue(t, 0, false, s);
    }
    return ImpValue();
}

ImpValue EVALVisitor::visit(LValue *exp) {
    return ImpValue();
}

ImpValue EVALVisitor::visit(IFExp *exp) {
    return ImpValue();
}

ImpValue EVALVisitor::visit(FCallExp *exp) {
    return ImpValue();
}

ImpValue EVALVisitor::visit(ArrayAccessExp *exp) {
    return ImpValue();
}

ImpValue EVALVisitor::visit(InitValue *iv) {
    return iv->value->accept(this);
}

// Statement

void EVALVisitor::visit(AssignStatement *stm) {
    string t = env.lookup_type(stm->lvalue->id); //tipo
    ImpValue val = stm->rhs->accept(this);

    if (t=="bool") {
        env.update(stm->lvalue->id, val.bool_value);
    }
    else if (t=="int") {
        env.update(stm->lvalue->id, val.int_value);
    }
    else if (t=="string") {
        env.update(stm->lvalue->id,val.string_value);
    }
}

void EVALVisitor::visit(IfStatement *stm) {
}

void EVALVisitor::visit(ForStatement *stm) {
}

void EVALVisitor::visit(StatementList *stm) {
    for (auto s : stm->stms) {
        s->accept(this);
    }
}

void EVALVisitor::visit(DoWhileStatement *stm) {
}

void EVALVisitor::visit(ReturnStatement *stm) {
    ImpValue ret = stm->e->accept(this);
}

void EVALVisitor::visit(WhileStatement *stm) {
}

void EVALVisitor::visit(PrintStatement *stm) {
    ImpValue arg = stm->e->accept(this);
    if (stm->format == "%s\n") {
        if (arg.type == "string") {
            cout << arg.string_value << endl;
        } else if (arg.type == "bool") {
            cout << (arg.bool_value ? "true" : "false") << endl;
        } else {
            cout << "Error: printf formato %s pero no string/bool" << endl;
        }
    } else if (stm->format == "%d\n") {
        if (arg.type == "int" || arg.type == "bool") {
            cout << (arg.type == "int" ? arg.int_value : (arg.bool_value ? 1 : 0)) << endl;
        } else {
            cout << "Error: printf formato %d pero no int/bool" << endl;
        }
    }
}

void EVALVisitor::visit(FCallStatement *stm) {
}

void EVALVisitor::visit(VarDecList *stm) {
    for (auto vd : stm->vardecs) {
        vd->accept(this);
    }
}

void EVALVisitor::visit(FunDecList *fdl) {
    for (auto vd : fdl->Fundecs) {
        vd->accept(this);
    }
}

void EVALVisitor::visit(FunDec *fd) {
    fd->cuerpo->accept(this);
}


void EVALVisitor::visit(Body *b) {
    b->vardecs->accept(this);
    b->slist->accept(this);
}


void EVALVisitor::visit(VarDec *stm) {
    for(auto i: stm->vars){
        // Si tiene valor inicial, lo evaluamos
        if (i->iv) {
            ImpValue val = i->iv->accept(this);
            if (stm->type == "int") {
                env.add_var(i->id, val.int_value, stm->type);
            } else if (stm->type == "bool") {
                env.add_var(i->id, val.bool_value ? 1 : 0, stm->type);
            } else if (stm->type == "string") {
                env.add_var(i->id, val.string_value, stm->type);
            }
        } else {
            env.add_var(i->id, stm->type); // Sin valor inicial, usa el default
        }
    }
}

void EVALVisitor::visit(Var *stm) {
}
