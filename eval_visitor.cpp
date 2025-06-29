#include "eval_visitor.h"

#include <token.h>

void EVALVisitor::ejecutar(Program* program) {
    env.add_level();
    for (auto f : program->func->Fundecs) {
        funciones[f->nombre] = f;
    }
    if (funciones.count("main")) {
        FunDec* mainFun = funciones["main"];
        env.add_level();
        mainFun->cuerpo->accept(this);
    }
    cout << "\nEstado del environment antes de limpiar:" << endl;
    env.debug_print();
    env.remove_level();
    env.remove_level();
}

void EVALVisitor::visit(Program* program) {
    if (program->func)
        program->func->accept(this);
}

// Exp

ImpValue EVALVisitor::visit(BinaryExp *exp) {
    int result1 = 0;
    bool result2 = false;

    ImpValue left = exp->left->accept(this);
    ImpValue right;

    if (exp->right) {
        right = exp->right->accept(this);
    }

    switch (exp->op) {
        case PLUS_OP:
            result1 = left.int_value + right.int_value;
            return ImpValue("int", result1, false,"");
        case MINUS_OP:
            result1 = left.int_value - right.int_value;
            return ImpValue("int", result1, false,"");
        case MUL_OP:
            result1 = left.int_value * right.int_value;
            return ImpValue("int", result1, false,"");
        case DIV_OP:
            if (right.int_value != 0) {
                result1 = left.int_value / right.int_value;
            } else {
                cout << "Error: división por cero" << endl;
                result1 = 0;
            }
            return ImpValue("int", result1, false,"");

        case LT_OP:
            result2 = left.int_value < right.int_value;
            return ImpValue("bool", 0, result2,"");
            /*case LE_OP:
                result2 = left.int_value <= right.int_value;*/
            return ImpValue("bool", 0, result2,"");
        case EQ_OP:
            if (left.type == "int" && right.type == "int") {
                result2 = left.int_value == right.int_value;
            } else {
                result2 = left.bool_value == right.bool_value;
            }
            return ImpValue("bool", 0, result2,"");

        case AND_OP:
            result2 = left.bool_value && right.bool_value;
            return ImpValue("bool", 0, result2,"");
        case OR_OP:
            result2 = left.bool_value || right.bool_value;
            return ImpValue("bool", 0, result2,"");
        case NOT_OP:
            result2 = !left.bool_value;
            return ImpValue("bool", 0, result2,"");

        default:
            cout << "Operador desconocido" << endl;
            return ImpValue();
    }
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
    if (!exp->condi || exp->condi->accept(this).bool_value) {
        exp->body->accept(this);
    }
    return ImpValue();
}

ImpValue EVALVisitor::visit(FCallExp *exp) {
    if (funciones.count(exp->nombre) == 0) {
        std::cerr << "Error: función no encontrada: " << exp->nombre << std::endl;
        return ImpValue();
    }
    FunDec* f = funciones[exp->nombre];

    vector<ImpValue> args;
    for (auto arg_exp : exp->argumentos) {
        args.push_back(arg_exp->accept(this));
    }

    env.add_level();

    for (size_t i = 0; i < f->parametros.size(); ++i) {
        std::string pname = f->parametros[i];
        std::string ptype = f->tipos[i];
        if (ptype == "int")
            env.add_var(pname, args[i].int_value, "int");
        else if (ptype == "bool")
            env.add_var(pname, args[i].bool_value ? 1 : 0, "bool");
        else if (ptype == "string")
            env.add_var(pname, args[i].string_value, "string");
    }

    this->return_encountered = false;
    this->return_value = ImpValue();

    f->cuerpo->accept(this);
    env.remove_level();
    return this->return_value;
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
    for (auto stms : stm->sent_if) {
        if (!stms->condi || stms->condi->accept(this).bool_value) {
            stms->body->accept(this);
            break;
        }
    }
}

void EVALVisitor::visit(ForStatement *stm) {
    env.add_level();

    env.add_var(stm->id, 0, stm->type);
    int init = stm->start->accept(this).int_value;
    env.update(stm->id, init);

    while (stm->condition->accept(this).bool_value) {
        stm->b->accept(this);
        int next = stm->step->accept(this).int_value;
        env.update(stm->id, next);
    }

    env.remove_level();
}

void EVALVisitor::visit(StatementList *stm) {
    for (auto s : stm->stms) {
        s->accept(this);
        if (this->return_encountered) break;
    }
}

void EVALVisitor::visit(DoWhileStatement *stm) {
    stm->b->accept(this);
    if(stm->condition->accept(this).bool_value) {
        stm->b->accept(this);
    }
}

void EVALVisitor::visit(ReturnStatement *stm) {
    this->return_value = stm->e->accept(this);
    this->return_encountered = true;
}

void EVALVisitor::visit(WhileStatement *stm) {
    if(stm->condition->accept(this).bool_value) {
        stm->b->accept(this);
    }
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
        funciones[vd->nombre] = vd;
    }
}

void EVALVisitor::visit(FunDec *fd) {
    env.add_level();
    fd->cuerpo->accept(this);
    env.remove_level();
}


void EVALVisitor::visit(Body *b) {
    if (b->vardecs) b->vardecs->accept(this);
    if (b->slist) b->slist->accept(this);
}


void EVALVisitor::visit(VarDec *stm) {
    for(auto i: stm->vars){
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
            env.add_var(i->id, stm->type);
        }
    }
}

void EVALVisitor::visit(Var *stm) {
}